/*
 *	Window system
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* Count no. of main windows */

int countmain(Screen *t)
{
	int nmain = 1;
	W *m = t->curwin->main;
	W *q;

	for (q = t->curwin->link.next; q != t->curwin; q = q->link.next)
		if (q->main != m) {
			++nmain;
			m = q->main;
		}
	return nmain;
}

/* Redraw a window */

void wredraw(W *w)
{
	msetI(w->t->t->updtab + w->y, 1, w->h);
}

/* Find first window in a group */

W *findtopw(W *w)
{
	W *x;

	for (x = w; x->link.prev->main == w->main && x->link.prev != w; x = x->link.prev) ;
	return x;
}

/* Determine height of a window.  Returns reqh if it is set, otherwise
 * used fixed or hh scaled to the current screen size */

static int geth(W *w)
{
	if (w->reqh)
		return w->reqh;
	else if (w->fixed)
		return w->fixed;
	else
		return (((long) w->t->h - w->t->wind) * w->hh) / 1000;
}

/* Set the height of a window */

static void seth(W *w, int h)
{
	long tmp;

	w->reqh = h;
	tmp = 1000L * h;
	w->hh = tmp / (w->t->h - w->t->wind) + (tmp % (w->t->h - w->t->wind) ? 1 : 0);
}

/* Determine height of a family of windows.  Uses 'reqh' if it's set */

int getgrouph(W *w)
{
	W *x;
	int h;

	/* Find first window in family */
	x = findtopw(w);

	/* Add heights of all windows in family */
	for (w = x, h = geth(w); w->link.next != x && w->link.next->main == x->main; w = w->link.next, h += geth(w)) ;

	return h;
}

/* Determine minimum height of a family */

static int getminh(W *w)
{
	W *x;
	int h;

	x = findtopw(w);
	for (w = x, h = (w->fixed ? w->fixed : 2); w->link.next != x && w->link.next->main == x->main; w = w->link.next, h += (w->fixed ? w->fixed : 2)) ;

	return h;
}

/* Find last window in a group */

W *findbotw(W *w)
{
	W *x;

	for (x = w; x->link.next->main == w->main && x->link.next != w; x = x->link.next) ;
	return x;
}

/* Demote group of window to end of window list.  Returns true if top window
   was demoted */

int demotegroup(W *w)
{
	W *top = findtopw(w);
	W *bot = findbotw(w);
	W *next;
	int flg = 0;

	for (w = top; w != bot; w = next) {
		next = w->link.next;
		if (w == w->t->topwin) {
			flg = 1;
			w->t->topwin = next;
		} else
			demote(W, link, w->t->topwin, w);
		w->y = -1;
	}
	if (w == w->t->topwin)
		flg = 1;
	else
		demote(W, link, w->t->topwin, w);
	w->y = -1;
	return flg;
}

/* Find last window on the screen */

W *lastw(Screen *t)
{
	W *x;

	for (x = t->topwin; x->link.next != t->topwin && x->link.next->y >= 0; x = x->link.next) ;
	return x;
}

/* Create a screen object */

Screen *scr;

Screen *screate(SCRN *scrn)
{
	Screen *t = (Screen *) joe_malloc(sizeof(Screen));

	t->t = scrn;
	t->w = scrn->co;
	t->h = scrn->li;
	t->topwin = NULL;
	t->curwin = NULL;
	t->wind = skiptop;
	scr = t;
	return t;
}

void sresize(Screen *t)
{
	SCRN *scrn = t->t;
	W *w;

	t->w = scrn->co;
	t->h = scrn->li;
	if (t->h - t->wind < FITHEIGHT)
		t->wind = t->h - FITHEIGHT;
	if (t->wind < 0)
		t->wind = 0;
	w = t->topwin;
	do {
		w->y = -1;
		w->w = t->w;
		w = w->link.next;
	} while (w != t->topwin);
	wfit(t);
	updall();
}

void updall(void)
{
	int y;

	for (y = 0; y != scr->h; ++y) {
		scr->t->updtab[y] = 1;
	}
}

void scrins(B *b, long l, long n, int flg)
{
	W *w;

	if (scr && (w = scr->topwin) != NULL) {
		do {
			if (w->y >= 0) {
				if (w->object && w->watom->ins)
					w->watom->ins(w->object, b, l, n, flg);
			}
		w = w->link.next;
		} while (w != scr->topwin);
	}
}

