/*
 *	Compiler error handler
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* Error database */

typedef struct error ERROR;

struct error {
	LINK(ERROR) link;	/* Linked list of errors */
	long line;		/* Target line number */
	long org;		/* Original target line number */
	unsigned char *file;	/* Target file name */
	long src;		/* Error-file line number */
	unsigned char *msg;	/* The message */
} errors = { { &errors, &errors} };
ERROR *errptr = &errors;	/* Current error row */

B *errbuf = NULL;		/* Buffer with error messages */

/* Function which allows stepping through all error buffers,
   for multi-file search and replace.  Give it a buffer.  It finds next
   buffer in error list.  Look at 'berror' for error information. */

/* This is made to work like bafter: it does not increment refcount of buffer */

B *beafter(B *b)
{
	struct error *e;
	unsigned char *name = b->name;
	if (!name) name = USTR "";
	for (e = errors.link.next; e != &errors; e = e->link.next)
		if (!zcmp(name, e->file))
			break;
	if (e == &errors) {
		/* Given buffer is not in list?  Return first buffer in list. */
		e = errors.link.next;
	}
	while (e != &errors && !zcmp(name, e->file))
		e = e->link.next;
	berror = 0;
	if (e != &errors) {
		B *b = bfind(e->file);
		/* bfind bumps refcount, so we have to unbump it */
		if (b->count == 1)
			b->orphan = 1; /* Oops */
		else
			--b->count;
		return b;
	}
	return 0;
}

/* Insert and delete notices */

void inserr(unsigned char *name, long int where, long int n, int bol)
{
	ERROR *e;

	if (!n)
		return;

	if (name) {
		for (e = errors.link.next; e != &errors; e = e->link.next) {
			if (!zcmp(e->file, name)) {
				if (e->line > where)
					e->line += n;
				else if (e->line == where && bol)
					e->line += n;
			}
		}
	}
}

void delerr(unsigned char *name, long int where, long int n)
{
	ERROR *e;

	if (!n)
		return;

	if (name) {
		for (e = errors.link.next; e != &errors; e = e->link.next) {
			if (!zcmp(e->file, name)) {
				if (e->line > where + n)
					e->line -= n;
				else if (e->line > where)
					e->line = where;
			}
		}
	}
}

/* Abort notice */

void abrerr(unsigned char *name)
{
	ERROR *e;

	if (name)
		for (e = errors.link.next; e != &errors; e = e->link.next)
			if (!zcmp(e->file, name))
				e->line = e->org;
}

/* Save notice */

void saverr(unsigned char *name)
{
	ERROR *e;

	if (name)
		for (e = errors.link.next; e != &errors; e = e->link.next)
			if (!zcmp(e->file, name))
				e->org = e->line;
}

/* Pool of free error nodes */
ERROR errnodes = { {&errnodes, &errnodes} };

/* Free an error node */

static void freeerr(ERROR *n)
{
	vsrm(n->file);
	vsrm(n->msg);
	enquef(ERROR, link, &errnodes, n);
}

/* Free all errors */

static int freeall(void)
{
	int count = 0;
	while (!qempty(ERROR, link, &errors)) {
		freeerr(deque_f(ERROR, link, errors.link.next));
		++count;
	}
	errptr = &errors;
	return count;
}

/* Parse error messages into database */

/* From joe's joe 2.9 */

/* First word (allowing ., /, _ and -) with a . is the file name.  Next number
   is line number.  Then there should be a ':' */

static void parseone(struct charmap *map,unsigned char *s,unsigned char **rtn_name,long *rtn_line)
{
	int x, y, flg;
	unsigned char *name = NULL;
	long line = -1;

	y=0;
	flg=0;

	do {
		/* Skip to first word */
		for (x = y; s[x] && !(joe_isalnum_(map,s[x]) || s[x] == '.' || s[x] == '/'); ++x) ;

		/* Skip to end of first word */
		for (y = x; joe_isalnum_(map,s[y]) || s[y] == '.' || s[y] == '/' || s[y]=='-'; ++y)
			if (s[y] == '.')
				flg = 1;
	} while (!flg && x!=y);

	/* Save file name */
	if (x != y)
		name = vsncpy(NULL, 0, s + x, y - x);

	/* Skip to first number */
	for (x = y; s[x] && (s[x] < '0' || s[x] > '9'); ++x) ;

	/* Skip to end of first number */
	for (y = x; s[y] >= '0' && s[y] <= '9'; ++y) ;

	/* Save line number */
	if (x != y)
		sscanf((char *)(s + x), "%ld", &line);
	if (line != -1)
		--line;

	/* Look for ':' */
	flg = 0;
	while (s[y]) {
	/* Allow : anywhere on line: works for MIPS C compiler */
/*
	for (y = 0; s[y];)
*/
		if (s[y]==':') {
			flg = 1;
			break;
		}
		++y;
	}

	if (!flg)
		line = -1;

	*rtn_name = name;
	*rtn_line = line;
}

