/*
 *	Edit buffer window generation
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* Display modes */
int dspasis = 0;
int marking = 0;

static P *getto(P *p, P *cur, P *top, long int line)
{

	if (p == NULL) {
		P *best = cur;
		long dist = MAXLONG;
		long d;

		d = (line >= cur->line ? line - cur->line : cur->line - line);
		if (d < dist) {
			dist = d;
			best = cur;
		}
		d = (line >= top->line ? line - top->line : top->line - line);
		if (d < dist) {
			dist = d;
			best = top;
		}
		p = pdup(best, USTR "getto");
		p_goto_bol(p);
	}
	while (line > p->line)
		if (!pnextl(p))
			break;
	if (line < p->line) {
		while (line < p->line)
			pprevl(p);
		p_goto_bol(p);
	}
	return p;
}

/* Scroll window to follow cursor */

int mid = 0;

/* For hex */

void bwfllwh(BW *w)
{
	/* Top must be a muliple of 16 bytes */
	if (w->top->byte%16) {
		pbkwd(w->top,w->top->byte%16);
	}

	/* Move backward */
	if (w->cursor->byte < w->top->byte) {
		long new_top = w->cursor->byte/16;
		if (mid) {
			if (new_top >= w->h / 2)
				new_top -= w->h / 2;
			else
				new_top = 0;
		}
		if (w->top->byte/16 - new_top < w->h)
			nscrldn(w->t->t, w->y, w->y + w->h, (int) (w->top->byte/16 - new_top));
		else
			msetI(w->t->t->updtab + w->y, 1, w->h);
		pgoto(w->top,new_top*16);
	}

	/* Move forward */
	if (w->cursor->byte >= w->top->byte+(w->h*16)) {
		long new_top;
		if (mid) {
			new_top = w->cursor->byte/16 - w->h / 2;
		} else {
			new_top = w->cursor->byte/16 - (w->h - 1);
		}
		if (new_top - w->top->byte/16 < w->h)
			nscrlup(w->t->t, w->y, w->y + w->h, (int) (new_top - w->top->byte/16));
		else {
			msetI(w->t->t->updtab + w->y, 1, w->h);
		}
		pgoto(w->top, new_top*16);
	}

	/* Adjust scroll offset */
	if (w->cursor->byte%16+60 < w->offset) {
		w->offset = w->cursor->byte%16+60;
		msetI(w->t->t->updtab + w->y, 1, w->h);
	} else if (w->cursor->byte%16+60 >= w->offset + w->w) {
		w->offset = w->cursor->byte%16+60 - (w->w - 1);
		msetI(w->t->t->updtab + w->y, 1, w->h);
	}
}

/* For text */

void bwfllwt(BW *w)
{
	P *newtop;

	if (!pisbol(w->top)) {
		p_goto_bol(w->top);
	}

	if (w->cursor->line < w->top->line) {
		newtop = pdup(w->cursor, USTR "bwfllwt");
		p_goto_bol(newtop);
		if (mid) {
			if (newtop->line >= w->h / 2)
				pline(newtop, newtop->line - w->h / 2);
			else
				pset(newtop, newtop->b->bof);
		}
		if (w->top->line - newtop->line < w->h)
			nscrldn(w->t->t, w->y, w->y + w->h, (int) (w->top->line - newtop->line));
		else {
			msetI(w->t->t->updtab + w->y, 1, w->h);
		}
		pset(w->top, newtop);
		prm(newtop);
	} else if (w->cursor->line >= w->top->line + w->h) {
		/* newtop = pdup(w->top); */
		/* getto() creates newtop */
		if (mid)
			newtop = getto(NULL, w->cursor, w->top, w->cursor->line - w->h / 2);
		else
			newtop = getto(NULL, w->cursor, w->top, w->cursor->line - (w->h - 1));
		if (newtop->line - w->top->line < w->h)
			nscrlup(w->t->t, w->y, w->y + w->h, (int) (newtop->line - w->top->line));
		else {
			msetI(w->t->t->updtab + w->y, 1, w->h);
		}
		pset(w->top, newtop);
		prm(newtop);
	}

/* Adjust column */
	if (w->cursor->xcol < w->offset) {
		long target = w->cursor->xcol;
		if (target < 5)
			target = 0;
		else {
			target -= 5;
			target -= (target % 5);
		}
		w->offset = target;
		msetI(w->t->t->updtab + w->y, 1, w->h);
	}
	if (w->cursor->xcol >= w->offset + w->w) {
		w->offset = w->cursor->xcol - (w->w - 1);
		msetI(w->t->t->updtab + w->y, 1, w->h);
	}
}

/* For either */

void bwfllw(BW *w)
{
	if (w->o.hex)
		bwfllwh(w);
	else
		bwfllwt(w);
}

/* Determine highlighting state of a particular line on the window.
   If the state is not known, it is computed and the state for all
   of the remaining lines of the window are also recalculated. */

