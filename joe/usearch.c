/*
 *	Search & Replace system
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

int wrap = 0;			/* Allow wrap */
int smode = 0;			/* Decremented to zero by execmd */
int csmode = 0;			/* Set for continued search mode */
int icase = 0;			/* Set to force case insensitive search */
int pico = 0;			/* Pico search prompting */

B *findhist = NULL;		/* Search string history */
B *replhist = NULL;		/* Replacement string history */

SRCH *globalsrch = NULL;	/* Most recent completed search data */

SRCHREC fsr = { {&fsr, &fsr} };

/* Completion stuff: should go somewhere else */

unsigned char **word_list;

#define MAX_WORD_SIZE 64
unsigned char **get_word_list(B *b,long ignore)
{
	unsigned char buf[MAX_WORD_SIZE];
	unsigned char *s;
	unsigned char **list = 0;
	HASH *h;
	HENTRY *t;
	P *p;
	int c;
	int idx;
	long start = 0;

	h = htmk(1024);

	p = pdup(b->bof, USTR "get_word_list");
	idx = 0;
	do {
		c = pgetc(p);
		if (idx) {
			if (joe_isalnum_(b->o.charmap, c)) {
				if (b->o.charmap->type) {
					if (idx + 8 < MAX_WORD_SIZE) {
						idx += utf8_encode(buf+idx, c);
					}
				} else {
					if (idx!=MAX_WORD_SIZE)
						buf[idx++] = c;
				}
			} else {
				if (idx!=MAX_WORD_SIZE && start!=ignore) {
					buf[idx] = 0;
					if (!htfind(h,buf)) {
						s = vsncpy(NULL,0,buf,idx);
						htadd(h, s, s);
					}
				}
				idx = 0;
			}
		} else {
			start=p->byte-1;
			if (joe_isalpha_(b->o.charmap, c))
				if (b->o.charmap->type) {
					idx += utf8_encode(buf+idx, c);
				} else {
					buf[idx++] = c;
				}
		}
	} while (c != NO_MORE_DATA);
	prm(p);

	for (idx = 0;idx != h->len;++idx)
		for (t = h->tab[idx];t;t=t->next)
			list = vaadd(list, t->name);
	if (list)
		vasort(list,sLEN(list));	

	htrm(h);

	return list;
}

void fcmplt_ins(BW *bw, unsigned char *line)
{
	P *p = pdup(bw->cursor, USTR "fcmplt_ins");
	int c;

	if (!piseol(bw->cursor)) {
		int c = brch(bw->cursor);
		if (joe_isalnum_(bw->b->o.charmap,c))
			return;
	}

	/* Move p to beginning of word */

	p = pdup(bw->cursor, USTR "fcmplt_ins");
	do
		c = prgetc(p);
		while (joe_isalnum_(bw->b->o.charmap,c));
	if (c!=NO_MORE_DATA)
		pgetc(p);

	if (bw->cursor->byte!=p->byte && bw->cursor->byte-p->byte<64) {
		/* Insert single match */
		bdel(p,bw->cursor);
		binsm(bw->cursor,sv(line));
		pfwrd(bw->cursor,sLEN(line));
		bw->cursor->xcol = piscol(bw->cursor);
		prm(p);
	} else {
		prm(p);
	}
}

int fcmplt_abrt(BW *bw, int x, unsigned char *line)
{
	if (line) {
		fcmplt_ins(bw, line);
		vsrm(line);
	}
	return -1;
}

int fcmplt_rtn(MENU *m, int x, unsigned char *line)
{
	fcmplt_ins(m->parent->win->object, m->list[x]);
	vsrm(line);
	m->object = NULL;
	wabort(m->parent);
	return 0;
}