void scrdel(B *b, long l, long n, int flg)
{
	W *w;

	if ((w = scr->topwin) != NULL) {
		do {
			if (w->y >= 0) {
				if (w->object && w->watom->del)
					w->watom->del(w->object, b, l, n, flg);
			}
		w = w->link.next;
		} while (w != scr->topwin);
	}
}

W *watpos(Screen *t,int x,int y)
{
	W *w=t->topwin;
	do
		if(w->y>=0 && w->y<=y && w->y+w->h>y && w->x<=x && w->x+w->w>x)
			return w;
		while(w=w->link.next, w!=t->topwin);
	return 0;
}

/* Fit as many windows on the screen as is possible beginning with the window
 * at topwin.  Give any extra space which couldn't be used to fit in another
 * window to the last text window on the screen.  This function guarentees
 * to fit on the window with the cursor in it (moves topwin to next group
 * of windows until window with cursor fits on screen).
 */

static int doabort(W *w, int *ret);

void wfit(Screen *t)
{
	int y;			/* Where next window goes */
	int left;		/* Lines left on screen */
	W *w;			/* Current window we're fitting */
	W *pw;			/* Main window of previous family */
	int req;		/* Amount this family needs */
	int adj;		/* Amount family needs to be adjusted */
	int flg = 0;		/* Set if cursor window was placed on screen */
	int ret;

	dostaupd = 1;

      tryagain:
	y = t->wind;
	left = t->h - y;
	pw = NULL;

	w = t->topwin;
	do {
		w->ny = -1;
		w->nh = geth(w);
		w = w->link.next;
	} while (w != t->topwin);

	/* Fit a group of windows on the screen */
	w = t->topwin;
	do {
		req = getgrouph(w);
		if (req > left)	/* If group is taller than lines left */
			adj = req - left;	/* then family gets shorter */
		else
			adj = 0;

		/* Fit a family of windows on the screen */
		do {
			w->ny = y;	/* Set window's y position */
			if (!w->win) {
				pw = w;
				w->nh -= adj;	/* Adjust main window of the group */
			}
			if (!w->win && w->nh < 2)
				while (w->nh < 2)
					w->nh += doabort(w->link.next, &ret);
			if (w == t->curwin)
				flg = 1;	/* Set if we got window with cursor */
			y += w->nh;
			left -= w->nh;	/* Increment y value by height of window */
			w = w->link.next;	/* Next window */
		} while (w != t->topwin && w->main == w->link.prev->main);
	} while (w != t->topwin && left >= FITHEIGHT);

	/* We can't use extra space to fit a new family on, so give space to parent of
	 * previous family */
	pw->nh += left;

	/* Adjust that family's children which are below the parent */
	while ((pw = pw->link.next) != w)
		pw->ny += left;

	/* Make sure the cursor window got on the screen */
	if (!flg) {
		t->topwin = findbotw(t->topwin)->link.next;
		goto tryagain;
	}

	/* All of the windows are now on the screen.  Scroll the screen to reflect what
	 * happened
	 */
	w = t->topwin;
	do {
		if (w->y >= 0 && w->ny >= 0)
			if (w->ny > w->y) {
				W *l = pw = w;

				while (pw->link.next != t->topwin && (pw->link.next->y < 0 || pw->link.next->ny < 0 || pw->link.next->ny > pw->link.next->y)) {
					pw = pw->link.next;
					if (pw->ny >= 0 && pw->y >= 0)
						l = pw;
				}
				/* Scroll windows between l and w */
			      loop1:
				if (l->ny >= 0 && l->y >= 0)
					nscrldn(t->t, l->y, l->ny + uns_min(l->h, l->nh), l->ny - l->y);
				if (w != l) {
					l = l->link.prev;
					goto loop1;
				}
				w = pw->link.next;
			} else if (w->ny < w->y) {
				W *l = pw = w;

				while (pw->link.next != t->topwin && (pw->link.next->y < 0 || pw->link.next->ny < 0 || pw->link.next->ny < pw->link.next->y)) {
					pw = pw->link.next;
					if (pw->ny >= 0 && pw->y >= 0)
						l = pw;
				}
				/* Scroll windows between l and w */
			      loop0:
				if (w->ny >= 0 && w->y >= 0)
					nscrlup(t->t, w->ny, w->y + uns_min(w->h, w->nh), w->y - w->ny);
				if (w != l) {
					w = w->link.next;
					goto loop0;
				}
				w = pw->link.next;
			} else
				w = w->link.next;
		else
			w = w->link.next;
	} while (w != t->topwin);

	/* Update current height and position values */
	w = t->topwin;
	do {
		if (w->ny >= 0) {
			if (w->y == -1) {
				msetI(t->t->updtab + w->ny, 1, w->nh);
			}
			w->y = w->ny;
		} else
			w->y = -1;
		w->h = w->nh;
		w->reqh = 0;
		w = w->link.next;
	} while (w != t->topwin);

	/* Call move and resize in a second pass so that they see valid positions for all windows */
	w = t->topwin;
	do {
		if (w->y >= 0) {
			if (w->object) {
				if (w->watom->move)
					w->watom->move(w->object, w->x, w->y);
				if (w->watom->resize)
					w->watom->resize(w->object, w->w, w->h);
			}
		}
		w = w->link.next;
	} while (w != t->topwin);
}