HIGHLIGHT_STATE get_highlight_state(BW *w, P *p, int line)
{
	HIGHLIGHT_STATE state;

	if(!w->o.highlight || !w->o.syntax) {
		invalidate_state(&state);
		return state;
	}

	return lattr_get(w->db, w->o.syntax, p, line);

	/* Old way... */
#ifdef junk

	ln = line;

	lattr_get(w->db, &ln, &state);

	if (ln != line) {
		tmp = pdup(p, USTR "get_highlight_state");
		pline(tmp, ln);
		while (tmp->line < line && !piseof(tmp)) {
			state = parse(w->o.syntax, tmp, state);
		}
		if (tmp->line == line)
			lattr_set(w->db, line, state);
		prm(tmp);
	}
	return state;
#endif

#ifdef junk
	P *tmp = 0;
	HIGHLIGHT_STATE state;

	/* Screen y position of requested line */
	int y = line-w->top->line+w->y;

	if(!w->o.highlight || !w->o.syntax) {
		invalidate_state(&state);
		return state;
	}

	/* If we know the state, just return it */
	if (w->parent->t->t->syntab[y].state>=0)
		return w->parent->t->t->syntab[y];

	/* Scan upwards until we have a line with known state or
	   we're on the first line */
	while (y > w->y && w->parent->t->t->syntab[y].state < 0) --y;

	/* If we don't have state for this line, calculate by going 100 lines back */
	if (w->parent->t->t->syntab[y].state<0) {
		/* We must be on the top line */
		clear_state(&state);
		tmp = pdup(w->top, USTR "get_highlight_state");
		if(w->o.syntax->sync_lines >= 0 && tmp->line > w->o.syntax->sync_lines)
			pline(tmp, tmp->line-w->o.syntax->sync_lines);
		else
			p_goto_bof(tmp);
		while(tmp->line!=y-w->y+w->top->line)
			state = parse(w->o.syntax,tmp,state);
		w->parent->t->t->syntab[y] = state;
		w->parent->t->t->updtab[y] = 1;
		prm(tmp);
	}

	/* Color to end of screen */
	tmp = pdup(w->top, USTR "get_highlight_state");
	pline(tmp, y-w->y+w->top->line);
	state = w->parent->t->t->syntab[y];
	while(tmp->line!=w->top->line+w->h-1 && !piseof(tmp)) {
		state = parse(w->o.syntax,tmp,state);
		w->parent->t->t->syntab[++y] = state;
		w->parent->t->t->updtab[y] = 1; /* This could be smarter: update only if we changed what was there before */
		}
	prm(tmp);
	while(y<w->y+w->h-1) {
		w->parent->t->t->syntab[++y] = state;
		}

	/* Line after window */
	/* state = parse_c(state,syn,tmp); */

	/* If we changed, fix other windows */
	/* w->state = state; */

	/* Return state of requested line */
	y = line - w->top->line + w->y;
	return w->parent->t->t->syntab[y];
#endif
}

/* Scroll a buffer window after an insert occured.  'flg' is set to 1 if
 * the first line was split
 */

void bwins(BW *w, long int l, long int n, int flg)
{
	/* If highlighting is enabled... */
	if (w->o.highlight && w->o.syntax) {
		/* Invalidate cache */
		/* lattr_cut(w->db, l + 1); */
		/* Force updates */
		if (l < w->top->line) {
			msetI(w->t->t->updtab + w->y, 1, w->h);
		} else if ((l + 1) < w->top->line + w->h) {
			int start = l + 1 - w->top->line;
			int size = w->h - start;
			msetI(w->t->t->updtab + w->y + start, 1, size);
		}
	}

	/* Scroll */
	if (l + flg + n < w->top->line + w->h && l + flg >= w->top->line && l + flg <= w->b->eof->line) {
		if (flg)
			w->t->t->sary[w->y + l - w->top->line] = w->t->t->li;
		nscrldn(w->t->t, (int) (w->y + l + flg - w->top->line), w->y + w->h, (int) n);
	}

	/* Force update of lines in opened hole */
	if (l < w->top->line + w->h && l >= w->top->line) {
		if (n >= w->h - (l - w->top->line)) {
			msetI(w->t->t->updtab + w->y + l - w->top->line, 1, w->h - (int) (l - w->top->line));
		} else {
			msetI(w->t->t->updtab + w->y + l - w->top->line, 1, (int) n + 1);
		}
	}
}

/* Scroll current windows after a delete */

void bwdel(BW *w, long int l, long int n, int flg)
{
	/* If highlighting is enabled... */
	if (w->o.highlight && w->o.syntax) {
		/* lattr_cut(w->db, l + 1); */
		if (l < w->top->line) {
			msetI(w->t->t->updtab + w->y, 1, w->h);
		} else if ((l + 1) < w->top->line + w->h) {
			int start = l + 1 - w->top->line;
			int size = w->h - start;
			msetI(w->t->t->updtab + w->y + start, 1, size);
		}
	}

	/* Update the line where the delete began */
	if (l < w->top->line + w->h && l >= w->top->line)
		w->t->t->updtab[w->y + l - w->top->line] = 1;

	/* Update the line where the delete ended */
	if (l + n < w->top->line + w->h && l + n >= w->top->line)
		w->t->t->updtab[w->y + l + n - w->top->line] = 1;

	if (l < w->top->line + w->h && (l + n >= w->top->line + w->h || (l + n == w->b->eof->line && w->b->eof->line >= w->top->line + w->h))) {
		if (l >= w->top->line)
			/* Update window from l to end */
			msetI(w->t->t->updtab + w->y + l - w->top->line, 1, w->h - (int) (l - w->top->line));
		else
			/* Update entire window */
			msetI(w->t->t->updtab + w->y, 1, w->h);
	} else if (l < w->top->line + w->h && l + n == w->b->eof->line && w->b->eof->line < w->top->line + w->h) {
		if (l >= w->top->line)
			/* Update window from l to end of file */
			msetI(w->t->t->updtab + w->y + l - w->top->line, 1, (int) n);
		else
			/* Update from beginning of window to end of file */
			msetI(w->t->t->updtab + w->y, 1, (int) (w->b->eof->line - w->top->line));
	} else if (l + n < w->top->line + w->h && l + n > w->top->line && l + n < w->b->eof->line) {
		if (l + flg >= w->top->line)
			nscrlup(w->t->t, (int) (w->y + l + flg - w->top->line), w->y + w->h, (int) n);
		else
			nscrlup(w->t->t, w->y, w->y + w->h, (int) (l + n - w->top->line));
	}
}

