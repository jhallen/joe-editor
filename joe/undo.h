/*
 *	UNDO system
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

struct undorec {
	LINK(UNDOREC)	link;	/* Doubly-linked list of undo records */
	UNDOREC	*unit;		/* If this is first record of a unit: points to last.  If this is last record of a unit: points to first. */
	int	min;		/* Set to allow merging of undo information with this record (like to small inserts) */
	int	changed;	/* Status of modified flag before this record */
	off_t	where;		/* Buffer address of this record */
	off_t	len;		/* Length of insert or delete */
	int	del;		/* Set if this is a delete */
	B	*big;		/* Set to buffer containing a large amount of deleted data */
	char	*small;		/* Set to malloc block containg a small amount of deleted data */
};

struct undo {
	LINK(UNDO)	link;
	B	*b;
	ptrdiff_t	nrecs;		/* Number of units */
	UNDOREC	recs;		/* The undo records: recs.link.prev is latest */
	UNDOREC	*ptr;		/* Pointer to latest "undone" record. */
	UNDOREC	*first;		/* Pointers to first and last records of a group.  The group is */
	UNDOREC	*last;		/* treated as a single undo record. */
};

extern int inundo; /* Set if inserts/deletes are part of an undo operation */
extern int justkilled; /* Last edit was a delete, so store data in yank buffer */

UNDO *undomk(B *b);
void undorm(UNDO *undo);
int uundo(W *w, int k);
int uredo(W *w, int k);
void umclear(void); /* Call this to finalize current undo records.  New changes will create new records. */
void undomark(void);
void undoins(UNDO *undo, P *p, off_t size);
void undodel(UNDO *undo, off_t where, B *b);
int uyank(W *w, int k);
int uyankpop(W *w, int k);
int uyapp(W *w, int k);
int unotmod(W *w, int k);
int ucopy(W *w, int k);

void load_yank(FILE *f);
void save_yank(FILE *f);
void bw_unlock(BW *bw);

extern int undo_keep;