int ufinish(BW *bw)
{
	unsigned char *line;
	unsigned char *line1;
	unsigned char **lst;
	P *p;
	int c;
	MENU *m;

	/* Make sure we're not in a word */

	if (!piseol(bw->cursor)) {
		int c = brch(bw->cursor);
		if (joe_isalnum_(bw->b->o.charmap,c))
			return -1;
	}

	/* Move p to beginning of word */

	p = pdup(bw->cursor, USTR "ufinish");
	do
		c = prgetc(p);
		while (joe_isalnum_(bw->b->o.charmap,c));
	if (c!=NO_MORE_DATA)
		pgetc(p);

	if (bw->cursor->byte!=p->byte && bw->cursor->byte-p->byte<64) {
		line = brvs(p, bw->cursor->byte-p->byte);

		/* We have a word */

		/* Get word list */
		if (word_list)
			varm(word_list);

		word_list = get_word_list(bw->b, p->byte);

		if (!word_list) {
			vsrm(line);
			prm(p);
			return -1;
		}

		line1 = vsncpy(NULL,0,sv(line));
		line1 = vsadd(line1,'*');
		lst = regsub(word_list, aLEN(word_list), line1);
		vsrm(line1);

		if (!lst) {
			ttputc(7);
			vsrm(line);
			return -1;
		}

		m = mkmenu(bw->parent, bw->parent, lst, fcmplt_rtn, fcmplt_abrt, NULL, 0, line, NULL);
		if (!m) {
			varm(lst);
			vsrm(line);
			return -1;
		}

		/* Possible match list is now in lst */

		if (aLEN(lst) == 1)
			return fcmplt_rtn(m, 0, line);
		else if (smode)
			return 0;
		else {
			unsigned char *com = mcomplete(m);
			vsrm(m->object);
			m->object = com;
			wabort(m->parent);
			smode = 2;
			ttputc(7);
			return 0;
		}
	} else {
		prm(p);
		return -1;
	}
}

static int srch_cmplt(BW *bw)
{
	if (word_list)
		varm(word_list);

	word_list = get_word_list(((BW *)bw->parent->win->object)->b, -1);

	if (!word_list) {
		ttputc(7);
		return 0;
	}

	return simple_cmplt(bw,word_list);
}

/* Search forward.
   bw, pattern and ignore must be set

   The first possible string we can find is the one beginning under p

   Returns p if we found a string:
     The found string is placed in entire/pieces
     p is placed right after the found string

   Return 0 if we did not find the string:
     p is left in its orignal spot
*/

static P *searchf(BW *bw,SRCH *srch, P *p)
{
	unsigned char *pattern;
	P *start;
	P *end;
	int x;
	int flag = 0;

	pattern = srch->pattern;
	start = pdup(p, USTR "searchf");
	end = pdup(p, USTR "searchf");

	try_again:

	for (x = 0; x != sLEN(pattern) && pattern[x] != '\\' && (pattern[x]<128 || !p->b->o.charmap->type); ++x)
		if (srch->ignore)
			pattern[x] = joe_tolower(p->b->o.charmap,pattern[x]);
	wrapped:
	while (srch->ignore ? pifind(start, pattern, x) : pfind(start, pattern, x)) {
		pset(end, start);
		pfwrd(end, (long) x);
		if (srch->wrap_flag && start->byte>=srch->wrap_p->byte)
			break;
		if (pmatch(srch->pieces, pattern + x, sLEN(pattern) - x, end, 0, srch->ignore)) {
			if (end->byte == srch->last_repl && !flag) {
				/* Stuck on zero-width regex? */
				pattern = srch->pattern;
				pset(start, p);
				if (pgetc(start) == NO_MORE_DATA)
					break;
				pset(end, start);
				++flag; /* Try repeating, but only one time */
				goto try_again;
			} else {
				srch->entire = vstrunc(srch->entire, (int) (end->byte - start->byte));
				brmem(start, srch->entire, (int) (end->byte - start->byte));
				pset(p, end);
				prm(start);
				prm(end);
				srch->last_repl = p->byte; /* Prevent getting stuck with zero-length regex */
				return p;
			}
		}
		if (pgetc(start) == NO_MORE_DATA)
			break;
	}
	if (srch->allow_wrap && !srch->wrap_flag && srch->wrap_p) {
		msgnw(bw->parent, joe_gettext(_("Wrapped")));
		srch->wrap_flag = 1;
		p_goto_bof(start);
		goto wrapped;
	}
	srch->last_repl = -1;
	prm(start);
	prm(end);
	return NULL;
}

/* Search backwards.
   bw, pattern and ignore must be set

   The first possible string we can find is the one beginning one position
   to the left of p.

   Returns 1 if we found a string:
     The found string is placed in entire
     p is placed at the beginning of the string

   Return 0 if we did not find the string:
     p is left in its orignal spot
*/

