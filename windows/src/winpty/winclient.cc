// Copyright (c) 2011-2012 Ryan Prichard
// Portions copyright (c) 2015 John J. Jordan -- ported to pure-Win32
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include "winpty.h"
#include "DebugClient.h"
#include <string>
#include <vector>


static volatile bool ioHandlerDied;
static HANDLE wakehandle;

// Create a manual reset, initially unset event.
static HANDLE createEvent()
{
	return CreateEvent(NULL, TRUE, FALSE, NULL);
}

// Connect winpty overlapped I/O to Cygwin blocking STDOUT_FILENO.
class OutputHandler {
public:
	OutputHandler(HANDLE winpty);
	HANDLE getThread() { return thread; }
	void *threadProc();
private:
	HANDLE winpty;
	HANDLE thread;
};

static DWORD WINAPI outputthread(LPVOID arg)
{
	((OutputHandler*)arg)->threadProc();
	return 0;
}

OutputHandler::OutputHandler(HANDLE winpty) : winpty(winpty)
{
	thread = CreateThread(NULL, 0, outputthread, this, 0, NULL);
}

// TODO: See whether we can make the pipe non-overlapped if we still use
// an OVERLAPPED structure in the ReadFile/WriteFile calls.
void *OutputHandler::threadProc()
{
	HANDLE event = createEvent();
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	OVERLAPPED over;
	const int bufferSize = 4096;
	char *buffer = new char[bufferSize];
	while (true) {
		DWORD amount;
		memset(&over, 0, sizeof(over));
		over.hEvent = event;
		BOOL ret = ReadFile(this->winpty,
			buffer, bufferSize,
			&amount,
			&over);
		if (!ret && GetLastError() == ERROR_IO_PENDING)
			ret = GetOverlappedResult(this->winpty, &over, &amount, TRUE);
		if (!ret || amount == 0)
			break;
		// TODO: partial writes?
		// I don't know if this write can be interrupted or not, but handle it
		// just in case.
		DWORD written;
		WriteFile(hstdout, buffer, amount, &written, NULL);
		if (written != (int)amount)
			break;
	}
	delete [] buffer;
	CloseHandle(event);
	ioHandlerDied = true;
	SetEvent(wakehandle);
	return NULL;
}


// Connect Cygwin non-blocking STDIN_FILENO to winpty overlapped I/O.
class InputHandler {
public:
	InputHandler(HANDLE winpty);
	HANDLE getThread() { return thread; }
	void *threadProc();
private:
	HANDLE winpty;
	HANDLE thread;
};

static DWORD WINAPI inputthread(LPVOID arg)
{
	((InputHandler*)arg)->threadProc();
	return 0;
}

InputHandler::InputHandler(HANDLE winpty) : winpty(winpty)
{
	thread = CreateThread(NULL, 0, inputthread, this, 0, NULL);
}

void *InputHandler::threadProc()
{
	HANDLE event = createEvent();
	HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
	const int bufferSize = 4096;
	char *buffer = new char[bufferSize];
	while (true) {
		DWORD amount;
		BOOL rres = ReadFile(hstdin, buffer, bufferSize, &amount, NULL);

		if (!rres || amount < 1)
			break;

		DWORD written;
		OVERLAPPED over;
		memset(&over, 0, sizeof(over));
		over.hEvent = event;
		BOOL ret = WriteFile(this->winpty,
			buffer, amount,
			&written,
			&over);
		if (!ret && GetLastError() == ERROR_IO_PENDING)
			ret = GetOverlappedResult(this->winpty, &over, &written, TRUE);
		// TODO: partial writes?
		if (!ret || (int)written != amount)
			break;
	}
	delete [] buffer;
	CloseHandle(event);
	ioHandlerDied = true;
	SetEvent(wakehandle);
	return NULL;
}

