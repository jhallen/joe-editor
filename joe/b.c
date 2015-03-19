/*
 *	Editor engine
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#ifndef S_ISLNK
#ifdef S_IFLNK
#define S_ISLNK(n) (((n) & (S_IFMT)) == (S_IFLNK))
#else
#define S_ISLNK(n) (0)
#endif
#endif

extern int errno;

#ifdef WITH_SELINUX
#include <selinux/selinux.h>
static int selinux_enabled = -1;
#endif

unsigned char stdbuf[stdsiz];

int guesscrlf = 0;
int guessindent = 0;

int berror;
int force = 0;
VFILE *vmem;

int nodeadjoe = 0;

unsigned char *msgs[] = {
	USTR _("No error"),
	USTR _("New File"),
	USTR _("Error reading file"),
	USTR _("Error seeking file"),
	USTR _("Error opening file"),
	USTR _("Error writing file"),
	USTR _("File on disk is newer")
};

/* Get size of gap (amount of free space) */
#define GGAPSZ(hdr) ((hdr)->ehole - (hdr)->hole)

/* Get number of characters in gap buffer */
#define GSIZE(hdr) (SEGSIZ - GGAPSZ(hdr))

/* Get char from buffer (with jumping around the gap) */
#define GCHAR(p) ((p)->ofst >= (p)->hdr->hole ? (p)->ptr[(p)->ofst + GGAPSZ((p)->hdr)] \
					      : (p)->ptr[(p)->ofst])

/* Set position of gap */
static void gstgap(H *hdr, unsigned char *ptr, int ofst)
{
	if (ofst > hdr->hole) {
		mmove(ptr + hdr->hole, ptr + hdr->ehole, ofst - hdr->hole);
		vchanged(ptr);
	} else if (ofst < hdr->hole) {
		mmove(ptr + hdr->ehole - (hdr->hole - ofst), ptr + ofst, hdr->hole - ofst);
		vchanged(ptr);
	}
	hdr->ehole = ofst + hdr->ehole - hdr->hole;
	hdr->hole = ofst;
}

/* Insert a block */
static void ginsm(H *hdr, unsigned char *ptr, int ofst, unsigned char *blk, int size)
{
	if (ofst != hdr->hole)
		gstgap(hdr, ptr, ofst);
	mmove(ptr + hdr->hole, blk, size);
	hdr->hole += size;
	vchanged(ptr);
}

/* Read block */
static void grmem(H *hdr, unsigned char *ptr, int ofst, unsigned char *blk, int size)
{
	if (ofst < hdr->hole)
		if (size > hdr->hole - ofst) {
			mmove(blk, ptr + ofst, hdr->hole - ofst);
			mmove(blk + hdr->hole - ofst, ptr + hdr->ehole, size - (hdr->hole - ofst));
		} else
			mmove(blk, ptr + ofst, size);
	else
		mmove(blk, ptr + ofst + hdr->ehole - hdr->hole, size);
}


static H nhdrs = { {&nhdrs, &nhdrs} };
static H ohdrs = { {&ohdrs, &ohdrs} };

/* Header allocation */
static H *halloc(void)
{
	H *h;

	if (qempty(H, link, &ohdrs)) {
		h = (H *) alitem(&nhdrs, sizeof(H));
		h->seg = my_valloc(vmem, (long) SEGSIZ);
	} else
		h = deque_f(H, link, ohdrs.link.next);
	h->hole = 0;
	h->ehole = SEGSIZ;
	h->nlines = 0;
	izque(H, link, h);
	return h;
}

static void hfree(H *h)
{
	enquef(H, link, &ohdrs, h);
}

static void hfreechn(H *h)
{
	splicef(H, link, &ohdrs, h);
}


static P frptrs = { {&frptrs, &frptrs} };

/* Pointer allocation */
static P *palloc(void)
{
	return alitem(&frptrs, sizeof(P));
}

static void pfree(P *p)
{
	enquef(P, link, &frptrs, p);
}

/* Doubly linked list of buffers and free buffer structures */
B bufs = { {&bufs, &bufs} };
static B frebufs = { {&frebufs, &frebufs} };

void set_file_pos_orphaned()
{
	B *b;
	for (b = bufs.link.next; b != &bufs; b = b->link.next)
		if (b->orphan && b->oldcur)
			set_file_pos(b->name,b->oldcur->line);
}

/* Find next buffer in list: for multi-file search and replace */
/* This does not bump reference count on found buffer */

B *bafter(B *b)
{
	for (b = b->link.next; b->internal || b->scratch || b == &bufs; b = b->link.next);
	return b;
}

int udebug_joe(BW *bw)
{
	unsigned char buf[1024];

	B *b;
	P *p;

	binss(bw->cursor, USTR "Buffers and pointers (the number of pointers per buffer should not grow, except for 20 from markpos):\n\n");
	pnextl(bw->cursor);

	for (b = bufs.link.next; b != &bufs; b = b->link.next) {
		if (b->name)
			joe_snprintf_1(buf, sizeof(buf), "Buffer %s\n", b->name);
		else
			joe_snprintf_1(buf, sizeof(buf), "Buffer 0x%p\n", (void *)b);
		binss(bw->cursor, buf);
		pnextl(bw->cursor);
		for (p = b->bof->link.next; p != b->bof; p = p->link.next) {
			joe_snprintf_1(buf, sizeof(buf), "  Pointer created by %s\n", p->tracker);
			binss(bw->cursor, buf);
			pnextl(bw->cursor);
		}
	}
	dump_syntax(bw);
	return 0;
}

B *bnext(void)
{
	B *b;

	do {
		b = bufs.link.prev;
		deque(B, link, &bufs);
		enqueb(B, link, b, &bufs);
	} while (b->internal);
	return b;
}

B *bprev(void)
{
	B *b;

	do {
		b = bufs.link.next;
		deque(B, link, &bufs);
		enquef(B, link, b, &bufs);
	} while (b->internal);
	return b;
}

/* Make a buffer out of a chain */
static B *bmkchn(H *chn, B *prop, long amnt, long nlines)
{
	B *b = alitem(&frebufs, sizeof(B));

	b->undo = undomk(b);
	if (prop)
		b->o = prop->o;
	else
		b->o = pdefault;
	mset(b->marks, 0, sizeof(b->marks));
	b->rdonly = 0;
	b->orphan = 0;
	b->oldcur = NULL;
	b->err = NULL;
 	b->oldtop = NULL;
 	b->current_dir = NULL;
	b->shell_flag = 0;
	b->backup = 1;
	b->internal = 1;
	b->scratch = 0;
	b->changed = 0;
	b->gave_notice = 0;
	b->locked = 0;
	b->ignored_lock = 0;
	b->didfirst = 0;
	b->count = 1;
	b->name = NULL;
	b->er = -3;
	b->bof = palloc();
	b->mod_time = 0;
	b->check_time = time(NULL);
	izque(P, link, b->bof);
	b->bof->end = 0;
	b->bof->b = b;
	b->bof->owner = NULL;
	b->bof->hdr = chn;
	b->bof->ptr = vlock(vmem, b->bof->hdr->seg);
	b->bof->ofst = 0;
	b->bof->byte = 0;
	b->bof->line = 0;
	b->bof->col = 0;
	b->bof->xcol = 0;
	b->bof->valcol = 1;
	b->bof->attr = 0;
	b->bof->valattr = 1;
	b->bof->tracker = USTR "bmkchn";
	b->eof = pdup(b->bof, USTR "bmkchn");
	b->eof->end = 1;
	vunlock(b->eof->ptr);
	b->eof->hdr = chn->link.prev;
	b->eof->ptr = vlock(vmem, b->eof->hdr->seg);
	b->eof->ofst = GSIZE(b->eof->hdr);
	b->eof->byte = amnt;
	b->eof->line = nlines;
	b->eof->valcol = 0;
	b->eof->valattr = 0;
	b->pid = 0;
	b->out = -1;
	b->vt = 0;
	b->db = 0;
	b->parseone = 0;
	enquef(B, link, &bufs, b);
	pcoalesce(b->bof);
	pcoalesce(b->eof);
	return b;
}

/* Create an empty buffer */
B *bmk(B *prop)
{
	return bmkchn(halloc(), prop, 0L, 0L);
}

/* Eliminate a buffer */
void brm(B *b)
{
	if (b && !--b->count) {
		if (b->changed)
			abrerr(b->name);
		if (b->locked && !b->ignored_lock && plain_file(b))
			unlock_it(b->name);
		if (b == errbuf)
			errbuf = NULL;
		if (b->undo)
			undorm(b->undo);
		if (b->eof) {
			hfreechn(b->eof->hdr);
			while (!qempty(P, link, b->bof))
				prm(b->bof->link.next);
			prm(b->bof);
		}
		if (b->name)
			joe_free(b->name);
		if (b->db)
			rm_all_lattr_db(b->db);
		vsrm(b->current_dir);
		demote(B, link, &frebufs, b);
	}
}

void brmall()
{
	while (!qempty(B, link, &bufs))
		brm(bufs.link.next);
}

/* Replace contents of b with n.  n is destroyed. */

void breplace(B *b, B *n)
{
	P *p, *next;

	/* Take out many references to b */

	abrerr(b->name);

	if (b->locked && !b->ignored_lock && plain_file(b)) {
		unlock_it(b->name);
		b->locked = 0;
	}

	if (b == errbuf)
		errbuf = NULL;

	if (b->undo) {
		undorm(b->undo);
		b->undo = 0;
	}

	/* Remove all vfile references */
	for (p = b->eof->link.next; p != b->eof; p = p->link.next)
		if (p->ptr)
			vunlock(p->ptr);
	if (b->eof->ptr)
		vunlock(b->eof->ptr);

	/* Delete buffer */
	hfreechn(b->eof->hdr);

	/* Delete file name */
	if (b->name)
		joe_free(b->name);

	reset_all_lattr_db(b->db);
	
	/* Take new name */
	b->name = zdup(n->name);

	/* Take bof Pointer */
	/* Taking n->bof's vlock */
	b->bof->ofst = n->bof->ofst;
	b->bof->ptr = n->bof->ptr;
	b->bof->hdr = n->bof->hdr;
	b->bof->byte = 0;
	b->bof->line = 0;
	b->bof->col = 0;
	b->bof->xcol = 0;
	b->bof->valcol = 1;
	b->bof->attr = 0;
	b->bof->valattr = 1;
	b->bof->end = 0;

	/* Take eof Pointer */
	/* Taking b->eof's vlock */
	b->eof->ofst = n->eof->ofst;
	b->eof->ptr = n->eof->ptr;
	b->eof->hdr = n->eof->hdr;
	b->eof->byte = n->eof->byte;
	b->eof->line = n->eof->line;
	b->eof->col = n->eof->col;
	b->eof->xcol = n->eof->xcol;
	b->eof->valcol = n->eof->valcol;
	b->eof->attr = n->eof->attr;
	b->eof->valattr = n->eof->valattr;
	b->eof->end = 1;

	/* Reset other pointers */
	for (p = b->eof->link.next; p != b->eof; p = p->link.next)
		if (p != b->bof) {
			long goal_line = p->line;
			long goal_col = p->xcol;
			p->ptr = 0; /* No need for pset to unlock: we already did it */
			if (goal_line > b->eof->line) {
				pset(p, b->eof);
				p_goto_bol(p);
			} else {
				pset(p, b->bof);
				pline(p, goal_line);
				pcol(p, goal_col);
			}
		}

	/* OK, delete pointers from n */
	for (p = n->eof->link.next; p != n->eof; p = next) {
		next = p->link.next;
		if (p != n->bof)
			prm(p);
	}

	/* Delete bof and eof pointers */
	/* Don't delete their locks, they were transferred. */
	n->bof->ptr = 0;
	prm(n->bof);
	n->bof = 0;
	n->eof->ptr = 0;
	prm(n->eof);
	n->eof = 0;

	b->undo = undomk(b);
	b->changed = 0;
	b->rdonly = n->rdonly;
	b->mod_time = n->mod_time;

	/* Delete rest of n */
	brm(n);
}