static P *searchb(BW *bw,SRCH *srch, P *p)
{
	unsigned char *pattern = srch->pattern;
	P *start;
	P *end;
	int x;
	int flag = 0;

	start = pdup(p, USTR "searchb");
	end = pdup(p, USTR "searchb");

	try_again:

	for (x = 0; x != sLEN(pattern) && pattern[x] != '\\' && (pattern[x]<128 || !p->b->o.charmap->type); ++x)
		if (srch->ignore)
			pattern[x] = joe_tolower(p->b->o.charmap,pattern[x]);

	wrapped:
	while (pbkwd(start, 1L)
	       && (srch->ignore ? prifind(start, pattern, x) : prfind(start, pattern, x))) {
		pset(end, start);
		pfwrd(end, (long) x);
		if (srch->wrap_flag && start->byte<srch->wrap_p->byte)
			break;
		if (pmatch(srch->pieces, pattern + x, sLEN(pattern) - x, end, 0, srch->ignore)) {
			if (start->byte == srch->last_repl && !flag) {
				/* Stuck? */
				pattern = srch->pattern;
				pset(start, p);
				if (prgetc(start) == NO_MORE_DATA)
					break;
				pset(end, start);
				++flag;
				goto try_again;
			} else {
				srch->entire = vstrunc(srch->entire, (int) (end->byte - start->byte));
				brmem(start, srch->entire, (int) (end->byte - start->byte));
				pset(p, start);
				prm(start);
				prm(end);
				return p;
			}
		}
	}

	if (srch->allow_wrap && !srch->wrap_flag && srch->wrap_p) {
		msgnw(bw->parent, joe_gettext(_("Wrapped")));
		srch->wrap_flag = 1;
		p_goto_eof(start);
		goto wrapped;
	}
	srch->last_repl = -1;
	prm(start);
	prm(end);
	return NULL;
}

/* Make a search stucture */

static SRCH *setmark(SRCH *srch)
{
	if (markv(0))
		srch->valid = 1;

	srch->markb = markb;
	if (srch->markb)
		srch->markb->owner = &srch->markb;
	markb = NULL;

	srch->markk = markk;
	if (srch->markk)
		srch->markk->owner = &srch->markk;
	markk = NULL;

	return srch;
}

SRCH *mksrch(unsigned char *pattern, unsigned char *replacement, int ignore, int backwards, int repeat, int replace, int rest, int all)
{
	SRCH *srch = (SRCH *) joe_malloc(sizeof(SRCH));
	int x;

	srch->first = NULL;
	srch->current = NULL;
	srch->all = all;
	srch->pattern = pattern;
	srch->replacement = replacement;
	srch->ignore = ignore;
	srch->backwards = backwards;
	srch->repeat = repeat;
	srch->replace = replace;
	srch->rest = rest;
	srch->entire = NULL;
	srch->flg = 0;
	srch->addr = -1;
	srch->last_repl = -1;
	srch->markb = NULL;
	srch->markk = NULL;
	srch->wrap_p = NULL;
	srch->allow_wrap = wrap;
	srch->wrap_flag = 0;
	srch->valid = 0;
	srch->block_restrict = 0;
	izque(SRCHREC, link, &srch->recs);
	for (x = 0; x != 26; ++x)
		srch->pieces[x] = NULL;
	return srch;
}

/* Eliminate a search structure */

void rmsrch(SRCH *srch)
{
	int x;

	prm(markb);
	prm(markk);
	prm(srch->wrap_p);
	if (srch->markb) {
		markb = srch->markb;
		markb->owner = &markb;
		markb->xcol = piscol(markb);
	}
	if (srch->markk) {
		markk = srch->markk;
		markk->owner = &markk;
		markk->xcol = piscol(markk);
	}
	for (x = 0; x != 26; ++x)
		vsrm(srch->pieces[x]);
	frchn(&fsr, &srch->recs);
	vsrm(srch->pattern);
	vsrm(srch->replacement);
	vsrm(srch->entire);
	joe_free(srch);
	updall();
}

/* Insert a replacement string
 * p is advanced past the inserted text
 */

static P *insert(SRCH *srch, P *p, unsigned char *s, int len)
{
	int x;
	long starting = p->byte;

	while (len) {
		for (x = 0; x != len && s[x] != '\\'; ++x) ;
		if (x) {
			binsm(p, s, x);
			pfwrd(p, (long) x);
			len -= x;
			s += x;
		} else if (len >= 2) {
			if (((s[1] >= 'a' && s[1] <= 'z') || (s[1] >= 'A' && s[1] <= 'Z'))
				 && srch->pieces[(s[1] & 0x1f) - 1]) {
				binsm(p, sv(srch->pieces[(s[1] & 0x1f) - 1]));
				pfwrd(p, (long) sLEN(srch->pieces[(s[1] & 0x1f) - 1]));
				s += 2;
				len -= 2;
			} else if (s[1] >= '0' && s[1] <= '9' && srch->pieces[s[1] - '0']) {
				binsm(p, sv(srch->pieces[s[1] - '0']));
				pfwrd(p, (long) sLEN(srch->pieces[s[1] - '0']));
				s += 2;
				len -= 2;
			} else if (s[1] == '&' && srch->entire) {
				binsm(p, sv(srch->entire));
				pfwrd(p, (long) sLEN(srch->entire));
				s += 2;
				len -= 2;
			} else {
				unsigned char *a=(unsigned char *)s+x;
				int l=len-x;
				binsc(p,escape(p->b->o.charmap->type,&a,&l));
				pgetc(p);
				len -= a - (unsigned char *)s;
				s = a;
			}
		} else
			len = 0;
	}

	if (srch->backwards)
		pbkwd (p, p->byte - starting);

	return p;
}

