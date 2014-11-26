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
#include <assert.h>

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

/* Set signals for JOE */
void sigjoe(void)
{
	ttysig = 1;
}

/* Restore signals for exiting */
void signrm(void)
{
	ttysig = 0;
}

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
		jwWriteIO(JW_SIDE_EDITOR, obuf, obufp);
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

		/* If there is something in the input buffer, have and havec should be set! */
		assert(ibufp == ibufsiz);

		m = jwWaitForEditorComm(timeout);

		if (!m)
		{
			/* Timeout */
			ticked = 1;
			goto loop;
		}

		if (JWISIO(m))
		{
			/* IO */
			ibufsiz = jwReadIO(m, ibuf);
			jwReleaseComm(JW_SIDE_EDITOR, m);

			assert(ibufsiz > 0);
			havec = ibuf[0];
			ibufp = 1;
		}
		else
		{
			if (handlejwcontrol(m))
			{
				return -1;
			}

			jwReleaseComm(JW_SIDE_EDITOR, m);
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
	if (m->msg == COMM_WINRESIZE)
	{
		if (winwidth != m->arg1 || winheight != m->arg2)
		{
			winwidth = m->arg1;
			winheight = m->arg2;
			winched = 1;
		}
	}
	else if (m->msg == COMM_DROPFILES)
	{
		struct CommMessage *n = m;
		unsigned char **files = vamk(m->arg3);
		int x = m->arg1, y = m->arg2;

		/* We will be sent a packet with count=0 to denote no more files */
		while (n && n->arg3 > 0)
		{
			files = vaadd(files, vsdupz(USTR n->buffer->buffer));

			if (n != m)
			{
				jwReleaseComm(JW_SIDE_EDITOR, n);
			}

			n = jwWaitForEditorComm(INFINITE);
			assert(n);
			assert(n->msg == COMM_DROPFILES);
		}

		if (n != m)
		{
			jwReleaseComm(JW_SIDE_EDITOR, n);
		}

		co_call(dodropfiles, files, x, y);

		varm(files);
		edupd(1);
	}
	else if (m->msg == COMM_COLORSCHEME)
	{
		static CMD *retype = NULL;
		struct jwcolors *colors = (struct jwcolors *)m->ptr;
		W *w = maint->topwin;

		applyscheme(colors);

		/* Redraw screen */
		if (!retype)
		{
			retype = findcmd(USTR "retype");
		}

		execmd(retype, 0);
		edupd(1);
	}
	else if (m->msg == COMM_EXEC)
	{
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
	}
	else if (m->msg == COMM_EXIT)
	{
		/* Shut down and exit */
		CMD *c = findcmd(USTR "killjoe");
		execmd(c, 0);

		/* Recycle this message first because we're about to... */
		jwReleaseComm(JW_SIDE_EDITOR, m);

		/* ...exit the thread */
		return 1;
	}

	send_buffer_changes();

	return 0;
}

void ttsusp(void)
{
}