struct ansi_sm
{
	int state;
};

int ansi_decode(struct ansi_sm *sm, int bc)
{
	if (sm->state) {
		if ((bc >= 'a' && bc <= 'z') || (bc >= 'A' && bc <= 'Z'))
			sm->state = 0;
		return -1;
	} else if (bc == '\033') {
		sm->state = 1;
		return -1;
	} else
		return bc;
}

void ansi_init(struct ansi_sm *sm)
{
	sm->state = 0;
}

/* Update a single line */

static int lgen(SCRN *t, int y, int *screen, int *attr, int x, int w, P *p, long int scr, long int from, long int to,HIGHLIGHT_STATE st,BW *bw)
        
      
            			/* Screen line address */
      				/* Window */
     				/* Buffer pointer */
         			/* Starting column to display */
              			/* Range for marked block */
{
	int ansi = bw->o.ansi;
	int ox = x;
	int tach;
	int done = 1;
	long col = 0;
	long byte = p->byte;
	unsigned char *bp;	/* Buffer pointer, 0 if not set */
	int amnt;		/* Amount left in this segment of the buffer */
	int c, ta, c1;
	unsigned char bc;
	int ungetit = -1;

	struct utf8_sm utf8_sm;
	struct ansi_sm ansi_sm;

        int *syn = 0;
        P *tmp;
        int idx=0;
        int atr = BG_COLOR(bg_text); 

	utf8_init(&utf8_sm);
	ansi_init(&ansi_sm);

	if(st.state!=-1) {
		tmp=pdup(p, USTR "lgen");
		p_goto_bol(tmp);
		parse(bw->o.syntax,tmp,st);
		syn = attr_buf;
		prm(tmp);
	}

/* Initialize bp and amnt from p */
	if (p->ofst >= p->hdr->hole) {
		bp = p->ptr + p->hdr->ehole + p->ofst - p->hdr->hole;
		amnt = SEGSIZ - p->hdr->ehole - (p->ofst - p->hdr->hole);
	} else {
		bp = p->ptr + p->ofst;
		amnt = p->hdr->hole - p->ofst;
	}

	if (col == scr)
		goto loop;
      lp:			/* Display next character */
	if (amnt)
		do {
			if (ungetit== -1)
				bc = *bp++;
			else {
				bc = ungetit;
				ungetit = -1;
			}
			if(st.state!=-1) {
				atr = syn[idx++] & ~CONTEXT_MASK;
				if (!((atr & BG_VALUE) >> BG_SHIFT))
					atr |= BG_COLOR(bg_text);
			}
			if (p->b->o.crlf && bc == '\r') {
				++byte;
				if (!--amnt) {
				      pppl:
					if (bp == p->ptr + SEGSIZ) {
						if (pnext(p)) {
							bp = p->ptr;
							amnt = p->hdr->hole;
						} else
							goto nnnl;
					} else {
						bp = p->ptr + p->hdr->ehole;
						amnt = SEGSIZ - p->hdr->ehole;
						if (!amnt)
							goto pppl;
					}
				}
				if (*bp == '\n') {
					++bp;
					++byte;
					++amnt;
					goto eobl;
				}
			      nnnl:
				--byte;
				++amnt;
			}
			if (square)
				if (bc == '\t') {
					long tcol = col + p->b->o.tab - col % p->b->o.tab;

					if (tcol > from && tcol <= to)
						c1 = INVERSE;
					else
						c1 = 0;
				} else if (col >= from && col < to)
					c1 = INVERSE;
				else
					c1 = 0;
			else if (byte >= from && byte < to)
				c1 = INVERSE;
			else
				c1 = 0;
			++byte;
			if (bc == '\t') {
				ta = p->b->o.tab - col % p->b->o.tab;
				if (ta + col > scr) {
					ta -= scr - col;
					tach = ' ';
					goto dota;
				}
				if ((col += ta) == scr) {
					--amnt;
					goto loop;
				}
			} else if (bc == '\n')
				goto eobl;
			else {
				int wid = 1;
				if (p->b->o.charmap->type) {
					c = utf8_decode(&utf8_sm,bc);

					if (c>=0) /* Normal decoded character */
						if (ansi) {
							c = ansi_decode(&ansi_sm, c);
							if (c >= 0) /* Not ansi */
								wid = joe_wcwidth(1, c);
							else { /* Skip ansi character */
								wid = 0;
								++idx;
							}
						} else
							wid = joe_wcwidth(1,c);
					else if(c== -1) /* Character taken */
						wid = -1;
					else if(c== -2) { /* Incomplete sequence (FIXME: do something better here) */
						wid = 1;
						ungetit = c;
						++amnt;
						--byte;
					}
					else if(c== -3) /* Control character 128-191, 254, 255 */
						wid = 1;
				} else {
					if (ansi) {
						c = ansi_decode(&ansi_sm, bc);
						if (c>=0) /* Not ansi */
							wid = 1;
						else {
							wid = 0;
							++idx;
						}
					} else {
						wid = 1;
					}
				}

				if(wid>0) {
					col += wid;
					if (col == scr) {
						--amnt;
						goto loop;
					} else if (col > scr) {
						ta = col - scr;
						tach = '<';
						goto dota;
					}
				} else
					--idx;	/* Get highlighting character again.. */
			}
		} while (--amnt);
	if (bp == p->ptr + SEGSIZ) {
		if (pnext(p)) {
			bp = p->ptr;
			amnt = p->hdr->hole;
			goto lp;
		}
	} else {
		bp = p->ptr + p->hdr->ehole;
		amnt = SEGSIZ - p->hdr->ehole;
		goto lp;
	}
	goto eof;

      loop:			/* Display next character */
	if (amnt)
		do {
			if (ungetit== -1)
				bc = *bp++;
			else {
				bc = ungetit;
				ungetit = -1;
			}
			if(st.state!=-1) {
				atr = syn[idx++] & ~CONTEXT_MASK;
				if (!(atr & BG_MASK))
					atr |= BG_COLOR(bg_text);
			}
			if (p->b->o.crlf && bc == '\r') {
				++byte;
				if (!--amnt) {
				      ppl:
					if (bp == p->ptr + SEGSIZ) {
						if (pnext(p)) {
							bp = p->ptr;
							amnt = p->hdr->hole;
						} else
							goto nnl;
					} else {
						bp = p->ptr + p->hdr->ehole;
						amnt = SEGSIZ - p->hdr->ehole;
						if (!amnt)
							goto ppl;
					}
				}
				if (*bp == '\n') {
					++bp;
					++byte;
					++amnt;
					goto eobl;
				}
			      nnl:
				--byte;
				++amnt;
			}
			if (square)
				if (bc == '\t') {
					long tcol = scr + x - ox + p->b->o.tab - (scr + x - ox) % p->b->o.tab;

					if (tcol > from && tcol <= to)
						c1 = INVERSE;
					else
						c1 = 0;
				} else if (scr + x - ox >= from && scr + x - ox < to)
					c1 = INVERSE;
				else
					c1 = 0;
			else if (byte >= from && byte < to)
				c1 = INVERSE;
			else
				c1 = 0;
			++byte;
			if (bc == '\t') {
				ta = p->b->o.tab - ((x - ox + scr) % p->b->o.tab);
				tach = ' ';
			      dota:
				do {
					outatr(bw->b->o.charmap, t, screen + x, attr + x, x, y, tach, c1|atr);
					if (ifhave)
						goto bye;
					if (++x == w)
						goto eosl;
				} while (--ta);
			} else if (bc == '\n')
				goto eobl;
			else {
				int wid = -1;
				int utf8_char;
				if (p->b->o.charmap->type) { /* UTF-8 */

					utf8_char = utf8_decode(&utf8_sm,bc);

					if (utf8_char >= 0) { /* Normal decoded character */
						if (ansi) {
							utf8_char = ansi_decode(&ansi_sm, utf8_char);
							if (utf8_char >= 0) {
								wid = joe_wcwidth(1, utf8_char);
							} else {
								wid = -1;
								++idx;
							}
						} else
							wid = joe_wcwidth(1,utf8_char);
					} else if(utf8_char== -1) { /* Character taken */
						wid = -1;
					} else if(utf8_char== -2) { /* Incomplete sequence (FIXME: do something better here) */
						ungetit = bc;
						++amnt;
						--byte;
						utf8_char = 'X';
						wid = 1;
					} else if(utf8_char== -3) { /* Invalid UTF-8 start character 128-191, 254, 255 */
						/* Show as control character */
						wid = 1;
						utf8_char = 'X';
					}
				} else { /* Regular */
					if (ansi) {
						utf8_char = ansi_decode(&ansi_sm, bc);
						if (utf8_char >= 0) /* Not ansi */
							wid = 1;
						else {
							wid = -1;
							++idx;
						}
					} else {
						utf8_char = bc;
						wid = 1;
					}
				}

				if(wid>=0) {
					if (x+wid > w) {
						/* If character hits right most column, don't display it */
						while (x < w) {
							outatr(bw->b->o.charmap, t, screen + x, attr + x, x, y, '>', c1|atr);
							x++;
						}
					} else {
						outatr(bw->b->o.charmap, t, screen + x, attr + x, x, y, utf8_char, c1|atr);
						x += wid;
					}
				} else
					--idx;

				if (ifhave)
					goto bye;
				if (x >= w)
					goto eosl;
			}
		} while (--amnt);
	if (bp == p->ptr + SEGSIZ) {
		if (pnext(p)) {
			bp = p->ptr;
			amnt = p->hdr->hole;
			goto loop;
		}
	} else {
		bp = p->ptr + p->hdr->ehole;
		amnt = SEGSIZ - p->hdr->ehole;
		goto loop;
	}
	goto eof;

      eobl:			/* End of buffer line found.  Erase to end of screen line */
	++p->line;
      eof:
	if (x != w)
		done = eraeol(t, x, y, BG_COLOR(bg_text));
	else
		done = 0;

/* Set p to bp/amnt */
      bye:
	if (bp - p->ptr <= p->hdr->hole)
		p->ofst = bp - p->ptr;
	else
		p->ofst = bp - p->ptr - (p->hdr->ehole - p->hdr->hole);
	p->byte = byte;
	return done;

      eosl:
	if (bp - p->ptr <= p->hdr->hole)
		p->ofst = bp - p->ptr;
	else
		p->ofst = bp - p->ptr - (p->hdr->ehole - p->hdr->hole);
	p->byte = byte;
	pnextl(p);
	return 0;
}

