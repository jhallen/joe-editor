/*
 *	Search & Replace system
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_USEARCH_H
#define _JOE_USEARCH_H 1

struct srchrec {
	LINK(SRCHREC)	link;	/* Linked list of search & replace locations */
	int	yn;		/* Did we replace? */
	int	wrap_flag;	/* Did we wrap? */
	long	addr;		/* Where we were */
	B *b;			/* Buffer address is in */
	long	last_repl;
};

struct search {
	unsigned char	*pattern;	/* Search pattern */
	unsigned char	*replacement;	/* Replacement string */
	int	backwards;	/* Set if search should go backwards */
	int	ignore;		/* Set if we should ignore case */
	int	repeat;		/* Set with repeat count (or -1 for no repeat count) */
	int	replace;	/* Set if this is search & replace */
	int	rest;		/* Set to do remainder of search & replace w/o query */
	unsigned char	*entire;	/* Entire matched string */
	unsigned char	*pieces[26];	/* Peices of the matched string */
	int	flg;		/* Set after prompted for first replace */
	SRCHREC	recs;		/* Search & replace position history */
	P	*markb, *markk;	/* Original marks */
	P	*wrap_p;	/* Wrap point */
	int	wrap_flag;	/* Set if we've wrapped */
	int	allow_wrap;	/* Set to allow wrapping */
	int	valid;		/* Set if original marks are a valid block */
	long	addr;		/* Where to place cursor after failed restruct_to_block() test */
	long	last_repl;	/* Address of last replacement (prevents infinite loops) */
	int	block_restrict;	/* Search restricted to marked block */
	int	all;		/* Set to continue in other windows */
	B	*first;		/* Starting buffer */
	B	*current;	/* Current buffer */
};

SRCH *mksrch PARAMS((unsigned char *pattern, unsigned char *replacement, int ignore, int backwards, int repeat, int replace, int rest, int all));
void rmsrch PARAMS((SRCH *srch));

int dopfnext PARAMS((BW *bw, SRCH *srch, int *notify));

int pffirst PARAMS((BW *bw));
int pfnext PARAMS((BW *bw));

int pqrepl PARAMS((BW *bw));
int prfirst PARAMS((BW *bw));

int ufinish PARAMS((BW *bw));
int dofirst PARAMS((BW *bw, int back, int repl, unsigned char *hint));

extern B *findhist; /* Search history buffer */
extern B *replhist; /* Replace history buffer */

void save_srch PARAMS((FILE *f));
void load_srch PARAMS((FILE *f));

extern int smode;
extern int csmode;
extern int icase;
extern int wrap;
extern int pico;
extern unsigned char srchstr[];
extern SRCH *globalsrch;

extern unsigned char *rest_key;
extern unsigned char *backup_key;

int fwrd_c PARAMS((unsigned char **s));

#endif