P *poffline(P *p)
{
	if (p->ptr) {
		vunlock(p->ptr);
		p->ptr = NULL;
	}
	return p;
}

P *ponline(P *p)
{
	if (!p->ptr)
		p->ptr = vlock(vmem, p->hdr->seg);
	return p;
}

B *boffline(B *b)
{
	P *p = b->bof;

	do {
		poffline(p);
	} while ((p = p->link.next) != b->bof);
	return b;
}

B *bonline(B *b)
{
	P *p = b->bof;

	do {
		ponline(p);
	} while ((p = p->link.next) != b->bof);
	return b;
}

P *pdup(P *p, unsigned char *tr)
{
	P *n = palloc();

	n->end = 0;
	n->ptr = NULL;
	n->owner = NULL;
	n->tracker = tr;
	enquef(P, link, p, n);
	return pset(n, p);
}

P *pdupown(P *p, P **o, unsigned char *tr)
{
	P *n = palloc();

	n->end = 0;
	n->ptr = NULL;
	n->owner = o;
	n->tracker = tr;
	enquef(P, link, p, n);
	pset(n, p);
	if (*o)
		prm(*o);
	*o = n;
	return n;
}

void prm(P *p)
{
	if (!p)
		return;
	if (p->owner)
		*p->owner = NULL;
	if (p->ptr)
		vunlock(p->ptr);
	pfree(deque_f(P, link, p));
}

P *pset(P *n, P *p)
{
	if (n != p) {
		n->b = p->b;
		n->ofst = p->ofst;
		n->hdr = p->hdr;
		if (n->ptr)
			vunlock(n->ptr);
		if (p->ptr) {
			n->ptr = p->ptr;
			vupcount(n->ptr);
		} else
			n->ptr = vlock(vmem, n->hdr->seg);
		n->byte = p->byte;
		n->line = p->line;
		n->col = p->col;
		n->valcol = p->valcol;
		n->attr = p->attr;
		n->valattr = p->valattr;
	}
	return n;
}

P *p_goto_bof(P *p)
{
	return pset(p, p->b->bof);
}

P *p_goto_eof(P *p)
{
	return pset(p, p->b->eof);
}

/* is p at the beginning of file? */
int pisbof(P *p)
{
	return p->hdr == p->b->bof->hdr && !p->ofst;
}

/* is p at the end of file? */
int piseof(P *p)
{
	return p->ofst == GSIZE(p->hdr);
}

/* is p at the end of line? */
int piseol(P *p)
{
	int c;

	if (piseof(p))
		return 1;
	c = brc(p);
	if (c == '\n')
		return 1;
	if (p->b->o.crlf)
		if (c == '\r') {
			P *q = pdup(p, USTR "piseol");

			pfwrd(q, 1L);
			if (pgetb(q) == '\n') {
				prm(q);
				return 1;
			} else
				prm(q);
		}
	return 0;
}

/* is p at the beginning of line? */
/* This needs to be fast and should not disturb valcol or valattr.  It's used by fixupins(). */
int pisbol(P *p)
{
	unsigned char c;

	if (pisbof(p))
		return 1;

	if (!p->ofst)
		pprev(p);

	--p->ofst;
	c = GCHAR(p);

	if (++p->ofst == GSIZE(p->hdr))
		pnext(p);

	return c == '\n';
}

/* is p at the beginning of word? */
int pisbow(P *p)
{
	P *q = pdup(p, USTR "pisbow");
	int c = brc(p);
	int d = prgetc(q);

	prm(q);
	if (joe_isalnum_(p->b->o.charmap,c) && (!joe_isalnum_(p->b->o.charmap,d) || pisbof(p)))
		return 1;
	else
		return 0;
}

/* is p at the end of word? */
int piseow(P *p)
{
	P *q = pdup(p, USTR "piseow");
	int d = brc(q);
	int c = prgetc(q);

	prm(q);
	if (joe_isalnum_(p->b->o.charmap,c) && (!joe_isalnum_(p->b->o.charmap,d) || piseof(p)))
		return 1;
	else
		return 0;
}

/* is p on the blank line (ie. full of spaces/tabs)? */
int pisblank(P *p)
{
	P *q = pdup(p, USTR "pisblank");

	p_goto_bol(q);
	while (joe_isblank(p->b->o.charmap,brc(q)))
		pgetb(q);
	if (piseol(q)) {
		prm(q);
		return 1;
	} else {
		prm(q);
		return 0;
	}
}

/* is p at end of line or spaces followed by end of line? */
int piseolblank(P *p)
{
	P *q = pdup(p, USTR "piseolblank");

	while (joe_isblank(p->b->o.charmap,brc(q)))
		pgetb(q);
	if (piseol(q)) {
		prm(q);
		return 1;
	} else {
		prm(q);
		return 0;
	}
}

/* return column of first nonblank character */
long pisindent(P *p)
{
	P *q = pdup(p, USTR "pisindent");
	long col;

	p_goto_bol(q);
	while (joe_isblank(p->b->o.charmap,brc(q)))
		pgetc(q);
	col = q->col;
	prm(q);
	return col;
}

/* return true if all characters to left of cursor match c */

int pispure(P *p,int c)
{
	P *q = pdup(p, USTR "pispure");
	p_goto_bol(q);
	while (q->byte!=p->byte)
		if (pgetc(q)!=c) {
			prm(q);
			return 0;
                }
	prm(q);
	return 1;
}

int pnext(P *p)
{
	if (p->hdr == p->b->eof->hdr) {
		p->ofst = GSIZE(p->hdr);
		return 0;
	}
	p->hdr = p->hdr->link.next;
	p->ofst = 0;
	vunlock(p->ptr);
	p->ptr = vlock(vmem, p->hdr->seg);
	return 1;
}

int pprev(P *p)
{
	if (p->hdr == p->b->bof->hdr) {
		p->ofst = 0;
		return 0;
	}
	p->hdr = p->hdr->link.prev;
	p->ofst = GSIZE(p->hdr);
	vunlock(p->ptr);
	p->ptr = vlock(vmem, p->hdr->seg);
	return 1;
}

/* return current byte and move p to the next byte.  column will be unchanged. */
int pgetb(P *p)
{
	unsigned char c;

	if (p->ofst == GSIZE(p->hdr))
		return NO_MORE_DATA;
	c = GCHAR(p);
	if (++p->ofst == GSIZE(p->hdr))
		pnext(p);
	++p->byte;
	if (c == '\n') {
		++(p->line);
		p->col = 0;
		p->valcol = 1;
		p->attr = 0;
		p->valattr = 1;
	} else if (p->b->o.crlf && c == '\r') {
		if (brc(p) == '\n')
			return pgetb(p);
		else {
			p->valcol = 0;
			p->valattr = 0;
		}
	} else {
		p->valcol = 0;
		p->valattr = 0;
	}
	return c;
}

/* Interned string table for ansi sequences */

HASH *ansi_hash;
struct ansi_entry {
	int code;
	unsigned char *name;
};
struct ansi_entry **ansi_table;
int ansi_siz;
int ansi_len;

int ansi_code(unsigned char *s)
{
	struct ansi_entry *e;
	if (!ansi_hash)
		ansi_hash = htmk(128);
	e = htfind(ansi_hash, s);
	if (!e) {
		e = joe_malloc(sizeof(struct ansi_entry));
		e->name = zdup(s);
		e->code = ansi_len;
		htadd(ansi_hash, e->name, e);
		if (!ansi_siz)
			ansi_table = (struct ansi_entry **)malloc(ansi_siz = 128);
		if (ansi_siz == ansi_len)
			ansi_table = (struct ansi_entry **)realloc(ansi_table, ansi_siz *= 2);
		ansi_table[ansi_len++] = e;
	}
	return (e->code | ANSI_BIT);
}

unsigned char *ansi_string(int code)
{
	code &= ~ANSI_BIT;
	if (code < 0 || code >= ansi_len)
		return 0;
	else
		return ansi_table[code]->name;
}

#define ANSIMAX 64

/* return current character and move p to the next character.  column will be updated if it was valid. */
int pgetc(P *p)
{
	if (p->b->o.charmap->type) {
		int val;
		int valattr;
		int c; /* , oc; */
		int d;
		int n; /* , m; */
		int wid = 0;

		val = p->valcol;	/* Remember if column number was valid */
		valattr = p->valattr;
		c = pgetb(p);		/* Get first byte */
		/* oc = c; */

		if (c==NO_MORE_DATA)
			return c;

                if (p->b->o.ansi && c == '\033') { /* Hide ansi */
                	unsigned char buf[ANSIMAX];
                	int bufx = 0;
                	buf[bufx++] = c;
                        while ((d = pgetb(p)) != NO_MORE_DATA) {
                        	if (bufx < sizeof(buf) - 1)
                        		buf[bufx++] = d;
                                if ((d >= 'A' && d <= 'Z') || (d >= 'a' && d <= 'z') || d == '\n')
                                        break;
			}
                        p->valcol |= val;
                        p->valattr = 0;
                        buf[bufx] = 0;
                        return ansi_code(buf);
                }

		if ((c&0xE0)==0xC0) { /* Two bytes */
			n = 1;
			c &= 0x1F;
		} else if ((c&0xF0)==0xE0) { /* Three bytes */
			n = 2;
			c &= 0x0F;
		} else if ((c&0xF8)==0xF0) { /* Four bytes */
			n = 3;
			c &= 0x07;
		} else if ((c&0xFC)==0xF8) { /* Five bytes */
			n = 4;
			c &= 0x03;
		} else if ((c&0xFE)==0xFC) { /* Six bytes */
			n = 5;
			c &= 0x01;
		} else if ((c&0x80)==0x00) { /* One byte */
			n = 0;
		} else { /* 128-191, 254, 255: Not a valid UTF-8 start character */
			n = 0;
			c = 'X';
			/* c -= 384; */
		}

		/* m = n; */

		if (n) {
			while (n) {
				d = brc(p);
				if ((d&0xC0)!=0x80)
					break;
				pgetb(p);
				c = ((c<<6)|(d&0x3F));
				--n;
			}
			if (n) { /* FIXME: there was a bad UTF-8 sequence */
				/* How to represent this? */
				/* pbkwd(p,m-n);
				c = oc - 384; */
				c = 'X';
				wid = 1;
			} else if (val)
				wid = joe_wcwidth(1,c);
		} else {
			wid = 1;
		}

		if (val) { /* Update column no. if it was valid to start with */
			p->valcol = 1;
			if (c=='\t')
				p->col += (p->b->o.tab) - (p->col) % (p->b->o.tab);
			else if (c=='\n')
				p->col = 0;
			else
				p->col += wid;
		}
		p->valattr |= valattr;

		return c;
	} else {
		unsigned char c;

		if (p->ofst == GSIZE(p->hdr))
			return NO_MORE_DATA;
		c = GCHAR(p);
		if (++p->ofst == GSIZE(p->hdr))
			pnext(p);
		++p->byte;

		if (c == '\n') {
			++(p->line);
			p->col = 0;
			p->valcol = 1;
			p->attr = 0;
			p->valattr = 1;
                } else if (p->b->o.ansi && c == '\033') { /* Hide ansi */
                        int d;
                        int v = p->valcol;
                        unsigned char buf[ANSIMAX];
                        int bufx = 0;
                        buf[bufx++] = c;
                        while ((d = pgetb(p)) != NO_MORE_DATA) {
                        	if (bufx < sizeof(buf) - 1)
                        		buf[bufx++] = d;
                                if ((d >= 'A' && d <= 'Z') || (d >= 'a' && d <= 'z') || d == '\n')
                                        break;
			}
			buf[bufx] = 0;
                        p->valcol |= v;
                        p->valattr = 0;
                        return ansi_code(buf);
		} else if (p->b->o.crlf && c == '\r') {
			if (brc(p) == '\n')
				return pgetc(p);
			else
				++p->col;
		} else {
			if (c == '\t')
				p->col += (p->b->o.tab) - (p->col) % (p->b->o.tab);
			else
				++(p->col);
		}
		return c;
	}
}

