/* 
 *	Text editing windows
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

int staen = 0;
int staupd = 0;
int keepup = 0;
int bg_stalin;

/* Move text window */

static void movetw(BW *bw, int x, int y)
{
	TW *tw = (TW *) bw->object;

	if (y || !staen) {
		if (!tw->staon) {	/* Scroll down and shrink */
			nscrldn(bw->parent->t->t, y, bw->parent->nh + y, 1);
		}
		bwmove(bw, x + (bw->o.linums ? LINCOLS : 0), y + 1);
		tw->staon = 1;
	} else {
		if (tw->staon) {	/* Scroll up and grow */
			nscrlup(bw->parent->t->t, y, bw->parent->nh + y, 1);
		}
		bwmove(bw, x + (bw->o.linums ? LINCOLS : 0), y);
		tw->staon = 0;
	}
}

/* Resize text window */

static void resizetw(BW *bw, int wi, int he)
{
	if (bw->parent->ny || !staen)
		bwresz(bw, wi - (bw->o.linums ? LINCOLS : 0), he - 1);
	else
		bwresz(bw, wi - (bw->o.linums ? LINCOLS : 0), he);
}

/* Get current context */

/* Find first line (going backwards) which has 0 indentation level
 * and is not a comment, blank, or block structuring line.  This is
 * likely to be the line with the function name.
 *
 * There are actually two possibilities:
 *
 * We want the first line- 
 *
 * int
 * foo(int x,int y) {
 *
 *   }
 *
 * We want the last line-
 *
 * program foo(input,output);
 * var a, b, c : real;
 * begin
 *
 */

unsigned char *get_context(BW *bw)
{
	P *p = pdup(bw->cursor, USTR "get_context");
	unsigned char *buf = 0;
	unsigned char *buf1;
	int i, spc;


	buf1 = vsmk(128);
	/* Find first line with 0 indentation which is not a comment line */
	do {
		p_goto_bol(p);
		if (!pisindent(p) && !pisblank(p)) {
			/* next: */
			buf = brlinevs(buf, p);
			/* Ignore comment and block structuring lines */
			if (!(buf[0]=='{' ||
			    (buf[0]=='/' && buf[1]=='*') ||
			    buf[0]=='\f' ||
			    (buf[0]=='/' && buf[1]=='/') ||
			    buf[0]=='#' ||
			    (buf[0]=='b' && buf[1]=='e' && buf[2]=='g' && buf[3]=='i' && buf[4]=='n') ||
			    (buf[0]=='B' && buf[1]=='E' && buf[2]=='G' && buf[3]=='I' && buf[4]=='N') ||
			    (buf[0]=='-' && buf[1]=='-') ||
			    buf[0]==';')) {
 				/* replace tabs to spaces and remove adjoining spaces */
 				buf1 = vstrunc(buf1, 0);
 				for (i=0,spc=0; buf[i]; i++) {
 					if (buf[i]=='\t' || buf[i]==' ') {
 						if (spc) continue;
 						spc = 1;
 					}
 					else spc = 0;
 					if (buf[i]=='\t')
 						buf1 = vsadd(buf1, ' ');
					else if (buf[i]=='\\') {
						buf1 = vsadd(buf1, '\\');
						buf1 = vsadd(buf1, '\\');
					} else
						buf1 = vsadd(buf1, buf[i]);
 				}
				/* Uncomment to get the last line instead of the first line (see above)
			    	if (pprevl(p)) {
			    		p_goto_bol(p);
			    		if (!pisindent(p) && !pisblank(p))
			    			goto next;
			    	}
			    	*/
			    	break;
			}
			
		}
	} while (!buf1[0] && pprevl(p));

	prm(p);

	return buf1;
}

unsigned char *duplicate_backslashes(unsigned char *s, int len)
{
	unsigned char *m;
	int x, count;
	for (x = count = 0; x != len; ++x)
		if (s[x] == '\\')
			++count;
	m = vsmk(len + count);
	for (x = 0; x != len; ++x) {
		m = vsadd(m, s[x]);
		if (s[x] == '\\')
			m = vsadd(m, '\\');
	}
	return m;
}

