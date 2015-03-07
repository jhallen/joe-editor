/* GPM/xterm mouse functions
   Copyright (C) 1999 Jesse McGrew

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
JOE; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

#include "types.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

int auto_scroll = 0;		/* Set for autoscroll */
int auto_rate;			/* Rate */
int auto_trig_time;		/* Time of next scroll */

int rtbutton=0;			/* use button 3 instead of 1 */
int floatmouse=0;		/* don't fix xcol after tomouse */
int joexterm=0;			/* set if we're using Joe's modified xterm */

static int selecting = 0;	/* Set if we did any selecting */

static int Cb, Cx, Cy;
static int last_msec=0;		/* time in ms when event occurred */
static int clicks;

static void fake_key(int c)
{
	MACRO *m=dokey(maint->curwin->kbd,c);
	int x=maint->curwin->kbd->x;
	maint->curwin->main->kbd->x=x;
	if(x)
		maint->curwin->main->kbd->seq[x-1]=maint->curwin->kbd->seq[x-1];
	if(m)
		exemac(m);
}

/* Translate mouse coordinates */

static int mcoord(int x)
{
	if (x>=33 && x<=240)
		return x - 33 + 1;
	else if (x==32)
		return -1 + 1;
	else if (x>240)
		return x - 257 + 1;
	else
		return 0; /* This should not happen */
}

static int mouse_event(BW *bw)
{
	if ((Cb & 0x41) == 0x40) {
		fake_key(KEY_MWUP);
		return 0;
	}

	if ((Cb & 0x41) == 0x41) {
		fake_key(KEY_MWDOWN);
		return 0;
	}

	if ((Cb & 3) == 3)
		/* button released */
		mouseup(Cx,Cy);
	else if ((Cb & 3) == (rtbutton ? 2 : 0))	/* preferred button */
		if ((Cb & 32) == 0)
			/* button pressed */
			mousedn(Cx,Cy);
		else
			/* drag */
			mousedrag(Cx,Cy);
	else if ((maint->curwin->watom->what & TYPETW ||
	          maint->curwin->watom->what & TYPEPW) &&
	          joexterm && (Cb & 3) == 1)		/* Paste */
		ttputs(USTR "\33]52;;?\33\\");
	return 0;
}

/* Parse old style mouse event parameters. */

int uxtmouse(BW *bw)
{
	Cb = ttgetc()-32;
	if (Cb < 0)
		return -1;
	Cx = ttgetc();
	if (Cx < 32)
		return -1;
	Cy = ttgetc();
	if (Cy < 32)
		return -1;

	Cx = mcoord(Cx);
	Cy = mcoord(Cy);
	return mouse_event(bw);
}

/* Parse xterm extended 1006 mode mouse event parameters. */

int uextmouse(BW *bw)
{
	int c;
	Cb = Cx = Cy = 0;
	while ((c = ttgetc()) != ';') {
		if (c < '0' || c > '9')
			return -1;
		Cb = 10 * Cb + c - '0';
	}
	while ((c = ttgetc()) != ';') {
		if (c < '0' || c > '9')
			return -1;
		Cx = 10 * Cx + c - '0';
	}
	while ((c = ttgetc()) != 'M' && c != 'm') {
		if (c < '0' || c > '9')
			return -1;
		Cy = 10 * Cy + c - '0';
	}
	if (c == 'm')
		Cb |= 3;
	return mouse_event(bw);
}

int mnow()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void mousedn(int x,int y)
{
	Cx = x, Cy = y;
	if (last_msec == 0 || mnow() - last_msec > MOUSE_MULTI_THRESH) {
		/* not a multiple click */
		clicks=1;
		fake_key(KEY_MDOWN);
	} else if(clicks==1) {
		/* double click */
		clicks=2;
		fake_key(KEY_M2DOWN);
	} else if(clicks==2) {
		/* triple click */
		clicks=3;
		fake_key(KEY_M3DOWN);
	} else {
		/* start over */
		clicks=1;
		fake_key(KEY_MDOWN);
	}
}

/* Return base64 code character given 6-bit number */

char base64_code[]="\
ABCDEFGHIJKLMNOPQRSTUVWXYZ\
abcdefghijklmnopqrstuvwxyz\
0123456789+/";

