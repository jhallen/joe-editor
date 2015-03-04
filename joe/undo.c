/*
 *	UNDO system
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

int undo_keep = 100; /* Number of undo records to keep */

#define SMALL 1024

#define MAX_YANK 100

static UNDO undos = { {&undos, &undos} };
static UNDO frdos = { {&frdos, &frdos} };

int inundo = 0;
int inredo = 0;	/* Turns off recording of undo records */

UNDOREC yanked = { {&yanked, &yanked} };
int nyanked = 0;
int inyank = 0;
int justkilled = 0;

UNDOREC frrecs = { {&frrecs, &frrecs} };

static UNDOREC *alrec(void)
{
	UNDOREC *rec = (UNDOREC *) alitem(&frrecs, sizeof(UNDOREC));

	return rec;
}

static void frrec(UNDOREC *rec)
{
	if (rec->del) {
		if (rec->len < SMALL)
			joe_free(rec->small);
		else {
			B *b = rec->big;

			bonline(b);
			brm(b);
		}
	}
	enquef(UNDOREC, link, &frrecs, rec);
}

UNDO *undomk(B *b)
{
	UNDO *undo = (UNDO *) alitem(&frdos, sizeof(UNDO));

	undo->nrecs = 0;
	undo->ptr = NULL;
	undo->last = NULL;
	undo->first = NULL;
	undo->b = b;
	izque(UNDOREC, link, &undo->recs);
	enquef(UNDO, link, &undos, undo);
	return undo;
}

void undorm(UNDO *undo)
{
	frchn(&frrecs, &undo->recs);
	demote(UNDO, link, &frdos, undo);
}

void bw_unlock(BW *bw)
{
	if (bw->b->locked && !bw->b->ignored_lock && plain_file(bw->b)) {
		unlock_it(bw->b->name);
		bw->b->locked = 0;
	}
}

static void doundo(BW *bw, UNDOREC *ptr)
{
	dostaupd = 1;

	if (ptr->del) {
		if (ptr->len < SMALL)
			binsm(bw->cursor, ptr->small, (int) ptr->len);
		else {
			B *b = ptr->big;

			bonline(b);
			binsb(bw->cursor, bcpy(b->bof, b->eof));
			boffline(b);
		}
	} else {
		P *q = pdup(bw->cursor, USTR "doundo");

		pfwrd(q, ptr->len);
		bdel(bw->cursor, q);
		prm(q);
	}
	if (bw->b->changed && !ptr->changed)
		bw_unlock(bw);
	bw->b->changed = ptr->changed;
}

int uundo(BW *bw)
{
	UNDOREC *upto;
	UNDO *undo = bw->b->undo;

	if (!undo)
		return -1;
	if (!undo->nrecs)
		return -1;
	if (!undo->ptr) {
		pgoto(bw->cursor, undo->recs.link.prev->where);
		undo->ptr = &undo->recs;
		/* If this return is uncommented, then uundo will jump
		   to where the undo is about to occur before actually
		   undoing anything */
		/* return 0; */
	}
	if (undo->ptr->link.prev == &undo->recs)
		return -1;
	upto = undo->ptr->link.prev->unit;
	do {
		undo->ptr = undo->ptr->link.prev;
		pgoto(bw->cursor, undo->ptr->where);
		inundo = 1;
		doundo(bw, undo->ptr);
		inundo = 0;
	} while (upto && upto != undo->ptr);
	return 0;
}

int uredo(BW *bw)
{
	UNDOREC *upto;
	UNDOREC *ptr;
	UNDO *undo = bw->b->undo;

	if (!undo)
		return -1;
	if (!undo->ptr)
		return -1;
	if (undo->ptr == &undo->recs)
		return -1;
	upto = undo->recs.link.prev->unit;
	do {
		ptr = undo->recs.link.prev;
		pgoto(bw->cursor, ptr->where);
		inredo = 1;
		doundo(bw, ptr);
		inredo = 0;
		frrec(deque_f(UNDOREC, link, ptr)); /* Delete record created by undo command */
		undo->ptr = undo->ptr->link.next;
	} while (upto && upto != ptr);
	/* We just deleted one undo record */
	--undo->nrecs;
	return 0;
}

void umclear(void)
{
	UNDO *undo;

	for (undo = undos.link.next; undo != &undos; undo = undo->link.next) {
		UNDOREC *rec;

		for (rec = undo->recs.link.next; rec != &undo->recs; rec = rec->link.next)
			rec->min = 0;
	}
}

/* Eliminate excess undo records */

static void undogc(UNDO *undo)
{
	UNDOREC *unit = undo->recs.link.next->unit;
	int flg = 0;

	if (unit)
		while (unit != undo->recs.link.next) {
			if (undo->recs.link.next == undo->ptr)
				flg = 1;
			frrec(deque_f(UNDOREC, link, undo->recs.link.next));
		}
	if (undo->recs.link.next == undo->ptr)
		flg = 1;
	frrec(deque_f(UNDOREC, link, undo->recs.link.next));
	--undo->nrecs;
	if (flg)
		undo->ptr = undo->recs.link.next;
}