/* static */unsigned char *stagen(unsigned char *stalin, BW *bw, unsigned char *s, int fill)
{
	unsigned char buf[80];
	int x;
	W *w = bw->parent;
	time_t n=time(NULL);
	struct tm *cas;
	cas=localtime(&n);

	stalin = vstrunc(stalin, 0);
	obj_perm(stalin);
	while (*s) {
		if (*s == '%' && s[1]) {
			switch (*++s) {
			case 'x': /* Context (but only if autoindent is enabled) */
				{
					if ( bw->o.autoindent) {
						unsigned char *s = get_context(bw);
						unsigned char *t = my_iconv(NULL,locale_map,s,bw->o.charmap);
						stalin = vscat(stalin, sv(t));
					}
				}
				break;

			case 'y':
				{
					if (bw->o.syntax) {
						joe_snprintf_1(buf, sizeof(buf), "(%s)", bw->o.syntax->name);
						stalin = vsncpy(sv(stalin), sz(buf));
					}
				}
				break;
			case 't':
				{
					time_t n = time(NULL);
					int l;
					unsigned char *d = (unsigned char *)ctime(&n);

					l = (d[11] - '0') * 10 + d[12] - '0';
					if (l > 12)
						l -= 12;
					joe_snprintf_1(buf, sizeof(buf), "%2.2d", l);
					if (buf[0] == '0')
						buf[0] = fill;
					stalin = vsncpy(sv(stalin), buf, 2);
					stalin = vsncpy(sv(stalin), d + 13, 3);
				}
				break;
			    case 'd':
				{
				if (s[1]) switch (*++s) {
					case 'd' : joe_snprintf_1(buf,sizeof(buf),"%02d",cas->tm_mday); break;
					case 'm' : joe_snprintf_1(buf,sizeof(buf),"%02d",cas->tm_mon + 1); break;
					case 'y' : joe_snprintf_1(buf,sizeof(buf),"%02d",cas->tm_year % 100); break;
					case 'Y' : joe_snprintf_1(buf,sizeof(buf),"%04d",cas->tm_year + 1900); break;
					case 'w' : joe_snprintf_1(buf,sizeof(buf),"%d",cas->tm_wday); break;
					case 'D' : joe_snprintf_1(buf,sizeof(buf),"%03d",cas->tm_yday); break;
					default : buf[0]='d'; buf[1]=*s; buf[2]=0;
					}
				else { buf[0]='d'; buf[1]=0;}
				stalin=vsncpy(sv(stalin),sz(buf));
				}
				break;

				case 'E':
				{
				int l;
				unsigned char *ch;
				buf[0]=0;
				for(l=0;s[l+1] && s[l+1] != '%'; l++) buf[l]=s[l+1];
				if (s[l+1]=='%' && buf[0]) {
					buf[l]=0;
					s+=l+1;
					ch=(unsigned char *)getenv((char *)buf);
					if (ch) stalin=vsncpy(sv(stalin),sz(ch));
					} 
				}
				break;

			case 'Z':
				{
					int l;
					unsigned char *ch;
					buf[0]=0;
					for(l=0;s[l+1] && s[l+1] != '%'; l++) buf[l]=s[l+1];
					if (s[l+1]=='%' && buf[0]) {
						buf[l]=0;
						s+=l+1;
						ch=get_status(bw, buf);
						if (ch) stalin=vsncpy(sv(stalin),sz(ch));
					} 
				}
				break;

			case 'u':
				{
					time_t n = time(NULL);
					unsigned char *d = (unsigned char *)ctime(&n);

					stalin = vsncpy(sv(stalin), d + 11, 5);
				}
				break;
			case 'T':
				if (bw->o.overtype)
					stalin = vsadd(stalin, 'O');
				else
					stalin = vsadd(stalin, 'I');
				break;
			case 'W':
				if (bw->o.wordwrap)
					stalin = vsadd(stalin, 'W');
				else
					stalin = vsadd(stalin, fill);
				break;
			case 'I':
				if (bw->o.autoindent)
					stalin = vsadd(stalin, 'A');
				else
					stalin = vsadd(stalin, fill);
				break;
			case 'X':
				if (square)
					stalin = vsadd(stalin, 'X');
				else
					stalin = vsadd(stalin, fill);
				break;
			case 'n':
				{
				if (bw->b->name) {
					unsigned char *tmp = simplify_prefix(bw->b->name);
					unsigned char *tmp1 = duplicate_backslashes(sv(tmp));
					stalin = vsncpy(sv(stalin), sv(tmp1));
				} else {
					stalin = vsncpy(sv(stalin), sz(joe_gettext(_("Unnamed"))));
				}
				}
				break;
			case 'm':
				if (bw->b->changed)
					stalin = vsncpy(sv(stalin), sz(joe_gettext(_("(Modified)"))));
				break;
			case 'R':
				if (bw->b->rdonly)
					stalin = vsncpy(sv(stalin), sz(joe_gettext(_("(Read only)"))));
				break;
			case '*':
				if (bw->b->changed)
					stalin = vsadd(stalin, '*');
				else
					stalin = vsadd(stalin, fill);
				break;
			case 'r':
				joe_snprintf_1(buf, sizeof(buf), "%-4ld", bw->cursor->line + 1);
				for (x = 0; buf[x]; ++x)
					if (buf[x] == ' ')
						buf[x] = fill;
				stalin = vsncpy(sv(stalin), sz(buf));
				break;
			case 'o':
#ifdef HAVE_LONG_LONG
				joe_snprintf_1(buf, sizeof(buf), "%-4lld", (long long)bw->cursor->byte);
#else
				joe_snprintf_1(buf, sizeof(buf), "%-4ld", (long)bw->cursor->byte);
#endif
				for (x = 0; buf[x]; ++x)
					if (buf[x] == ' ')
						buf[x] = fill;
				stalin = vsncpy(sv(stalin), sz(buf));
				break;
			case 'O':
#ifdef HAVE_LONG_LONG
				joe_snprintf_1(buf, sizeof(buf), "%-4llX", (unsigned long long)bw->cursor->byte);
#else
				joe_snprintf_1(buf, sizeof(buf), "%-4lX", (unsigned long)bw->cursor->byte);
#endif
				for (x = 0; buf[x]; ++x)
					if (buf[x] == ' ')
						buf[x] = fill;
				stalin = vsncpy(sv(stalin), sz(buf));
				break;
			case 'a':
				if (!piseof(bw->cursor))
					joe_snprintf_1(buf, sizeof(buf), "%3d", brch(bw->cursor));
				else
					joe_snprintf_0(buf, sizeof(buf), "   ");
				for (x = 0; buf[x]; ++x)
					if (buf[x] == ' ')
						buf[x] = fill;
				stalin = vsncpy(sv(stalin), sz(buf));
				break;
			case 'A':
				if (!piseof(bw->cursor))
					joe_snprintf_1(buf, sizeof(buf), "%2.2X", brch(bw->cursor));
				else
					joe_snprintf_0(buf, sizeof(buf), "  ");
				for (x = 0; buf[x]; ++x)
					if (buf[x] == ' ')
						buf[x] = fill;
				stalin = vsncpy(sv(stalin), sz(buf));
				break;
			case 'c':
				joe_snprintf_1(buf, sizeof(buf), "%-3ld", piscol(bw->cursor) + 1);
				for (x = 0; buf[x]; ++x)
					if (buf[x] == ' ')
						buf[x] = fill;
				stalin = vsncpy(sv(stalin), sz(buf));
				break;
			case 'p':
				if (bw->b->eof->byte >= 1024*1024)
#ifdef HAVE_LONG_LONG
					joe_snprintf_1(buf, sizeof(buf), "%3lld", ((long long)bw->cursor->byte >> 10) * 100 / ((long long)bw->b->eof->byte >> 10));
#else
					joe_snprintf_1(buf, sizeof(buf), "%3ld", ((long)bw->cursor->byte >> 10) * 100 / ((long)bw->b->eof->byte >> 10));
#endif
				else if (bw->b->eof->byte)
#ifdef HAVE_LONG_LONG
					joe_snprintf_1(buf, sizeof(buf), "%3lld", (long long)bw->cursor->byte * 100 / (long long)bw->b->eof->byte);
#else
					joe_snprintf_1(buf, sizeof(buf), "%3ld", (long)bw->cursor->byte * 100 / (long)bw->b->eof->byte);
#endif
				else
					joe_snprintf_0(buf, sizeof(buf), "100");
				for (x = 0; buf[x]; ++x)
					if (buf[x] == ' ')
						buf[x] = fill;
				stalin = vsncpy(sv(stalin), sz(buf));
				break;
			case 'l':
				joe_snprintf_1(buf, sizeof(buf), "%-4ld", bw->b->eof->line + 1);
				for (x = 0; buf[x]; ++x)
					if (buf[x] == ' ')
						buf[x] = fill;
				stalin = vsncpy(sv(stalin), sz(buf));
				break;
			case 'k':
				{
					int i;
					unsigned char *cpos = buf;

					buf[0] = 0;
					if (w->kbd->x && w->kbd->seq[0])
						for (i = 0; i != w->kbd->x; ++i) {
							int c = w->kbd->seq[i] & 127;

							if (c < 32) {
								cpos[0] = '^';
								cpos[1] = c + '@';
								cpos += 2;
							} else if (c == 127) {
								cpos[0] = '^';
								cpos[1] = '?';
								cpos += 2;
							} else {
								cpos[0] = c;
								cpos += 1;
							}
						}
					*cpos++ = fill;
					while (cpos - buf < 4)
						*cpos++ = fill;
					stalin = vsncpy(sv(stalin), buf, cpos - buf);
				}
				break;
			case 'S':
				if (bw->b->pid)
					stalin = vsncpy(sv(stalin), sz(joe_gettext(_("*SHELL*"))));
				break;
			case 'M':
				if (recmac) {
					joe_snprintf_1(buf, sizeof(buf), joe_gettext(_("(Macro %d recording...)")), recmac->n);
					stalin = vsncpy(sv(stalin), sz(buf));
				}
				break;
			default:
				stalin = vsadd(stalin, *s);
			}
		} else
			stalin = vsadd(stalin, *s);
		++s;
	}
	return stalin;
}