/* move p n bytes forward */
P *pfwrd(P *p, long n)
{
	if (!n)
		return p;
	p->valcol = 0;
	p->valattr = 0;
	do {
		if (p->ofst == GSIZE(p->hdr))
			do {
				if (!p->ofst) {
					p->byte += GSIZE(p->hdr);
					n -= GSIZE(p->hdr);
					p->line += p->hdr->nlines;
				}
				if (!pnext(p))
					return NULL;
			} while (n > GSIZE(p->hdr));
		if (GCHAR(p) == '\n')
			++p->line;
		++p->byte;
		++p->ofst;
	} while (--n);
	if (p->ofst == GSIZE(p->hdr))
		pnext(p);
	return p;
}

/* move p to the previous byte: does not take into account -crlf mode */
static int prgetb1(P *p)
{
	unsigned char c;

	if (!p->ofst)
		if (!pprev(p))
			return NO_MORE_DATA;
	--p->ofst;
	c = GCHAR(p);
	--p->byte;
	p->valcol = 0;
	p->valattr = 0;
	if (c == '\n')
		--p->line;
	return c;
}

/* move p to the previous byte */
int prgetb(P *p)
{
	int c = prgetb1(p);

	if (p->b->o.crlf && c == '\n') {
		c = prgetb1(p);
		if (c == '\r')
			return '\n';
		if (c != NO_MORE_DATA)
			pgetb(p);
		c = '\n';
	}
	return c;
}

/* move p to the previous character (try to keep col updated) */
int prgetc(P *p)
{
	if (p->b->o.charmap->type || p->b->o.ansi) {

		if (pisbol(p))
			return prgetb(p);
		else {
			P *q = pdup(p, USTR "prgetc");
			P *r;
			p_goto_bol(q);
			r = pdup(q, USTR "prgetc");
			while (q->byte<p->byte) {
				pset(r, q);
				pgetc(q);
			}
			pset(p,r);
			prm(r);
			prm(q);
			return brch(p);
		}

#if 0
		int d = 0;
		int c;
		int n = 0;
		int val = p->valcol;
		for(;;) {
			c = prgetb(p);
			if (c == NO_MORE_DATA)
				return NO_MORE_DATA;
			else if ((c&0xC0)==0x80) {
				d |= ((c&0x3F)<<n);
				n += 6;
			} else if ((c&0x80)==0x00) { /* One char */
				d = c;
				break;
			} else if ((c&0xE0)==0xC0) { /* Two chars */
				d |= ((c&0x1F)<<n);
				break;
			} else if ((c&0xF0)==0xE0) { /* Three chars */
				d |= ((c&0x0F)<<n);
				break;
			} else if ((c&0xF8)==0xF0) { /* Four chars */
				d |= ((c&0x07)<<n);
				break;
			} else if ((c&0xFC)==0xF8) { /* Five chars */
				d |= ((c&0x03)<<n);
				break;
			} else if ((c&0xFE)==0xFC) { /* Six chars */
				d |= ((c&0x01)<<n);
				break;
			} else { /* FIXME: Invalid (0xFE or 0xFF found) */
				break;
			}
		}

		if (val && c!='\t' && c!='\n') {
			p->valcol = 1;
			p->col -= joe_wcwidth(1,d);
		}
		
		return d;
#endif
	}
	else {
		return prgetb(p);
	}
}

/* move p n bytes backwards */
P *pbkwd(P *p, long n)
{
	if (!n)
		return p;
	p->valcol = 0;
	p->valattr = 0;
	do {
		if (!p->ofst)
			do {
				if (p->ofst) {
					p->byte -= p->ofst;
					n -= p->ofst;
					p->line -= p->hdr->nlines;
				}
				if (!pprev(p))
					return NULL;
			} while (n > GSIZE(p->hdr));
		--p->ofst;
		--p->byte;
		if (GCHAR(p) == '\n')
			--p->line;
	} while (--n);
	return p;
}

/* move p n characters forwards/backwards according to loc */
P *pgoto(P *p, long loc)
{
	if (loc > p->byte)
		pfwrd(p, loc - p->byte);
	else if (loc < p->byte)
		pbkwd(p, p->byte - loc);
	return p;
}

/* make p->col valid */
P *pfcol(P *p)
{
	long pos = p->byte;

	p_goto_bol(p);
	while (p->byte < pos)
		pgetc(p);
	return p;
}

/* move p to the beginning of line */
P *p_goto_bol(P *p)
{
	if (pprevl(p))
		pgetb(p);
	p->col = 0;
	p->valcol = 1;
	p->attr = 0;
	p->valattr = 1;
	return p;
}

/* move p to the indentation point */
P *p_goto_indent(P *p, int c)
{
	int d;
	p_goto_bol(p);
	while ((d=brc(p)), d==c || ((c==' ' || c=='\t') && (d==' ' || d=='\t')))
		pgetc(p);
	return p;
}

/* move p to the end of line */
P *p_goto_eol(P *p)
{
	if (p->b->o.crlf || p->b->o.charmap->type || p->b->o.ansi)
		while (!piseol(p))
			pgetc(p);
	else
		while (p->ofst != GSIZE(p->hdr)) {
			unsigned char c;

			c = GCHAR(p);
			if (c == '\n')
				break;
			else {
				++p->byte;
				++p->ofst;
				if (c == '\t')
					p->col += p->b->o.tab - p->col % p->b->o.tab;
				else
					++p->col;
				if (p->ofst == GSIZE(p->hdr))
					pnext(p);
			}
		}
	return p;
}

/* move p to the beginning of next line */
P *pnextl(P *p)
{
	int c;

	do {
		if (p->ofst == GSIZE(p->hdr))
			do {
				p->byte += GSIZE(p->hdr) - p->ofst;
				if (!pnext(p))
					return NULL;
			} while (!p->hdr->nlines);
		c = GCHAR(p);
		++p->byte;
		++p->ofst;
	} while (c != '\n');
	++p->line;
	p->col = 0;
	p->valcol = 1;
	p->attr = 0;
	p->valattr = 1;
	if (p->ofst == GSIZE(p->hdr))
		pnext(p);
	return p;
}

/* move p to the end of previous line */
P *pprevl(P *p)
{
	int c;

	p->valcol = 0;
	p->valattr = 0;
	do {
		if (!p->ofst)
			do {
				p->byte -= p->ofst;
				if (!pprev(p))
					return NULL;
			} while (!p->hdr->nlines);
		--p->ofst;
		--p->byte;
		c = GCHAR(p);
	} while (c != '\n');
	--p->line;
	if (p->b->o.crlf && c == '\n') {
		int k = prgetb1(p);

		if (k != '\r' && k != NO_MORE_DATA)
			pgetb(p);
	}
	return p;
}

/* move p to the given 'line' line */
P *pline(P *p, long line)
{
	if (line > p->b->eof->line) {
		pset(p, p->b->eof);
		return p;
	}
	if (line < labs(p->line - line)) {
		pset(p, p->b->bof);
	}
	if (labs(p->b->eof->line - line) < labs(p->line - line)) {
		pset(p, p->b->eof);
	}
	if (p->line == line) {
		p_goto_bol(p);
		return p;
	}
	while (line > p->line)
		pnextl(p);
	if (line < p->line) {
		while (line < p->line)
			pprevl(p);
		p_goto_bol(p);
	}
	return p;
}

/* move p to the given 'goalcol' column */
/* lands at exact column or on character which would cause us to go past goalcol */
P *pcol(P *p, long goalcol)
{
	p_goto_bol(p);
	if(p->b->o.charmap->type || p->b->o.ansi) {
		do {
			int c;
			int wid;

			c = brch(p);

			if (c == NO_MORE_DATA)
				break;

			if (c == '\n')
				break;

			if (p->b->o.crlf && c == '\r' && piseol(p))
				break;

			if (c == '\t')
				wid = p->b->o.tab - p->col % p->b->o.tab;
			else
				wid = joe_wcwidth(1,c);

			if (p->col + wid > goalcol)
				break;

			pgetc(p);
		} while (p->col != goalcol);
	} else {
		do {
			unsigned char c;
			int wid;

			if (p->ofst == GSIZE(p->hdr))
				break;
			c = GCHAR(p);
			if (c == '\n')
				break;
			if (p->b->o.crlf && c == '\r' && piseol(p))
				break;
			if (c == '\t')
				wid = p->b->o.tab - p->col % p->b->o.tab;
			else
				wid = 1;
			if (p->col + wid > goalcol)
				break;
			if (++p->ofst == GSIZE(p->hdr))
				pnext(p);
			++p->byte;
			p->col += wid;
		} while (p->col != goalcol);
	}
	return p;
}

/* Move to goal column, then skip backwards to just after first non-whitespace character */
P *pcolwse(P *p, long goalcol)
{
	int c;

	pcol(p, goalcol);
	do {
		c = prgetc(p);
	} while (c == ' ' || c == '\t');
	if (c != NO_MORE_DATA)
		pgetc(p);
	return p;
}

/* Move p to goalcol: stops after first character which equals or exceeds goal col (unlike
   pcol() which will stops before character which would exceed goal col) */
P *pcoli(P *p, long goalcol)
{
	p_goto_bol(p);
	if (p->b->o.charmap->type || p->b->o.ansi) {
		while (p->col < goalcol) {
			int c;
			c = brc(p);

			if (c == NO_MORE_DATA)
				break;

			if (c == '\n')
				break;

			if (p->b->o.crlf && c=='\r' && piseol(p))
				break;

			pgetc(p);
		}
	} else {
		while (p->col < goalcol) {
			unsigned char c;

			if (p->ofst == GSIZE(p->hdr))
				break;
			c = GCHAR(p);
			if (c == '\n')
				break;

			if (p->b->o.crlf && c == '\r' && piseol(p))
				break;

			if (c == '\t')
				p->col += p->b->o.tab - p->col % p->b->o.tab;
			else
				++p->col;
			if (++p->ofst == GSIZE(p->hdr))
				pnext(p);
			++p->byte;
		}
	}
	return p;
}