/* Generate line into an array */
#ifdef junk
static int lgena(SCRN *t, int y, int *screen, int x, int w, P *p, long int scr, long int from, long int to)
        
      
            			/* Screen line address */
      				/* Window */
     				/* Buffer pointer */
         			/* Starting column to display */
              			/* Range for marked block */
{
	int ox = x;
	int done = 1;
	long col = 0;
	long byte = p->byte;
	unsigned char *bp;	/* Buffer pointer, 0 if not set */
	int amnt;		/* Amount left in this segment of the buffer */
	int c, ta, c1;
	unsigned char bc;

/* Initialize bp and amnt from p */
	if (p->ofst >= p->hdr->hole) {
		bp = p->ptr + p->hdr->ehole + p->ofst - p->hdr->hole;
		amnt = SEGSIZ - p->hdr->ehole - (p->ofst - p->hdr->hole);
	} else {
		bp = p->ptr + p->ofst;
		amnt = p->hdr->hole - p->ofst;
	}

	if (col == scr)
		goto loop;
      lp:			/* Display next character */
	if (amnt)
		do {
			bc = *bp++;
			if (square)
				if (bc == '\t') {
					long tcol = col + p->b->o.tab - col % p->b->o.tab;

					if (tcol > from && tcol <= to)
						c1 = INVERSE;
					else
						c1 = 0;
				} else if (col >= from && col < to)
					c1 = INVERSE;
				else
					c1 = 0;
			else if (byte >= from && byte < to)
				c1 = INVERSE;
			else
				c1 = 0;
			++byte;
			if (bc == '\t') {
				ta = p->b->o.tab - col % p->b->o.tab;
				if (ta + col > scr) {
					ta -= scr - col;
					goto dota;
				}
				if ((col += ta) == scr) {
					--amnt;
					goto loop;
				}
			} else if (bc == '\n')
				goto eobl;
			else if (++col == scr) {
				--amnt;
				goto loop;
			}
		} while (--amnt);
	if (bp == p->ptr + SEGSIZ) {
		if (pnext(p)) {
			bp = p->ptr;
			amnt = p->hdr->hole;
			goto lp;
		}
	} else {
		bp = p->ptr + p->hdr->ehole;
		amnt = SEGSIZ - p->hdr->ehole;
		goto lp;
	}
	goto eobl;

      loop:			/* Display next character */
	if (amnt)
		do {
			bc = *bp++;
			if (square)
				if (bc == '\t') {
					long tcol = scr + x - ox + p->b->o.tab - (scr + x - ox) % p->b->o.tab;

					if (tcol > from && tcol <= to)
						c1 = INVERSE;
					else
						c1 = 0;
				} else if (scr + x - ox >= from && scr + x - ox < to)
					c1 = INVERSE;
				else
					c1 = 0;
			else if (byte >= from && byte < to)
				c1 = INVERSE;
			else
				c1 = 0;
			++byte;
			if (bc == '\t') {
				ta = p->b->o.tab - ((x - ox + scr) % p->b->o.tab);
			      dota:
				do {
					screen[x] = ' ' + c1;
					if (++x == w)
						goto eosl;
				} while (--ta);
			} else if (bc == '\n')
				goto eobl;
			else {
				/* xlat(&c, &bc);*/
				c ^= c1;
				screen[x] = c + bc;
				if (++x == w)
					goto eosl;
			}
		} while (--amnt);
	if (bp == p->ptr + SEGSIZ) {
		if (pnext(p)) {
			bp = p->ptr;
			amnt = p->hdr->hole;
			goto loop;
		}
	} else {
		bp = p->ptr + p->hdr->ehole;
		amnt = SEGSIZ - p->hdr->ehole;
		goto loop;
	}
	goto eof;
      eobl:			/* End of buffer line found.  Erase to end of screen line */
	++p->line;
      eof:
	while (x != w)
		screen[x++] = ' ';
	done = 0;

/* Set p to bp/amnt */
	if (bp - p->ptr <= p->hdr->hole)
		p->ofst = bp - p->ptr;
	else
		p->ofst = bp - p->ptr - (p->hdr->ehole - p->hdr->hole);
	p->byte = byte;
	return done;

      eosl:
	if (bp - p->ptr <= p->hdr->hole)
		p->ofst = bp - p->ptr;
	else
		p->ofst = bp - p->ptr - (p->hdr->ehole - p->hdr->hole);
	p->byte = byte;
	pnextl(p);
	return 0;
}
#endif

