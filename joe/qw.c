/*
 *	Query windows
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* Return width of a string */

static ptrdiff_t joe_wcswidth(struct charmap *map,const char *s, ptrdiff_t len)
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

static ptrdiff_t break_height(struct charmap *map,const char **src,ptrdiff_t *src_len,ptrdiff_t wid,ptrdiff_t n)
{
	const char *s = *src;
	ptrdiff_t len = *src_len;
	ptrdiff_t h = 1; /* Number of lines */
	ptrdiff_t col = 0; /* Current column */
	ptrdiff_t x = 0; /* Offset into string */
	ptrdiff_t start_of_line = 0; /* Start of most recent line */
	while (x != len) {
		ptrdiff_t space = 0;
		ptrdiff_t word = 0;
		ptrdiff_t start = x;
		ptrdiff_t start_word;
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

static void dispqw(W *w, int flg)
{
	QW *qw = (QW *)w->object;
	ptrdiff_t y;

	/* Generate prompt */
	for (y = 0; y != w->h; ++y) {
		const char *s = qw->prompt;
		ptrdiff_t l = qw->promptlen;
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

static void dispqwn(W *w, int flg)
{
	QW *qw = (QW *)w->object;
	ptrdiff_t y;

	/* Set cursor position */
	if (w->win->watom->follow && w->win->object)
		w->win->watom->follow(w->win);
	if (w->win->watom->disp && w->win->object)
		w->win->watom->disp(w->win, 1);
	w->curx = w->win->curx;
	w->cury = w->win->cury + w->win->y - w->y;

	/* Generate prompt */
	for (y = 0; y != w->h; ++y) {
		const char *s = qw->prompt;
		ptrdiff_t l = qw->promptlen;
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

static int utypeqw(W *w, int c)
{
	QW *qw = (QW *)w->object;
	W *win;
	int *notify = w->notify;
	int (*func)(W *w, int k, void *object, int *notify);
	void *object = qw->object;

	win = qw->parent->win;
	func = qw->func;
	vsrm(qw->prompt);
	joe_free(qw);
	w->object = NULL;
	w->notify = NULL;
	wabort(w);
	if (func)
		return func(win, c, object, notify);
	return -1;
}

static int abortqw(W *w)
{
	QW *qw = (QW *)w->object;
	W *win = w->win;
	void *object = qw->object;
	int (*abrt)(W *w, void *object) = qw->abrt;

	vsrm(qw->prompt);
	joe_free(qw);
	if (abrt)
		return abrt(win, object);
	else
		return -1;
}

static WATOM watomqw = {
	"query",
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
	"querya",
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
	"querysr",
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

QW *mkqw(W *w, const char *prompt, ptrdiff_t len, int (*func) (W *w, int k, void *object, int *notify), int (*abrt)(W *w, void *object), void *object, int *notify)
{
	W *neww;
	QW *qw;
	const char *s = prompt;
	ptrdiff_t l = len;
	ptrdiff_t h = break_height(locale_map, &s, &l, w->w, -1);

	neww = wcreate(w->t, &watomqw, w, w, w->main, h, NULL, notify);
	if (!neww) {
		if (notify)
			*notify = 1;
		return NULL;
	}
	wfit(neww->t);
	neww->object = (void *) (qw = (QW *) joe_malloc(SIZEOF(QW)));
	qw->parent = neww;
	qw->prompt = vsncpy(NULL, 0, prompt, len);
	qw->promptlen = len;
	qw->org_w = w->w;
	qw->org_h = h;
	qw->func = func;
	qw->abrt = abrt;
	qw->object = object;
	w->t->curwin = neww;
	return qw;
}

/* Same as above, but cursor is left in original window */
/* For Ctrl-Meta thing */

QW *mkqwna(W *w, const char *prompt, ptrdiff_t len, int (*func) (W *w, int k, void *object, int *notify), int (*abrt)(W *w, void *object), void *object, int *notify)
{
	W *neww;
	QW *qw;
	const char *s = prompt;
	ptrdiff_t l = len;
	ptrdiff_t h = break_height(locale_map, &s, &l, w->w, -1);

	neww = wcreate(w->t, &watqwn, w, w, w->main, h, NULL, notify);
	if (!neww) {
		if (notify)
			*notify = 1;
		return NULL;
	}
	wfit(neww->t);
	neww->object = (void *) (qw = (QW *) joe_malloc(SIZEOF(QW)));
	qw->parent = neww;
	qw->prompt = vsncpy(NULL, 0, prompt, len);
	qw->promptlen = len;
	qw->org_w = w->w;
	qw->org_h = h;
	qw->func = func;
	qw->abrt = abrt;
	qw->object = object;
	w->t->curwin = neww;
	return qw;
}

/* Same as above, but cursor is left in original window */
/* For search and replace thing */

QW *mkqwnsr(W *w, const char *prompt, ptrdiff_t len, int (*func) (W *w, int k, void *object, int *notify), int (*abrt) (W *w, void *object), void *object, int *notify)
{
	W *neww;
	QW *qw;
	const char *s = prompt;
	ptrdiff_t l = len;
	ptrdiff_t h = break_height(locale_map, &s, &l, w->w, -1);

	neww = wcreate(w->t, &watqwsr, w, w, w->main, h, NULL, notify);
	if (!neww) {
		if (notify)
			*notify = 1;
		return NULL;
	}
	wfit(neww->t);
	neww->object = (void *) (qw = (QW *) joe_malloc(SIZEOF(QW)));
	qw->parent = neww;
	qw->prompt = vsncpy(NULL, 0, prompt, len);
	qw->promptlen = len;
	qw->org_w = w->w;
	qw->org_h = h;
	qw->func = func;
	qw->abrt = abrt;
	qw->object = object;
	w->t->curwin = neww;
	return qw;
}