/* Goto next window */

int wnext(Screen *t)
{
	if (t->curwin->link.next != t->curwin) {
		t->curwin = t->curwin->link.next;
		if (t->curwin->y == -1)
			wfit(t);
		return 0;
	} else
		return -1;
}

/* Goto previous window */

int wprev(Screen *t)
{
	if (t->curwin->link.prev != t->curwin) {
		t->curwin = t->curwin->link.prev;
		if (t->curwin->y == -1) {
			t->topwin = findtopw(t->curwin);
			wfit(t);
		}
		return 0;
	} else
		return -1;
}

/* Grow window */

int wgrow(W *w)
{
	W *nextw;

	/* If we're the last window on the screen, shrink the previous window */
	if ((w->link.next == w->t->topwin || w->link.next->y == -1) && w != w->t->topwin)
		return wshrink(w->link.prev->main);

	/* Get to next variable size window */
	for (nextw = w->link.next; nextw->fixed && nextw != w->t->topwin; nextw = nextw->link.next) ;

	/* Is it below us, on screen and big enough to take space from? */
	if (nextw == w->t->topwin || nextw->y == -1 || nextw->h <= FITHEIGHT)
		return -1;

	/* Increase this window's height */
	seth(w, w->h + 1);

	/* Decrease next window's height */
	seth(nextw, nextw->h - 1);

	/* Do it */
	wfit(w->t);

	return 0;
}

/* Shrink window */

int wshrink(W *w)
{
	W *nextw;

	/* If we're the last window on the screen, grow the previous window */
	if ((w->link.next == w->t->topwin || w->link.next->y == -1) && w != w->t->topwin)
		return wgrow(w->link.prev->main);

	/* Is this window too small already? */
	if (w->h <= FITHEIGHT)
		return -1;

	/* Get to window below us */
	for (nextw = w->link.next; nextw != w->t->topwin && nextw->fixed; nextw = nextw->link.next) ;
	if (nextw == w->t->topwin)
		return -1;

	/* Decrease the size of this window */
	seth(w, w->h - 1);

	/* Increase the size of next window */
	seth(nextw, nextw->h + 1);

	/* Do it */
	wfit(w->t);

	return 0;
}

/* Grow window up */

int wgrowup(W *w)
{
	return wshrink(w->link.prev->main);
}

/* Grow window down */

int wgrowdown(W *w)
{
	return wgrow(w->link.prev->main);
}

/* Show all windows */

void wshowall(Screen *t)
{
	int n = 0;
	int set;
	W *w;

	/* Count no. of main windows */
	w = t->topwin;
	do {
		if (!w->win)
			++n;
		w = w->link.next;
	} while (w != t->topwin);

	/* Compute size to set each window */
	if ((set = (t->h - t->wind) / n) < FITHEIGHT)
		set = FITHEIGHT;

	/* Set size of each variable size window */
	w = t->topwin;
	do {
		if (!w->win) {
			int h = getminh(w);

			if (h >= set)
				seth(w, 2);
			else
				seth(w, set - (h - 2));
			w->orgwin = NULL;
		}
		w = w->link.next;
	} while (w != t->topwin);

	/* Do it */
	wfit(t);
}

