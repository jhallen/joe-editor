/*
 *  This file is part of Joe's Own Editor for Windows.
 *  Copyright (c) 2014 John J. Jordan.
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

int idleout = 1;

/* Global configuration variables */

int noxon = 0;			/* Set if ^S/^Q processing should be disabled */
int Baud = 38400;		/* Baud rate from joerc, cmd line or environment */

/* Output buffer, index and size */

unsigned char *obuf = NULL;
int obufp = 0;
int obufsiz;

unsigned char ibuf[UI_TO_EDITOR_IOSZ];
int ibufp = 0;
int ibufsiz = 0;

/* The baud rate */

unsigned baud;			/* Bits per second */
unsigned long upc;		/* Microseconds per character */

/* Input buffer */

int have = 0;			/* Set if we have pending input */
static unsigned char havec;	/* Character read in during pending input check */
int leave = 0;			/* When set, typeahead checking is disabled */

/* TTY mode flag.  1 for open, 0 for closed */
static int ttymode = 0;

/* Signal state flag.  1 for joe, 0 for normal */
static int ttysig = 0;

/* Current window size */
static int winwidth = 0, winheight = 0;

/* Stuff for shell windows */
static int nmpx = 0;
MPX asyncs[NPROC];
static void mpxdied(MPX *m);

/* Open terminal and set signals */

void ttopen(void)
{
	sigjoe();
	ttopnn();
}

/* Close terminal and restore signals */

void ttclose(void)
{
	ttclsn();
	signrm();
}

static int winched = 0;

/* Second ticker */

int ticked = 0;

/* Open terminal */

void ttopnn(void)
{
	if (ttymode)
	{
		return;
	}

	ttymode = 1;

	// Next bit in regular implementation turns off XON/XOFF, and ECHO
	// line discipline.  This is already set correctly on the PuTTY end.

	// Next bit in regular implementation is baud calcuation nonsense.
	// Note: High baud rate turns off scroll.  Normally, yeah this would be fine,
	// but it doesn't work very well over remote desktop without it.  baud ought
	// to be 9600 per the putty termcap, so keep it rather than assigning from Baud.
	baud = 9600;
	upc = DIVIDEND / baud;
	
	// Setup output buffer
	if (obuf)
	{
		joe_free(obuf);
	}

	// The next bit normally picks an obufsiz based on TIMES and DIVIDEND because
	// joe, at one point, would throttle its output so that the terminal could
	// keep up, but that logic is just a touch obsolete.  Use 4k...

	obufsiz = 4096;
	obuf = (unsigned char *) joe_malloc(obufsiz);

	winwidth = jw_initialcols;
	winheight = jw_initialrows;

	if (jw_initialcolorscheme)
	{
		applyscheme(jw_initialcolorscheme);
		jw_initialcolorscheme = NULL;
	}
}

/* Close terminal */

void ttclsn(void)
{
	int oleave;

	if (ttymode)
		ttymode = 0;
	else
		return;

	oleave = leave;
	leave = 1;

	ttflsh();
	leave = oleave;
}

/* Flush output and check for typeahead */

int ttflsh(void)
{
	/* Flush output */
	if (obufp) {
		jwWriteIO(JW_FROM_EDITOR, obuf, obufp);
		obufp = 0;
	}

	/* Check for typeahead */
	if (!have && !leave) {
		if (ibufp < ibufsiz)
		{
			have = 1;
			havec = ibuf[ibufp++];
		}
	}

	return 0;
}

/* Read next character from input */

time_t last_time;

int handlejwcontrol(struct CommMessage *m);

int ttgetc(void)
{
	time_t new_time;
	int flg;
	int set_tick = 0;
	DWORD timeout = 1000; // milliseconds
	int qds[1 + NPROC];

	set_tick = 1;

	/* Send any pending buffer changes on the outset */
	send_buffer_changes();

      loop:
      	flg = 0;
      	/* Status line clock */
	new_time = time(NULL);
	if (new_time != last_time) {
		last_time = new_time;
		dostaupd = 1;
		ticked = 1;
	}
	/* Autoscroller */
	if (auto_scroll && mnow() >= auto_trig_time) {
		do_auto_scroll();
		ticked = 1;
		flg = 1;
	}
	ttflsh();
	while (winched) {
		winched = 0;
		dostaupd = 1;
		edupd(1);
		ttflsh();
	}
	if (ticked) {
		edupd(flg);
		ttflsh();
		ticked = 0;
		set_tick = 1;
	}

	if (set_tick) {
		set_tick = 0;
		if (auto_scroll) {
			timeout = auto_trig_time - mnow();
		} else {
			timeout = 1000;
		}
	}

	if (have) {
		have = 0;
	} else {
		struct CommMessage *m;
		int i, n;
		int qd;

		/* If there is something in the input buffer, have and havec should be set! */
		assert(ibufp == ibufsiz);

		qds[0] = JW_TO_EDITOR;
		n = 1;

		if (nmpx) {
			for (i = 0; i < NPROC; i++) {
				if (asyncs[i].func) {
					qds[n++] = asyncs[i].dataqd;
				}
			}
		}

		m = jwWaitForComm(qds, n, timeout, &qd);

		if (!m) {
			/* Timeout */
			ticked = 1;
			goto loop;
		}

		if (qd == JW_TO_EDITOR) {
			if (JWISIO(m)) {
				/* IO */
				ibufsiz = jwReadIO(m, ibuf);
				jwReleaseComm(JW_TO_EDITOR, m);

				assert(ibufsiz > 0);
				havec = ibuf[0];
				ibufp = 1;
			} else {
				if (handlejwcontrol(m)) {
					return -1;
				}

				jwReleaseComm(qd, m);
				goto loop;
			}
		} else {
			MPX *mpx = (MPX*)m->ptr;

			if (m->msg == COMM_MPXDATA) {
				jwSendComm0(mpx->ackfd, COMM_ACK);

				mpx->func(mpx->object, USTR m->buffer->buffer, m->buffer->size);
				edupd(1);
			} else if (m->msg == COMM_EXIT) {
				mpxdied(mpx);
			}

			jwReleaseComm(qd, m);
			goto loop;
		}
	}

	return havec;
}