static void gennum(BW *w, int *screen, int *attr, SCRN *t, int y, int *comp)
{
	unsigned char buf[12];
	int z;
	int lin = w->top->line + y - w->y;

	if (lin <= w->b->eof->line)
		joe_snprintf_1(buf, sizeof(buf), "%9ld ", w->top->line + y - w->y + 1);
	else {
		int x;
		for (x = 0; x != LINCOLS; ++x)
			buf[x] = ' ';
		buf[x] = 0;
	}
	for (z = 0; buf[z]; ++z) {
		outatr(w->b->o.charmap, t, screen + z, attr + z, z, y, buf[z], BG_COLOR(bg_text)); 
		if (ifhave)
			return;
		comp[z] = buf[z];
	}
}

void bwgenh(BW *w)
{
	int *screen;
	int *attr;
	P *q = pdup(w->top, USTR "bwgenh");
	int bot = w->h + w->y;
	int y;
	SCRN *t = w->t->t;
	int flg = 0;
	long from;
	long to;
	int dosquare = 0;

	from = to = 0;

	if (markv(0) && markk->b == w->b)
		if (square) {
			from = markb->xcol;
			to = markk->xcol;
			dosquare = 1;
		} else {
			from = markb->byte;
			to = markk->byte;
		}
	else if (marking && w == (BW *)maint->curwin->object && markb && markb->b == w->b && w->cursor->byte != markb->byte && !from) {
		if (square) {
			from = long_min(w->cursor->xcol, markb->xcol);
			to = long_max(w->cursor->xcol, markb->xcol);
			dosquare = 1;
		} else {
			from = long_min(w->cursor->byte, markb->byte);
			to = long_max(w->cursor->byte, markb->byte);
		}
	}

	if (marking && w == (BW *)maint->curwin->object)
		msetI(t->updtab + w->y, 1, w->h);

	if (dosquare) {
		from = 0;
		to = 0;
	}

	y=w->y;
	attr = t->attr + y*w->t->w;
	for (screen = t->scrn + y * w->t->w; y != bot; ++y, (screen += w->t->w), (attr += w->t->w)) {
		unsigned char txt[80];
		int fmt[80];
		unsigned char bf[16];
		int x;
		memset(txt,' ',76);
		msetI(fmt,BG_COLOR(bg_text),76);
		txt[76]=0;
		if (!flg) {
#if HAVE_LONG_LONG
			sprintf((char *)bf,"%8llx ",(unsigned long long)q->byte);
#else
			sprintf((char *)bf,"%8lx ",(unsigned long)q->byte);
#endif
			memcpy(txt,bf,9);
			for (x=0; x!=8; ++x) {
				int c;
				if (q->byte==w->cursor->byte && !flg) {
					fmt[10+x*3] |= INVERSE;
					fmt[10+x*3+1] |= INVERSE;
				}
				if (q->byte>=from && q->byte<to && !flg) {
					fmt[10+x*3] |= UNDERLINE;
					fmt[10+x*3+1] |= UNDERLINE;
					fmt[60+x] |= INVERSE;
				}
				c = pgetb(q);
				if (c >= 0) {
					sprintf((char *)bf,"%2.2x",c);
					txt[10+x*3] = bf[0];
					txt[10+x*3+1] = bf[1];
					if (c >= 0x20 && c <= 0x7E)
						txt[60+x] = c;
					else
						txt[60+x] = '.';
				} else
					flg = 1;
			}
			for (x=8; x!=16; ++x) {
				int c;
				if (q->byte==w->cursor->byte && !flg) {
					fmt[11+x*3] |= INVERSE;
					fmt[11+x*3+1] |= INVERSE;
				}
				if (q->byte>=from && q->byte<to && !flg) {
					fmt[11+x*3] |= UNDERLINE;
					fmt[11+x*3+1] |= UNDERLINE;
					fmt[60+x] |= INVERSE;
				}
				c = pgetb(q);
				if (c >= 0) {
					sprintf((char *)bf,"%2.2x",c);
					txt[11+x*3] = bf[0];
					txt[11+x*3+1] = bf[1];
					if (c >= 0x20 && c <= 0x7E)
						txt[60+x] = c;
					else
						txt[60+x] = '.';
				} else
					flg = 1;
			}
		}
		genfield(t, screen, attr, 0, y, w->offset, txt, 76, 0, w->w, 1, fmt);
	}
	prm(q);
}