/* fill space between curent column and 'to' column with tabs/spaces */
void pfill(P *p, long to, int usetabs)
{
	if (usetabs=='\t')
		while (piscol(p) < to)
			if (p->col + p->b->o.tab - p->col % p->b->o.tab <= to) {
				binsc(p, '\t');
				pgetc(p);
			} else {
				binsc(p, ' ');
				pgetc(p);
			}
	else
		while (piscol(p) < to) {
			binsc(p, usetabs);
			pgetc(p);
		}
}

/* delete sequence of whitespaces - backwards */
void pbackws(P *p)
{
	int c;
	P *q = pdup(p, USTR "pbackws");

	do {
		c = prgetc(q);
	} while (c == ' ' || c == '\t');
	if (c != NO_MORE_DATA)
		pgetc(q);
	bdel(q, p);
	prm(q);
}

static int frgetc(P *p)
{
	if (!p->ofst)
		pprev(p);
	--p->ofst;
	return GCHAR(p);
}

static void ffwrd(P *p, int n)
{
	while (n > GSIZE(p->hdr) - p->ofst) {
		n -= GSIZE(p->hdr) - p->ofst;
		if (!pnext(p))
			return;
	}
	if ((p->ofst += n) == GSIZE(p->hdr))
		pnext(p);
}

/* forward find pattern 's' in text pointed by 'p' (Boyer-Moore algorithm) */
static P *ffind(P *p, unsigned char *s, int len)
{
	long amnt = p->b->eof->byte - p->byte;
	int x;
	unsigned char table[256], c;

	if (len > amnt)
		return NULL;
	if (!len)
		return p;
	p->valcol = 0;
	p->valattr = 0;
	mset(table, 255, 256);
	for (x = 0; x != len - 1; ++x)
		table[s[x]] = x;
	ffwrd(p, len);
	amnt -= len;
	x = len;
	do {
		if ((c = frgetc(p)) != s[--x]) {
			if (table[c] == 255) {
				ffwrd(p, len + 1);
				amnt -= x + 1;
			} else if (x <= table[c]) {
				ffwrd(p, len - x + 1);
				--amnt;
			} else {
				ffwrd(p, len - table[c]);
				amnt -= x - table[c];
			}
			if (amnt < 0)
				return NULL;
			else
				x = len;
		}
	} while (x);
	return p;
}

/* forward find (case insensitive) pattern 's' in text pointed by 'p' (Boyer-Moore algorithm) */
static P *fifind(P *p, unsigned char *s, int len)
{
	long amnt = p->b->eof->byte - p->byte;
	int x;
	struct charmap *map = p->b->o.charmap;
	unsigned char table[256], c;

	if (len > amnt)
		return NULL;
	if (!len)
		return p;
	p->valcol = 0;
	p->valattr = 0;
	mset(table, 255, 256);
	for (x = 0; x != len - 1; ++x)
		table[s[x]] = x;
	ffwrd(p, len);
	amnt -= len;
	x = len;
	do {
		if ((c = joe_tolower(map,frgetc(p))) != s[--x]) {
			if (table[c] == 255) {
				ffwrd(p, len + 1);
				amnt -= x + 1;
			} else if (x <= table[c]) {
				ffwrd(p, len - x + 1);
				--amnt;
			} else {
				ffwrd(p, len - table[c]);
				amnt -= x - table[c];
			}
			if (amnt < 0)
				return NULL;
			else
				x = len;
		}
	} while (x);
	return p;
}

/* move cursor p to q's position and set p's col, line, ofst, byte etc. accordingly */
/* same as rgetto() but p is before q */
static P *getto(P *p, P *q)
{
	while (p->hdr != q->hdr || p->ofst != q->ofst) {
		if (GCHAR(p) == '\n')
			++p->line;
		++p->byte;
		++p->ofst;
		if (p->ofst == GSIZE(p->hdr))
			pnext(p);
		while (!p->ofst && p->hdr != q->hdr) {
			p->byte += GSIZE(p->hdr);
			p->line += p->hdr->nlines;
			pnext(p);
		}
	}
	return p;
}

/* find forward substring s in text pointed by p and set p after found substring */
P *pfind(P *p, unsigned char *s, int len)
{
	P *q = pdup(p, USTR "pfind");

	if (ffind(q, s, len)) {
		getto(p, q);
		prm(q);
		return p;
	} else {
		prm(q);
		return NULL;
	}
}

/* same as pfind() but case insensitive */
P *pifind(P *p, unsigned char *s, int len)
{
	P *q = pdup(p, USTR "pifind");

	if (fifind(q, s, len)) {
		getto(p, q);
		prm(q);
		return p;
	} else {
		prm(q);
		return NULL;
	}
}

static void fbkwd(P *p, int n)
{
	while (n > p->ofst) {
		n -= p->ofst;
		if (!pprev(p))
			return;
	}
	if (p->ofst >= n)
		p->ofst -= n;
	else
		p->ofst = 0;
}

static int fpgetc(P *p)
{
	int c;

	if (p->ofst == GSIZE(p->hdr))
		return NO_MORE_DATA;
	c = GCHAR(p);
	if (++p->ofst == GSIZE(p->hdr))
		pnext(p);
	return c;
}

/* backward find pattern 's' in text pointed by 'p' (Boyer-Moore algorithm) */
static P *frfind(P *p, unsigned char *s, int len)
{
	long amnt = p->byte;
	int x;
	unsigned char table[256], c;

	if (len > p->b->eof->byte - p->byte) {
		x = len - (p->b->eof->byte - p->byte);
		if (amnt < x)
			return NULL;
		amnt -= x;
		fbkwd(p, x);
	}
	if (!len)
		return p;
	p->valcol = 0;
	p->valattr = 0;
	mset(table, 255, 256);
	for (x = len; --x; table[s[x]] = len - x - 1) ;
	x = 0;
	do {
		if ((c = fpgetc(p)) != s[x++]) {
			if (table[c] == 255) {
				fbkwd(p, len + 1);
				amnt -= len - x + 1;
			} else if (len - table[c] <= x) {
				fbkwd(p, x + 1);
				--amnt;
			} else {
				fbkwd(p, len - table[c]);
				amnt -= len - table[c] - x;
			}
			if (amnt < 0)
				return NULL;
			else
				x = 0;
		}
	} while (x != len);
	fbkwd(p, len);
	return p;
}

/* backward find (case insensitive) pattern 's' in text pointed by 'p' (Boyer-Moore algorithm) */
static P *frifind(P *p, unsigned char *s, int len)
{
	long amnt = p->byte;
	int x;
	unsigned char table[256], c;
	struct charmap *map = p->b->o.charmap;

	if (len > p->b->eof->byte - p->byte) {
		x = len - (p->b->eof->byte - p->byte);
		if (amnt < x)
			return NULL;
		amnt -= x;
		fbkwd(p, x);
	}
	if (!len)
		return p;
	p->valcol = 0;
	p->valattr = 0;
	mset(table, 255, 256);
	for (x = len; --x; table[s[x]] = len - x - 1) ;
	x = 0;
	do {
		if ((c = joe_tolower(map,fpgetc(p))) != s[x++]) {
			if (table[c] == 255) {
				fbkwd(p, len + 1);
				amnt -= len - x + 1;
			} else if (len - table[c] <= x) {
				fbkwd(p, x + 1);
				--amnt;
			} else {
				fbkwd(p, len - table[c]);
				amnt -= len - table[c] - x;
			}
			if (amnt < 0)
				return NULL;
			else
				x = 0;
		}
	} while (x != len);
	fbkwd(p, len);
	return p;
}

/* move cursor p to q's position and set p's col, line, ofst, byte etc. accordingly */
/* same as getto() but q is before p */
static P *rgetto(P *p, P *q)
{
	while (p->hdr != q->hdr || p->ofst != q->ofst) {
		if (!p->ofst)
			do {
				if (p->ofst) {
					p->byte -= p->ofst;
					p->line -= p->hdr->nlines;
				}
				pprev(p);
			} while (p->hdr != q->hdr);
		--p->ofst;
		--p->byte;
		if (GCHAR(p) == '\n')
			--p->line;
	}
	return p;
}

/* find backward substring s in text pointed by p and set p on the first of found substring */
P *prfind(P *p, unsigned char *s, int len)
{
	P *q = pdup(p, USTR "prfind");

	if (frfind(q, s, len)) {
		rgetto(p, q);
		prm(q);
		return p;
	} else {
		prm(q);
		return NULL;
	}
}

/* same as prfind() but case insensitive */
P *prifind(P *p, unsigned char *s, int len)
{
	P *q = pdup(p, USTR "prifind");

	if (frifind(q, s, len)) {
		rgetto(p, q);
		prm(q);
		return p;
	} else {
		prm(q);
		return NULL;
	}
}

/* copy text between 'from' and 'to' into new buffer */
B *bcpy(P *from, P *to)
{
	H anchor, *l;
	unsigned char *ptr;
	P *q;

	if (from->byte >= to->byte)
		return bmk(from->b);

	q = pdup(from, USTR "bcpy");
	izque(H, link, &anchor);

	if (q->hdr == to->hdr) {
		l = halloc();
		ptr = vlock(vmem, l->seg);
		if (q->ofst != q->hdr->hole)
			gstgap(q->hdr, q->ptr, q->ofst);
		l->nlines = mcnt(q->ptr + q->hdr->ehole, '\n', l->hole = to->ofst - q->ofst);
		mmove(ptr, q->ptr + q->hdr->ehole, l->hole);
		vchanged(ptr);
		vunlock(ptr);
		enqueb(H, link, &anchor, l);
	} else {
		l = halloc();
		ptr = vlock(vmem, l->seg);
		if (q->ofst != q->hdr->hole)
			gstgap(q->hdr, q->ptr, q->ofst);
		l->nlines = mcnt(q->ptr + q->hdr->ehole, '\n', l->hole = SEGSIZ - q->hdr->ehole);
		mmove(ptr, q->ptr + q->hdr->ehole, l->hole);
		vchanged(ptr);
		vunlock(ptr);
		enqueb(H, link, &anchor, l);
		pnext(q);
		while (q->hdr != to->hdr) {
			l = halloc();
			ptr = vlock(vmem, l->seg);
			l->nlines = q->hdr->nlines;
			mmove(ptr, q->ptr, q->hdr->hole);
			mmove(ptr + q->hdr->hole, q->ptr + q->hdr->ehole, SEGSIZ - q->hdr->ehole);
			l->hole = GSIZE(q->hdr);
			vchanged(ptr);
			vunlock(ptr);
			enqueb(H, link, &anchor, l);
			pnext(q);
		}
		if (to->ofst) {
			l = halloc();
			ptr = vlock(vmem, l->seg);
			if (to->ofst != to->hdr->hole)
				gstgap(to->hdr, to->ptr, to->ofst);
			l->nlines = mcnt(to->ptr, '\n', to->ofst);
			mmove(ptr, to->ptr, l->hole = to->ofst);
			vchanged(ptr);
			vunlock(ptr);
			enqueb(H, link, &anchor, l);
		}
	}

	l = anchor.link.next;
	deque(H, link, &anchor);
	prm(q);

	return bmkchn(l, from->b, to->byte - from->byte, to->line - from->line);
}