int base64_accu = 0;
int base64_count = 0;
int base64_pad = 0;

static void ttputs64(unsigned char *p, unsigned length)
{
        unsigned char buf[65];
        unsigned x = 0;
        while (length--) {
            switch (base64_count) {
                case 0:
                    buf[x++] = base64_code[*p >> 2];
                    base64_accu = (*p & 0x3);
                    base64_count = 2;
                    ++p;
                    break;
                case 2:
                    buf[x++] = base64_code[(base64_accu << 4) + (*p >> 4)];
                    base64_accu = (*p & 0xF);
                    base64_count = 4;
                    ++p;
                    break;
                case 4:
                    buf[x++] = base64_code[(base64_accu << 2) + (*p >> 6)];
                    buf[x++] = base64_code[*p & 0x3F];
                    base64_accu = 0;
                    base64_count = 0;
                    ++p;
                    break;
            }
            if (x >= 63) {
                /* Write 63 or 64 characters */
                base64_pad += x;
                buf[x] = 0;
                ttputs(buf);
                x = 0;
            }
        }
        if (x != 0) {
            base64_pad += x;
            buf[x] = 0;
            ttputs(buf);
        }
}

static void ttputs64_flush()
{
    unsigned char x;
    switch (base64_count) {
        case 0:
            break;
        case 2:
            x = base64_code[base64_accu << 4];
            ttputc(x);
            break;
        case 4:
            x = base64_code[base64_accu << 2];
            ttputc(x);
            break;
    }
    if (base64_pad & 3) {
        x = 4 - (base64_pad & 3);
        while (x--)
        	ttputc('=');
    }
    base64_count = 0;
    base64_accu = 0;
    base64_pad = 0;
}

void select_done(struct charmap *map)
{
	/* Feed text to xterm */
	if (joexterm && markv(1)) {
		long left = markb->xcol;
		long right = markk->xcol;
		P *q = pdup(markb, USTR "select_done");
		int c;
		/* ttputs(USTR "\33[?2P"); JOE's xterm */
		ttputs(USTR "\33]52;;"); /* New xterm */
		while (q->byte < markk->byte) {
			unsigned char buf[16];
			int len;
			/* Skip until we're within columns */
			while (q->byte < markk->byte && square && (piscol(q) < left || piscol(q) >= right))
				pgetc(q);

			/* Copy text into buffer */
			while (q->byte < markk->byte && (!square || (piscol(q) >= left && piscol(q) < right))) {
				c = pgetc(q);
				if (map->type)
					if (locale_map->type) {
						/* UTF-8 char to UTF-8 terminal */
						len = utf8_encode(buf,c);
						ttputs64(buf, len);
					} else {
						/* UTF-8 char to non-UTF-8 terminal */
						c = from_uni(locale_map,c);
						if (c == -1)
							c = '?';
						buf[0] = c;
						ttputs64(buf, 1);
					}
				else
					if (locale_map->type) {
						/* Non-UTF-8 to UTF-8 terminal */
						c = to_uni(map, c);
						if (c == -1)
							c = '?';
						len = utf8_encode(buf,c);
						ttputs64(buf, len);
					} else {
						/* Non-UTF-8 to non-UTF-8 terminal */
						buf[0] = c;
						ttputs64(buf, 1);
					}
			}
			/* Add a new line if we went past right edge of column */
			if (square && q->byte<markk->byte && piscol(q) >= right) {
				buf[0] = 10;
				ttputs64(buf, 1);
			}
		}
		ttputs64_flush();
		ttputs(USTR "\33\\");
		prm(q);
	}
}

void mouseup(int x,int y)
{
	auto_scroll = 0;
	Cx = x, Cy = y;
	if (selecting) {
		select_done(((BW *)maint->curwin->object)->b->o.charmap);
		selecting = 0;
	}
	switch(clicks) {
		case 1:
			fake_key(KEY_MUP);
			break;
  
		case 2:
			fake_key(KEY_M2UP);
			break;
  
		case 3:
			fake_key(KEY_M3UP);
			break;
	}
	last_msec = mnow();
}