/* Write string to output */

void ttputs(unsigned char *s)
{
	while (*s) {
		obuf[obufp++] = *s++;
		if (obufp == obufsiz)
			ttflsh();
	}
}

/* Get window size */

void ttgtsz(int *x, int *y)
{
	*x = winwidth;
	*y = winheight;
}

int handlejwcontrol(struct CommMessage *m)
{
	if (m->msg == COMM_WINRESIZE) {
		if (winwidth != m->arg1 || winheight != m->arg2) {
			winwidth = m->arg1;
			winheight = m->arg2;
			winched = 1;
		}
	} else if (m->msg == COMM_DROPFILES) {
		struct CommMessage *n = m;
		unsigned char **files = vamk(m->arg3);
		int x = m->arg1, y = m->arg2;
		int qd = JW_TO_EDITOR;

		/* We will be sent a packet with count=0 to denote no more files */
		while (n && n->arg3 > 0) {
			files = vaadd(files, vsdupz(USTR n->buffer->buffer));

			if (n != m) {
				jwReleaseComm(JW_TO_EDITOR, n);
			}

			n = jwWaitForComm(&qd, 1, INFINITE, NULL);
			assert(n);
			assert(n->msg == COMM_DROPFILES);
		}

		if (n != m) {
			jwReleaseComm(JW_TO_EDITOR, n);
		}

		co_call(dodropfiles, files, x, y);

		varm(files);
		edupd(1);
	} else if (m->msg == COMM_COLORSCHEME) {
		static CMD *retype = NULL;
		struct jwcolors *colors = (struct jwcolors *)m->ptr;
		W *w = maint->topwin;

		applyscheme(colors);

		/* Redraw screen */
		if (!retype) {
			retype = findcmd(USTR "retype");
		}

		execmd(retype, 0);
		edupd(1);
	} else if (m->msg == COMM_EXEC) {
		/* Called with either a buffer attached or a pointer to a const string */

		char *data;
		char buf[256];
		char *dealloc = NULL;

		/* mparse writes back to buffer, make sure it is in writeable memory! */
		if (m->buffer) {
			data = m->buffer->buffer;
		} else if (m->ptr) {
			if (strlen((char*)m->ptr) > sizeof(buf)) {
				data = strdup((char*)m->ptr);
				dealloc = data;
			} else {
				strcpy(buf, (char*)m->ptr);
				data = buf;
			}
		}

		if (data) {
			int sta;
			MACRO *m = mparse(NULL, USTR data, &sta);
			co_call(exemac, m);
			rmmacro(m);
			if (dealloc) free(dealloc);
			edupd(1);
		}
	} else if (m->msg == COMM_EXIT) {
		/* Shut down and exit */
		CMD *c = findcmd(USTR "killjoe");
		execmd(c, 0);

		/* Recycle this message first because we're about to... */
		jwReleaseComm(JW_TO_EDITOR, m);

		/* ...exit the thread */
		return 1;
	}

	send_buffer_changes();

	return 0;
}

void ttsusp(void)
{
}

/* Exception handling */

static DWORD editorthread;

static LONG WINAPI SehHandler(LPEXCEPTION_POINTERS lpe)
{
	/* Only handle errors in the editor */
	if (editorthread != GetCurrentThreadId())
		return EXCEPTION_EXECUTE_HANDLER;
	
	ttsig(lpe->ExceptionRecord->ExceptionCode);
}

/* Set signals for JOE */
void sigjoe(void)
{
	if (ttysig)
		return;
	ttysig = 1;

	joe_set_signal(SIGSEGV, ttsig);
	joe_set_signal(SIGABRT, ttsig);
	joe_set_signal(SIGTERM, ttsig);
	joe_set_signal(SIGILL, ttsig);

	editorthread = GetCurrentThreadId();
	SetUnhandledExceptionFilter(SehHandler);
}

/* Restore signals for exiting */
void signrm(void)
{
	if (!ttysig)
		return;
	ttysig = 0;
	joe_set_signal(SIGSEGV, SIG_DFL);
	joe_set_signal(SIGABRT, SIG_DFL);
	joe_set_signal(SIGTERM, SIG_DFL);
	joe_set_signal(SIGILL, SIG_DFL);

	SetUnhandledExceptionFilter(NULL);
}

/* Subprocess stuff */

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

MPX *mpxmk(int *ptyfd, unsigned char *cmd, unsigned char **args, void (*func) (void*, unsigned char*, int), void *object, void (*die) (void*), void *dieobj, int out_only)
{
	HANDLE prr, prw, pwr, pww, hthread, hconsole;
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

static void mpxdied(MPX *m)
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

static void controlc(MPX *m)
{
	/* TODO: This doesn't work as expected.  Documentation suggests that cmd.exe might be swallowing
	   these events, but it's worth some further investigation. */
	AttachConsole(m->pid);
	SetConsoleCtrlHandler(NULL, TRUE);
	GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0); /* Or should this be CTRL_BREAK_EVENT? */
	FreeConsole();
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