/* Search system user interface */

/* Query for search string, search options, possible replacement string,
 * and execute first search */

unsigned char srchstr[] = "Search";	/* Context sensitive help identifier */
unsigned char srchopt[] = "SearchOptions";

static int pfabort(BW *bw, SRCH *srch)
{
	if (srch)
		rmsrch(srch);
	return -1;
}

static int pfsave(BW *bw, SRCH *srch)
{
	if (srch) {
		if (globalsrch)
			rmsrch(globalsrch);
		globalsrch = srch;
		srch->rest = 0;
		srch->repeat = -1;
		srch->flg = 0;

		prm(markb);
		prm(markk);
		if (srch->markb) {
			markb = srch->markb;
			markb->owner = &markb;
			markb->xcol = piscol(markb);
		}
		if (srch->markk) {
			markk = srch->markk;
			markk->owner = &markk;
			markk->xcol = piscol(markk);
		}
		srch->markb = NULL;
		srch->markk = NULL;

		updall();
	}
	return -1;
}

static int set_replace(BW *bw, unsigned char *s, SRCH *srch, int *notify)
{
	if (sLEN(s) || !globalsrch || !pico)
		srch->replacement = s;
	else {
		/* Use previous string: this prevents replace with empty string */
		/* vsrm(s);
		srch->replacement = vsdup(globalsrch->replacement); */
		srch->replacement = s;
	}
	return dopfnext(bw, setmark(srch), notify);
}

/* Option characters */

unsigned char *all_key = (unsigned char *) _("|all files|aA");
unsigned char *list_key = (unsigned char *) _("|error list files|eE");
unsigned char *replace_key = (unsigned char *) _("|search and replace|rR");
unsigned char *backwards_key = (unsigned char *) _("|backwards|bB");
unsigned char *ignore_key = (unsigned char *) _("|ignore case|iI");
unsigned char *block_key = (unsigned char *) _("|restrict to highlighted block|kK");
unsigned char *noignore_key = (unsigned char *) _("|don't ignore case|sS");
unsigned char *wrap_key = (unsigned char *) _("|wrap|wW");
unsigned char *nowrap_key = (unsigned char *) _("|don't wrap|nN");

/* Get next character from string and advance it, locale dependent */

int fwrd_c(unsigned char **s)
{
	if (locale_map->type)
		return utf8_decode_fwrd(s, NULL);
	else {
		int c = **s;
		*s = *s + 1;
		return c;
	}
}

static int set_options(BW *bw, unsigned char *s, SRCH *srch, int *notify)
{
	unsigned char buf[80];
	unsigned char *t;

	srch->ignore = icase;

	t = s;
	while (*t) {
		int c = fwrd_c(&t);
		if (yncheck(all_key, c))
			srch->all = 1;
		else if (yncheck(list_key, c))
			srch->all = 2;
		else if (yncheck(replace_key, c))
			srch->replace = 1;
		else if (yncheck(backwards_key, c))
			srch->backwards = 1;
		else if (yncheck(ignore_key, c))
			srch->ignore = 1;
		else if (yncheck(noignore_key, c))
			srch->ignore = 0;
		else if (yncheck(wrap_key, c))
			srch->allow_wrap = 1;
		else if (yncheck(nowrap_key, c))
			srch->allow_wrap = 0;
		else if (yncheck(block_key, c))
			srch->block_restrict = 1;
		else if (c >= '0' && c <= '9') {
			if (srch->repeat == -1)
				srch->repeat = 0;
			srch->repeat = srch->repeat * 10 + c - '0';
		}
	}	
	vsrm(s);
	if (srch->replace) {
		/* if (pico && globalsrch && globalsrch->replacement) {
			joe_snprintf_1(bf1,30,"%s",globalsrch->replacement);
			if (zlen(globalsrch->replacement)>29)
				zlcat(bf1, sizeof(bf1), USTR "$");
			joe_snprintf_1(buf,sizeof(buf),joe_gettext(_("Replace with (^C to abort) [%s]: ")),bf1);
		} else */
			joe_snprintf_0(buf, sizeof(buf), joe_gettext(_("Replace with (^C to abort): ")));
		if (wmkpw(bw->parent, buf, &replhist, set_replace, srchstr, pfabort, srch_cmplt, srch, notify, bw->b->o.charmap, 0))
			return 0;
		else
			return -1;
	} else
		return dopfnext(bw, setmark(srch), notify);
}