void bwgen(BW *w, int linums)
{
	int *screen;
	int *attr;
	P *p = NULL;
	P *q;
	int bot = w->h + w->y;
	int y;
	int dosquare = 0;
	long from, to;
	long fromline, toline;
	SCRN *t = w->t->t;

	/* Set w.db to correct value */
	if (w->o.highlight && w->o.syntax && (!w->db || w->db->syn != w->o.syntax))
		w->db = find_lattr_db(w->b, w->o.syntax);

	fromline = toline = from = to = 0;

	if (w->b == errbuf && w->b->err) {
		P *tmp = pdup(w->b->err, USTR "bwgen");
		p_goto_bol(tmp);
		from = tmp->byte;
		pnextl(tmp);
		to = tmp->byte;
		prm(tmp);
	} else if (markv(0) && markk->b == w->b)
		if (square) {
			from = markb->xcol;
			to = markk->xcol;
			dosquare = 1;
			fromline = markb->line;
			toline = markk->line;
		} else {
			from = markb->byte;
			to = markk->byte;
		}
	else if (marking && w == (BW *)maint->curwin->object && markb && markb->b == w->b && w->cursor->byte != markb->byte && !from) {
		if (square) {
			from = long_min(w->cursor->xcol, markb->xcol);
			to = long_max(w->cursor->xcol, markb->xcol);
			fromline = long_min(w->cursor->line, markb->line);
			toline = long_max(w->cursor->line, markb->line);
			dosquare = 1;
		} else {
			from = long_min(w->cursor->byte, markb->byte);
			to = long_max(w->cursor->byte, markb->byte);
		}
	}

	if (marking && w == (BW *)maint->curwin->object)
		msetI(t->updtab + w->y, 1, w->h);

	q = pdup(w->cursor, USTR "bwgen");

	y = w->cursor->line - w->top->line + w->y;
	attr = t->attr + y*w->t->w;
	for (screen = t->scrn + y * w->t->w; y != bot; ++y, (screen += w->t->w), (attr += w->t->w)) {
		if (ifhave && !linums)
			break;
		if (linums)
			gennum(w, screen, attr, t, y, t->compose);
		if (t->updtab[y]) {
			p = getto(p, w->cursor, w->top, w->top->line + y - w->y);
/*			if (t->insdel && !w->x) {
				pset(q, p);
				if (dosquare)
					if (w->top->line + y - w->y >= fromline && w->top->line + y - w->y <= toline)
						lgena(t, y, t->compose, w->x, w->x + w->w, q, w->offset, from, to);
					else
						lgena(t, y, t->compose, w->x, w->x + w->w, q, w->offset, 0L, 0L);
				else
					lgena(t, y, t->compose, w->x, w->x + w->w, q, w->offset, from, to);
				magic(t, y, screen, attr, t->compose, (int) (w->cursor->xcol - w->offset + w->x));
			} */
			if (dosquare)
				if (w->top->line + y - w->y >= fromline && w->top->line + y - w->y <= toline)
					t->updtab[y] = lgen(t, y, screen, attr, w->x, w->x + w->w, p, w->offset, from, to, get_highlight_state(w,p,w->top->line+y-w->y),w);
				else
					t->updtab[y] = lgen(t, y, screen, attr, w->x, w->x + w->w, p, w->offset, 0L, 0L, get_highlight_state(w,p,w->top->line+y-w->y),w);
			else
				t->updtab[y] = lgen(t, y, screen, attr, w->x, w->x + w->w, p, w->offset, from, to, get_highlight_state(w,p,w->top->line+y-w->y),w);
		}
	}

	y = w->y;
	attr = t->attr + w->y * w->t->w;
	for (screen = t->scrn + w->y * w->t->w; y != w->y + w->cursor->line - w->top->line; ++y, (screen += w->t->w), (attr += w->t->w)) {
		if (ifhave && !linums)
			break;
		if (linums)
			gennum(w, screen, attr, t, y, t->compose);
		if (t->updtab[y]) {
			p = getto(p, w->cursor, w->top, w->top->line + y - w->y);
/*			if (t->insdel && !w->x) {
				pset(q, p);
				if (dosquare)
					if (w->top->line + y - w->y >= fromline && w->top->line + y - w->y <= toline)
						lgena(t, y, t->compose, w->x, w->x + w->w, q, w->offset, from, to);
					else
						lgena(t, y, t->compose, w->x, w->x + w->w, q, w->offset, 0L, 0L);
				else
					lgena(t, y, t->compose, w->x, w->x + w->w, q, w->offset, from, to);
				magic(t, y, screen, attr, t->compose, (int) (w->cursor->xcol - w->offset + w->x));
			} */
			if (dosquare)
				if (w->top->line + y - w->y >= fromline && w->top->line + y - w->y <= toline)
					t->updtab[y] = lgen(t, y, screen, attr, w->x, w->x + w->w, p, w->offset, from, to, get_highlight_state(w,p,w->top->line+y-w->y),w);
				else
					t->updtab[y] = lgen(t, y, screen, attr, w->x, w->x + w->w, p, w->offset, 0L, 0L, get_highlight_state(w,p,w->top->line+y-w->y),w);
			else
				t->updtab[y] = lgen(t, y, screen, attr, w->x, w->x + w->w, p, w->offset, from, to, get_highlight_state(w,p,w->top->line+y-w->y),w);
		}
	}
	prm(q);
	if (p)
		prm(p);
}

