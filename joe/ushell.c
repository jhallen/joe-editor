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

int cstart(BW *bw, unsigned char *name, unsigned char **s, void *obj, int *notify, int build, int out_only)
{
#ifdef __MSDOS__
	if (notify) {
		*notify = 1;
	}
	varm(s);
	msgnw(bw->parent, joe_gettext(_("Sorry, no sub-processes in DOS (yet)")));
	return -1;
#else
	MPX *m;

	if (notify) {
		*notify = 1;
	}
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
	s = vsncpy(NULL, 0, sz(sh));
	a = vaadd(a, s);
	s = vsncpy(NULL, 0, sc("-i"));
	a = vaadd(a, s);
	return cstart(bw, sh, a, NULL, NULL, 0, 0);
}

/* Run a program in a window */

static int dorun(BW *bw, unsigned char *s, void *object, int *notify)
{
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
	return cstart(bw, USTR "/bin/sh", a, NULL, notify, 0, 0);
}

B *runhist = NULL;

int urun(BW *bw)
{
	if (wmkpw(bw->parent, joe_gettext(_("Program to run: ")), &runhist, dorun, USTR "Run", NULL, NULL, NULL, NULL, locale_map, 1)) {
		return 0;
	} else {
		return -1;
	}
}

static int dobuild(BW *bw, unsigned char *s, void *object, int *notify)
{
	unsigned char **a = vamk(10);
	unsigned char *cmd = vsncpy(NULL, 0, sc("/bin/sh"));

	a = vaadd(a, cmd);
	cmd = vsncpy(NULL, 0, sc("-c"));
	a = vaadd(a, cmd);
	a = vaadd(a, s);
	return cstart(bw, USTR "/bin/sh", a, NULL, notify, 1, 0);
}

B *buildhist = NULL;

int ubuild(BW *bw)
{
	if (buildhist) {
		if ((bw=wmkpw(bw->parent, joe_gettext(_("Build command: ")), &buildhist, dobuild, USTR "Run", NULL, NULL, NULL, NULL, locale_map, 1))) {
			uuparw(bw);
			u_goto_eol(bw);
			bw->cursor->xcol = piscol(bw->cursor);
			return 0;
		} else {
		return -1;
		}
	} else {
		if (wmkpw(bw->parent, joe_gettext(_("Enter build command (for example, 'make'): ")), &buildhist, dobuild, USTR "Run", NULL, NULL, NULL, NULL, locale_map, 1)) {
			return 0;
		} else {
		return -1;
		}
	}
}

B *grephist = NULL;

int ugrep(BW *bw)
{
	/* Set parser to grep */
	bw->b->parseone = parseone_grep;
	if (grephist) {
		if ((bw=wmkpw(bw->parent, joe_gettext(_("Grep command: ")), &grephist, dobuild, USTR "Run", NULL, NULL, NULL, NULL, locale_map, 1))) {
			uuparw(bw);
			u_goto_eol(bw);
			bw->cursor->xcol = piscol(bw->cursor);
			return 0;
		} else {
		return -1;
		}
	} else {
		if (wmkpw(bw->parent, joe_gettext(_("Enter grep command (for example, 'grep -n foo *.c'): ")), &grephist, dobuild, USTR "Run", NULL, NULL, NULL, NULL, locale_map, 1)) {
			return 0;
		} else {
		return -1;
		}
	}
}

/* Kill program */

static int pidabort(BW *bw, int c, void *object, int *notify)
{
	if (notify) {
		*notify = 1;
	}
	if (c != YES_CODE && !yncheck(yes_key, c)) {
		return -1;
	}
	if (bw->b->pid) {
		kill(bw->b->pid, 1);
		return -1;
	} else {
		return -1;
	}
}

int ukillpid(BW *bw)
{
	if (bw->b->pid) {
		if (mkqw(bw->parent, sz(joe_gettext(_("Kill program (y,n,^C)?"))), pidabort, NULL, NULL, NULL)) {
			return 0;
		} else {
			return -1;
		}
	} else {
		return 0;
	}
}
