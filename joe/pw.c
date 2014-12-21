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
unsigned char *current_dir;

void set_current_dir(unsigned char *s,int simp)
{
	if (s[0]=='!' || (s[0]=='>' && s[1]=='>'))
		return;
	obj_free(current_dir);
	if (s) {
		current_dir=dirprt(s);
		if (simp) {
			current_dir = simplify_prefix(current_dir);
		}
	}
	else
		current_dir = 0;
	obj_perm(current_dir);
}

static void disppw(BW *bw, int flg)
{
	unsigned char *bf;
	int i;
	W *w = bw->parent;
	PW *pw = (PW *) bw->object;

	if (!flg) {
		return;
	}

	/* Try a nice graphics separator */
	bf = joe_malloc(w->w + 1);
	for (i = 0; i != w->w; ++i)
		bf[i] = '-';
	bf[i] = 0;
	genfmt(w->t->t, w->x, w->y, 0, bf, bg_stalin, 0);
	joe_free(bf);

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
			pw->promptofst = pw->promptlen - (w->w - piscol(bw->cursor) - 1);
			bw->offset = piscol(bw->cursor) - (w->w - (pw->promptlen - pw->promptofst) - 1);
		}
	}

	/* Set cursor position */
	w->curx = piscol(bw->cursor) - bw->offset + pw->promptlen - pw->promptofst;
	w->cury = bw->cursor->line - bw->top->line + 1;
	/* w->cury = w->h - 1; */

	/* Generate prompt */
	w->t->t->updtab[w->y + w->cury] = 1;
	if (w->cury != pw->oldcury) {
		int n;
		for (n = 0; n != w->h; ++n)
			w->t->t->updtab[w->y + n] = 1;
		pw->oldcury = w->cury;
	}
	/* w->t->t->updtab[w->y + w->h - 1] = 1;
	genfmt(w->t->t, w->x, w->y + w->h - 1, pw->promptofst, pw->prompt, BG_COLOR(bg_prompt), 0); */

	/* Position and size buffer */
	/* bwmove(bw, w->x + pw->promptlen - pw->promptofst, w->y);
	bwresz(bw, w->w - (pw->promptlen - pw->promptofst), w->h); */
	bwmove(bw, w->x, w->y + 1);
	bwresz(bw, w->w, w->h - 1);

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

void append_history(B *hist,unsigned char *s,int len)
{
	P *q = pdup(hist->eof, USTR "append_history");
	binsm(q, s, len);
	p_goto_eof(q);
	binsc(q, '\n');
	prm(q);
}

/* Promote line to end of history buffer */

void promote_history(B *hist, long line)
{
	P *q = pdup(hist->bof, USTR "promote_history");
	P *r;
	P *t;

	pline(q, line);
	r = pdup(q, USTR "promote_history");
	pnextl(r);
	t = pdup(hist->eof, USTR "promote_history");
	binsb(t, bcpy(q, r));
	bdel(q, r);
	prm(q);
	prm(r);
	prm(t);
}

/* When user hits return in a prompt window */

static int rtnpw(BW *bw)
{
	W *w = bw->parent;
	PW *pw = (PW *) bw->object;
	unsigned char *s;
	W *win;
	int (*pfunc) ();
	void *object;
	long byte;

	/* Extract entered text from buffer */
	p_goto_eol(bw->cursor);
	byte = bw->cursor->byte;
	p_goto_bol(bw->cursor);
	s = brvs(NULL, bw->cursor, (int) (byte - bw->cursor->byte));

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
		set_current_dir(s,1);
	}

	if (pw->file_prompt) {
		s = canonical(s);
	}

	win = w->win;
	pfunc = pw->pfunc;
	object = pw->object;
	bwrm(bw);
	joe_free(pw->prompt);
	joe_free(pw);
	w->object = NULL;
	wabort(w);
	dostaupd = 1;

	/* Call callback function */
	if (pfunc) {
		return pfunc(win->object, s, object);
	} else {
		return -1;
	}
}

int ucmplt(BW *bw, int k)
{
	PW *pw = (PW *) bw->object;

	if (pw->tab) {
		return pw->tab(bw, k);
	} else {
		return -1;
	}
}

static void inspw(BW *bw, B *b, long l, long n, int flg)
{
	if (b == bw->b) {
		bwins(bw, l, n, flg);
	}
}

static void delpw(BW *bw, B *b, long l, long n, int flg)
{
	if (b == bw->b) {
		bwdel(bw, l, n, flg);
	}
}

static int abortpw(BW *b)
{
	PW *pw = b->object;
	void *object = pw->object;
	int (*abrt) () = pw->abrt;

	W *win = b->parent->win;

	bwrm(b);
	joe_free(pw->prompt);
	joe_free(pw);
	if (abrt) {
		return abrt(win->object, object);
	} else {
		return -1;
	}
}

WATOM watompw = {
	USTR "prompt",
	disppw,
	bwfllwt,
	abortpw,
	rtnpw,
	utypebw,
	NULL,
	NULL,
	inspw,
	delpw,
	TYPEPW
};

/* Create a prompt window */