/* Parser for file name lists from grep, find and ls.
 *
 * filename
 * filename:*
 * filename:line-number:*
 */

void parseone_grep(struct charmap *map,unsigned char *s,unsigned char **rtn_name,long *rtn_line)
{
	int y;
	unsigned char *name = NULL;
	long line = -1;

	/* Skip to first : or end of line */
	for (y = 0;s[y] && s[y] != ':';++y);
	if (y) {
		/* This should be the file name */
		name = vsncpy(NULL,0,s,y);
		line = 0;
		if (s[y] == ':') {
			/* Maybe there's a line number */
			++y;
			while (s[y] >= '0' && s[y] <= '9')
				line = line * 10 + (s[y++] - '0');
			--line;
			if (line < 0 || s[y] != ':') {
				/* Line number is only valid if there's a second : */
				line = 0;
			}
		}
	}

	*rtn_name = name;
	*rtn_line = line;
}

static int parseit(struct charmap *map,unsigned char *s, long int row,
  void (*parseone)(struct charmap *map, unsigned char *s, unsigned char **rtn_name, long *rtn_line), unsigned char *current_dir)
{
	unsigned char *name = NULL;
	long line = -1;
	ERROR *err;

	parseone(map,s,&name,&line);

	if (name) {
		if (line != -1) {
			/* We have an error */
			err = (ERROR *) alitem(&errnodes, sizeof(ERROR));
			err->file = name;
			if (current_dir) {
				err->file = vsncpy(NULL, 0, sv(current_dir));
				err->file = vsncpy(sv(err->file), sv(name));
				err->file = canonical(err->file);
				vsrm(name);
			} else {
				err->file = name;
			}
			err->org = err->line = line;
			err->src = row;
			err->msg = vsncpy(NULL, 0, sc("\\i"));
			err->msg = vsncpy(sv(err->msg), sv(s));
			enqueb(ERROR, link, &errors, err);
			return 1;
		} else
			vsrm(name);
	}
	return 0;
}

/* Parse the error output contained in a buffer */

void kill_ansi(unsigned char *s);

static long parserr(B *b)
{
	if (markv(1)) {
		P *p = pdup(markb, USTR "parserr1");
		P *q = pdup(markb, USTR "parserr2");
		long nerrs = 0;
		errbuf = markb->b;

		freeall();

		p_goto_bol(p);

		do {
			unsigned char *s;

			pset(q, p);
			p_goto_eol(p);
			s = brvs(q, (int) (p->byte - q->byte));
			if (s) {
				kill_ansi(s);
				nerrs += parseit(q->b->o.charmap, s, q->line, (q->b->parseone ? q->b->parseone : parseone),q->b->current_dir);
				vsrm(s);
			}
			pgetc(p);
		} while (p->byte < markk->byte);
		prm(p);
		prm(q);
		return nerrs;
	} else {
		P *p = pdup(b->bof, USTR "parserr3");
		P *q = pdup(p, USTR "parserr4");
		long nerrs = 0;
		errbuf = b;

		freeall();
		do {
			unsigned char *s;

			pset(q, p);
			p_goto_eol(p);
			s = brvs(q, (int) (p->byte - q->byte));
			if (s) {
				kill_ansi(s);
				nerrs += parseit(q->b->o.charmap, s, q->line, (q->b->parseone ? q->b->parseone : parseone), q->b->current_dir);
				vsrm(s);
			}
		} while (pgetc(p) != NO_MORE_DATA);
		prm(p);
		prm(q);
		return nerrs;
	}
}

BW *find_a_good_bw(B *b)
{
	W *w;
	BW *bw = 0;
	/* Find lowest window with buffer */
	if ((w = maint->topwin) != NULL) {
		do {
			if ((w->watom->what&TYPETW) && ((BW *)w->object)->b==b && w->y>=0)
				bw = (BW *)w->object;
			w = w->link.next;
		} while (w != maint->topwin);
	}
	if (bw)
		return bw;
	/* Otherwise just find lowest window */
	if ((w = maint->topwin) != NULL) {
		do {
			if ((w->watom->what&TYPETW) && w->y>=0)
				bw = (BW *)w->object;
			w = w->link.next;
		} while (w != maint->topwin);
	}
	return bw;
}

int parserrb(B *b)
{
	BW *bw;
	int n;
	freeall();
	bw = find_a_good_bw(b);
	unmark(bw);
	n = parserr(b);
	if (n)
		joe_snprintf_1(msgbuf, JOE_MSGBUFSIZE, joe_gettext(_("%d messages found")), n);
	else
		joe_snprintf_0(msgbuf, sizeof(msgbuf), joe_gettext(_("No messages found")));
	msgnw(bw->parent, msgbuf);
	return 0;
}

