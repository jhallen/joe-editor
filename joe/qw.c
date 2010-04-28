/*
 *	Query windows
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* Return width of a string */

int joe_wcswidth(struct charmap *map,unsigned char *s, int len)
{
	if (!map->type) {
		return len;
	} else {
		int width = 0;
		while (len) {
			int c = utf8_decode_fwrd(&s, &len);
			if (c >= 0) {
				width += joe_wcwidth(1, c);
			} else
				++width;
		}
		return width;
	}
}

/* Calculate number of lines needed for a given prompt string and a given window width.
   Also this finds the nth line and returns the position of the substring which is
   that line. Set n to -1 if you just want the height. */

int break_height(struct charmap *map,unsigned char **src,int *src_len,int wid,int n)
{
	unsigned char *s = *src;
	int len = *src_len;
	int h = 1; /* Number of lines */
	int col = 0; /* Current column */
	int x = 0; /* Offset into string */
	int start_of_line = 0; /* Start of most recent line */
	while (x != len) {
		int space = 0;
		int word = 0;
		int start = x;
		int start_word;
		while (x != len && s[x] == ' ') {
			++space;
			++x;
		}
		start_word = x;
		while (x != len && s[x] != ' ') {
			++x;
		}
		word = joe_wcswidth(map, s + start_word, x - start_word);
		if (col + space + word < wid || !col) {
			/* Leading space and word fit on current line */
			col += space + word;
		} else {
			/* They don't fit, start a new line */
			if (!n--) {
				x = start;
				break;
			}
			++h;
			col = word;
			start_of_line = start_word;
		}
	}
	*src = s + start_of_line;
	*src_len = x - start_of_line;
	return h;
}

static void dispqw(QW *qw)
{
	int y;
	W *w = qw->parent;

	/* Generate prompt */
	for (y = 0; y != w->h; ++y) {
		unsigned char *s = qw->prompt;
		int l = qw->promptlen;
		break_height(locale_map, &s, &l, qw->org_w, y);
		w->t->t->updtab[w->y + y] = 1;
		genfield(w->t->t,
		         w->t->t->scrn + (w->y + y) * w->t->t->co + w->x,
		         w->t->t->attr + (w->y + y) * w->t->t->co + w->x,
		         w->x,
		         w->y + y,
		         0,
		         s,
		         l,
		         BG_COLOR(bg_prompt),
		         w->w - w->x,
		         1,NULL);
		w->cury = y;
		w->curx = w->x + joe_wcswidth(locale_map, s, l);
	}
}

static void dispqwn(QW *qw)
{
	int y;
	W *w = qw->parent;

	/* Set cursor position */
	if (w->win->watom->follow && w->win->object)
		w->win->watom->follow(w->win->object);
	if (w->win->watom->disp && w->win->object)
		w->win->watom->disp(w->win->object, 1);
	w->curx = w->win->curx;
	w->cury = w->win->cury + w->win->y - w->y;

	/* Generate prompt */
	for (y = 0; y != w->h; ++y) {
		unsigned char *s = qw->prompt;
		int l = qw->promptlen;
		break_height(locale_map, &s, &l, qw->org_w, y);
		w->t->t->updtab[w->y + y] = 1;
		genfield(w->t->t,
		         w->t->t->scrn + (w->y + y) * w->t->t->co + w->x,
		         w->t->t->attr + (w->y + y) * w->t->t->co + w->x,
		         w->x,
		         w->y + y,
		         0,
		         s,
		         l,
		         BG_COLOR(bg_prompt),
		         w->w - w->x,
		         1,NULL);
	}
}

/* When user hits a key in a query window */

struct utf8_sm qw_sm;

static int utypeqw(QW *qw, int c)
{
	W *win;
	W *w = qw->parent;
	int *notify = w->notify;
	int (*func) ();
	void *object = qw->object;

	if (locale_map->type) {
		c = utf8_decode(&qw_sm, c);
		if (c<0)
			return 0;
	}

	win = qw->parent->win;
	func = qw->func;
	vsrm(qw->prompt);
	joe_free(qw);
	w->object = NULL;
	w->notify = NULL;
	wabort(w);
	if (func)
		return func(win->object, c, object, notify);
	return -1;
}