void mousedrag(int x,int y)
{
	Cx = x, Cy = y;
	switch(clicks) {
		case 1:
			fake_key(KEY_MDRAG);
			break;
  
		case 2:
			fake_key(KEY_M2DRAG);
			break;
  
		case 3:
			fake_key(KEY_M3DRAG);
			break;
	}
}

int drag_size; /* Set if we are resizing a window */

int utomouse(BW *xx)
{
	BW *bw;
	int x = Cx - 1, y = Cy - 1;
	W *w = watpos(maint,x,y);
	if (!w)
		return -1;
	maint->curwin = w;
	bw = w->object;
	drag_size = 0;
	if (w->watom->what == TYPETW) {
		if (bw->o.hex) {
			int goal_col = x - w->x + bw->offset - 60;
			int goal_line;
			long goal_byte;
			if (goal_col < 0)
				goal_col = 0;
			if (goal_col >15)
				goal_col = 15;
			/* window has a status line? */
			if (((TW *)bw->object)->staon)
				/* clicked on it? */
				if (y == w->y) {
					if (y != maint->wind)
						drag_size = y;
					return -1;
				} else
					goal_line = y - w->y + bw->top->byte/16 - 1;
			else
				goal_line = y - w->y + bw->top->byte/16;
			goal_byte = goal_line*16L + goal_col;
			if (goal_byte > bw->b->eof->byte)
				goal_byte = bw->b->eof->byte;
			pgoto(bw->cursor, goal_byte);
			return 0;
		} else {
			int goal_col = x - w->x + bw->offset - (bw->o.linums ? LINCOLS : 0);
			int goal_line;
			if (goal_col < 0)
				goal_col = 0;
			/* window has a status line? */
			if (((TW *)bw->object)->staon)
				/* clicked on it? */
				if (y == w->y) {
					if (y != maint->wind)
						drag_size = y;
					return -1;
				} else
					goal_line = y - w->y + bw->top->line - 1;
			else
				goal_line = y - w->y + bw->top->line;
			pline(bw->cursor, goal_line);
			pcol(bw->cursor, goal_col);
			if (floatmouse)
				bw->cursor->xcol = goal_col;
			else
				bw->cursor->xcol = piscol(bw->cursor);
			return 0;
		}
	} else if (w->watom->what == TYPEPW) {
		PW *pw = (PW *)bw->object;
		/* only one line in prompt windows */
		pcol(bw->cursor,x - w->x + bw->offset - pw->promptlen + pw->promptofst);
		bw->cursor->xcol = piscol(bw->cursor);
		return 0;
	} else if (w->watom->what == TYPEMENU) {
		menujump((MENU *)w->object, x - w->x, y - w->y);
		return 0;
	} else return -1;
}

/* same as utomouse but won't change windows, and always floats. puts the
 * position that utomouse would use into tmspos. */
static int tmspos;