void undomark(void)
{
	UNDO *undo;

	/* Force undo_keep to be a multiple of 2.  Redo needs pairs of undo records. */
	if (undo_keep & 1)
		++undo_keep;

	for (undo = undos.link.next; undo != &undos; undo = undo->link.next)
		if (undo->first) {
			undo->first->unit = undo->last;
			undo->last->unit = undo->first;
			undo->first = undo->last = 0;
			++undo->nrecs;
			if (undo_keep)
				while (undo->nrecs > undo_keep)
					undogc(undo);
		}
}

/* Forget pointer to latest "undone" record.  This is called when the first non-undo
 * command is executed after a bunch of undo commands, which then prevents redo. */

static void undoover(UNDO *undo)
{
	undo->ptr = NULL;
}

void undoins(UNDO *undo, P *p, long size)
{
	UNDOREC *rec;

	if (inredo)
		return;
	if (!inundo)
		if (undo->ptr && undo->ptr != &undo->recs)
			undoover(undo);
	rec = undo->recs.link.prev;
	if (rec != &undo->recs && rec->min && !rec->del && (p->byte == rec->where + rec->len || p->byte == rec->where))
		rec->len += size;
	else {
		rec = alrec();
		rec->del = 0;
		if (!undo->first)
			undo->first = rec;
		undo->last = rec;
		rec->where = p->byte;
		rec->min = 1;
		rec->unit = NULL;
		rec->len = size;
		rec->changed = undo->b->changed;
		enqueb(UNDOREC, link, &undo->recs, rec);
	}
}


int uyapp(BW *bw)
{
	UNDOREC *rec = yanked.link.prev;

	if (rec != &yanked)
		rec->where = bw->cursor->byte;
	return 0;
}

static void yankdel(long where, B *b)
{
	UNDOREC *rec;
	long size = b->eof->byte;

	/* Store in yank buffer */
	rec = yanked.link.prev;
	if (!inyank) {
		if (rec != &yanked && where == rec->where && justkilled) {
			if (rec->len + size >= SMALL) {
				if (rec->len < SMALL) {
					rec->big = bmk(NULL);
					binsm(rec->big->bof, rec->small, (int) rec->len);
					boffline(rec->big);
					joe_free(rec->small);
				}
				bonline(rec->big);
				binsb(rec->big->eof, bcpy(b->bof, b->eof));
				boffline(rec->big);
			} else {
				rec->small = (unsigned char *) joe_realloc(rec->small, rec->len + size);
				brmem(b->bof, rec->small + rec->len, (int) size);
			}
			rec->len += size;
		} else if (rec != &yanked && where + size == rec->where && justkilled) {
			if (rec->len + size >= SMALL) {
				if (rec->len < SMALL) {
					rec->big = bmk(NULL);
					binsm(rec->big->bof, rec->small, (int) rec->len);
					boffline(rec->big);
					joe_free(rec->small);
				}
				bonline(rec->big);
				binsb(rec->big->bof, bcpy(b->bof, b->eof));
				boffline(rec->big);
			} else {
				rec->small = (unsigned char *) joe_realloc(rec->small, rec->len + size);
				mmove(rec->small + size, rec->small, (int) rec->len);
				brmem(b->bof, rec->small, (int) size);
			}
			rec->len += size;
			rec->where = where;
		} else {
			if (++nyanked == MAX_YANK) {
				frrec(deque_f(UNDOREC, link, yanked.link.next));
				--nyanked;
			}
			rec = alrec();
			if (size < SMALL && size > 0) {
				rec->small = (unsigned char *) joe_malloc(size);
				brmem(b->bof, rec->small, (int) b->eof->byte);
			} else {
				rec->big = bcpy(b->bof, b->eof);
				boffline(rec->big);
			}
			rec->where = where;
			rec->len = size;
			rec->del = 1;
			enqueb(UNDOREC, link, &yanked, rec);
		}
	}
}