static int set_pattern(BW *bw, unsigned char *s, SRCH *srch, int *notify)
{
	BW *pbw;
	unsigned char *p;

	if (icase)
		p = joe_gettext(_("case (S)ensitive (R)eplace (B)ackwards Bloc(K) (A)ll files NNN (^C to abort): "));
	else
		p = joe_gettext(_("(I)gnore (R)eplace (B)ackwards Bloc(K) (A)ll files NNN (^C to abort): "));

	vsrm(srch->pattern);
	if (sLEN(s) || !globalsrch || !pico)
		srch->pattern = s;
	else {
		vsrm(s);
		srch->pattern = vsdup(globalsrch->pattern);
	}
	if ((pbw = wmkpw(bw->parent, p, NULL, set_options, srchopt, pfabort, utypebw, srch, notify, bw->b->o.charmap, 0)) != NULL) {
		unsigned char buf[10];

		if (srch->ignore) {
			unsigned char *t = joe_gettext(ignore_key);
			binsc(pbw->cursor, fwrd_c(&t));
		}
		if (srch->replace) {
			unsigned char *t = joe_gettext(replace_key);
			binsc(pbw->cursor, fwrd_c(&t));
		}
		if (srch->backwards) {
			unsigned char *t = joe_gettext(backwards_key);
			binsc(pbw->cursor, fwrd_c(&t));
		}
		if (srch->repeat >= 0)
			joe_snprintf_1(buf, sizeof(buf), "%d", srch->repeat), binss(pbw->cursor, buf);
		pset(pbw->cursor, pbw->b->eof);
		pbw->cursor->xcol = piscol(pbw->cursor);
		srch->ignore = 0;
		srch->replace = 0;
		srch->backwards = 0;
		srch->repeat = -1;
		return 0;
	} else {
		rmsrch(srch);
		return -1;
	}
}

/* Unescape for text going to genfmt */

void unesc_genfmt(unsigned char *d, unsigned char *s, int len,int max)
{
	while (max > 0 && len) {
		if (!*s) {
			*d++ = '\\';
			*d++ = '@';
			++s;
		} else {
			if (*s == '\\') {
				*d++ = '\\';
				--max;
			}
			*d++ = *s++;
		}
		--len;
		--max;
	}
	if (len)
		*d++ = '$';
	*d = 0;
}

int dofirst(BW *bw, int back, int repl, unsigned char *hint)
{
	SRCH *srch;
	BW *pbw;
	unsigned char bf1[80];
	unsigned char buf[80];

	if (smode && globalsrch) {
		globalsrch->backwards = back;
		globalsrch->replace = repl;
		return pfnext(bw);
	}
	if (bw->parent->huh == srchstr) {
		long byte;

		p_goto_eol(bw->cursor);
		byte = bw->cursor->byte;
		p_goto_bol(bw->cursor);
		if (byte == bw->cursor->byte)
			prgetc(bw->cursor);
		return urtn((BASE *)bw, -1);
	}
	srch = mksrch(NULL, NULL, 0, back, -1, repl, 0, 0);
	srch->addr = bw->cursor->byte;
	srch->wrap_p = pdup(bw->cursor, USTR "dofirst");
	srch->wrap_p->owner = &srch->wrap_p;
	if (pico && globalsrch && globalsrch->pattern) {
		unesc_genfmt(bf1, sv(globalsrch->pattern), 30);
		joe_snprintf_1(buf,sizeof(buf),joe_gettext(_("Find (^C to abort) [%s]: ")),bf1);
	} else
		joe_snprintf_0(buf, sizeof(buf), joe_gettext(_("Find (^C to abort): ")));
	if ((pbw=wmkpw(bw->parent, buf, &findhist, set_pattern, srchstr, pfabort, srch_cmplt, srch, NULL, bw->b->o.charmap, 0))) {
		if (hint) {
			binss(pbw->cursor, hint);
			pset(pbw->cursor, pbw->b->eof);
			pbw->cursor->xcol = piscol(pbw->cursor);
		}
		return 0;
	} else {
		rmsrch(srch);
		return -1;
	}
}