void bwmove(BW *w, int x, int y)
{
	w->x = x;
	w->y = y;
}

void bwresz(BW *w, int wi, int he)
{
	if (he > w->h && w->y != -1) {
		msetI(w->t->t->updtab + w->y + w->h, 1, he - w->h);
	}
	w->w = wi;
	w->h = he;
	if (w->b->vt && w->b->pid && w == vtmaster(w->parent->t, w->b)) {
		vt_resize(w->b->vt, w->top, he, wi);
		ttstsz(w->b->out, wi, he);
	}
}

BW *bwmk(W *window, B *b, int prompt)
{
	BW *w = (BW *) joe_malloc(sizeof(BW));

	w->parent = window;
	w->b = b;
	if (prompt || (!window->y && staen)) {
		w->y = window->y;
		w->h = window->h;
	} else {
		w->y = window->y + 1;
		w->h = window->h - 1;
	}
	if (b->oldcur) {
		w->top = b->oldtop;
		b->oldtop = NULL;
		w->top->owner = NULL;
		w->cursor = b->oldcur;
		b->oldcur = NULL;
		w->cursor->owner = NULL;
	} else {
		w->top = pdup(b->bof, USTR "bwmk");
		w->cursor = pdup(b->bof, USTR "bwmk");
	}
	w->t = window->t;
	w->object = NULL;
	w->offset = 0;
	w->o = w->b->o;
	if (w->o.linums) {
		w->x = window->x + LINCOLS;
		w->w = window->w - LINCOLS;
	} else {
		w->x = window->x;
		w->w = window->w;
	}
	if (window == window->main) {
		rmkbd(window->kbd);
		window->kbd = mkkbd(kmap_getcontext(w->o.context));
	}
	w->top->xcol = 0;
	w->cursor->xcol = 0;
	w->linums = 0;
	w->top_changed = 1;
	w->linums = 0;
	w->db = 0;
	w->shell_flag = 0;
	return w;
}

/* Database of last file positions */

#define MAX_FILE_POS 20 /* Maximum number of file positions we track */

static struct file_pos {
	LINK(struct file_pos) link;
	unsigned char *name;
	long line;
} file_pos = { { &file_pos, &file_pos } };

static int file_pos_count;

struct file_pos *find_file_pos(unsigned char *name)
{
	struct file_pos *p;
	for (p = file_pos.link.next; p != &file_pos; p = p->link.next)
		if (!zcmp(p->name, name)) {
			promote(struct file_pos,link,&file_pos,p);
			return p;
		}
	p = (struct file_pos *)malloc(sizeof(struct file_pos));
	p->name = zdup(name);
	p->line = 0;
	enquef(struct file_pos,link,&file_pos,p);
	if (++file_pos_count == MAX_FILE_POS) {
		free(deque_f(struct file_pos,link,file_pos.link.prev));
		--file_pos_count;
	}
	return p;
}

int restore_file_pos;

long get_file_pos(unsigned char *name)
{
	if (name && restore_file_pos) {
		struct file_pos *p = find_file_pos(name);
		return p->line;
	} else {
		return 0;
	}
}