static void disptw(BW *bw, int flg)
{
	W *w = bw->parent;
	TW *tw = (TW *) bw->object;

	if (bw->o.linums != bw->linums) {
		bw->linums = bw->o.linums;
		resizetw(bw, w->w, w->h);
		movetw(bw, w->x, w->y);
		bwfllw(bw);
	}

	if (bw->o.hex) {
		w->cury = (bw->cursor->byte-bw->top->byte)/16 + bw->y - w->y;
		w->curx = (bw->cursor->byte-bw->top->byte)%16 + 60 - bw->offset;
	} else {
		w->cury = bw->cursor->line - bw->top->line + bw->y - w->y;
		w->curx = bw->cursor->xcol - bw->offset + (bw->o.linums ? LINCOLS : 0);
	}

	if ((staupd || keepup || bw->cursor->line != tw->prevline || bw->b->changed != tw->changed || bw->b != tw->prev_b) && (w->y || !staen)) {
		int fill;

		tw->prevline = bw->cursor->line;
		tw->changed = bw->b->changed;
		tw->prev_b = bw->b;
		if (bw->o.rmsg[0])
			fill = bw->o.rmsg[0];
		else
			fill = ' ';
		tw->stalin = stagen(tw->stalin, bw, bw->o.lmsg, fill);
		tw->staright = stagen(tw->staright, bw, bw->o.rmsg, fill);
		if (fmtlen(tw->staright) < w->w) {
			int x = fmtpos(tw->stalin, w->w - fmtlen(tw->staright));

			if (x > vslen(tw->stalin))
				tw->stalin = vsfill(sv(tw->stalin), fill, x - vslen(tw->stalin));
			tw->stalin = vsncpy(tw->stalin, fmtpos(tw->stalin, w->w - fmtlen(tw->staright)), sv(tw->staright));
		}
		tw->stalin = vstrunc(tw->stalin, fmtpos(tw->stalin, w->w));
		genfmt(w->t->t, w->x, w->y, 0, tw->stalin, bg_stalin, 0);
		w->t->t->updtab[w->y] = 0;
	}

	if (flg) {
		if (bw->o.hex)
			bwgenh(bw);
		else
			bwgen(bw, bw->o.linums);
	}
}