/* Coalesce small blocks into a single larger one */
void pcoalesce(P *p)
{
	if (p->hdr != p->b->eof->hdr && GSIZE(p->hdr) + GSIZE(p->hdr->link.next) <= SEGSIZ - SEGSIZ / 4) {
		H *hdr = p->hdr->link.next;
		unsigned char *ptr = vlock(vmem, hdr->seg);
		int osize = GSIZE(p->hdr);
		int size = GSIZE(hdr);
		P *q;

		gstgap(hdr, ptr, size);
		ginsm(p->hdr, p->ptr, GSIZE(p->hdr), ptr, size);
		p->hdr->nlines += hdr->nlines;
		vunlock(ptr);
		hfree(deque_f(H, link, hdr));
		for (q = p->link.next; q != p; q = q->link.next)
			if (q->hdr == hdr) {
				q->hdr = p->hdr;
				if (q->ptr) {
					vunlock(q->ptr);
					q->ptr = vlock(vmem, q->hdr->seg);
				}
				q->ofst += osize;
			}
	}
	if (p->hdr != p->b->bof->hdr && GSIZE(p->hdr) + GSIZE(p->hdr->link.prev) <= SEGSIZ - SEGSIZ / 4) {
		H *hdr = p->hdr->link.prev;
		unsigned char *ptr = vlock(vmem, hdr->seg);
		int size = GSIZE(hdr);
		P *q;

		gstgap(hdr, ptr, size);
		ginsm(p->hdr, p->ptr, 0, ptr, size);
		p->hdr->nlines += hdr->nlines;
		vunlock(ptr);
		hfree(deque_f(H, link, hdr));
		p->ofst += size;
		for (q = p->link.next; q != p; q = q->link.next)
			if (q->hdr == hdr) {
				q->hdr = p->hdr;
				if (q->ptr)
					vunlock(q->ptr);
				q->ptr = vlock(vmem, q->hdr->seg);
			} else if (q->hdr == p->hdr)
				q->ofst += size;
	}
}

/* Delete the text between two pointers from a buffer and return it in a new
 * buffer.
 *
 * This routine calls these functions:
 *  gstgap	- to position gaps
 *  halloc	- to allocate new header/segment pairs
 *  vlock	- virtual memory routines
 *  vunlock
 *  vchanged
 *  vupcount
 *  mcnt	- to count NLs
 *  snip	- queue routines
 *  enqueb
 *  splicef
 *  scrdel	- to tell screen update to scroll when NLs are deleted
 *  bmkchn	- to make a buffer out of a chain
 */

/* This is only to be used for bdel() */
static B *bcut(P *from, P *to)
{
	H *h,			/* The deleted text */
	*i;
	unsigned char *ptr;
	P *p;
	long nlines;		/* No. EOLs to delete */
	long amnt;		/* No. bytes to delete */
	int toamnt;		/* Amount to delete from segment in 'to' */
	int bofmove = 0;	/* Set if bof got deleted */
	struct lattr_db *db;

	if (!(amnt = to->byte - from->byte))
		return NULL;	/* ...nothing to delete */

	nlines = to->line - from->line;

	if (from->hdr == to->hdr) {	/* Delete is within a single segment */
		/* Move gap to deletion point */
		if (from->ofst != from->hdr->hole)
			gstgap(from->hdr, from->ptr, from->ofst);

		/* Store the deleted text */
		h = halloc();
		ptr = vlock(vmem, h->seg);
		mmove(ptr, from->ptr + from->hdr->ehole, (int) amnt);
		h->hole = amnt;
		h->nlines = nlines;
		vchanged(ptr);
		vunlock(ptr);

		/* Delete */
		from->hdr->ehole += amnt;
		from->hdr->nlines -= nlines;

		toamnt = amnt;
	} else {		/* Delete crosses segments */
		H *a;

		if ((toamnt = to->ofst) != 0) {
			/* Delete beginning of to */
			/* Move gap to deletion point */
			/* To could be deleted if it's at the end of the file */
			if (to->ofst != to->hdr->hole)
				gstgap(to->hdr, to->ptr, to->ofst);

			/* Save deleted text */
			i = halloc();
			ptr = vlock(vmem, i->seg);
			mmove(ptr, to->ptr, to->hdr->hole);
			i->hole = to->hdr->hole;
			i->nlines = mcnt(to->ptr, '\n', to->hdr->hole);
			vchanged(ptr);
			vunlock(ptr);

			/* Delete */
			to->hdr->nlines -= i->nlines;
			to->hdr->hole = 0;
		} else
			i = 0;

		/* Delete end of from */
		if (!from->ofst) {
			/* ... unless from needs to be deleted too */
			a = from->hdr->link.prev;
			h = NULL;
			if (a == from->b->eof->hdr)
				bofmove = 1;
		} else {
			a = from->hdr;
			/* Move gap to deletion point */
			if (from->ofst != from->hdr->hole)
				gstgap(from->hdr, from->ptr, from->ofst);

			/* Save deleted text */
			h = halloc();
			ptr = vlock(vmem, h->seg);
			mmove(ptr, from->ptr + from->hdr->ehole, SEGSIZ - from->hdr->ehole);
			h->hole = SEGSIZ - from->hdr->ehole;
			h->nlines = mcnt(ptr, '\n', h->hole);
			vchanged(ptr);
			vunlock(ptr);

			/* Delete */
			from->hdr->nlines -= h->nlines;
			from->hdr->ehole = SEGSIZ;
		}

		/* Make from point to header/segment of to */
		from->hdr = to->hdr;
		vunlock(from->ptr);
		from->ptr = to->ptr;
		vupcount(to->ptr);
		from->ofst = 0;

		/* Delete headers/segments between a and to->hdr (if there are any) */
		if (a->link.next != to->hdr)
			if (!h) {
				h = snip(H, link, a->link.next, to->hdr->link.prev);
				if (i)
					enqueb(H, link, h, i);
			} else {
				splicef(H, link, h, snip(H, link, a->link.next, to->hdr->link.prev));
				if (i)
					enqueb(H, link, h, i);
		} else if (!h)
			h = i;
		else if (i)
			enqueb(H, link, h, i);
	}

	/* If to is empty, then it must have been at the end of the file.  If
	   the file did not become empty, delete to */
	if (!GSIZE(to->hdr) && from->byte) {
		H *ph = from->hdr->link.prev;

		hfree(deque_f(H, link, from->hdr));
		vunlock(from->ptr);
		from->hdr = ph;
		from->ptr = vlock(vmem, from->hdr->seg);
		from->ofst = GSIZE(ph);
		vunlock(from->b->eof->ptr);
		from->b->eof->ptr = from->ptr;
		vupcount(from->ptr);
		from->b->eof->hdr = from->hdr;
		from->b->eof->ofst = from->ofst;
	}

	/* The deletion is now done */

	/* Scroll if necessary */

	if (bofmove)
		pset(from->b->bof, from);
	for (db = from->b->db; db; db = db->next)
		lattr_del(db, from->line, nlines);
	if (!pisbol(from)) {
		scrdel(from->b, from->line, nlines, 1);
		delerr(from->b->name, from->line, nlines);
	} else {
		scrdel(from->b, from->line, nlines, 0);
		delerr(from->b->name, from->line, nlines);
	}

	/* Fix pointers */

	for (p = from->link.next; p != from; p = p->link.next)
		if (p->line == from->line && p->byte > from->byte) {
			p->valcol = 0;
			p->valattr = 0;
		}
	for (p = from->link.next; p != from; p = p->link.next) {
		if (p->byte >= from->byte) {
			if (p->byte <= from->byte + amnt) {
				if (p->ptr) {
					pset(p, from);
				} else {
					poffline(pset(p, from));
				}
			} else {
				if (p->hdr == to->hdr)
					p->ofst -= toamnt;
				p->byte -= amnt;
				p->line -= nlines;
			}
		}
	}

	pcoalesce(from);

	/* Make buffer out of deleted text and return it */
	return bmkchn(h, from->b, amnt, nlines);
}

void bdel(P *from, P *to)
{
	if (to->byte - from->byte) {
		B *b = bcut(from, to);

		if (from->b->undo)
			undodel(from->b->undo, from->byte, b);
		else
			brm(b);
		from->b->changed = 1;
	}
}

/* Split a block at p's ofst */
/* p is placed in the new block such that it points to the same text but with
 * p->ofst==0
 */
static void bsplit(P *p)
{
	if (p->ofst) {
		H *hdr;
		unsigned char *ptr;
		P *pp;

		hdr = halloc();
		ptr = vlock(vmem, hdr->seg);

		if (p->ofst != p->hdr->hole)
			gstgap(p->hdr, p->ptr, p->ofst);
		mmove(ptr, p->ptr + p->hdr->ehole, SEGSIZ - p->hdr->ehole);
		hdr->hole = SEGSIZ - p->hdr->ehole;
		hdr->nlines = mcnt(ptr, '\n', hdr->hole);
		p->hdr->nlines -= hdr->nlines;
		vchanged(ptr);
		p->hdr->ehole = SEGSIZ;

		enquef(H, link, p->hdr, hdr);

		vunlock(p->ptr);

		for (pp = p->link.next; pp != p; pp = pp->link.next)
			if (pp->hdr == p->hdr && pp->ofst >= p->ofst) {
				pp->hdr = hdr;
				if (pp->ptr) {
					vunlock(pp->ptr);
					pp->ptr = ptr;
					vupcount(ptr);
				}
				pp->ofst -= p->ofst;
			}

		p->ptr = ptr;
		p->hdr = hdr;
		p->ofst = 0;
	}
}

/* Make a chain out of a block of memory (the block must not be empty) */
static H *bldchn(unsigned char *blk, int size, long *nlines)
{
	H anchor, *l;

	*nlines = 0;
	izque(H, link, &anchor);
	do {
		unsigned char *ptr;
		int amnt;

		ptr = vlock(vmem, (l = halloc())->seg);
		if (size > SEGSIZ)
			amnt = SEGSIZ;
		else
			amnt = size;
		mmove(ptr, blk, amnt);
		l->hole = amnt;
		l->ehole = SEGSIZ;
		(*nlines) += (l->nlines = mcnt(ptr, '\n', amnt));
		vchanged(ptr);
		vunlock(ptr);
		enqueb(H, link, &anchor, l);
		blk += amnt;
		size -= amnt;
	} while (size);
	l = anchor.link.next;
	deque(H, link, &anchor);
	return l;
}