static int abortqw(QW *qw)
{
	W *win = qw->parent->win;
	void *object = qw->object;
	int (*abrt) () = qw->abrt;

	vsrm(qw->prompt);
	joe_free(qw);
	if (abrt)
		return abrt(win->object, object);
	else
		return -1;
}

static WATOM watomqw = {
	USTR "query",
	dispqw,
	NULL,
	abortqw,
	NULL,
	utypeqw,
	NULL,
	NULL,
	NULL,
	NULL,
	TYPEQW
};

static WATOM watqwn = {
	USTR "querya",
	dispqwn,
	NULL,
	abortqw,
	NULL,
	utypeqw,
	NULL,
	NULL,
	NULL,
	NULL,
	TYPEQW
};

static WATOM watqwsr = {
	USTR "querysr",
	dispqwn,
	NULL,
	abortqw,
	NULL,
	utypeqw,
	NULL,
	NULL,
	NULL,
	NULL,
	TYPEQW
};

/* Create a query window */

QW *mkqw(W *w, unsigned char *prompt, int len, int (*func) (/* ??? */), int (*abrt) (/* ??? */), void *object, int *notify)
{
	W *new;
	QW *qw;
	unsigned char *s = prompt;
	int l = len;
	int h = break_height(locale_map, &s, &l, w->w, -1);

	new = wcreate(w->t, &watomqw, w, w, w->main, h, NULL, notify);
	if (!new) {
		if (notify)
			*notify = 1;
		return NULL;
	}
	wfit(new->t);
	new->object = (void *) (qw = (QW *) joe_malloc(sizeof(QW)));
	qw->parent = new;
	qw->prompt = vsncpy(NULL, 0, prompt, len);
	qw->promptlen = len;
	qw->org_w = w->w;
	qw->org_h = h;
	qw->func = func;
	qw->abrt = abrt;
	qw->object = object;
	w->t->curwin = new;
	return qw;
}

/* Same as above, but cursor is left in original window */
/* For Ctrl-Meta thing */

QW *mkqwna(W *w, unsigned char *prompt, int len, int (*func) (/* ??? */), int (*abrt) (/* ??? */), void *object, int *notify)
{
	W *new;
	QW *qw;
	unsigned char *s = prompt;
	int l = len;
	int h = break_height(locale_map, &s, &l, w->w, -1);

	new = wcreate(w->t, &watqwn, w, w, w->main, h, NULL, notify);
	if (!new) {
		if (notify)
			*notify = 1;
		return NULL;
	}
	wfit(new->t);
	new->object = (void *) (qw = (QW *) joe_malloc(sizeof(QW)));
	qw->parent = new;
	qw->prompt = vsncpy(NULL, 0, prompt, len);
	qw->promptlen = len;
	qw->org_w = w->w;
	qw->org_h = h;
	qw->func = func;
	qw->abrt = abrt;
	qw->object = object;
	w->t->curwin = new;
	return qw;
}

/* Same as above, but cursor is left in original window */
/* For search and replace thing */

QW *mkqwnsr(W *w, unsigned char *prompt, int len, int (*func) (/* ??? */), int (*abrt) (/* ??? */), void *object, int *notify)
{
	W *new;
	QW *qw;
	unsigned char *s = prompt;
	int l = len;
	int h = break_height(locale_map, &s, &l, w->w, -1);

	new = wcreate(w->t, &watqwsr, w, w, w->main, h, NULL, notify);
	if (!new) {
		if (notify)
			*notify = 1;
		return NULL;
	}
	wfit(new->t);
	new->object = (void *) (qw = (QW *) joe_malloc(sizeof(QW)));
	qw->parent = new;
	qw->prompt = vsncpy(NULL, 0, prompt, len);
	qw->promptlen = len;
	qw->org_w = w->w;
	qw->org_h = h;
	qw->func = func;
	qw->abrt = abrt;
	qw->object = object;
	w->t->curwin = new;
	return qw;
}