static void wspread(Screen *t)
{
	int n = 0;
	W *w = t->topwin;

	do {
		if (w->y >= 0 && !w->win)
			++n;
		w = w->link.next;
	} while (w != t->topwin);
	if (!n) {
		wfit(t);
		return;
	}
	if ((t->h - t->wind) / n >= FITHEIGHT)
		n = (t->h - t->wind) / n;
	else
		n = FITHEIGHT;
	w = t->topwin;
	do {
		if (!w->win) {
			int h = getminh(w);

			if (h >= n)
				seth(w, 2);
			else
				seth(w, n - (h - 2));
			w->orgwin = NULL;
		}
		w = w->link.next;
	} while (w != t->topwin);
	wfit(t);
}

/* Show just one family of windows */

void wshowone(W *w)
{
	W *q = w->t->topwin;

	do {
		if (!q->win) {
			seth(q, w->t->h - w->t->wind - (getminh(q) - 2));
			q->orgwin = NULL;
		}
		q = q->link.next;
	} while (q != w->t->topwin);
	wfit(w->t);
}

/* Create a window */

W *wcreate(Screen *t, WATOM *watom, W *where, W *target, W *original, int height, unsigned char *huh, int *notify)
{
	W *new;

	if (height < 1)
		return NULL;

	/* Create the window */
	new = (W *) joe_malloc(sizeof(W));
	new->notify = notify;
	new->t = t;
	new->w = t->w;
	seth(new, height);
	new->h = new->reqh;
	new->y = -1;
	new->ny = 0;
	new->nh = 0;
	new->x = 0;
	new->huh = huh;
	new->orgwin = original;
	new->watom = watom;
	new->object = NULL;
	new->msgb = NULL;
	new->msgt = NULL;
	new->bstack = 0;
	/* Set window's target and family */
/* was:	if (new->win = target) {	which may be mistyped == */
	if ((new->win = target) != NULL) {	/* A subwindow */
		new->main = target->main;
		new->fixed = height;
	} else {		/* A parent window */
		new->main = new;
		new->fixed = 0;
	}

	/* Get space for window */
	if (original) {
		if (original->h - height <= 2) {
			/* Not enough space for window */
			joe_free(new);
			return NULL;
		} else
			seth(original, original->h - height);
	}

	/* Create new keyboard handler for window */
	if (watom->context)
		new->kbd = mkkbd(kmap_getcontext(watom->context));
	else
		new->kbd = NULL;

	/* Put window on the screen */
	if (where)
		enquef(W, link, where, new);
	else {
		if (t->topwin)
			enqueb(W, link, t->topwin, new);
		else {
			izque(W, link, new);
			t->curwin = t->topwin = new;
		}
	}

	return new;
}

/* Abort group of windows */

static int doabort(W *w, int *ret)
{
	int amnt = geth(w);
	W *z;

	w->y = -2;
	if (w->t->topwin == w)
		w->t->topwin = w->link.next;
      loop:
	z = w->t->topwin;
	do {
		if (z->orgwin == w)
			z->orgwin = NULL;
		if ((z->win == w || z->main == w) && z->y != -2) {
			amnt += doabort(z, ret);
			goto loop;
		}
	} while (z = z->link.next, z != w->t->topwin);
	if (w->orgwin)
		seth(w->orgwin, geth(w->orgwin) + geth(w));
	if (w->t->curwin == w) {
		if (w->t->curwin->win)
			w->t->curwin = w->t->curwin->win;
		else if (w->orgwin)
			w->t->curwin = w->orgwin;
		else
			w->t->curwin = w->link.next;
	}
	if (qempty(W, link, w)) {
		leave = 1;
		amnt = 0;
	}
	deque(W, link, w);
	if (w->watom->abort && w->object) {
		*ret = w->watom->abort(w->object);
		if (w->notify)
			*w->notify = -1;
	} else {
		*ret = -1;
		if (w->notify)
			*w->notify = 1;
	}
	rmkbd(w->kbd);
	joe_free(w);
	windie(w);
	return amnt;
}

/* Abort a window and its children */