int pffirst(BW *bw)
{
	return dofirst(bw, 0, 0, NULL);
}

int prfirst(BW *bw)
{
	return dofirst(bw, 1, 0, NULL);
}

int pqrepl(BW *bw)
{
	return dofirst(bw, 0, 1, NULL);
}

/* Execute next search */

static int doreplace(BW *bw, SRCH *srch)
{
	P *q;

	if (!modify_logic(bw,bw->b))
		return -1;
	if (markk)
		markk->end = 1;
	if (srch->markk)
		srch->markk->end = 1;
	q = pdup(bw->cursor, USTR "doreplace");
	if (srch->backwards) {
		q = pfwrd(q, (long) sLEN(srch->entire));
		bdel(bw->cursor, q);
		prm(q);
	} else {
		q = pbkwd(q, (long) sLEN(srch->entire));
		bdel(q, bw->cursor);
		prm(q);
	}
	insert(srch, bw->cursor, sv(srch->replacement));
	srch->addr = bw->cursor->byte;
	srch->last_repl = bw->cursor->byte;
	if (markk)
		markk->end = 0;
	if (srch->markk)
		srch->markk->end = 0;
	return 0;
}

static void visit(SRCH *srch, BW *bw, int yn)
{
	SRCHREC *r = (SRCHREC *) alitem(&fsr, sizeof(SRCHREC));
	r->addr = bw->cursor->byte;
	r->yn = yn;
	r->wrap_flag = srch->wrap_flag;
	r->last_repl = srch->last_repl;
	r->b = bw->b;
	enqueb(SRCHREC, link, &srch->recs, r);
}

static void goback(SRCH *srch, BW *bw)
{
	SRCHREC *r = srch->recs.link.prev;

	if (r != &srch->recs) {
		srch->current = r->b;
		if (r->yn) {
			uundo(bw);
		}
		if (r->b != bw->b) {
			W *w = bw->parent;
			get_buffer_in_window(bw, r->b);
			bw = (BW *)w->object;
		}
		if (bw->cursor->byte != r->addr)
			pgoto(bw->cursor, r->addr);
		srch->wrap_flag = r->wrap_flag;
		srch->last_repl = r->last_repl;
		demote(SRCHREC, link, &fsr, r);
	}
}

unsigned char *rest_key = (unsigned char *) _("|rest of file|rR");
unsigned char *backup_key = (unsigned char *) _("|backup|bB");

static int dopfrepl(BW *bw, int c, SRCH *srch, int *notify)
{
	srch->addr = bw->cursor->byte;
	/* Backspace means no for jmacs */
	if (c == NO_CODE || c == 8 || c == 127 || yncheck(no_key, c))
		return dopfnext(bw, srch, notify);
	else if (c == YES_CODE || yncheck(yes_key, c) || c == ' ') {
		srch->recs.link.prev->yn = 1;
		if (doreplace(bw, srch)) {
			pfsave(bw, srch);
			return -1;
		} else
			return dopfnext(bw, srch, notify);
	} else if (yncheck(rest_key, c) || c == '!') {
		if (doreplace(bw, srch))
			return -1;
		srch->rest = 1;
		return dopfnext(bw, srch, notify);
	} else if (/* c == 8 || c == 127 || */ yncheck(backup_key, c)) {
		W *w = bw->parent;
		goback(srch, bw);
		goback(srch, (BW *)w->object);
		return dopfnext((BW *)w->object, srch, notify);
	} else if (c != -1) {
		if (notify)
			*notify = 1;
		pfsave(bw, srch);
		nungetc(c);
		return 0;
	}
	if (mkqwnsr(bw->parent, sz(joe_gettext(_("Replace (Y)es (N)o (R)est (B)ackup (^C to abort)?"))), dopfrepl, pfsave, srch, notify))
		return 0;
	else
		return pfsave(bw, srch);
}

/* Test if found text is within region
 * return 0 if it is,
 * -1 if we should keep searching
 * 1 if we're done
 */