/* Insert a chain into a buffer (this does not update pointers) */
static void inschn(P *p, H *a)
{
	if (!p->b->eof->byte) {	/* P's buffer is empty: replace the empty segment in p with a */
		hfree(p->hdr);
		p->hdr = a;
		vunlock(p->ptr);
		p->ptr = vlock(vmem, a->seg);
		pset(p->b->bof, p);

		p->b->eof->hdr = a->link.prev;
		vunlock(p->b->eof->ptr);
		p->b->eof->ptr = vlock(vmem, p->b->eof->hdr->seg);
		p->b->eof->ofst = GSIZE(p->b->eof->hdr);
	} else if (piseof(p)) {	/* We're at the end of the file: append a to the file */
		p->b->eof->hdr = a->link.prev;
		spliceb(H, link, p->b->bof->hdr, a);
		vunlock(p->b->eof->ptr);
		p->b->eof->ptr = vlock(vmem, p->b->eof->hdr->seg);
		p->b->eof->ofst = GSIZE(p->b->eof->hdr);
		p->hdr = a;
		vunlock(p->ptr);
		p->ptr = vlock(vmem, p->hdr->seg);
		p->ofst = 0;
	} else if (pisbof(p)) {	/* We're at the beginning of the file: insert chain and set bof pointer */
		p->hdr = spliceb_f(H, link, p->hdr, a);
		vunlock(p->ptr);
		p->ptr = vlock(vmem, a->seg);
		pset(p->b->bof, p);
	} else {		/* We're in the middle of the file: split and insert */
		bsplit(p);
		p->hdr = spliceb_f(H, link, p->hdr, a);
		vunlock(p->ptr);
		p->ptr = vlock(vmem, a->seg);
	}
}

static void fixupins(P *p, long amnt, long nlines, H *hdr, int hdramnt)
{
	P *pp;
	struct lattr_db *db;

	if (!pisbol(p))
		scrins(p->b, p->line, nlines, 1);
	else
		scrins(p->b, p->line, nlines, 0);

	for (db = p->b->db; db; db = db->next)
		lattr_ins(db, p->line, nlines);

	inserr(p->b->name, p->line, nlines, pisbol(p));	/* FIXME: last arg ??? */

	for (pp = p->link.next; pp != p; pp = pp->link.next)
		if (pp->line == p->line && (pp->byte > p->byte || (pp->end && pp->byte == p->byte))) {
			pp->valcol = 0;
			pp->valattr = 0;
		}
	for (pp = p->link.next; pp != p; pp = pp->link.next)
		if (pp->byte == p->byte && !pp->end)
			if (pp->ptr)
				pset(pp, p);
			else
				poffline(pset(pp, p));
		else if (pp->byte > p->byte || (pp->end && pp->byte == p->byte)) {
			pp->byte += amnt;
			pp->line += nlines;
			if (pp->hdr == hdr)
				pp->ofst += hdramnt;
		}
	if (p->b->undo)
		undoins(p->b->undo, p, amnt);
	p->b->changed = 1;

}

/* Insert a buffer at pointer position (the buffer goes away) */
P *binsb(P *p, B *b)
{
	if (b->eof->byte) {
		P *q = pdup(p, USTR "binsb");

		inschn(q, b->bof->hdr);
		b->eof->hdr = halloc();
		fixupins(q, b->eof->byte, b->eof->line, NULL, 0);
		pcoalesce(q);
		prm(q);
	}
	brm(b);
	return p;
}

/* insert memory block 'blk' at 'p' */
P *binsm(P *p, unsigned char *blk, int amnt)
{
	long nlines;
	H *h = NULL;
	int hdramnt = 0; /* Only used if h is set */
	P *q;

	if (!amnt)
		return p;
	q = pdup(p, USTR "binsm");
	if (amnt <= GGAPSZ(q->hdr)) {
		h = q->hdr;
		hdramnt = amnt;
		ginsm(q->hdr, q->ptr, q->ofst, blk, amnt);
		q->hdr->nlines += (nlines = mcnt(blk, '\n', amnt));
	} else if (!q->ofst && q->hdr != q->b->bof->hdr && amnt <= GGAPSZ(q->hdr->link.prev)) {
		pprev(q);
		ginsm(q->hdr, q->ptr, q->ofst, blk, amnt);
		q->hdr->nlines += (nlines = mcnt(blk, '\n', amnt));
	} else {
		H *a = bldchn(blk, amnt, &nlines);

		inschn(q, a);
	}
	fixupins(q, (long) amnt, nlines, h, hdramnt);
	pcoalesce(q);
	prm(q);
	return p;
}

/* insert byte 'c' at 'p' */
P *binsbyte(P *p, unsigned char c)
{
	if (p->b->o.crlf && c == '\n')
		return binsm(p, USTR "\r\n", 2);
	else
		return binsm(p, &c, 1);
}

/* UTF-8 encode a character and insert it */
P *binsc(P *p, int c)
{
	if ((c & ANSI_BIT) && p->b->o.ansi) {
		unsigned char *s = ansi_string(c);
		return binsm(p, s, zlen(s));
	} else if (c>127 && p->b->o.charmap->type) {
		unsigned char buf[8];
		int len = utf8_encode(buf,c);
		return binsm(p,buf,len);
	} else {
		unsigned char ch = c;
		if (p->b->o.crlf && c == '\n')
			return binsm(p, USTR "\r\n", 2);
		else
			return binsm(p, &ch, 1);
	}
}

/* insert zero-terminated string 's' at 'p' */
P *binss(P *p, unsigned char *s)
{
	return binsm(p, s, zlen(s));
}

/* Read 'size' bytes from file or stream.  Stops and returns amnt. read
 * when requested size has been read or when end of file condition occurs.
 * Returns with -2 in error for read error or 0 in error for success.
 */
static int bkread(int fi, unsigned char *buff, int size)
{
	int a, b;

	if (!size) {
		berror = 0;
		return 0;
	}
	for (a = b = 0; (a < size) && ((b = joe_read(fi, buff + a, size - a)) > 0); a += b) ;
	if (b < 0)
		berror = -2;
	else
		berror = 0;
	return a;
}

/* Read up to 'max' bytes from a file into a buffer */
/* Returns with 0 in error or -2 in error for read error */
B *bread(int fi, long int max)
{
	H anchor, *l;
	long lines = 0, total = 0;
	int amnt;
	unsigned char *seg;

	izque(H, link, &anchor);
	berror = 0;
	while (seg = vlock(vmem, (l = halloc())->seg), !berror && (amnt = bkread(fi, seg, max >= SEGSIZ ? SEGSIZ : (int) max))) {
		total += amnt;
		max -= amnt;
		l->hole = amnt;
		lines += (l->nlines = mcnt(seg, '\n', amnt));
		vchanged(seg);
		vunlock(seg);
		enqueb(H, link, &anchor, l);
	}
	hfree(l);
	vunlock(seg);
	if (!total)
		return bmk(NULL);
	l = anchor.link.next;
	deque(H, link, &anchor);
	return bmkchn(l, NULL, total, lines);
}

/* Parse file name.
 *
 * Removes ',xxx,yyy' from end of name and puts their value into skip and amnt
 * Replaces ~user/ with directory of given user
 * Replaces ~/ with $HOME
 *
 * Returns new variable length string.
 */
unsigned char *parsens(unsigned char *s, off_t *skip, off_t *amnt)
{
	unsigned char *n = vsncpy(NULL, 0, sz(s));
	int x;
#ifdef HAVE_LONG_LONG
	unsigned long long skipr;
#else
	unsigned long skipr;
#endif

	*skip = 0;
	*amnt = MAXLONG;
	x = sLEN(n) - 1;
	if (x > 0 && n[x] >= '0' && n[x] <= '9') {
		for (x = sLEN(n) - 1; x > 0 && ((n[x] >= '0' && n[x] <= '9') || n[x] == 'x' || n[x] == 'X'); --x) ;
		if (n[x] == ',' && x && n[x-1] != '\\') {
			n[x] = 0;

#if HAVE_LONG_LONG
			if (n[x + 1] == 'x' || n[x + 1] == 'X')
				sscanf((char *)(n + x + 2), "%llx", &skipr);
			else if (n[x + 1] == '0' && (n[x + 2] == 'x' || n[x + 2] == 'X'))
				sscanf((char *)(n + x + 3), "%llx", &skipr);
			else if (n[x + 1] == '0')
				sscanf((char *)(n + x + 1), "%llo", &skipr);
			else
				sscanf((char *)(n + x + 1), "%llu", &skipr);
#else
			if (n[x + 1] == 'x' || n[x + 1] == 'X')
				sscanf((char *)(n + x + 2), "%lx", &skipr);
			else if (n[x + 1] == '0' && (n[x + 2] == 'x' || n[x + 2] == 'X'))
				sscanf((char *)(n + x + 3), "%lx", &skipr);
			else if (n[x + 1] == '0')
				sscanf((char *)(n + x + 1), "%lo", &skipr);
			else
				sscanf((char *)(n + x + 1), "%lu", &skipr);
#endif
			*skip = (signed)skipr;
			--x;
			if (x > 0 && n[x] >= '0' && n[x] <= '9') {
				for (; x > 0 && ((n[x] >= '0' && n[x] <= '9') || n[x] == 'x' || n[x] == 'X'); --x) ;
				if (n[x] == ',' && x && n[x-1]!='\\') {
					n[x] = 0;
					*amnt = *skip;
#ifdef HAVE_LONG_LONG
					if (n[x + 1] == 'x' || n[x + 1] == 'X')
						sscanf((char *)(n + x + 2), "%llx", &skipr);
					else if (n[x + 1] == '0' && (n[x + 2] == 'x' || n[x + 2] == 'X'))
						sscanf((char *)(n + x + 3), "%llx", &skipr);
					else if (n[x + 1] == '0')
						sscanf((char *)(n + x + 1), "%llo", &skipr);
					else
						sscanf((char *)(n + x + 1), "%llu", &skipr);
#else
					if (n[x + 1] == 'x' || n[x + 1] == 'X')
						sscanf((char *)(n + x + 2), "%lx", &skipr);
					else if (n[x + 1] == '0' && (n[x + 2] == 'x' || n[x + 2] == 'X'))
						sscanf((char *)(n + x + 3), "%lx", &skipr);
					else if (n[x + 1] == '0')
						sscanf((char *)(n + x + 1), "%lo", &skipr);
					else
						sscanf((char *)(n + x + 1), "%lu", &skipr);
#endif
					*skip = (signed)skipr;
				}
			}
		}
	}
	/* Don't do this here: do it in prompt buffer instead, so we're just like
	   the shell doing it on the command line. */
	/* n = canonical(n); */
	return n;
}

/* Canonicalize file name: do ~ expansion */

unsigned char *canonical(unsigned char *n)
{
	int y = 0;
#ifndef __MSDOS__
	int x;
	unsigned char *s;
	for (y = zlen(n); ; --y)
		if (y <= 2) {
			y = 0;
			break;
		} else if (n[y-2] == '/' && (n[y-1] == '/' || n[y-1] == '~')) {
			y -= 1;
			break;
		}
	
	if (n[y] == '~') {
		for (x = y + 1; n[x] && n[x] != '/'; ++x) ;
		if (n[x] == '/') {
			if (x == y + 1) {
				unsigned char *z;

				s = (unsigned char *)getenv("HOME");
				z = vsncpy(NULL, 0, sz(s));
				z = vsncpy(z, sLEN(z), sz(n + x));
				vsrm(n);
				n = z;
				y = 0;
			} else {
				struct passwd *passwd;

				n[x] = 0;
				passwd = getpwnam((char *)(n + y + 1));
				n[x] = '/';
				if (passwd) {
					unsigned char *z = vsncpy(NULL, 0,
							 sz((unsigned char *)(passwd->pw_dir)));

					z = vsncpy(z, sLEN(z), sz(n + x));
					vsrm(n);
					n = z;
					y = 0;
				}
			}
		}
	}
#endif
	if (y) {
		unsigned char *z = vsncpy(NULL, 0, n + y, zlen(n + y));
		vsrm(n);
		return z;
	} else
		return n;
}

