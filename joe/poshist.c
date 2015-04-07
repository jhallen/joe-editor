/*
 *	Position history
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

typedef struct pos POS;

struct pos {
	LINK(POS) link;
	P *p;
	W *w;
};

POS pos = { {&pos, &pos} };
POS frpos = { {&frpos, &frpos} };
POS *curpos = &pos;
int npos = 0;

static void markpos(W *w, P *p)
{
	POS *newpos = (POS *)alitem(&frpos, SIZEOF(POS));

	newpos->p = NULL;
	pdupown(p, &newpos->p, "markpos");
	poffline(newpos->p);
	newpos->w = w;
	enqueb(POS, link, &pos, newpos);
	if (npos == 20) {
		newpos = pos.link.next;
		prm(newpos->p);
		demote(POS, link, &frpos, newpos);
	} else {
		++npos;
	}
}

void afterpos(void)
{
	if (curpos != &pos) {
		demote(POS, link, &pos, curpos);
		curpos = &pos;
	}
}

void aftermove(W *w, P *p)
{
	if (pos.link.prev != &pos && pos.link.prev->w == w && pos.link.prev->p && oabs(pos.link.prev->p->line - p->line) < 3) {
		poffline(pset(pos.link.prev->p, p));
	} else {
		markpos(w, p);
	}
}

void windie(W *w)
{
	POS *n;

	for (n = pos.link.prev; n != &pos; n = n->link.prev) {
		if (n->w == w) {
			n->w = NULL;
		}
	}
}

int unextpos(W *w, int k)
{
	BW *bw;
	WIND_BW(bw, w);

      lp:
	if (curpos->link.next != &pos && curpos != &pos) {
		curpos = curpos->link.next;
		if (!curpos->p || !curpos->w) {
			goto lp;
		}
		if (!(curpos->w->watom->what & (TYPETW|TYPEPW)))
			goto lp;
		bw = (BW *)curpos->w->object;
		if (bw->b != curpos->p->b)
			goto lp;
		if (w->t->curwin == curpos->w && curpos->p->byte == ((BW *) w->t->curwin->object)->cursor->byte) {
			goto lp;
		}
		if (w->t->curwin != curpos->w) {
			w->t->curwin = curpos->w;
			if (w->t->curwin->y == -1) {
				wfit(w->t);
			}
		}
		w = w->t->curwin;
		bw = (BW *) w->object;
		if (bw->cursor->byte != curpos->p->byte) {
			pset(bw->cursor, curpos->p);
		}
		return 0;
	} else {
		return -1;
	}
}

int uprevpos(W *w, int k)
{
	BW *bw;
	WIND_BW(bw, w);

      lp:
	if (curpos->link.prev != &pos) {
		curpos = curpos->link.prev;
		if (!curpos->p || !curpos->w) {
			goto lp;
		}
		if (!(curpos->w->watom->what & (TYPETW|TYPEPW)))
			goto lp;
		bw = (BW *)curpos->w->object;
		if (bw->b != curpos->p->b)
			goto lp;
		if (w->t->curwin == curpos->w && curpos->p->byte == ((BW *) w->t->curwin->object)->cursor->byte) {
			goto lp;
		}
		if (w->t->curwin != curpos->w) {
			w->t->curwin = curpos->w;
			if (w->t->curwin->y == -1) {
				wfit(w->t);
			}
		}
		w = w->t->curwin;
		bw = (BW *) w->object;
		if (bw->cursor->byte != curpos->p->byte) {
			pset(bw->cursor, curpos->p);
		}
		return 0;
	} else {
		return -1;
	}
}