static int restrict_to_block(BW *bw, SRCH *srch)
{
	if (!srch->valid || !srch->block_restrict)
		return 0;
	bw->cursor->xcol = piscol(bw->cursor);
	if (srch->backwards)
		if (!square) {
			if (bw->cursor->byte < srch->markb->byte)
				return 1;
			else if (bw->cursor->byte + sLEN(srch->entire) > srch->markk->byte)
				return -1;
		} else {
			if (bw->cursor->line < srch->markb->line)
				return 1;
			else if (bw->cursor->line > srch->markk->line)
				return -1;
			else if (piscol(bw->cursor) + sLEN(srch->entire) > srch->markk->xcol || piscol(bw->cursor) < srch->markb->xcol)
				return -1;
	} else if (!square) {
		if (bw->cursor->byte > srch->markk->byte)
			return 1;
		else if (bw->cursor->byte - sLEN(srch->entire) < srch->markb->byte)
			return -1;
	} else {
		if (bw->cursor->line > srch->markk->line)
			return 1;
		if (bw->cursor->line < srch->markb->line)
			return -1;
		if (piscol(bw->cursor) > srch->markk->xcol || piscol(bw->cursor) - sLEN(srch->entire) < srch->markb->xcol)
			return -1;
	}
	return 0;
}

/* Possible results:
 *   0) Search or search & replace is finished.
 *   1) Search string was not found.
 *   2) Search string was found.
 *   3) Abort due to infinite loop
 */

static int fnext(BW *bw, SRCH *srch)
{
	P *sta;

	if (!srch->first) {
		srch->first = bw->b;
		srch->current = bw->b;
	}

	next:
	if (srch->repeat != -1) {
		if (!srch->repeat)
			return 0;
		else
			--srch->repeat;
	}
	again:
	if (srch->backwards)
		sta = searchb(bw, srch, bw->cursor);
	else
		sta = searchf(bw, srch, bw->cursor);
	if (!sta && srch->all) {
		B *b;
		if (srch->all == 2)
			b = beafter(srch->current);
		else {
			berror = 0;
			b = bafter(srch->current);
		}
		if (b && b != srch->first && !berror) {
			W *w = bw->parent;
			srch->current = b;
			/* this bumps reference count of b */
			get_buffer_in_window(bw, b);
			bw = (BW *)w->object;
			p_goto_bof(bw->cursor);
			goto again;
		} else if (berror) {
			msgnw(bw->parent, joe_gettext(msgs[-berror]));
		}
	}
	if (!sta) {
		srch->repeat = -1;
		return 1;
	}
	if (srch->rest || (srch->repeat != -1 && srch->replace)) {
		if (srch->valid)
			switch (restrict_to_block(bw, srch)) {
			case -1:
				goto again;
			case 1:
				if (srch->addr >= 0)
					pgoto(bw->cursor, srch->addr);
				return !srch->rest;
			}
		if (doreplace(bw, srch))
			return 0;
		goto next;
	} else if (srch->repeat != -1) {
		if (srch->valid)
			switch (restrict_to_block(bw, srch)) {
			case -1:
				goto again;
			case 1:
				if (srch->addr >= 0)
					pgoto(bw->cursor, srch->addr);
				return 1;
			}
		srch->addr = bw->cursor->byte;
		goto next;
	} else
		return 2;
}

int dopfnext(BW *bw, SRCH *srch, int *notify)
{
	W *w;
	int fnr;
	int orgmid = mid;	/* Original mid status */
	int ret = 0;

	mid = 1;		/* Screen recenters mode during search */
	if (csmode)
		smode = 2;	/* We have started a search mode */
	if (srch->replace)
		visit(srch, bw, 0);
again:	w = bw->parent;
	fnr = fnext(bw, srch);
	bw  = (BW *)w->object;
	switch (fnr) {
	case 0:
		break;
	case 1:
bye:		if (!srch->flg && !srch->rest) {
			if (srch->valid && srch->block_restrict)
				msgnw(bw->parent, joe_gettext(_("Not found (search restricted to marked block)")));
			else
				msgnw(bw->parent, joe_gettext(_("Not found")));
			ret = -1;
		}
		break;
	case 3:
		msgnw(bw->parent, joe_gettext(_("Infinite loop aborted: your search repeatedly matched same place")));
		ret = -1;
		break;
	case 2:
		if (srch->valid)
			switch (restrict_to_block(bw, srch)) {
			case -1:
				goto again;
			case 1:
				if (srch->addr >= 0)
					pgoto(bw->cursor, srch->addr);
				goto bye;
			}
		srch->addr = bw->cursor->byte;

		/* Make sure found text is fully on screen */
		if(srch->backwards) {
			bw->offset=0;
			pfwrd(bw->cursor,sLEN(srch->entire));
			bw->cursor->xcol = piscol(bw->cursor);
			dofollows();
			pbkwd(bw->cursor,sLEN(srch->entire));
		} else {
			bw->offset=0;
			pbkwd(bw->cursor,sLEN(srch->entire));
			bw->cursor->xcol = piscol(bw->cursor);
			dofollows();
			pfwrd(bw->cursor,sLEN(srch->entire));
		}

		if (srch->replace) {
			if (square)
				bw->cursor->xcol = piscol(bw->cursor);
			if (srch->backwards) {
				pdupown(bw->cursor, &markb, USTR "dopfnext");
				markb->xcol = piscol(markb);
				pdupown(markb, &markk, USTR "dopfnext");
				pfwrd(markk, (long) sLEN(srch->entire));
				markk->xcol = piscol(markk);
			} else {
				pdupown(bw->cursor, &markk, USTR "dopfnext");
				markk->xcol = piscol(markk);
				pdupown(bw->cursor, &markb, USTR "dopfnext");
				pbkwd(markb, (long) sLEN(srch->entire));
				markb->xcol = piscol(markb);
			}
			srch->flg = 1;
			if (dopfrepl(bw, -1, srch, notify))
				ret = -1;
			notify = 0;
			srch = 0;
		}
		break;
	}
	bw->cursor->xcol = piscol(bw->cursor);
	dofollows();
	mid = orgmid;
	if (notify)
		*notify = 1;
	if (srch)
		pfsave(bw, srch);
	else
		updall();
	return ret;
}

