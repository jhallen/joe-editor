/*
 *	Prompt windows
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* The current directory */

int bg_prompt;
int nocurdir;

char *get_cd(W *w)
{
	BW *bw;
	w = w->main;
	bw = (BW *)w->object;
	return bw->b->current_dir;
}

void set_current_dir(BW *bw, char *s,int simp)
{
	W *w = bw->parent->main;
	B *b;
	bw = (BW *)w->object;
	b = bw->b;
	
	if (s[0]=='!' || (s[0]=='>' && s[1]=='>'))
		return;
	vsrm(b->current_dir);
	if (s) {
		b->current_dir=dirprt(s);
		if (simp) {
			char *tmp = simplify_prefix(b->current_dir);
			vsrm(b->current_dir);
			b->current_dir = tmp;
		}
	}
	else
		b->current_dir = 0;
}

static void disppw(W *w, int flg)
{
	BW *bw = (BW *)w->object;
	PW *pw = (PW *) bw->object;

	if (!flg) {
		return;
	}

	/* Scroll buffer and position prompt */
	if (pw->promptlen > w->w - 5) {
		pw->promptofst = pw->promptlen - w->w / 2;
		if (piscol(bw->cursor) < w->w - (pw->promptlen - pw->promptofst)) {
			bw->offset = 0;
		} else {
			bw->offset = piscol(bw->cursor) - (w->w - (pw->promptlen - pw->promptofst) - 1);
		}
	} else {
		if (piscol(bw->cursor) < w->w - pw->promptlen) {
			pw->promptofst = 0;
			bw->offset = 0;
		} else if (piscol(bw->cursor) >= w->w) {
			pw->promptofst = pw->promptlen;
			bw->offset = piscol(bw->cursor) - (w->w - 1);
		} else {
			pw->promptofst = pw->promptlen - TO_DIFF_OK((w->w - piscol(bw->cursor) - 1));
			bw->offset = piscol(bw->cursor) - (w->w - (pw->promptlen - pw->promptofst) - 1);
		}
	}

	/* Set cursor position */
	w->curx = TO_DIFF_OK(piscol(bw->cursor) - bw->offset + pw->promptlen - pw->promptofst);
	w->cury = 0;

	/* Generate prompt */
	w->t->t->updtab[w->y] = 1;
	genfmt(w->t->t, w->x, w->y, pw->promptofst, pw->prompt, BG_COLOR(bg_prompt), 0);

	/* Position and size buffer */
	bwmove(bw, w->x + pw->promptlen - pw->promptofst, w->y);
	bwresz(bw, w->w - (pw->promptlen - pw->promptofst), 1);

	/* Generate buffer */
	bwgen(bw, 0);
}

/* History functions */

void setup_history(B **history)
{
	if (!*history) {
		*history = bmk(NULL);
	}
}

/* Add line to history buffer */

void append_history(B *hist,char *s,ptrdiff_t len)
{
	P *q = pdup(hist->eof, "append_history");
	binsm(q, s, len);
	p_goto_eof(q);
	binsc(q, '\n');
	prm(q);
}

/* Promote line to end of history buffer */

void promote_history(B *hist, off_t line)
{
	P *q = pdup(hist->bof, "promote_history");
	P *r;
	P *t;

	pline(q, line);
	r = pdup(q, "promote_history");
	pnextl(r);
	t = pdup(hist->eof, "promote_history");
	binsb(t, bcpy(q, r));
	bdel(q, r);
	prm(q);
	prm(r);
	prm(t);
}

/* When user hits return in a prompt window */

static int rtnpw(W *w)
{
	BW *bw = (BW *)w->object;
	PW *pw = (PW *)bw->object;
	char *s;
	W *win;
	int *notify;
	int (*pfunc)(W *w, char *s, void *object, int *notify);
	void *object;
	off_t byte;

	/* Extract entered text from buffer */
	p_goto_eol(bw->cursor);
	byte = bw->cursor->byte;
	p_goto_bol(bw->cursor);
	s = brvs(bw->cursor, TO_DIFF_OK(byte - bw->cursor->byte));

	if (pw->file_prompt) {
		s = canonical(s);
	}

	/* Save text into history buffer */
	if (pw->hist) {
		if (bw->b->changed) {
			append_history(pw->hist, sv(s));
		} else {
			promote_history(pw->hist, bw->cursor->line);
		}
	}

	/* Do ~ expansion and set new current directory */
	if (pw->file_prompt&2) {
		set_current_dir(bw, s,1);
	}

	win = w->win;
	pfunc = pw->pfunc;
	object = pw->object;
	bwrm(bw);
	joe_free(pw->prompt);
	joe_free(pw);
	w->object = NULL;
	notify = w->notify;
	w->notify = 0;
	wabort(w);
	dostaupd = 1;

	/* Call callback function */
	if (pfunc) {
		return pfunc(win, s, object, notify);
	} else {
		return -1;
	}
}

int ucmplt(W *w, int k)
{
	BW *bw;
	PW *pw;
	WIND_BW(bw, w);
	pw = (PW *) bw->object;

	if (pw->tab) {
		return pw->tab(bw, k);
	} else {
		return -1;
	}
}

static void inspw(W *w, B *b, off_t l, off_t n, int flg)
{
	BW *bw = (BW *)w->object;
	if (b == bw->b) {
		bwins(bw, l, n, flg);
	}
}

static void delpw(W *w, B *b, off_t l, off_t n, int flg)
{
	BW *bw = (BW *)w->object;
	if (b == bw->b) {
		bwdel(bw, l, n, flg);
	}
}