int euclid(int a, int b)
{
	if (!a)
		return b;
	while (b)
		if (a > b)
			a -= b;
		else
			b -= a;
	return a;
}

/* return column of first nonblank character, but don't count comments */
int found_space;
int found_tab;

long pisindentg(P *p)
{
	int i_spc = 0;
	int i_tab = 0;
	P *q = pdup(p, USTR "pisindentg");
	long col;
	int ch;

	p_goto_bol(q);
	while (joe_isblank(p->b->o.charmap,ch = brc(q))) {
		if (ch == ' ')
			i_spc = 1;
		else if (ch == '\t')
			i_tab = 1;
		pgetc(q);
	}
	col = q->col;
	if (ch == '*' || ch == '/' || ch == '-' || ch =='%' || ch == '#' || ch == '\r' || ch == '\n')
		col = 0;
	if (col) {
		found_space |= i_spc;
		found_tab |= i_tab;
	}
	prm(q);
	return col;
}

unsigned char *dequote(unsigned char *s)
{
        static unsigned char buf[1024];
        unsigned char *p = buf;
        while (*s) {
                if (*s =='\\')
                        ++s;
                if (*s)
                        *p++ = *s++;
        }
        *p = 0;
        return buf;
}

/* Load file into new buffer and return the new buffer */
/* Returns with error set to 0 for success,
 * -1 for new file (file doesn't exist)
 * -2 for read error
 * -3 for seek error
 * -4 for open error
 */
B *bload(unsigned char *s)
{
	unsigned char buffer[SEGSIZ];
	FILE *fi = 0;
	B *b = 0;
	off_t skip, amnt;
	unsigned char *n;
	int nowrite = 0;
	P *p;
	int x;
	long mod_time = 0;
	struct stat sbuf;

	if (!s || !s[0]) {
		berror = -1;
		b = bmk(NULL);
		setopt(b,USTR "");
		b->rdonly = b->o.readonly;
		b->er = berror;
		return b;
	}

	n = parsens(s, &skip, &amnt);

	/* Open file or stream */
#ifndef __MSDOS__
	if (n[0] == '!') {
		nescape(maint->t);
		ttclsn();
		fi = popen((char *)dequote(n + 1), "r");
	} else
#endif
	if (!zcmp(n, USTR "-")) {
#ifdef junk
		FILE *f;
		struct stat y;
		fi = stdin;
		/* Make sure stdin is not tty */
		if (fstat(fileno(fi), &y)) 
			goto no_stat;
		if (y.st_mode & S_IFCHR) {
			no_stat:
			b = bmk(NULL);
			goto empty;
		}
#endif
		/* Now we always just create an empty buffer for "-" */
		b = bmk(NULL);
		goto empty;
	} else {
		fi = fopen((char *)dequote(n), "r+");
		if (!fi)
			nowrite = 1;
		else
			fclose(fi);
		fi = fopen((char *)dequote(n), "r");
		if (!fi)
			nowrite = 0;
		if (fi) {
			fstat(fileno(fi),&sbuf);
			mod_time = sbuf.st_mtime;
		}
	}
	joesep(n);

	/* Abort if couldn't open */
	if (!fi) {
		if (errno == ENOENT)
			berror = -1;
		else
			berror = -4;
		b = bmk(NULL);
		setopt(b,n);
		b->rdonly = b->o.readonly;
		goto opnerr;
	}

	/* Skip data if we need to */
	if (skip && lseek(fileno(fi), skip, 0) < 0) {
		int r;

		while (skip > SEGSIZ) {
			r = bkread(fileno(fi), buffer, SEGSIZ);
			if (r != SEGSIZ || berror) {
				berror = -3;
				goto err;
			}
			skip -= SEGSIZ;
		}
		skip -= bkread(fileno(fi), buffer, (int) skip);
		if (skip || berror) {
			berror = -3;
			goto err;
		}
	}

	/* Read from stream into new buffer */
	b = bread(fileno(fi), amnt);
	empty:
	b->mod_time = mod_time;
	setopt(b,n);
	b->rdonly = b->o.readonly;

	/* Close stream */
err:
#ifndef __MSDOS__
	if (s[0] == '!')
		pclose(fi);
	else
#endif
	if (zcmp(n, USTR "-"))
		fclose(fi);

opnerr:
	if (s[0] == '!') {
		ttopnn();
		nreturn(maint->t);
	}

	/* Set name */
	b->name = joesep(zdup(s));

	/* Set flags */
	if (berror || s[0] == '!' || skip || amnt != MAXLONG) {
		b->backup = 1;
		b->changed = 0;
	} else if (!zcmp(n, USTR "-")) {
		b->backup = 1;
		b->changed = 1;
	} else {
		b->backup = 0;
		b->changed = 0;
	}
	if (nowrite)
		b->rdonly = b->o.readonly = 1;

	/* If first line has CR-LF, assume MS-DOS file */
	if (guesscrlf) {
		p=pdup(b->bof, USTR "bload");
		b->o.crlf = 0;
		for(x=0;x!=1024;++x) {
			int c = pgetc(p);
			if(c == '\r') {
				b->o.crlf = 1;
				break;
				}
			if(c == '\n') {
				b->o.crlf = 0;
				break;
				}
			if(c == NO_MORE_DATA)
				break;
		}
		prm(p);
	}

	/* Search backwards through file: if first indented line
	   is indented with a tab, assume indentc is tab */
	if (guessindent) {
		int i, x, y;
		int guessed_step = 0;
		int hist[20];
		int hist_val[20];
		int nhist = 0;
		int max;
		int maxi;
		int space_lines = 0;
		int tab_lines = 0;
		
		p=pdup(b->eof, USTR "bload");
		/* Create histogram of indentation values */
		for (y = 0; y != 50; ++y) {
			found_space = 0;
			found_tab = 0;
			
			p_goto_bol(p);
			if ((i = pisindentg(p))) {
				for (x = 0; x != nhist; ++x)
					if (hist_val[x] == i)
						break;
				if (x == nhist && nhist != 20) {
					hist[nhist] = 1;
					hist_val[nhist] = i;
					++nhist;
				} else if (x != nhist) {
					++hist[x];
				}
				
				/* Count characters used for indent */
				if (found_tab) tab_lines++;
				else if (found_space) space_lines++;
			}
			
			if (prgetc(p) == NO_MORE_DATA)
				break;
		}
		
		if (tab_lines > space_lines) {
			/* If more lines were indented with tabs than spaces, pick tabs. */
			b->o.indentc = '\t';
			b->o.istep = 1;
		} else if (space_lines > 0) {
			/* Find GCM of top 3 most popular indentation values */
			for (y = 0; y != 3; ++y) {
				max = 0;
				for (x = 0; x != nhist; ++x)
					if (hist[x] > max) {
						max = hist[x];
						maxi = x;
					}
				if (max) {
					if (guessed_step)
						guessed_step = euclid(guessed_step, hist_val[maxi]);
					else
						guessed_step = hist_val[maxi];
					hist[maxi] = 0;
				}
			}
			
			/* If guessed value is large, scale it down some */
			while (!(guessed_step & 1) && guessed_step > 8)
				guessed_step >>= 1;
			
			b->o.indentc = ' ';
			if (guessed_step)
				b->o.istep = guessed_step;
		}

		prm(p);
	}

	/* Eliminate parsed name */
	vsrm(n);

	b->er = berror;
	return b;
}

/* Find already loaded buffer or load file into new buffer */
B *bfind(unsigned char *s)
{
	B *b;

	if (!s || !s[0]) {
		berror = -1;
		b = bmk(NULL);
		setopt(b,USTR "");
		b->rdonly = b->o.readonly;
		b->internal = 0;
		b->er = berror;
		return b;
	}
	for (b = bufs.link.next; b != &bufs; b = b->link.next)
		if (b->name && !zcmp(s, b->name)) {
			if (!b->orphan)
				++b->count; /* Assumes caller is going to put this in a window! */
			else
				b->orphan = 0;
			berror = 0;
			b->internal = 0;
			return b;
		}
	b = bload(s); /* Returns count==1 */
	b->internal = 0;
	return b;
}

/* Find already loaded buffer or load file into new buffer */
B *bfind_scratch(unsigned char *s)
{
	B *b;

	if (!s || !s[0]) {
		berror = -1;
		b = bmk(NULL);
		setopt(b,USTR "");
		b->rdonly = b->o.readonly;
		b->internal = 0;
		b->er = berror;
		return b;
	}
	for (b = bufs.link.next; b != &bufs; b = b->link.next)
		if (b->name && !zcmp(s, b->name)) {
			if (!b->orphan)
				++b->count;
			else
				b->orphan = 0;
			berror = 0;
			b->internal = 0;
			return b;
		}
	b = bmk(NULL);
	berror = -1;
	setopt(b,s);
	b->internal = 0;
	b->rdonly = b->o.readonly;
	b->er = berror;
	b->name = zdup(s);
	b->scratch = 1;
	return b;
}

B *bfind_reload(unsigned char *s)
{
	B *b;
	b = bload(s);
	b->internal = 0;
	return b;
}

B *bcheck_loaded(unsigned char *s)
{
	B *b;

	if (!s || !s[0]) {
		return NULL;
	}
	for (b = bufs.link.next; b != &bufs; b = b->link.next)
		if (b->name && !zcmp(s, b->name)) {
			return b;
		}

	return NULL;
}

unsigned char **getbufs(void)
{
	unsigned char **s = vamk(16);
	B *b;

	for (b = bufs.link.next; b != &bufs; b = b->link.next)
		if (b->name)
			s = vaadd(s, vsncpy(NULL, 0, sz(b->name)));
	return s;
}

/* Find an orphaned buffer: b->count of returned buffer should be 1. */
B *borphan(void)
{
	B *b;

	for (b = bufs.link.next; b != &bufs; b = b->link.next)
		if (b->orphan && (!b->scratch || b->pid)) {
			b->orphan = 0;
			return b;
		}
	return NULL;
}

/* Write 'size' bytes from file beginning at 'p' to open file 'fd'.
 * Returns error.
 * error is set to -5 for write error or 0 for success.
 * Don't attempt to write past the end of the file
 */
int bsavefd(P *p, int fd, off_t size)
{
	P *np = pdup(p, USTR "bsavefd");
	int amnt;

	while (size > (amnt = GSIZE(np->hdr) - np->ofst)) {
		if (np->ofst < np->hdr->hole) {
			if (joe_write(fd, np->ptr + np->ofst, np->hdr->hole - np->ofst) < 0)
				goto err;
			if (joe_write(fd, np->ptr + np->hdr->ehole, SEGSIZ - np->hdr->ehole) < 0)
				goto err;
		} else if (joe_write(fd, np->ptr + np->ofst + GGAPSZ(np->hdr), amnt) < 0)
			goto err;
		size -= amnt;
		pnext(np);
	}
	if (size) {
		if (np->ofst < np->hdr->hole) {
			if (size > np->hdr->hole - np->ofst) {
				if (joe_write(fd, np->ptr + np->ofst, np->hdr->hole - np->ofst) < 0)
					goto err;
				if (joe_write(fd, np->ptr + np->hdr->ehole, (int) size - np->hdr->hole + np->ofst) < 0)
					goto err;
			} else {
				if (joe_write(fd, np->ptr + np->ofst, (int) size) < 0)
					goto err;
			}
		} else {
			if (joe_write(fd, np->ptr + np->ofst + GGAPSZ(np->hdr), (int) size) < 0)
				goto err;
		}
	}
	prm(np);
	return berror = 0;
err:
	prm(np);
	return berror = -5;
}