static int tomousestay()
{
	BW *bw;
	int x = Cx - 1,y = Cy - 1;
	W *w;
	/*
	w = watpos(maint,x,y);
	if(!w || w != maint->curwin)
		return -1;
	*/
	w = maint->curwin;
	bw = w->object;
	if (w->watom->what == TYPETW) {
		if (bw->o.hex) {
			int goal_col = x - w->x + bw->offset - 60;
			int goal_line;
			long goal_byte;
			if (goal_col < 0)
				goal_col = 0;
			if (goal_col > 15)
				goal_col = 15;
			/* window has a status line? */
			if (((TW *)bw->object)->staon)
				if (y <= w->y) {
					goal_col = 0;
					goal_line = bw->top->byte/16;
				} else if (y >= w->y + w->h) {
					goal_line = bw->top->byte/16 + w->h - 2;
					goal_col = 15;
				} else
					goal_line = y - w->y + bw->top->byte/16 - 1;
			else
				if (y < w->y) {
					goal_col = 0;
					goal_line = bw->top->byte/16;
				} else if (y >= w->y + w->h) {
					goal_line = bw->top->byte/16 + w->h - 1;
					goal_col = 15;
				} else
					goal_line = y - w->y + bw->top->byte/16;
			goal_byte = goal_line*16L + goal_col;
			if (goal_byte > bw->b->eof->byte)
				goal_byte = bw->b->eof->byte;
			pgoto(bw->cursor, goal_byte);
			/* This is not right... */
			tmspos = bw->cursor->xcol = piscol(bw->cursor);
			return 0;
		} else {
			int goal_col = x - w->x + bw->offset - (bw->o.linums ? LINCOLS : 0);
			int goal_line;
			if (goal_col < 0)
				goal_col = 0;
			/* window has a status line? */
			if (((TW *)bw->object)->staon)
				if (y <= w->y) {
					goal_col = 0;
					goal_line = bw->top->line;
				} else if (y >= w->y + w->h) {
					goal_col = 1000;
					goal_line = w->h + bw->top->line - 2;
				} else
					goal_line = y - w->y + bw->top->line - 1;
			else
				if (y < w->y) {
					goal_col = 0;
					goal_line = bw->top->line;
				} else if (y >= w->y + w->h) {
					goal_col = 1000;
					goal_line = w->h + bw->top->line - 1;
				} else
					goal_line = y - w->y + bw->top->line;
			pline(bw->cursor, goal_line);
			pcol(bw->cursor, goal_col);
			tmspos = bw->cursor->xcol = goal_col;
			if (!floatmouse)
				tmspos = piscol(bw->cursor);
			return 0;
		}
	} else if (w->watom->what == TYPEPW) {
		PW *pw = (PW *)bw->object;
		/* only one line in prompt windows */
		pcol(bw->cursor,x - w->x + bw->offset - pw->promptlen + pw->promptofst);
		tmspos = bw->cursor->xcol = piscol(bw->cursor);
		return 0;
	} else return -1;
}

static long anchor;		/* byte where mouse was originally pressed */
static long anchorn;		/* near side of the anchored word */
static int marked;		/* mark was set by defmdrag? */
static int reversed;		/* mouse was dragged above the anchor? */

int udefmdown(BW *xx)
{
	BW *bw;
	if (utomouse(xx))
		return -1;
	if ((maint->curwin->watom->what & (TYPEPW | TYPETW)) == 0)
		return 0;
	bw = (BW *)maint->curwin->object;
	anchor = bw->cursor->byte;
	marked = reversed = 0;
	return 0;
}

void reset_trig_time()
{
	if (!auto_rate)
		auto_rate = 1;
	auto_trig_time = mnow() + 300 / (1 + auto_rate);
}

int udefmdrag(BW *xx)
{
	BW *bw = (BW *)maint->curwin->object;
	int ay = Cy - 1;
	int new_scroll;
	int new_rate;
	if (drag_size) {
		while (ay > bw->parent->y) {
			int y = bw->parent->y;
			wgrowdown(bw->parent);
			if (y == bw->parent->y)
				return -1;
		}
		while (ay < bw->parent->y) {
			int y = bw->parent->y;
			wgrowup(bw->parent);
			if (y == bw->parent->y)
				return -1;
		}
		return 0;
	}

	if (ay < bw->y) {
		new_scroll = -1;
		new_rate = bw->y - ay;
	}
	else if (ay >= bw->y + bw->h) {
		new_scroll = 1;
		new_rate = ay - (bw->y + bw->h) + 1;
	} else {
		new_scroll = 0;
		new_rate = 1;
	}

	if (new_rate > 10)
		new_rate = 10;

	if (!new_scroll)
		auto_scroll = 0;
	else if (new_scroll != auto_scroll) {
		auto_scroll = new_scroll;
		auto_rate = new_rate;
		reset_trig_time();
	} else if (new_rate != auto_rate) {
/*
		int left = auto_trig_time - mnow();
		if (left > 0) {
			left = left * auto_rate / new_rate;
		}
*/
		auto_rate = new_rate;
	}

	if (!marked)
		marked++, umarkb(bw);
	if (tomousestay())
		return -1;
	selecting = 1;
	if (reversed)
		umarkb(bw);
	else
		umarkk(bw);
	if ((!reversed && bw->cursor->byte < anchor) || (reversed && bw->cursor->byte > anchor)) {
		P *q = pdup(markb, USTR "udefmdrag");
		int tmp = markb->xcol;
		pset(markb,markk);
		pset(markk,q);
		markb->xcol = markk->xcol;
		markk->xcol = tmp;
		prm(q);
		reversed = !reversed;
	}
	bw->cursor->xcol = tmspos;
	return 0;
}