static int abortpw(W *w)
{
	BW *bw = (BW *)w->object;
	PW *pw = (PW *)bw->object;
	void *object = pw->object;
	int (*abrt)(W *w, void *object) = pw->abrt;
	W *win = bw->parent->win;

	bwrm(bw);
	joe_free(pw->prompt);
	joe_free(pw);
	if (abrt) {
		return abrt(win, object);
	} else {
		return -1;
	}
}

WATOM watompw = {
	"prompt",
	disppw,
	bwfllwt,
	abortpw,
	rtnpw,
	utypew,
	NULL,
	NULL,
	inspw,
	delpw,
	TYPEPW
};

/* Create a prompt window */

BW *wmkpw(W *w, const char *prompt, B **history, int (*func) (W *w, char *s, void *object, int *notify), const char *huh,
          int (*abrt)(W *w, void *object),
          int (*tab)(BW *bw, int k), void *object, int *notify,struct charmap *map,int file_prompt)
{
	W *neww;
	PW *pw;
	BW *bw;

	neww = wcreate(w->t, &watompw, w, w, w->main, 1, huh, notify);
	if (!neww) {
		if (notify) {
			*notify = 1;
		}
		return NULL;
	}
	wfit(neww->t);
	neww->object = (void *) (bw = bwmk(neww, bmk(NULL), 1));
	bw->b->o.charmap = map;
	bw->object = (void *) (pw = (PW *) joe_malloc(SIZEOF(PW)));
	pw->abrt = abrt;
	pw->tab = tab;
	pw->object = object;
	pw->prompt = zdup(prompt);
	pw->promptlen = fmtlen(prompt);
	pw->promptofst = 0;
	pw->pfunc = func;
	pw->file_prompt = file_prompt;
	if (pw->file_prompt) {
		bw->b->o.syntax = load_syntax("filename");
		bw->b->o.highlight = 1;
		bw->o.syntax = bw->b->o.syntax;
		bw->o.highlight = bw->b->o.highlight;
	}
	if (history) {
		setup_history(history);
		pw->hist = *history;
		binsb(bw->cursor, bcpy(pw->hist->bof, pw->hist->eof));
		bw->b->changed = 0;
		p_goto_eof(bw->cursor);
		p_goto_eof(bw->top);
		p_goto_bol(bw->top);
	} else {
		pw->hist = NULL;
	}
	/* Install current directory */
	if ((file_prompt&4) && !nocurdir) {
		char *curd = get_cd(w);
		binsm (bw->cursor, sv(curd));
		p_goto_eof(bw->cursor);
		bw->cursor->xcol = piscol(bw->cursor);
	}
	w->t->curwin = neww;
	return bw;
}

/* Tab completion functions */

char **regsub(char **z, ptrdiff_t len, char *s)
{
	char **lst = NULL;
	int x;

	for (x = 0; x != len; ++x)
		if (rmatch(s, z[x]))
			lst = vaadd(lst, vsncpy(NULL, 0, sz(z[x])));
	return lst;
}

void cmplt_ins(BW *bw, char *line)
{
	P *p = pdup(bw->cursor, "cmplt_ins");

	p_goto_bol(p);
	p_goto_eol(bw->cursor);
	bdel(p, bw->cursor);
	binsm(bw->cursor, sv(line));
	p_goto_eol(bw->cursor);
	prm(p);
	bw->cursor->xcol = piscol(bw->cursor);
}

int cmplt_abrt(W *w, ptrdiff_t x, void *object)
{
	char *line = (char *)object;
	if (line) {
		/* cmplt_ins(bw, line); */
		vsrm(line);
	}
	return -1;
}

int cmplt_rtn(MENU *m, ptrdiff_t x, void *object, int k)
{
	char *line = (char *)object;
	cmplt_ins((BW *)m->parent->win->object, m->list[x]);
	vsrm(line);
	m->object = NULL;
	wabort(m->parent);
	return 0;
}

int simple_cmplt(BW *bw,char **list)
{
	MENU *m;
	P *p, *q;
	char *line;
	char *line1;
	char **lst;

	p = pdup(bw->cursor, "simple_cmplt");
	p_goto_bol(p);
	q = pdup(bw->cursor, "simple_cmplt");
	p_goto_eol(q);
	line = brvs(p, (int) (q->byte - p->byte));	/* Assumes short lines :-) */
	prm(p);
	prm(q);

	line1 = vsncpy(NULL, 0, sv(line));
	line1 = vsadd(line1, '*');
	lst = regsub(list, aLEN(list), line1);
	vsrm(line1);

	if (!lst) {
		ttputc(7);
		vsrm(line);
		return -1;
	}

	if (menu_above) {
		if (bw->parent->link.prev->watom==&watommenu) {
			wabort(bw->parent->link.prev);
		}
	} else {
		if (bw->parent->link.next->watom==&watommenu) {
			wabort(bw->parent->link.next);
		}
	}

	m = mkmenu((menu_above ? bw->parent->link.prev : bw->parent), bw->parent, lst, cmplt_rtn, cmplt_abrt, NULL, 0, line, NULL);
	if (!m) {
		varm(lst);
		vsrm(line);
		return -1;
	}
	if (aLEN(lst) == 1)
		return cmplt_rtn(m, 0, line, 0);
	else if (smode || isreg(line)) {
		if (!menu_jump)
			bw->parent->t->curwin=bw->parent;
		return 0;
	} else {
		char *com = mcomplete(m);

		vsrm((char *)m->object);
		m->object = com;
		
		cmplt_ins(bw, com);
		wabort(m->parent);
		smode = 2;
		ttputc(7);
		return 0;
	}
}