void set_file_pos(unsigned char *name, long pos)
{
	if (name) {
		struct file_pos *p = find_file_pos(name);
		p->line = pos;
	}
}

void save_file_pos(FILE *f)
{
	struct file_pos *p;
	for (p = file_pos.link.prev; p != &file_pos; p = p->link.prev) {
		fprintf(f,"	%ld ",p->line);
		emit_string(f,p->name,zlen(p->name));
		fprintf(f,"\n");
	}
	fprintf(f,"done\n");
}

void load_file_pos(FILE *f)
{
	unsigned char buf[1024];
	while (fgets((char *)buf,sizeof(buf)-1,f) && zcmp(buf,USTR "done\n")) {
		unsigned char *p = buf;
		long pos;
		unsigned char name[1024];
		parse_ws(&p,'#');
		if (!parse_long(&p, &pos)) {
			parse_ws(&p, '#');
			if (parse_string(&p, name, sizeof(name)) > 0) {
				set_file_pos(name, pos);
			}
		}
	}
}

/* Save file position for all windows */

void set_file_pos_all(Screen *t)
{
	/* Step through all windows */
	W *w = t->topwin;
	do {
		if (w->watom == &watomtw) {
			BW *bw = w->object;
			set_file_pos(bw->b->name, bw->cursor->line);
		}
		w = w->link.next;
	} while(w != t->topwin);
	/* Set through orphaned buffers */
	set_file_pos_orphaned();
}

/* Return master BW for a B.  It's the last window on the screen with the B.  If the B has a VT, then
 * it's the last window on the screen with the B and where the cursor matches the VT cursor. */

BW *vtmaster(Screen *t, B *b)
{
	W *w = t->topwin;
	BW *m = 0;
	do {
		if (w->watom == &watomtw) {
			BW *bw = w->object;
			if (bw && w->y != -1 && bw->b == b && (!b->vt || b->vt->vtcur->byte == bw->cursor->byte))
				m = bw;
		}
		w = w->link.next;
	} while (w != t->topwin);
	return m;
}

void bwrm(BW *w)
{
	if (w->b == errbuf && w->b->count == 1) {
		/* Do not lose message buffer */
		orphit(w);
	}
	set_file_pos(w->b->name,w->cursor->line);
	prm(w->top);
	prm(w->cursor);
	brm(w->b);
	joe_free(w);
}

unsigned char *ustat_line;

int ustat(BW *bw)
{
#if 0
	static unsigned char buf[160];
	unsigned char bf1[100];
	unsigned char bf2[100];
	int c = brch(bw->cursor);

#if HAVE_LONG_LONG
		joe_snprintf_1(bf1, sizeof(bf1), "%llu", (unsigned long long)bw->cursor->byte);
		joe_snprintf_1(bf2, sizeof(bf2), "%llx", (unsigned long long)bw->cursor->byte);
#else
		joe_snprintf_1(bf1, sizeof(bf1), "%lu", (unsigned long)bw->cursor->byte);
		joe_snprintf_1(bf2, sizeof(bf2), "%lx", (unsigned long)bw->cursor->byte);
#endif

	if (c == NO_MORE_DATA)
		joe_snprintf_4(buf, sizeof(buf), joe_gettext(_("** Line %ld  Col %ld  Offset %s(0x%s) **")), bw->cursor->line + 1, piscol(bw->cursor) + 1, bf1, bf2);
	else
		joe_snprintf_9(buf, sizeof(buf), joe_gettext(_("** Line %ld  Col %ld  Offset %s(0x%s)  %s %d(0%o/0x%X) Width %d **")), bw->cursor->line + 1, piscol(bw->cursor) + 1, bf1, bf2, bw->b->o.charmap->name, c, c, c, joe_wcwidth(bw->o.charmap->type,c));
	msgnw(bw->parent, buf);
#endif

	int c = brch(bw->cursor);
	unsigned char *msg;

	if (c == NO_MORE_DATA) {
		if (bw->o.zmsg) msg = bw->o.zmsg;
		else msg = USTR "** Line %r Col %c Offset %o(0x%O) **";
	} else {
		if (bw->o.smsg) msg = bw->o.smsg;
		else msg = USTR "** Line %r Col %c Offset %o(0x%O) %e %a(0x%A) Width %w **";
	}

	ustat_line = stagen(ustat_line, bw, msg, zlen(msg) ? msg[zlen(msg) - 1] : ' ');
	msgnw(bw->parent, ustat_line);

	return 0;
}

int ucrawlr(BW *bw)
{
	int amnt = bw->w / 2;

	pcol(bw->cursor, bw->cursor->xcol + amnt);
	bw->cursor->xcol += amnt;
	bw->offset += amnt;
	updall();
	return 0;
}

int ucrawll(BW *bw)
{
	int amnt = bw->w / 2;
	int curamnt = bw->w / 2;

	if (amnt > bw->offset) {
		amnt = bw->offset;
		curamnt = bw->offset;
	}
	if (!bw->offset)
		curamnt = bw->cursor->xcol;
	if (!curamnt)
		return -1;
	pcol(bw->cursor, bw->cursor->xcol - curamnt);
	bw->cursor->xcol -= curamnt;
	bw->offset -= amnt;
	updall();
	return 0;
}

/* If we are about to call bwrm, and b->count is 1, and orphan mode
 * is set, call this. */

void orphit(BW *bw)
{
	++bw->b->count; /* Assumes bwrm() is about to be called */
	bw->b->orphan = 1;
	pdupown(bw->cursor, &bw->b->oldcur, USTR "orphit");
	pdupown(bw->top, &bw->b->oldtop, USTR "orphit");
}