int urelease(BW *bw)
{
	bw->b->parseone = 0;
	if (qempty(ERROR, link, &errors) && !errbuf) {
		joe_snprintf_0(msgbuf, sizeof(msgbuf), joe_gettext(_("No messages")));
	} else {
		int count = freeall();
		errbuf = NULL;
		joe_snprintf_1(msgbuf, sizeof(msgbuf), joe_gettext(_("%d messages cleared")), count);
	}
	msgnw(bw->parent, msgbuf);
	updall();
	return 0;
}

int uparserr(BW *bw)
{
	int n;
	freeall();
	bw->b->parseone = parseone;
	n = parserr(bw->b);
	if (n)
		joe_snprintf_1(msgbuf, JOE_MSGBUFSIZE, joe_gettext(_("%d messages found")), n);
	else
		joe_snprintf_0(msgbuf, sizeof(msgbuf), joe_gettext(_("No messages found")));
	msgnw(bw->parent, msgbuf);
	return 0;
}

int ugparse(BW *bw)
{
	int n;
	freeall();
	bw->b->parseone = parseone_grep;
	n = parserr(bw->b);
	if (n)
		joe_snprintf_1(msgbuf, JOE_MSGBUFSIZE, joe_gettext(_("%d messages found")), n);
	else
		joe_snprintf_0(msgbuf, sizeof(msgbuf), joe_gettext(_("No messages found")));
	msgnw(bw->parent, msgbuf);
	return 0;
}

int jump_to_file_line(BW *bw,unsigned char *file,int line,unsigned char *msg)
{
	int omid;
	if (!bw->b->name || zcmp(file, bw->b->name)) {
		if (doswitch(bw, vsdup(file), NULL, NULL))
			return -1;
		bw = (BW *) maint->curwin->object;
	}
	omid = mid;
	mid = 1;
	pline(bw->cursor, line);
	dofollows();
	mid = omid;
	bw->cursor->xcol = piscol(bw->cursor);
	msgnw(bw->parent, msg);
	return 0;
}

/* Show current message */

int ucurrent_msg(BW *bw)
{
	if (errptr != &errors) {
		msgnw(bw->parent, errptr->msg);
		return 0;
	} else {
		msgnw(bw->parent, joe_gettext(_("No messages")));
		return -1;
	}
}

/* Find line in error database: return pointer to message */

ERROR *srcherr(BW *bw,unsigned char *file,long line)
{
	ERROR *p;
	for (p = errors.link.next; p != &errors; p=p->link.next)
		if (!zcmp(p->file,file) && p->org == line) {
			errptr = p;
			setline(errbuf, errptr->src);
			return errptr;
			}
	return 0;
}

/* Delete ansi formatting */

void kill_ansi(unsigned char *s)
{
	unsigned char *d = s;
	while (*s)
		if (*s == 27) {
			while (*s && (*s == 27 || *s == '[' || (*s >= '0' && *s <= '9') || *s == ';'))
				++s;
			if (*s)
				++s;
		} else
			*d++ = *s++;
	*d = 0;
}

int ujump(BW *bw)
{
	int rtn = -1;
	P *p = pdup(bw->cursor, USTR "ujump");
	P *q = pdup(p, USTR "ujump");
	unsigned char *s;
	p_goto_bol(p);
	p_goto_eol(q);
	s = brvs(p, (int) (q->byte - p->byte));
	kill_ansi(s);
	prm(p);
	prm(q);
	if (s) {
		unsigned char *name = NULL;
		unsigned char *fullname = NULL;
		unsigned char *curd = get_cd(bw->parent);
		long line = -1;
		if (bw->b->parseone)
			bw->b->parseone(bw->b->o.charmap,s,&name,&line);
		else
			parseone_grep(bw->b->o.charmap,s,&name,&line);
		/* Prepend current directory.. */
		fullname = vsncpy(NULL, 0, sv(curd));
		fullname = vsncpy(sv(fullname), sv(name));
		vsrm(name);
		name = canonical(fullname);
		if (name && line != -1) {
			ERROR *p = srcherr(bw, name, line);
			uprevw((BASE *)bw);
			/* Check that we made it to a tw */
			if (p)
				rtn = jump_to_file_line(maint->curwin->object,name,p->line,NULL /* p->msg */);
			else
				rtn = jump_to_file_line(maint->curwin->object,name,line,NULL);
			vsrm(name);
		}
		vsrm(s);
	}
	return rtn;
}

int unxterr(BW *bw)
{
	if (errptr->link.next == &errors) {
		msgnw(bw->parent, joe_gettext(_("No more errors")));
		return -1;
	}
	errptr = errptr->link.next;
	setline(errbuf, errptr->src);
	return jump_to_file_line(bw,errptr->file,errptr->line,NULL /* errptr->msg */);
}

int uprverr(BW *bw)
{
	if (errptr->link.prev == &errors) {
		msgnw(bw->parent, joe_gettext(_("No more errors")));
		return -1;
	}
	errptr = errptr->link.prev;
	setline(errbuf, errptr->src);
	return jump_to_file_line(bw,errptr->file,errptr->line,NULL /* errptr->msg */);
}
