/*
 *	Line attribute cache
 *	Copyright
 *		(C) 2006 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

struct lattr_db
  {
  struct lattr_db *next;	/* Linked list of attribute databases owned by a B */
  struct high_syntax *syn;	/* This database is for this syntax */
  B *b;				/* This database is for this buffer */

  /* Use a gap buffer for the attribute records */

  HIGHLIGHT_STATE *buffer;	/* Address of buffer */
  long hole;			/* Offset to hole */
  long ehole;			/* Offset to end of hole */
  long end;			/* Malloc() size of buffer */

  long first_invalid;		/* Lines beginning with this are invalid */
  long invalid_window;		/* Lines beyond first_invalid+invalid_window might be valid */
                                /* -1 means all lines are valid */
  };

struct lattr_db *mk_lattr_db PARAMS((B *new_b, struct high_syntax *new_syn));
                                /* Create database structure */

void rm_lattr_db PARAMS((struct lattr_db *db));
                                /* Delete database structure */

void rm_all_lattr_db PARAMS((struct lattr_db *db));
                                /* Delete linked-list of databases */

void reset_all_lattr_db PARAMS((struct lattr_db *db));
                                /* Delete linked-list of databases */

struct lattr_db *find_lattr_db PARAMS((B *b, struct high_syntax *y));
                                /* Find database for a particular syntax.  If one doesn't
                                   exist, create it and add it to the list for the B */

void drop_lattr_db PARAMS((B *b, struct lattr_db *db));
                                /* Drop a database if it's no longer needed. This checks through all BWs on a B
                                   to see if any of them refer to db.  If none, the db is dropped. */

#define lattr_size(db) ((db)->end - ((db)->ehole - (db)->hole))

void lattr_hole PARAMS((struct lattr_db *db, long pos));
  /* Set hole position */

void lattr_check PARAMS((struct lattr_db *db, long size));
  /* Make sure we have enough space for insert.  If not, expand buffer. */

void lattr_ins PARAMS((struct lattr_db *db,long line,long size));
  /* An insert occured, beginning on specified line.  'size' lines were inserted.
     Adjust invalid window to cover inserted area.
  */

void lattr_del PARAMS((struct lattr_db *db,long line,long size));
  /* A deletion occured, beginning on specified line.  'size' lines were deleted.
     Adjust invalid window to cover deleted area.
  */

HIGHLIGHT_STATE lattr_get PARAMS((struct lattr_db *db,struct high_syntax *y,P *p,long line));
  /* Get state for specified line.  If we don't have it, compute it.
     Records results of any computation so that we don't have to do it again.
     If first_invalid is < number of lines we have, compute forward until we
     start matching again as this is a very common case. */

#define lattr_lvalue(db, line) ((line) >= (db)->hole ? (db)->buffer[(line) - (db)->hole + (db)->ehole] : (db)->buffer[line])