/* Split current window */

static void iztw(TW *tw, int y)
{
	tw->stalin = NULL;
	tw->staright = NULL;
	tw->changed = -1;
	tw->prevline = -1;
	tw->staon = (!staen || y);
	tw->prev_b = 0;
}

int usplitw(BW *bw)
{
	W *w = bw->parent;
	int newh = getgrouph(w);
	W *new;
	TW *newtw;
	BW *newbw;

	dostaupd = 1;
	if (newh / 2 < FITHEIGHT)
		return -1;
	new = wcreate(w->t, w->watom, findbotw(w), NULL, w, newh / 2 + (newh & 1), NULL);
	if (!new)
		return -1;
	wfit(new->t);
	new->object = (void *) (newbw = bwmk(new, bw->b, 0, NULL));
	++bw->b->count;
	newbw->offset = bw->offset;
	newbw->object = (void *) (newtw = (TW *) joe_malloc(sizeof(TW)));
	iztw(newtw, new->y);
	pset(newbw->top, bw->top);
	pset(newbw->cursor, bw->cursor);
	newbw->cursor->xcol = bw->cursor->xcol;
	new->t->curwin = new;
	return 0;
}

int uduptw(BW *bw)
{
	W *w = bw->parent;
	int newh = getgrouph(w);
	W *new;
	TW *newtw;
	BW *newbw;

	dostaupd = 1;
	new = wcreate(w->t, w->watom, findbotw(w), NULL, NULL, newh, NULL);
	if (!new)
		return -1;
	if (demotegroup(w))
		new->t->topwin = new;
	new->object = (void *) (newbw = bwmk(new, bw->b, 0, NULL));
	++bw->b->count;
	newbw->offset = bw->offset;
	newbw->object = (void *) (newtw = (TW *) joe_malloc(sizeof(TW)));
	iztw(newtw, new->y);
	pset(newbw->top, bw->top);
	pset(newbw->cursor, bw->cursor);
	newbw->cursor->xcol = bw->cursor->xcol;
	new->t->curwin = new;
	wfit(w->t);
	return 0;
}

