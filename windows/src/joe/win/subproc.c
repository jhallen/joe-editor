/*
 *  This file is part of Joe's Own Editor for Windows.
 *  Copyright (c) 2015 John J. Jordan.
 *
 *  Joe's Own Editor for Windows is free software: you can redistribute it 
 *  and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2 of the 
 *  License, or (at your option) any later version.
 *
 *  Joe's Own Editor for Windows is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Joe's Own Editor for Windows.  If not, see
 *  <http://www.gnu.org/licenses/>.
 */

#include "types.h"
#include "jwcomm.h"

int nmpx = 0;

static void bakempx(MPX *m, unsigned char c, DWORD mode);
static void controlc(MPX *m);
static DWORD WINAPI mpxreadthread(LPVOID arg);

MPX *mpxmk(int *ptyfd, unsigned char *cmd, unsigned char **args, void (*func) (void*, unsigned char*, int), void *object, void (*die) (void*), void *dieobj, int out_only,
	   int w, int h)
{
	HANDLE prr, prw, pwr, pww;
	SECURITY_ATTRIBUTES sa;
	PROCESS_INFORMATION pinf;
	STARTUPINFOW si;
	unsigned char *allargs = 0;
	wchar_t *wargs = NULL, *wcmd = NULL;
	MPX *m = NULL;
	DWORD res;
	int i;

	if (!nmpx) {
		ZeroMemory(asyncs, sizeof(asyncs));
	}

	for (i = 0; i < NPROC; i++) {
		if (!asyncs[i].func) {
			m = &asyncs[i];
			break;
		}
	}

	if (i == NPROC) {
		return NULL;
	}

	nmpx++;

	/* Remember callback function */
	m->func = func;
	m->object = object;
	m->die = die;
	m->dieobj = dieobj;

	/* Setup arguments */
	for (i = 0; i < valen(args); i++) {
		if (allargs)
			allargs = vscatz(allargs, USTR " ");

		allargs = vscat(allargs, args[i], vslen(args[i]));
	}

	i = utf8towcslen(allargs);
	wargs = (wchar_t*)joe_malloc((i + 1) * sizeof(wchar_t));

	if (utf8towcs(wargs, allargs, i)) {
		assert(0);
		goto fail;
	}

	i = utf8towcslen(cmd);
	wcmd = (wchar_t*)joe_malloc((i + 1) * sizeof(wchar_t));

	if (utf8towcs(wcmd, cmd, i)) {
		assert(0);
		goto fail;
	}

	/* Setup pipes */
	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&prr, &prw, &sa, 0) || !SetHandleInformation(prw, HANDLE_FLAG_INHERIT, 0)) {
		assert(0);
		goto fail;
	}

	if (!CreatePipe(&pwr, &pww, &sa, 0) || !SetHandleInformation(pwr, HANDLE_FLAG_INHERIT, 0)) {
		assert(0);
		CloseHandle(prr);
		CloseHandle(prw);
		goto fail;
	}

	/* Create child */
	ZeroMemory(&si, sizeof(STARTUPINFOW));
	si.cb = sizeof(STARTUPINFOW);
	si.hStdInput = prr;
	si.hStdOutput = pww;
	si.hStdError = pww;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = FALSE;

	res = CreateProcessW(wcmd, wargs, NULL, NULL, TRUE, CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pinf);

	CloseHandle(pww);
	CloseHandle(prr);

	joe_free(wcmd);
	joe_free(wargs);
	wcmd = wargs = NULL;

	if (res) {
		/* Finish setting up MPX */
		m->ackfd = jwCreateQueue(32, jwCreateWake());
		m->dataqd = jwCreateQueue(MPX_BUFSZ, JW_TO_EDITOR);
		m->hReadPipe = pwr;
		m->ptyfd = *ptyfd = _open_osfhandle((intptr_t)prw, 0);
		m->hProcess = pinf.hProcess;
		m->pid = pinf.dwProcessId;
		m->droplf = 0;
		m->raw = 0;

		m->linebuf = bmk(NULL);

		/* Start reader thread */
		m->hReadThread = CreateThread(NULL, 0, mpxreadthread, m, 0, 0);

		CloseHandle(pinf.hThread);

		return m;
	} else {
		CloseHandle(pwr);
		CloseHandle(prw);
	}

fail:
	if (wargs) free(wargs);
	if (wcmd) free(wcmd);
	--nmpx;
	ZeroMemory(m, sizeof(MPX));
	return NULL;
}

static DWORD WINAPI mpxreadthread(LPVOID arg)
{
	char buf[MPX_BUFSZ];
	int credits = 2;
	MPX *m = (MPX*)arg;
	DWORD amount;
	struct CommMessage *msg;
	int ackqd, dataqd;
	HANDLE hReadPipe;

	/* Can't assume this pointer will always be valid. */
	ackqd = m->ackfd;
	dataqd = m->dataqd;
	hReadPipe = m->hReadPipe;

	for (;;) {
		while (!credits) {
			msg = jwWaitForComm(&ackqd, 1, INFINITE, NULL);
			if (msg->msg == COMM_ACK) {
				credits++;
			} else if (msg->msg == COMM_EXIT) {
				CloseHandle(hReadPipe);
				jwSendComm0p(dataqd, COMM_EXIT, m);
				return 0;
			} else {
				assert(0);
			}

			jwReleaseComm(ackqd, msg);
		}

		if (!ReadFile(hReadPipe, (LPVOID)buf, MPX_BUFSZ, &amount, NULL)) {
			/* Shut it down */
			CloseHandle(hReadPipe);
			jwSendComm0p(dataqd, COMM_EXIT, m);
			return 0;
		}

		jwSendComm0pd(dataqd, COMM_MPXDATA, m, buf, amount);
		--credits;
	}
}

