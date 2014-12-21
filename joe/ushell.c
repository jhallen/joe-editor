/*
 *	Shell-window functions
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* Executed when shell process terminates */

static void cdone(B *b)
{
	b->pid = 0;
	close(b->out);
	b->out = -1;
}

static void cdone_parse(B *b)
{
	b->pid = 0;
	close(b->out);
	b->out = -1;
	parserrb(b);
}

/* Executed for each chunk of data we get from the shell */

static void cfollow(B *b,long byte)
{
	W *w;
	 if ((w = maint->topwin) != NULL) {
	 	do {
	 		if ((w->watom->what&TYPETW) && ((BW *)w->object)->b==b && ((BW *)w->object)->cursor->byte==byte) {
	 			BW *bw = (BW *)w->object;
	 			p_goto_eof(bw->cursor);
				bw->cursor->xcol = piscol(bw->cursor);
	 		}
		w = w->link.next;
	 	} while (w != maint->topwin);
	 }
}

static void cdata(B *b, unsigned char *dat, int siz)
{
	P *q = pdup(b->eof, USTR "cdata");
	P *r = pdup(b->eof, USTR "cdata");
	long byte = q->byte;
	unsigned char bf[1024];
	int x, y;

	for (x = y = 0; x != siz; ++x) {
		if (dat[x] == 13 || dat[x] == 0) {
			;
		} else if (dat[x] == 8 || dat[x] == 127) {
			if (y) {
				--y;
			} else {
				pset(q, r);
				prgetc(q);
				bdel(q, r);
				--byte;
			}
		} else if (dat[x] == 7) {
			ttputc(7);
		} else {
			bf[y++] = dat[x];
		}
	}
	if (y) {
		binsm(r, bf, y);
	}
	prm(r);
	prm(q);

	cfollow(b,byte);
}

int cstart(BW *bw, unsigned char *name, unsigned char **s, void *obj, int build, int out_only)
{
#if defined(__MSDOS__) || defined(JOEWIN)
	varm(s);
	msgnw(bw->parent, joe_gettext(_("Sorry, no sub-processes in DOS (yet)")));
	return -1;
#else
	MPX *m;

	if (bw->b->pid) {
		msgnw(bw->parent, joe_gettext(_("Program already running in this window")));
		varm(s);
		return -1;
	}
	/* p_goto_eof(bw->cursor); */

	if (!(m = mpxmk(&bw->b->out, name, s, cdata, bw->b, build ? cdone_parse : cdone, bw->b, out_only))) {
		varm(s);
		msgnw(bw->parent, joe_gettext(_("No ptys available")));
		return -1;
	} else {
		bw->b->pid = m->pid;
	}
	return 0;
#endif
}

int ubknd(BW *bw)
{
	unsigned char **a;
	unsigned char *s;
        unsigned char *sh;

        if (!modify_logic(bw,bw->b))
        	return -1;

        sh=(unsigned char *)getenv("SHELL");

        if (file_exists(sh) && zcmp(sh,USTR "/bin/sh")) goto ok;
        if (file_exists(sh=USTR "/bin/bash")) goto ok;
        if (file_exists(sh=USTR "/usr/bin/bash")) goto ok;
        if (file_exists(sh=USTR "/bin/sh")) goto ok;

        msgnw(bw->parent, joe_gettext(_("\"SHELL\" environment variable not defined or exported")));
        return -1;

        ok:
	a = vamk(3);
	vaperm(a);
	s = vsncpy(NULL, 0, sz(sh));
	a = vaadd(a, s);
	s = vsncpy(NULL, 0, sc("-i"));
	a = vaadd(a, s);
	return cstart(bw, sh, a, NULL, 0, 0);
}

/* Run a program in a window */

B *runhist = NULL;

int urun(BW *bw)
{
	unsigned char *s = ask(bw->parent, joe_gettext(_("Program to run: ")), &runhist, USTR "Run",
	                       utypebw, locale_map, 0, 0, NULL);

	if (s) {
		unsigned char **a;
		unsigned char *cmd;

		if (!modify_logic(bw,bw->b))
			return -1;

		a = vamk(10);
		cmd = vsncpy(NULL, 0, sc("/bin/sh"));

		a = vaadd(a, cmd);
		cmd = vsncpy(NULL, 0, sc("-c"));
		a = vaadd(a, cmd);
		a = vaadd(a, s);
		return cstart(bw, USTR "/bin/sh", a, NULL, 0, 0);
	} else {
		return -1;
	}
}

B *buildhist = NULL;

int ubuild(BW *bw)
{
	unsigned char *s;
	int prev = 0;
	if (buildhist) {
		s = joe_gettext(_("Build command: "));
		prev = 1;
	} else {
		s = joe_gettext(_("Enter build command (for example, 'make'): "));
	}
	/* "file prompt" was set for this... */
	s = ask(bw->parent, s, &buildhist, USTR "Run", utypebw, locale_map, 0, prev, NULL);
	if (s) {
		unsigned char **a = vamk(10);
		unsigned char *cmd = vsncpy(NULL, 0, sc("/bin/sh"));

		a = vaadd(a, cmd);
		cmd = vsncpy(NULL, 0, sc("-c"));
		a = vaadd(a, cmd);
		a = vaadd(a, s);
		return cstart(bw, USTR "/bin/sh", a, NULL, 1, 0);
	} else {
		return -1;
	}
}

B *grephist = NULL;

int ugrep(BW *bw)
{
	unsigned char *s;
	int prev = 0;
	if (grephist) {
		s = joe_gettext(_("Grep command: "));
		prev = 1;
	} else {
		s = joe_gettext(_("Enter grep command (for example, 'grep -n foo *.c'): "));
	}
	/* "file prompt" was set for this... */
	s = ask(bw->parent, s, &grephist, USTR "Run", utypebw, locale_map, 0, prev, NULL);
	if (s) {
		unsigned char **a = vamk(10);
		unsigned char *cmd = vsncpy(NULL, 0, sc("/bin/sh"));

		a = vaadd(a, cmd);
		cmd = vsncpy(NULL, 0, sc("-c"));
		a = vaadd(a, cmd);
		a = vaadd(a, s);
		return cstart(bw, USTR "/bin/sh", a, NULL, 1, 0);
	} else {
		return -1;
	}
}

/* Kill program */

int ukillpid(BW *bw)
{
#ifdef JOEWIN
	return 0;
#else
	if (bw->b->pid) {
		int c = query(bw->parent, sz(joe_gettext(_("Kill program (y,n,^C)?"))), 0);
		if (bw->b->pid && (c == YES_CODE || yncheck(yes_key, c)))
			kill(bw->b->pid, 1);
		return -1;
	} else {
		return 0;
	}
#endif
}