/* Save 'size' bytes beginning at 'p' in file 's' */

/* If flag is set, update original time of file if it makes
 * sense to do so (it's a normal file, we're saving with
 * same name as buffer or is about to get this name).
 *
 * If flag is set to 2, we update original time even if file
 * name changed (i.e., we're renaming the file).
 */

int break_links; /* Set to break hard links on writes */
int break_symlinks; /* Set to break symbolic links and hard links on writes */

int bsave(P *p, unsigned char *s, off_t size, int flag)
{
	struct stat sbuf;
	int have_stat = 0;
	FILE *f;
	off_t skip, amnt;
	int norm = 0;

	s = parsens(s, &skip, &amnt);

	if (amnt < size)
		size = amnt;

#ifndef __MSDOS__
	if (s[0] == '!') {
		nescape(maint->t);
		ttclsn();
		f = popen((char *)dequote(s + 1), "w");
	} else
#endif
	if (s[0] == '>' && s[1] == '>')
		f = fopen((char *)dequote(s + 2), "a");
	else if (!zcmp(s, USTR "-")) {
		nescape(maint->t);
		ttclsn();
		f = stdout;
	} else if (skip || amnt != MAXLONG)
		f = fopen((char *)dequote(s), "r+");
	else {
		have_stat = !stat((char *)dequote(s), &sbuf);
		if (!have_stat)
			sbuf.st_mode = 0666;
		/* Normal file save */
		if (break_links || break_symlinks) {
			struct stat lsbuf;

			/* Try to copy permissions */
			if (!lstat((char *)dequote(s),&lsbuf)) {
				int g;
				if (!break_symlinks && S_ISLNK(lsbuf.st_mode)) {
					goto nobreak;
				}

#ifdef WITH_SELINUX
				security_context_t se;
				if (selinux_enabled == -1)
					selinux_enabled = (is_selinux_enabled() > 0);
				
				if (selinux_enabled) {
					if (getfilecon((char *)dequote(s), &se) < 0) {
						berror = -4;
						goto opnerr;
					}
				}
#endif
				unlink((char *)dequote(s));
				g = creat((char *)dequote(s), sbuf.st_mode & ~(S_ISUID | S_ISGID));
#ifdef WITH_SELINUX
				if (selinux_enabled) {
					setfilecon((char *)dequote(s), &se);
					freecon(se);
				}
#endif
				close(g);
				nobreak:;
			} else {
				unlink((char *)dequote(s));
			}
		}

		f = fopen((char *)dequote(s), "w");
		norm = 1;
	}
	joesep(s);

	if (!f) {
		berror = -4;
		goto opnerr;
	}
	fflush(f);

	if (skip && lseek(fileno(f), skip, 0) < 0) {
		berror = -3;
		goto err;
	}

	bsavefd(p, fileno(f), size);

	if (!berror && force && size && !skip && amnt == MAXLONG) {
		P *q = pdup(p, USTR "bsave");
		unsigned char nl = '\n';

		pfwrd(q, size - 1);
		if (brc(q) != '\n' && joe_write(fileno(f), &nl, 1) < 0)
			berror = -5;
		prm(q);
	}

	/* Restore setuid bit */
	if (!berror && have_stat) {
		fchmod(fileno(f), sbuf.st_mode);
	}

err:
#ifndef __MSDOS__
	if (s[0] == '!')
		pclose(f);
	else
#endif
	if (zcmp(s, USTR "-"))
		fclose(f);
	else
		fflush(f);

	/* Update orignal date of file */
	/* If it's not named, it's about to be */
	if (!berror && norm && flag && (!p->b->name || flag == 2 || !zcmp(s,p->b->name))) {
		if (!stat((char *)dequote(s),&sbuf))
			p->b->mod_time = sbuf.st_mtime;
	}

opnerr:
	if (s[0] == '!' || !zcmp(s,USTR "-")) {
		ttopnn();
		nreturn(maint->t);
	}
	return berror;
}

/* Return byte at p */

int brc(P *p)
{
	if (p->ofst == GSIZE(p->hdr))
		return NO_MORE_DATA;
	return GCHAR(p);
}

/* Return character at p */

int brch(P *p)
{
	if (p->b->o.charmap->type) {
		P *q = pdup(p, USTR "brch");
		int c = pgetc(q);
		prm(q);
		return c;
	} else {
		return brc(p);
	}
}

unsigned char *brmem(P *p, unsigned char *blk, int size)
{
	unsigned char *bk = blk;
	P *np;
	int amnt;

	np = pdup(p, USTR "brmem");
	while (size > (amnt = GSIZE(np->hdr) - np->ofst)) {
		grmem(np->hdr, np->ptr, np->ofst, bk, amnt);
		bk += amnt;
		size -= amnt;
		pnext(np);
	}
	if (size)
		grmem(np->hdr, np->ptr, np->ofst, bk, size);
	prm(np);
	return blk;
}

unsigned char *brs(P *p, int size)
{
	unsigned char *s = (unsigned char *) joe_malloc(size + 1);

	s[size] = 0;
	return brmem(p, s, size);
}

unsigned char *brvs(P *p, int size)
{
	unsigned char *s = vstrunc(NULL, size);

	return brmem(p, (unsigned char *)s, size);
}

unsigned char *brzs(P *p, unsigned char *buf, int size)
{
	P *q=pdup(p, USTR "brzs");
	p_goto_eol(q);

	if(q->byte-p->byte<size)
		size = q->byte - p->byte;

	prm(q);
	brmem(p,buf,size);
	buf[size]=0;
	return buf;
}

/* Save edit buffers when editor dies */

static int ttsig_handled = 0;

RETSIGTYPE ttsig(int sig)
{
        FILE *ttsig_f = 0;
	time_t tim = time(NULL);
	B *b;
	int tmpfd;
	struct stat sbuf;

	/* Do not allow double-fault */
	if (ttsig_handled)
		_exit(1);
        
        ttsig_handled = 1;
        
        if (nodeadjoe)
                goto skipfile;

	if ((tmpfd = open("DEADJOE", O_RDWR | O_EXCL | O_CREAT, 0600)) < 0) {
		if (lstat("DEADJOE", &sbuf) < 0)
			_exit(1);
		if (!S_ISREG(sbuf.st_mode) || sbuf.st_uid != geteuid())
			_exit(1);
		/*
		   A race condition still exists between the lstat() and the open()
		   systemcall, which leads to a possible denial-of-service attack
		   by setting the file access mode to 600 for every file the
		   user executing joe has permissions to.
		   This can't be fixed w/o breacking the behavior of the orig. joe!
		 */
		if ((tmpfd = open("DEADJOE", O_RDWR | O_APPEND)) < 0)
			_exit(1);
		if (fchmod(tmpfd, S_IRUSR | S_IWUSR) < 0)
			_exit(1);
	}
	if ((ttsig_f = fdopen(tmpfd, "a")) == NULL)
		_exit(1);

	fprintf(ttsig_f, "\n*** These modified files were found in JOE when it aborted on %s", ctime(&tim));
	if (sig == -2)
		fprintf(ttsig_f, "*** JOE was aborted due to swap file I/O error\n");
	else if (sig == -1)
		fprintf(ttsig_f, "*** JOE was aborted due to malloc returning NULL\n");
	else if (sig)
		fprintf(ttsig_f, "*** JOE was aborted by UNIX signal %d\n", sig);
	else
		fprintf(ttsig_f, "*** JOE was aborted because the terminal closed\n");
	fflush(ttsig_f);
	for (b = bufs.link.next; b != &bufs; b = b->link.next)
		if (b->changed) {
			if (b->name)
				fprintf(ttsig_f, (char *)joe_gettext(_("\n*** File \'%s\'\n")), b->name);
			else
				fputs((char *)joe_gettext(_("\n*** File \'(Unnamed)\'\n")), ttsig_f);
			fflush(ttsig_f);
			bsavefd(b->bof, fileno(ttsig_f), b->eof->byte);
		}

skipfile:
	if (sig)
		ttclsn();
	if (sig == -2) {
		fprintf(stderr,"\n*** JOE was aborted due to swap file I/O error\n");
	} else if (sig == -1) {
		fprintf(stderr,"\n*** JOE was aborted due to malloc returning NULL.");
		if (nodeadjoe) {
		        fprintf(stderr, "\n");
		} else {
		        fprintf(stderr, "  Buffers saved in DEADJOE\n");
		}
	} else if (sig) {
		fprintf(stderr,"\n*** JOE was aborted by UNIX signal %d.", sig);
		if (nodeadjoe) {
		        fprintf(stderr, "\n");
		} else {
		        fprintf(stderr, "  Buffers saved in DEADJOE\n");
		}
	}
	_exit(1);
}


/* Create lock for a file
   Return 0 for success or -1 for failure
*/

int lock_it(unsigned char *qpath,unsigned char *bf)
{
        unsigned char *path = dequote(qpath);
	unsigned char *lock_name=dirprt(path);
	unsigned char *name=namprt(path);
	unsigned char buf[1024];
	unsigned char *user = (unsigned char *)getenv("USER");
	unsigned char *host = (unsigned char *)getenv("HOSTNAME");
	int len;
	if (!user) user=USTR "me";
	if (!host) host=USTR "here";
	lock_name=vsncpy(sv(lock_name),sc(".#"));
	lock_name=vsncpy(sv(lock_name),sv(name));
	joe_snprintf_3(buf,sizeof(buf),"%s@%s.%d",user,host,getpid());
	/* Fail only if there was an existing lock */
	if (!symlink((char *)buf,(char *)lock_name) || errno != EEXIST) {
		vsrm(lock_name);
		vsrm(name);
		return 0;
	}
	if (bf) {
		len = readlink((char *)lock_name,(char *)bf,255);
		if (len<0) len = 0;
		bf[len] = 0;
	}
	vsrm(lock_name);
	vsrm(name);
	return -1;
}

void unlock_it(unsigned char *qpath)
{
        unsigned char *path = dequote(qpath);
	unsigned char *lock_name=dirprt(path);
	unsigned char *name=namprt(path);
	lock_name=vsncpy(sv(lock_name),sc(".#"));
	lock_name=vsncpy(sv(lock_name),sv(name));
	unlink((char *)lock_name);
	vsrm(lock_name);
	vsrm(name);
}

/* True if file is regular */

int plain_file(B *b)
{
	if (b->name && zcmp(b->name,USTR "-") && b->name[0]!='!' && b->name[0]!='>' &&
	    !b->scratch)
		return 1;
	else
		return 0;
}

/* True if file changed under us */

int check_mod(B *b)
{
	struct stat sbuf;
	if (!plain_file(b))
		return 0;
	if (!stat((char *)b->name,&sbuf)) {
		if (sbuf.st_mtime>b->mod_time) {
			return 1;
		}
	}
	return 0;
}

/* True if file exists */

int file_exists(unsigned char *path)
{
	struct stat sbuf;
	if (!path) return 0;
	return !stat((char *)path, &sbuf);
}