void mpxdied(MPX *m)
{
	CloseHandle(m->hReadThread);
	CloseHandle(m->hProcess);

	if (m->die) {
		/* This should close the process's stdin fd */
		m->die(m->dieobj);
	}

	jwCloseQueue(m->ackfd);
	jwCloseQueue(m->dataqd);

	brm(m->linebuf);

	ZeroMemory(m, sizeof(MPX));

	edupd(1);
}

void killmpx(int pid, int sig)
{
	int i;

	for (i = 0; i < NPROC; i++) {
		MPX *mpx = &asyncs[i];

		/* TODO: Investigate assigning subprocess to a job and killing the job instead */

		if (mpx->pid == pid) {
			const int defalloc = 64;
			LPDWORD processes;
			DWORD nprocesses, me;
			int t;

			/* Notify reader thread to get out */
			jwSendComm0(mpx->ackfd, COMM_EXIT);

			/* Get list of processes to kill */
			AttachConsole(mpx->pid);
			processes = (LPDWORD)joe_malloc(sizeof(DWORD) * defalloc);
			nprocesses = GetConsoleProcessList(processes, defalloc);
			if (nprocesses > defalloc) {
				processes = (LPDWORD)realloc(processes, sizeof(DWORD) * nprocesses);
				nprocesses = GetConsoleProcessList(processes, nprocesses);
			}

			me = GetCurrentProcessId();

			for (t = 0; t < nprocesses; t++) {
				if (processes[t] == pid) {
					TerminateProcess(mpx->hProcess, 1);
				} else if (processes[t] != me) {
					HANDLE hproc = OpenProcess(PROCESS_TERMINATE, FALSE, processes[t]);
					TerminateProcess(hproc, 1);
					CloseHandle(hproc);
				}
			}

			FreeConsole();
			joe_free(processes);

			return;
		}
	}
}


int writempx(int fd, void *data, size_t amt)
{
	int i;

	/* See if we can find it, first */
	for (i = 0; i < NPROC; i++) {
		if (asyncs[i].func && asyncs[i].ptyfd == fd) {
			/* This is one of our shell windows, process as necessary. */
			unsigned char *str = USTR data;
			MPX *mpx = &asyncs[i];
			DWORD mode;
			int t;

			if (mpx->raw) {
				/* Just write */
				break;
			}

			AttachConsole(mpx->pid);
			GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode);
			FreeConsole();

			for (t = 0; t < amt; t++) {
				bakempx(mpx, str[t], mode);
			}

			return amt;
		}
	}

	return joe_write(fd, data, amt);
}


/* Pre-process data before sending it off to a subprocess */
static void bakempx(MPX *m, unsigned char c, DWORD mode)
{
	int droplf = m->droplf;
	m->droplf = (c == 13);

	if (mode & ENABLE_LINE_INPUT) {
		if (c == 13 || (c == 10 && !droplf)) {
			/* Enter */
			bsavefd(m->linebuf->bof, m->ptyfd, m->linebuf->eof->byte);
			bdel(m->linebuf->bof, m->linebuf->eof);
			joe_write(m->ptyfd, "\r\n", 2);
			m->func(m->object, USTR "\r\n", 2);
		} else if (c == 127) {
			/* Backspace */
			P *p;

			p = pdup(m->linebuf->eof, USTR "bakempx");
			prgetc(p);
			bdel(p, m->linebuf->eof);
			prm(p);

			m->func(m->object, &c, 1);
		} else if (c == 3) {
			/* Control-C */
			controlc(m);

			/* Drop current line */
			bdel(m->linebuf->bof, m->linebuf->eof);
		} else if (c != 10) {
			binsbyte(m->linebuf->eof, c);
			m->func(m->object, &c, 1);
		}
	} else if (c == 13 || (c == 10 && !droplf)) {
		/* Enter */
		joe_write(m->ptyfd, "\r\n", 2);
	} else if (c == 3) {
		/* Control-C */
		controlc(m);
	} else if (c != 10) {
		joe_write(m->ptyfd, &c, 1);
	}
}

static void controlc(MPX *m)
{
	/* TODO: This doesn't work as expected.  Documentation suggests that cmd.exe might be swallowing
	   these events, but it's worth some further investigation. */
	AttachConsole(m->pid);
	SetConsoleCtrlHandler(NULL, TRUE);
	GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0); /* Or should this be CTRL_BREAK_EVENT? */
	FreeConsole();
}

void ttstsz(int fd, int w, int h)
{
}

/* Set "raw" mode for a vt */

void vtraw(int fd)
{
	int i;

	for (i = 0; i < NPROC; i++) {
		if (asyncs[i].func && asyncs[i].ptyfd == fd) {
			asyncs[i].raw = 1;
			break;
		}
	}
}