static void instw(BW *bw, B *b, long int l, long int n, int flg)
{
	if (b == bw->b)
		bwins(bw, l, n, flg);
}

static void deltw(BW *bw, B *b, long int l, long int n, int flg)
{
	if (b == bw->b)
		bwdel(bw, l, n, flg);
}

WATOM watomtw = {
	USTR "main",
	disptw,
	bwfllw,
	NULL,
	rtntw,
	utypebw,
	resizetw,
	movetw,
	instw,
	deltw,
	TYPETW
};

int abortit(BW *bw)
{
	W *w;
	TW *tw;
	B *b;
	if (bw->parent->watom != &watomtw)
		return wabort(bw->parent);
	if (bw->b->pid && bw->b->count==1)
		return ukillpid(bw);
	w = bw->parent;
	tw = (TW *) bw->object;
	/* If only one main window on the screen... */
	if (countmain(w->t) == 1)
		/* Replace it with an orphaned buffer if there are any */
		if ((b = borphan()) != NULL) {
			void *object = bw->object;
			/* FIXME: Shouldn't we wabort() and wcreate here to kill
			   any prompt windows? */

			bwrm(bw);
			w->object = (void *) (bw = bwmk(w, b, 0, NULL));
			wredraw(bw->parent);
			bw->object = object;
			return 0;
		}
	bwrm(bw);
	obj_free(tw->stalin);
	joe_free(tw);
	w->object = NULL;
	wabort(w);	/* Eliminate this window and it's children */
#ifdef JOEWIN
	notify_selection();
#endif
	return 0;
}

/* k is last character types which lead to uabort.  If k is -1, it means uabort
   was called internally, and not by the user: which means uabort will not send
   Ctrl-C to process */