void undodel(UNDO *undo, long where, B *b)
{
	UNDOREC *rec;
	long size = b->eof->byte;

	if (inredo) {
		brm(b);
		return;
	}
	if (!inundo)
		if (undo->ptr && undo->ptr != &undo->recs)
			undoover(undo);

	yankdel(where, b);

	/* Store in undo buffer */
	rec = undo->recs.link.prev;
	if (rec != &undo->recs && rec->min && rec->del && where == rec->where) {
		if (rec->len + size >= SMALL) {
			if (rec->len < SMALL) {
				rec->big = bmk(NULL);
				binsm(rec->big->bof, rec->small, (int) rec->len);
				boffline(rec->big);
				joe_free(rec->small);
			}
			bonline(rec->big);
			binsb(rec->big->eof, b);
			boffline(rec->big);
		} else {
			rec->small = (unsigned char *) joe_realloc(rec->small, rec->len + size);
			brmem(b->bof, rec->small + rec->len, (int) size);
			brm(b);
		}
		rec->len += size;
	} else if (rec != &undo->recs && rec->min && rec->del && where + size == rec->where) {
		if (rec->len + size >= SMALL) {
			if (rec->len < SMALL) {
				rec->big = bmk(NULL);
				binsm(rec->big->bof, rec->small, (int) rec->len);
				boffline(rec->big);
				joe_free(rec->small);
			}
			bonline(rec->big);
			binsb(rec->big->bof, b);
			boffline(rec->big);
		} else {
			rec->small = (unsigned char *) joe_realloc(rec->small, rec->len + size);
			mmove(rec->small + size, rec->small, (int) rec->len);
			brmem(b->bof, rec->small, (int) size);
			brm(b);
		}
		rec->len += size;
		rec->where = where;
	} else {
		rec = alrec();
		if (size < SMALL) {
			rec->small = (unsigned char *) joe_malloc(size);
			brmem(b->bof, rec->small, (int) b->eof->byte);
			brm(b);
		} else {
			rec->big = b;
			boffline(b);
		}
		if (!undo->first)
			undo->first = rec;
		undo->last = rec;
		rec->where = where;
		rec->min = 1;
		rec->unit = NULL;
		rec->len = size;
		rec->del = 1;
		rec->changed = undo->b->changed;
		enqueb(UNDOREC, link, &undo->recs, rec);
	}
}

B *yankbuf = NULL;
long yankwhere = -1;

int uyank(BW *bw)
{
	UNDOREC *ptr = yanked.link.prev;

	if (ptr != &yanked) {
		if (ptr->len < SMALL)
			binsm(bw->cursor, ptr->small, (int) ptr->len);
		else {
			B *b = ptr->big;

			bonline(b);
			binsb(bw->cursor, bcpy(b->bof, b->eof));
			boffline(b);
		}
		pfwrd(bw->cursor, ptr->len);
		yankbuf = bw->b;
		yankwhere = bw->cursor->byte;
		return 0;
	} else
		return -1;
}

int uyankpop(BW *bw)
{
	if (bw->b == yankbuf && bw->cursor->byte == yankwhere) {
		P *q;
		UNDOREC *ptr = yanked.link.prev;

		deque(UNDOREC, link, &yanked);
		enqueb(UNDOREC, link, ptr, &yanked);
		q = pdup(bw->cursor, USTR "uyankpop");
		pbkwd(q, ptr->len);
		inyank = 1;
		bdel(q, bw->cursor);
		inyank = 0;
		prm(q);
		return uyank(bw);
	} else
		return uyank(bw);
}

/* Clear changed-flag: make buffer look unmodified */

int unotmod(BW *bw)
{
	bw_unlock(bw);
	bw->b->changed = 0;
	msgnw(bw->parent, joe_gettext(_("Modified flag cleared")));
	return 0;
}



int ucopy(BW *bw)
{
	if (markv(1) && !square) {
		B *b = bcpy(markb, markk);

		yankdel(markb->byte, b);
		brm(b);
		if (lightoff)
			unmark(bw);
		return 0;
	} else {
		msgnw(bw->parent, joe_gettext(_("No block")));
		return -1;
	}
}

/* Save yank buffers */

void save_yank(FILE *f)
{
	UNDOREC *rec;
	for (rec = yanked.link.next; rec != &yanked; rec = rec->link.next) {
		if (rec->len < SMALL) {
			fprintf(f,"	");
			emit_string(f,rec->small,rec->len);
			fprintf(f,"\n");
		}
	}
	fprintf(f,"done\n");
}

/* Load yank buffers */

void load_yank(FILE *f)
{
	UNDOREC *rec;
	unsigned char buf[SMALL*4+80];
	unsigned char bf[SMALL+1];
	while(fgets((char *)buf,sizeof(buf)-1,f) && zcmp(buf,USTR "done\n")) {
		unsigned char *p = buf;
		int len;
		parse_ws(&p,'#');
		len = parse_string(&p,bf,sizeof(bf));
		if (len>0 && len<=SMALL) {
			if (++nyanked == MAX_YANK) {
				frrec(deque_f(UNDOREC, link, yanked.link.next));
				--nyanked;
			}
			rec = alrec();
			rec->small = (unsigned char *) joe_malloc(len);
			memcpy(rec->small,bf,len);
			rec->where = -1;
			rec->len = len;
			rec->del = 1;
			enqueb(UNDOREC, link, &yanked, rec);
		}
	}
}
