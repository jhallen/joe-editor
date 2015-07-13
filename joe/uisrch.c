/*
 *	Incremental search
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

typedef struct irec IREC;

struct irec {
	LINK(IREC)	link;
	ptrdiff_t	what;		/* 0 repeat, >0 append n chars */
	off_t	start;		/* Cursor search position */
	off_t	disp;		/* Original cursor position */
	int	wrap_flag;	/* Wrap flag */
};

struct isrch {
	IREC	irecs;		/* Linked list of positions */
	char *pattern;	/* Search pattern string */
	char *prompt;	/* Prompt (usually same as pattern unless utf-8/byte conversion) */
	ptrdiff_t	ofst;		/* Offset in pattern past prompt */
	int	dir;		/* 0=fwrd, 1=bkwd */
	int	quote;		/* Set to quote next char */
};

struct isrch *lastisrch = NULL;	/* Previous search */

char *lastpat = NULL;	/* Previous pattern */

IREC fri = { {&fri, &fri} };	/* Free-list of irecs */

static IREC *alirec(void)
{				/* Allocate an IREC */
	return (IREC *)alitem(&fri, SIZEOF(IREC));
}

static void frirec(IREC *i)
{				/* Free an IREC */
	enquef(IREC, link, &fri, i);
}

static void rmisrch(struct isrch *isrch)
{				/* Eliminate a struct isrch */
	if (isrch) {
		vsrm(isrch->pattern);
		vsrm(isrch->prompt);
		frchn(&fri, &isrch->irecs);
		joe_free(isrch);
	}
}

static int iabrt(W *w, void *obj)
{				/* User hit ^C */
	struct isrch *isrch = (struct isrch *)obj;
	rmisrch(isrch);
	return -1;
}

static void iappend(BW *bw, struct isrch *isrch, char *s, ptrdiff_t len)
{				/* Append text and search */
	/* Append char and search */
	IREC *i = alirec();
	SRCH *srch;

	i->what = len;
	i->disp = bw->cursor->byte;
	isrch->pattern = vsncpy(sv(isrch->pattern), s, len);
	if (!qempty(IREC, link, &isrch->irecs)) {
		pgoto(bw->cursor, isrch->irecs.link.prev->start);
	}
	i->start = bw->cursor->byte;

	if (!globalsrch)
		srch = mksrch(NULL,NULL,opt_icase,isrch->dir,-1,0,0,0);
	else {
		srch = globalsrch;
		globalsrch = 0;
	}

	srch->addr = bw->cursor->byte;

	if (!srch->wrap_p || srch->wrap_p->b!=bw->b) {
		prm(srch->wrap_p);
		srch->wrap_p = pdup(bw->cursor, "iappend");
		srch->wrap_p->owner = &srch->wrap_p;
		srch->wrap_flag = 0;
	}

	i->wrap_flag = srch->wrap_flag;

	vsrm(srch->pattern);
	srch->pattern = vsncpy(NULL, 0, isrch->pattern, sLen(isrch->pattern));
	srch->backwards = isrch->dir;

	if (dopfnext(bw, srch, NULL)) {
		if(joe_beep)
			ttputc(7);
	}
	enqueb(IREC, link, &isrch->irecs, i);
}