int uabort(BW *bw, int k)
{
	if (bw->parent->watom != &watomtw)
		return wabort(bw->parent);
	if (bw->b->pid && bw->b->count==1)
		return ukillpid(bw);
	if (bw->b->changed && bw->b->count == 1 && !bw->b->scratch) {
		int c = query(bw->parent, sz(joe_gettext(_("Lose changes to this file (y,n,^C)? "))), 0);
		if (!yncheck(yes_key, c))
			return -1;
	}
	genexmsg(bw, 0, NULL);
	return abortit(bw);
}

int ucancel(BW *bw, int k)
{
	if (bw->parent->watom != &watomtw) {
		wabort(bw->parent);
		return 0;
	} else
		return uabort(bw,k);
}

/* Same as above, but only calls genexmsg if nobody else has */

int uabort1(BW *bw, int k)
{
	if (bw->parent->watom != &watomtw)
		return wabort(bw->parent);
	if (bw->b->pid && bw->b->count==1)
		return ukillpid(bw);
	if (bw->b->changed && bw->b->count == 1 && !bw->b->scratch) {
		int c = query(bw->parent, sz(joe_gettext(_("Lose changes to this file (y,n,^C)? "))), 0);
		if (!yncheck(yes_key, c))
			return -1;
	}
	if (!exmsg) genexmsg(bw, 0, NULL);
	return abortit(bw);
}

/* Abort buffer without prompting: just fail if this is last window on buffer */

int uabortbuf(BW *bw)
{
	W *w = bw->parent;
	B *b;

	if (bw->b->pid && bw->b->count==1)
		return ukillpid(bw);

	if (okrepl(bw))
		return -1;

	if ((b = borphan()) != NULL) {
		void *object = bw->object;

		bwrm(bw);
		w->object = (void *) (bw = bwmk(w, b, 0, NULL));
		wredraw(bw->parent);
		bw->object = object;
		return 0;
	}

	genexmsg(bw, 0, NULL);
	return abortit(bw);
}

/* Kill current window (orphans buffer) */

int utw0(BASE *b)
{
	BW *bw = b->parent->main->object;

	if (countmain(b->parent->t) == 1)
		return -1;
	if (bw->b->count == 1)
		orphit(bw);
	return uabort(bw, -1);
}

/* Kill all other windows (orphans buffers) */

int utw1(BASE *b)
{
	W *starting = b->parent;
	W *mainw = starting->main;
	Screen *t = mainw->t;
	int yn;

	do {
		yn = 0;
	      loop:
		do {
			wnext(t);
		} while (t->curwin->main == mainw && t->curwin != starting);
		if (t->curwin->main != mainw) {
			BW *bw = t->curwin->main->object;
			utw0((BASE *)bw);
			yn = 1;
			goto loop;
		}
	} while (yn);
	return 0;
}

void setline(B *b, long int line)
{
	W *w = maint->curwin;

	do {
		if (w->watom->what == TYPETW) {
			BW *bw = w->object;

			if (bw->b == b) {
				long oline = bw->top->line;

				/* pline(bw->top, line); */
				pline(bw->cursor, line);
				if (w->y >= 0 && bw->top->line > oline && bw->top->line - oline < bw->h)
					nscrlup(w->t->t, bw->y, bw->y + bw->h, (int) (bw->top->line - oline));
				else if (w->y >= 0 && bw->top->line < oline && oline - bw->top->line < bw->h)
					nscrldn(w->t->t, bw->y, bw->y + bw->h, (int) (oline - bw->top->line));
				msetI(bw->t->t->updtab + bw->y, 1, bw->h);
			}
		}
	} while ((w = w->link.next) != maint->curwin);
	/* In case error buffer was orphaned */
	if (errbuf == b && b->oldcur) {
		pline(b->oldcur, line);
	}
}

/* Create a text window.  It becomes the last window on the screen */

BW *wmktw(Screen *t, B *b)
{
	W *w;
	BW *bw;
	TW *tw;

	w = wcreate(t, &watomtw, NULL, NULL, NULL, t->h, NULL);
	wfit(w->t);
	w->object = (void *) (bw = bwmk(w, b, 0, NULL));
	bw->object = (void *) (tw = (TW *) joe_malloc(sizeof(TW)));
	iztw(tw, w->y);
#ifdef JOEWIN
	notify_new_buffer(b);
#endif
	return bw;
}