int pfnext(BW *bw)
{
	if (!globalsrch) {	/* Query for search string if there isn't any */
		return pffirst(bw);
	} else {
		SRCH *srch = globalsrch;

		globalsrch = 0;
		srch->addr = bw->cursor->byte;
		if (!srch->wrap_p || srch->wrap_p->b!=bw->b) {
			prm(srch->wrap_p);
			srch->wrap_p = pdup(bw->cursor, USTR "pfnext");
			srch->wrap_p->owner = &srch->wrap_p;
			srch->wrap_flag = 0;
		}
		return dopfnext(bw, setmark(srch), NULL);
	}
}

void save_srch(FILE *f)
{
	if(globalsrch) {
		if(globalsrch->pattern) {
			fprintf(f,"	pattern ");
			emit_string(f,globalsrch->pattern,sLEN(globalsrch->pattern));
			fprintf(f,"\n");
		}
		if(globalsrch->replacement) {
			fprintf(f,"	replacement ");
			emit_string(f,globalsrch->replacement,sLEN(globalsrch->replacement));
			fprintf(f,"\n");
		}
		fprintf(f,"	backwards %d\n",globalsrch->backwards);
		fprintf(f,"	ignore %d\n",globalsrch->ignore);
		fprintf(f,"	replace %d\n",globalsrch->replace);
		fprintf(f,"	block_restrict %d\n",globalsrch->block_restrict);
	}
	fprintf(f,"done\n");
}

void load_srch(FILE *f)
{
	unsigned char buf[1024];
	unsigned char bf[1024];
	unsigned char *pattern = 0;
	unsigned char *replacement = 0;
	int backwards = 0;
	int ignore = 0;
	int replace = 0;
	int block_restrict = 0;
	while(fgets((char *)buf,1023,f) && zcmp(buf,USTR "done\n")) {
		unsigned char *p=buf;
		parse_ws(&p,'#');
		if(!parse_kw(&p,USTR "pattern")) {
			int len;
			parse_ws(&p,'#');
			bf[0] = 0;
			len = parse_string(&p,bf,sizeof(bf));
			if (len>0)
				pattern = vsncpy(NULL,0,bf,len);
		} else if(!parse_kw(&p,USTR "replacement")) {
			int len;
			parse_ws(&p,'#');
			bf[0] = 0;
			len = parse_string(&p,bf,sizeof(bf));
			if (len>0)
				replacement = vsncpy(NULL,0,bf,len);
		} else if(!parse_kw(&p,USTR "backwards")) {
			parse_ws(&p,'#');
			parse_int(&p,&backwards);
		} else if(!parse_kw(&p,USTR "ignore")) {
			parse_ws(&p,'#');
			parse_int(&p,&ignore);
		} else if(!parse_kw(&p,USTR "replace")) {
			parse_ws(&p,'#');
			parse_int(&p,&replace);
		} else if(!parse_kw(&p,USTR "block_restrict")) {
			parse_ws(&p,'#');
			parse_int(&p,&block_restrict);
		}
	}
	globalsrch = mksrch(pattern,replacement,ignore,backwards,-1,replace,0,0);
	globalsrch->block_restrict = block_restrict;
}