int udefmup(BW *bw)
{
	return 0;
}

int udefm2down(BW *xx)
{
	BW *bw;
	if (utomouse(xx))
		return -1;
	if (maint->curwin->watom->what & TYPEMENU) {
		return maint->curwin->watom->rtn((MENU *)maint->curwin->object);
	}
	if ((maint->curwin->watom->what & (TYPEPW | TYPETW)) == 0)
		return 0;
	bw = (BW *)maint->curwin->object;
	/* set anchor to left side, anchorn to right side */
	u_goto_prev(bw); anchor = bw->cursor->byte; umarkb(bw); markb->xcol = piscol(markb);
	u_goto_next(bw); anchorn = bw->cursor->byte; umarkk(bw); markk->xcol = piscol(markk);
	reversed = 0;
	bw->cursor->xcol = piscol(bw->cursor);
	selecting = 1;
	return 0;
}

int udefm2drag(BW *xx)
{
	BW *bw=(BW *)maint->curwin->object;
	if (tomousestay())
		return -1;
	if (!reversed && bw->cursor->byte < anchor) {
		pgoto(markk,anchorn);
		markk->xcol = piscol(markk);
		reversed = 1;
	} else if(reversed && bw->cursor->byte > anchorn) {
		pgoto(markb,anchor);
		markb->xcol = piscol(markb);
		reversed = 0;
	}
	bw->cursor->xcol = piscol(bw->cursor);
	if(reversed) {
		if (!pisbol(bw->cursor))
			u_goto_prev(bw), bw->cursor->xcol = piscol(bw->cursor);
		umarkb(bw);
	} else {
		if (!piseol(bw->cursor))
			u_goto_next(bw), bw->cursor->xcol = piscol(bw->cursor);
		umarkk(bw);
	}
	return 0;
}

int udefm2up(BW *bw)
{
	return 0;
}

int udefm3down(BW *xx)
{
	BW *bw;
	if (utomouse(xx))
		return -1;
	if ((maint->curwin->watom->what & (TYPEPW | TYPETW)) == 0)
		return 0;
	bw = (BW *)maint->curwin->object;
	/* set anchor to beginning of line, anchorn to beginning of next line */
	p_goto_bol(bw->cursor); bw->cursor->xcol = piscol(bw->cursor);
	anchor = bw->cursor->byte; umarkb(bw);
	umarkk(bw); pnextl(markk); anchorn = markk->byte;
	reversed = 0;
	bw->cursor->xcol = piscol(bw->cursor);
	selecting = 1;
	return 0;
}

int udefm3drag(BW *xx)
{
	BW *bw = (BW *)maint->curwin->object;
	if (tomousestay())
		return -1;
	if (!reversed && bw->cursor->byte < anchor) {
		pgoto(markk,anchorn);
		markk->xcol = piscol(markk);
		reversed = 1;
	} else if (reversed && bw->cursor->byte > anchorn) {
		pgoto(markb,anchor);
		markb->xcol = piscol(markb);
		reversed = 0;
	}
	p_goto_bol(bw->cursor);
	bw->cursor->xcol = piscol(bw->cursor);
	if(reversed)
		umarkb(bw), markb->xcol = piscol(markb);
	else
		umarkk(bw), pnextl(markk), markk->xcol = piscol(markk);
	return 0;
}

int udefm3up(BW *bw)
{
	return 0;
}

void mouseopen()
{
#ifdef MOUSE_XTERM
	if (usexmouse) {
		ttputs(USTR "\33[?1002h");
		ttputs(USTR "\33[?1006h");
		if (joexterm)
			ttputs(USTR "\33[?2007h");
		ttflsh();
	}
#endif
}

void mouseclose()
{
#ifdef MOUSE_XTERM
	if (usexmouse) {
		if (joexterm)
			ttputs(USTR "\33[?2007l");
		ttputs(USTR "\33[?1006l");
		ttputs(USTR "\33[?1002l");
		ttflsh();
	}
#endif
}
