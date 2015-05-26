// Copyright (c) 2011-2012 Ryan Prichard
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

#include "Agent.h"
#include "AgentAssert.h"
#include <stdio.h>
#include <stdlib.h>

static int server(wchar_t *program, wchar_t *args, int width, int height);
static int winsize();
int winclient(int argc, wchar_t *argv[]);

int wmain(int argc, wchar_t *argv[])
{
	if (argc > 1) {
		if (!_wcsicmp(argv[1], L"agent") && argc == 6) {
			return server(argv[2], argv[3], _wtoi(argv[4]), _wtoi(argv[5]));
		} else if (!_wcsicmp(argv[1], L"console")) {
			return winclient(argc - 1, &argv[1]);
		} else if (!_wcsicmp(argv[1], L"winsize")) {
			return winsize();
		}
	}

	wprintf(L"%s - winpty host for JOE for Windows\n", argv[0]);
	wprintf(L"Copyright (c) 2011-2012 Ryan Prichard\n");
	wprintf(L"Portions copyright (c) 2015 John J. Jordan\n\n");

	wprintf(L"Permission is hereby granted, free of charge, to any person obtaining a copy\n");
	wprintf(L"of this software and associated documentation files (the \"Software\"), to\n");
	wprintf(L"deal in the Software without restriction, including without limitation the\n");
	wprintf(L"rights to use, copy, modify, merge, publish, distribute, sublicense, and/or\n");
	wprintf(L"sell copies of the Software, and to permit persons to whom the Software is\n");
	wprintf(L"furnished to do so, subject to the following conditions\n\n");

	wprintf(L"The above copyright notice and this permission notice shall be included in\n");
	wprintf(L"all copies or substantial portions of the Software.\n\n");

	wprintf(L"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n");
	wprintf(L"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n");
	wprintf(L"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n");
	wprintf(L"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n");
	wprintf(L"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING\n");
	wprintf(L"FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS\n");
	wprintf(L"IN THE SOFTWARE.\n\n");

	wprintf(L"Usage: %s console <program> <arguments>\n", argv[0]);
	wprintf(L"       %s winsize\n\n", argv[0]);

	wprintf(L"This program should generally be used by scripts in JOE for Windows rather\n");
	wprintf(L"than directly.  Refer to the source code for further information.\n");
	wprintf(L"http://sourceforge.net/p/joe-editor\n");

	return 0;
}

/*** server ***/

static int server(wchar_t *program, wchar_t *args, int width, int height)
{
	Agent agent(program, args, width, height);
	agent.run();
	return 0;
}

/*** window size helper ***/

static int winsize()
{
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	wchar_t buf[64];

	if (GetEnvironmentVariable(L"JOEWINSZPIPE", buf, 64)) {
		std::wstring pipename = L"\\\\.\\pipe\\";
		pipename.append(buf);

		HANDLE sizepipe = CreateFile(pipename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (sizepipe == INVALID_HANDLE_VALUE) {
			return 1;
		}

		for (;;) {
			char szbuf[128];
			DWORD amnt;
			BOOL ret = ReadFile(sizepipe, szbuf, sizeof(szbuf) - 1, &amnt, NULL);

			if (!ret || !amnt) break;

			ret = WriteFile(hstdout, szbuf, amnt, &amnt, NULL);
			if (!ret || !amnt) break;
		}

		CloseHandle(sizepipe);
	}

	return 0;
}