int wabort(W *w)
{
	Screen *t = w->t;
	int ret;

	if (w != w->main) {
		doabort(w, &ret);
		if (!leave)
			wfit(t);
	} else {
		unsigned char *msgt = w->msgt;
		unsigned char *msgb = w->msgb;
		doabort(w, &ret);
		if (!leave) {
			if (lastw(t)->link.next != t->topwin)
				wfit(t);
			else
				wspread(t);
			if (msgt && !maint->curwin->msgt) maint->curwin->msgt = msgt;
			if (msgb && !maint->curwin->msgb) maint->curwin->msgb = msgb;
		}
	}
	return ret;
}

/* Display a message and skip the next key */

int bg_msg;

static void mdisp(SCRN *t, int y, unsigned char *s)
{
	int ofst;
	int len;

	len = fmtlen(s);
	if (len <= (t->co))
		ofst = 0;
	else
		ofst = len - (t->co);
	genfmt(t, 0, y, ofst, s, BG_COLOR(bg_msg), 1);
	t->updtab[y] = 1;
}

void msgout(W *w)
{
	SCRN *t = w->t->t;

	if (w->msgb) {
		mdisp(t, w->y + w->h - 1, w->msgb);
		// w->msgb = 0;
	}
	if (w->msgt) {
		mdisp(t, w->y + ((w->h > 1 && (w->y || !staen)) ? 1 : 0), w->msgt);
		// w->msgt = 0;
	}
}

void msgclr(W *w)
{
	w->msgb = 0;
	w->msgt = 0;
}

/* Set temporary message */

unsigned char msgbuf[JOE_MSGBUFSIZE];

/* display message on bottom line of window */
void msgnw(W *w, unsigned char *s)
{
	w->msgb = s;
}

void msgnwt(W *w, unsigned char *s)
{
	w->msgt = s;
}

int urtn(BASE *b, int k)
{
	if (b->parent->watom->rtn)
		return b->parent->watom->rtn(b, k);
	else
		return -1;
}

int utype(BASE *b, int k)
{
	if (b->parent->watom->type)
		return b->parent->watom->type(b, k);
	else
		return -1;
}

/* Window user commands */

int uprevw(BASE *bw)
{
	return wprev(bw->parent->t);
}

int unextw(BASE *bw)
{
	return wnext(bw->parent->t);
}

int ugroww(BASE *bw)
{
	return wgrow(bw->parent);
}

int ushrnk(BASE *bw)
{
	return wshrink(bw->parent);
}

int uexpld(BASE *bw)
{
	if (bw->parent->t->h - bw->parent->t->wind == getgrouph(bw->parent))
		wshowall(bw->parent->t);
	else
		wshowone(bw->parent);
	return 0;
}

int uretyp(BASE *bw)
{
	nredraw(bw->parent->t->t);
	return 0;
}

/* Get message window on screen */

W *find_window(Screen *t, B *b)
{
	W *w = t->topwin;
	do {
		if (w->watom == &watomtw && ((BW *)w->object)->b == b)
			return w;
		w = w->link.next;
	} while(w != t->topwin);
	return 0;
}

int umwind(BW *bw)
{
	W *msgw;
	if (!errbuf) {
		msgnw(bw->parent, joe_gettext(_("There are no messages")));
		return -1;
	}

	/* Find message window */
	msgw = find_window(bw->parent->t, errbuf);

	if (msgw) {
		/* The window exists */
		bw->parent->t->curwin = msgw;
		wshowone(msgw);
		return 0;
	} else {
		/* Make it the current window */
		msgw = bw->parent;
		get_buffer_in_window(bw, errbuf);
		wshowone(msgw);
		return 0;
	}
}

/* Fit previous window and current window on screen.  If there is no previous window,
 * split the current window to create one. */

int umfit(BW *bw)
{
	W *p;
	W *w = bw->parent->main;
	Screen *t = w->t;
	wshowone(w);
	p = findtopw(w)->link.prev->main;
	if (p == w) {
		/* We have to split */
		usplitw(bw);
	}
	w = t->curwin;
	p = findtopw(w)->link.prev->main;
	if (p == w) {
		return -1;
	}
	/* Request size */
	if ((p->t->h >> 1) < 3) /* -6 */
		return -1;
	seth(p, (p->t->h >> 1)); /* -6 */
	t->topwin = p;
	t->curwin = p;
	/* Fit them on the screen */
	wfit(t);
	t->curwin = w;
	wfit(t);
	return 0;
}