// Convert argc/argv into a Win32 command-line following the escaping convention
// documented on MSDN.  (e.g. see CommandLineToArgvW documentation)
static std::wstring argvToCommandLine(const std::vector<std::wstring> &argv)
{
	std::wstring result;
	for (size_t argIndex = 0; argIndex < argv.size(); ++argIndex) {
		if (argIndex > 0)
			result.push_back(' ');
		const wchar_t *arg = argv[argIndex].c_str();
		const bool quote =
			wcschr(arg, L' ') != NULL ||
			wcschr(arg, L'\t') != NULL ||
			*arg == L'\0';
		if (quote)
			result.push_back(L'\"');
		int bsCount = 0;
		for (const wchar_t *p = arg; *p != L'\0'; ++p) {
			if (*p == L'\\') {
				bsCount++;
			} else if (*p == L'\"') {
				result.append(bsCount * 2 + 1, L'\\');
				result.push_back(L'\"');
				bsCount = 0;
			} else {
				result.append(bsCount, L'\\');
				bsCount = 0;
				result.push_back(*p);
			}
		}
		if (quote) {
			result.append(bsCount * 2, L'\\');
			result.push_back(L'\"');
		} else {
			result.append(bsCount, L'\\');
		}
	}
	return result;
}

int winclient(int argc, wchar_t *argv[])
{
	wchar_t buf[64];
	int cols, rows;
	int havesize = 0;

	if (GetEnvironmentVariable(L"LINES", buf, 64)) {
		rows = _wtoi(buf);
		if (GetEnvironmentVariable(L"COLUMNS", buf, 64)) {
			cols = _wtoi(buf);
			havesize = 1;
		}
	}

	if (!havesize) {
		cols = 80;
		rows = 25;
	}

	winpty_t *winpty = winpty_open(cols, rows);
	if (winpty == NULL) {
		fprintf(stderr, "Error creating winpty.\n");
		exit(1);
	}

	{
		// Start the child process under the console.
		std::vector<std::wstring> argVector;
		argVector.push_back(argv[1]);
		for (int i = 2; i < argc; ++i)
			argVector.push_back(argv[i]);
		std::wstring cmdLine = argvToCommandLine(argVector);
		int ret = winpty_start_process(winpty,
			NULL,
			cmdLine.c_str(),
			NULL,
			NULL);
		if (ret != 0) {
			fprintf(stderr,
				"Error %#x starting %s\n",
				(unsigned int)ret,
				cmdLine.c_str());
			exit(1);
		}
	}

	wakehandle = createEvent();

	OutputHandler outputHandler(winpty_get_data_pipe(winpty));
	InputHandler inputHandler(winpty_get_data_pipe(winpty));

	/* Connect to window size pipe if it exists... */

	HANDLE sizepipe = INVALID_HANDLE_VALUE;

	if (GetEnvironmentVariable(L"JOEWINSZPIPE", buf, 64)) {
		std::wstring pipename = L"\\\\.\\pipe\\";
		pipename.append(buf);

		sizepipe = CreateFile(pipename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

		// We don't bother trying to wait if it failed.  If that happens, then we just abandon
		// trying to resize the windows.
	}

	if (sizepipe == INVALID_HANDLE_VALUE) {
		// If no pipe, then just wait for IO handlers to exit
		WaitForSingleObject(wakehandle, INFINITE);
	} else {
		DWORD dwMode = PIPE_READMODE_MESSAGE;
		SetNamedPipeHandleState(sizepipe, &dwMode, NULL, NULL);

		OVERLAPPED ovr;
		ZeroMemory(&ovr, sizeof(OVERLAPPED));
		ovr.hEvent = wakehandle;

		while (!ioHandlerDied) {
			char szbuf[128];
			DWORD len;
			BOOL rres = ReadFile(sizepipe, szbuf, sizeof(szbuf) - 1, &len, &ovr);

			DWORD dwerr = GetLastError();
			if (!rres && dwerr == ERROR_IO_PENDING) {
				// Wait.
				WaitForSingleObject(wakehandle, INFINITE);

				rres = GetOverlappedResult(sizepipe, &ovr, &len, FALSE);
				if (!rres) continue;
			}

			if (rres && len) {
				int x, y;

				// Read success: handle result.
				szbuf[len] = 0;

				char *mid = strchr(szbuf, ' ');
				if (mid) {
					*mid = 0;
					x = atoi(szbuf);
					y = atoi(mid + 1);

					winpty_set_size(winpty, x, y);
				}

				continue;
			}

			// Error
			break;
		}

		CloseHandle(sizepipe);
	}

	int exitCode = winpty_get_exit_code(winpty);

	winpty_close(winpty);
	return exitCode;
}