BW *wmkpw(W *w, unsigned char *prompt, B **history, int (*func) (), unsigned char *huh, int (*abrt) (), int (*tab) (), void *object, struct charmap *map,int file_prompt)
{
	W *new;
	PW *pw;
	BW *bw;

	new = wcreate(w->t, &watompw, w, w, w->main, 2, huh);
	if (!new) {
		return NULL;
	}
	new->fixed = 0;
	wfit(new->t);
	new->object = (void *) (bw = bwmk(new, bmk(NULL), 0, prompt));
	bw->b->o.charmap = map;
	bw->object = (void *) (pw = (PW *) joe_malloc(sizeof(PW)));
	pw->abrt = abrt;
	pw->oldcury = -1;
	pw->tab = tab;
	pw->object = object;
	pw->prompt = zdup(prompt);
	pw->promptlen = fmtlen(prompt);
	pw->promptofst = 0;
	pw->pfunc = func;
	pw->file_prompt = file_prompt;
	if (history) {
		setup_history(history);
		pw->hist = *history;
		binsb(bw->cursor, bcpy(pw->hist->bof, pw->hist->eof));
		bw->b->changed = 0;
		p_goto_eof(bw->cursor);
		/* p_goto_eof(bw->top);
		p_goto_bol(bw->top); */
	} else {
		pw->hist = NULL;
	}
	/* Install current directory */
	if ((file_prompt&4) && !nocurdir) {
		binsm (bw->cursor, sv(current_dir));
		p_goto_eof(bw->cursor);
		bw->cursor->xcol = piscol(bw->cursor);
	}
	w->t->curwin = new;
	return bw;
}

/* Tab completion functions */

unsigned char **regsub(unsigned char **z, int len, unsigned char *s)
{
	unsigned char **lst = NULL;
	int x;

	for (x = 0; x != len; ++x)
		if (rmatch(s, z[x], 0))
			lst = vaadd(lst, vsncpy(NULL, 0, sz(z[x])));
	return lst;
}

void cmplt_ins(BW *bw, unsigned char *line)
{
	P *p = pdup(bw->cursor, USTR "cmplt_ins");

	p_goto_bol(p);
	p_goto_eol(bw->cursor);
	bdel(p, bw->cursor);
	binsm(bw->cursor, sv(line));
	p_goto_eol(bw->cursor);
	prm(p);
	bw->cursor->xcol = piscol(bw->cursor);
}

int cmplt_abrt(BW *bw, int x, unsigned char *line)
{
	if (line) {
		/* cmplt_ins(bw, line); */
		obj_free(line);
	}
	return -1;
}

int cmplt_rtn(MENU *m, int x, unsigned char *line)
{
	cmplt_ins(m->parent->win->object, m->list[x]);
	obj_free(line);
	m->object = NULL;
	wabort(m->parent);
	return 0;
}

int simple_cmplt(BW *bw,unsigned char **list)
{
	MENU *m;
	P *p, *q;
	unsigned char *line;
	unsigned char *line1;
	unsigned char **lst;

	p = pdup(bw->cursor, USTR "simple_cmplt");
	p_goto_bol(p);
	q = pdup(bw->cursor, USTR "simple_cmplt");
	p_goto_eol(q);
	line = brvs(NULL, p, (int) (q->byte - p->byte));	/* Assumes short lines :-) */
	prm(p);
	prm(q);

	line1 = vsncpy(NULL, 0, sv(line));
	line1 = vsadd(line1, '*');
	lst = regsub(av(list), line1);

	if (!lst) {
		ttputc(7);
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

	obj_perm(line);
	vaperm(lst);
	m = mkmenu((menu_above ? bw->parent->link.prev : bw->parent), bw->parent, lst, cmplt_rtn, cmplt_abrt, NULL, 0, line);
	if (!m) {
		return -1;
	}
	if (valen(lst) == 1)
		return cmplt_rtn(m, 0, line);
	else if (smode || isreg(line)) {
		if (!menu_jump)
			bw->parent->t->curwin=bw->parent;
		return 0;
	} else {
		unsigned char *com = mcomplete(m);

		obj_free(m->object);
		m->object = com;
		obj_perm(com);
		
		cmplt_ins(bw, com);
		wabort(m->parent);
		smode = 2;
		ttputc(7);
		return 0;
	}
}

/* Simplified prompting... convert original event-driven style to
 * coroutine model */

struct prompt_result {
	Coroutine t;
	unsigned char *answer;
};

int prompt_cont(BW *bw, unsigned char *s, void *object)
{
	struct prompt_result *r = (struct prompt_result *)object;
	r->answer = s;

	/* move answer to original coroutine's obj_stack */
	obj_perm(r->answer);

	co_resume(&r->t, 0);

	return 0;
}

int prompt_abrt(BW *bw, void *object)
{
	struct prompt_result *r = (struct prompt_result *)object;
	r->answer = 0;
	co_resume(&r->t, -1);
	return -1;
}

unsigned char *ask(W *w,			/* Prompt goes below this window */
                   unsigned char *prompt,	/* Prompt text */
                   B **history,			/* History buffer to use */
                   unsigned char *huh,		/* Name of help screen for this prompt */
                   int (*tab)(),		/* Called when tab key is pressed */
                   struct charmap *map,		/* Character map for prompt */
                   int file_prompt,		/* Set for file-name tilde expansion */
                   int retrieve_last,		/* Set for cursor to go on last line of history */
                   unsigned char *preload)	/* Text to preload into prompt */
{
	struct prompt_result t;
	BW *bw = wmkpw(w, prompt, history, prompt_cont, huh, prompt_abrt, tab, 
	               &t, map, file_prompt);
	if (!bw)
		return 0;

	bw->parent->coro = &t.t;
	if (preload) {
		/* Load hint, put cursor after it */
		binss(bw->cursor, preload);
		pset(bw->cursor, bw->b->eof);
		bw->cursor->xcol = piscol(bw->cursor);
	} else if (retrieve_last) {
		/* One step back through history */
		uuparw(bw);
		u_goto_eol(bw);
		bw->cursor->xcol = piscol(bw->cursor);
	}

	/* We get woken up when user hits return */
	if (!co_yield(&t.t, 0)) {
		/* Moving answer to original coroutine's stack */
		obj_temp(t.answer);
		return t.answer;
	} else {
		return 0;
	}
}