/* Main user interface */
/* When called with c==-1, it just creates the prompt */
static int itype(W *w, int c, void *obj, int *notify)
{
	IREC *i;
	int omid;
	BW *bw;
	struct isrch *isrch = (struct isrch *)obj;
	WIND_BW(bw,w);

	if (isrch->quote) {
		goto in;
	}
	if (c == 8 || c == 127) {	/* Backup */
		if ((i = isrch->irecs.link.prev) != &isrch->irecs) {
			pgoto(bw->cursor, i->disp);
			if (globalsrch)
				globalsrch->wrap_flag = i->wrap_flag;
			omid = opt_mid;
			opt_mid = 1;
			dofollows();
			opt_mid = omid;
			isrch->pattern = vstrunc(isrch->pattern, sLEN(isrch->pattern) - i->what);
			frirec(deque_f(IREC, link, i));
		} else {
			if(joe_beep)
				ttputc(7);
		}
	} else if (c == 'Q' - '@' /* || c == '`' */) {
		isrch->quote = 1;
	} else if (c == 'S' - '@' || c == '\\' - '@' || c == 'L' - '@' || c == 'R' - '@') {
		/* Repeat */
		if (c == 'R' - '@') {
			isrch->dir = 1;
		} else {
			isrch->dir = 0;
		}
		if (qempty(IREC, link, &isrch->irecs)) {
			if (lastpat && lastpat[0]) {
				iappend(bw, isrch, sv(lastpat));
			}
		} else {
			SRCH *srch;
			i = alirec();
			i->disp = i->start = bw->cursor->byte;
			i->what = 0;

			if (!globalsrch)
				srch = mksrch(NULL,NULL,opt_icase,isrch->dir,-1,0,0,0);
			else {
				srch = globalsrch;
				globalsrch = 0;
			}

			srch->addr = bw->cursor->byte;

			if (!srch->wrap_p || srch->wrap_p->b!=bw->b) {
				prm(srch->wrap_p);
				srch->wrap_p = pdup(bw->cursor, "itype");
				srch->wrap_p->owner = &srch->wrap_p;
				srch->wrap_flag = 0;
			}

			i->wrap_flag = srch->wrap_flag;

			vsrm(srch->pattern);
			srch->pattern = vsncpy(NULL, 0, isrch->pattern, sLen(isrch->pattern));
			srch->backwards = isrch->dir;

			if (dopfnext(bw, srch, NULL)) {
				if(joe_beep)
					ttputc(7);
				frirec(i);
			} else {
				enqueb(IREC, link, &isrch->irecs, i);
			}
		}
	} else if (c >= 0 && c < 32) {
		/* Done when a control character is received */
		nungetc(c);
		if (notify) {
			*notify = 1;
		}
		smode = 2;
		if (lastisrch) {
			lastpat = vstrunc(lastpat, 0);
			lastpat = vsncpy(lastpat, 0, lastisrch->pattern, sLen(lastisrch->pattern));
			rmisrch(lastisrch);
		}
		lastisrch = isrch;
		return 0;
	} else if (c != -1) {
		char buf[16];
		ptrdiff_t buf_len;
		/* Search */

		in:

		/* Convert to/from utf-8 */
		if (locale_map->type && !bw->b->o.charmap->type) {
			utf8_encode(buf,c);
			c = from_utf8(bw->b->o.charmap,buf);
		} else if(!locale_map->type && bw->b->o.charmap->type) {
			to_utf8(locale_map,buf,c);
			c = utf8_decode_string(buf);
		}

		if (bw->b->o.charmap->type) {
			buf_len = utf8_encode(buf,c);
		} else {
			buf[0] = TO_CHAR_OK(c);
			buf_len = 1;
		}		

		isrch->quote = 0;
		iappend(bw, isrch, buf, buf_len);
	}
	omid = opt_mid;
	opt_mid = 1;
	bw->cursor->xcol = piscol(bw->cursor);
	dofollows();
	opt_mid = omid;

	isrch->prompt = vstrunc(isrch->prompt, isrch->ofst);

	if (locale_map->type && !bw->b->o.charmap->type) {
		/* Translate bytes to utf-8 */
		char buf[16];
		int x;
		for (x=0; x!=sLEN(isrch->pattern); ++x) {
			int tc = to_uni(bw->b->o.charmap, isrch->pattern[x]);
			utf8_encode(buf, tc);
			isrch->prompt = vsncpy(sv(isrch->prompt),sz(buf));
		}
	} else if (!locale_map->type && bw->b->o.charmap->type) {
		/* Translate utf-8 to bytes */
		const char *p = isrch->pattern;
		ptrdiff_t len = sLEN(isrch->pattern);
		while (len) {
			int tc = utf8_decode_fwrd(&p, &len);
			if (tc >= 0) {
				tc = from_uni(locale_map, tc);
				isrch->prompt = vsadd(isrch->prompt, TO_CHAR_OK(tc));
			}
		}
	} else {
		/* FIXME: translate when charmaps do not match */
		isrch->prompt = vsncpy(sv(isrch->prompt),sv(isrch->pattern));
	}

	if (mkqwnsr(bw->parent, sv(isrch->prompt), itype, iabrt, isrch, notify)) {
		return 0;
	} else {
		rmisrch(isrch);
		return -1;
	}
}

static int doisrch(BW *bw, int dir)
{				/* Create a struct isrch */
	struct isrch *isrch = (struct isrch *) joe_malloc(SIZEOF(struct isrch));

	izque(IREC, link, &isrch->irecs);
	isrch->pattern = vsncpy(NULL, 0, NULL, 0);
	isrch->dir = dir;
	isrch->quote = 0;
	isrch->prompt = vsncpy(NULL, 0, sz(joe_gettext(_("I-find: "))));
	isrch->ofst = sLen(isrch->prompt);
	return itype(bw->parent, -1, isrch, NULL);
}

int uisrch(W *w, int k)
{
	BW *bw;
	WIND_BW(bw, w);
	if (smode && lastisrch) {
		struct isrch *isrch = lastisrch;

		lastisrch = 0;
		return itype(bw->parent, 'S' - '@', isrch, NULL);
	} else {
		if (globalsrch) {
			rmsrch(globalsrch);
			globalsrch = 0;
		}
		if (lastisrch) {
			lastpat = vstrunc(lastpat, 0);
			lastpat = vsncpy(lastpat, 0, lastisrch->pattern, sLen(lastisrch->pattern));
			rmisrch(lastisrch);
			lastisrch = 0;
		}
		return doisrch(bw, 0);
	}
}

int ursrch(W *w, int k)
{
	BW *bw;
	WIND_BW(bw, w);
	if (smode && lastisrch) {
		struct isrch *isrch = lastisrch;

		lastisrch = 0;
		return itype(bw->parent, 'R' - '@', isrch, NULL);
	} else {
		if (globalsrch) {
			rmsrch(globalsrch);
			globalsrch = 0;
		}
		if (lastisrch) {
			lastpat = vstrunc(lastpat, 0);
			lastpat = vsncpy(lastpat, 0, lastisrch->pattern, sLen(lastisrch->pattern));
			rmisrch(lastisrch);
			lastisrch = 0;
		}
		return doisrch(bw, 1);
	}
}
