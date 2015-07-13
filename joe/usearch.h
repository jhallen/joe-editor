/*
 *	Search & Replace system
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

struct srchrec {
	LINK(SRCHREC)	link;	/* Linked list of search & replace locations */
	int	yn;		/* Did we replace? */
	int	wrap_flag;	/* Did we wrap? */
	off_t	addr;		/* Where we were */
	B *b;			/* Buffer address is in */
	off_t	last_repl;
};

struct search {
	char	*pattern;	/* Search pattern */
	char	*replacement;	/* Replacement string */
	int	backwards;	/* Set if search should go backwards */
	int	ignore;		/* Set if we should ignore case */
	int	repeat;		/* Set with repeat count (or -1 for no repeat count) */
	int	replace;	/* Set if this is search & replace */
	int	rest;		/* Set to do remainder of search & replace w/o query */
	char	*entire;	/* Entire matched string */
	char	*pieces[26];	/* Peices of the matched string */
	int	flg;		/* Set after prompted for first replace */
	SRCHREC	recs;		/* Search & replace position history */
	P	*markb, *markk;	/* Original marks */
	P	*wrap_p;	/* Wrap point */
	int	wrap_flag;	/* Set if we've wrapped */
	int	allow_wrap;	/* Set to allow wrapping */
	int	valid;		/* Set if original marks are a valid block */
	off_t	addr;		/* Where to place cursor after failed restruct_to_block() test */
	off_t	last_repl;	/* Address of last replacement (prevents infinite loops) */
	int	block_restrict;	/* Search restricted to marked block */
	int	all;		/* Set to continue in other windows */
	B	*first;		/* Starting buffer */
	B	*current;	/* Current buffer */
};

SRCH *mksrch(char *pattern, char *replacement, int ignore, int backwards, int repeat, int replace, int rest, int all);
void rmsrch(SRCH *srch);

int dopfnext(BW *bw, SRCH *srch, int *notify);

int pffirst(W *w, int k);
int pfnext(W *w, int k);

int pqrepl(W *w, int k);
int prfirst(W *w, int k);

int ufinish(W *w, int k);
int dofirst(BW *bw, int back, int repl, char *hint);

extern B *findhist; /* Search history buffer */
extern B *replhist; /* Replace history buffer */

void save_srch(FILE *f);
void load_srch(FILE *f);

extern int smode;
extern int csmode;
extern int opt_icase;
extern int wrap;
extern int pico;
extern char srchstr[];
extern SRCH *globalsrch;

extern const char *rest_key;
extern const char *backup_key;

int fwrd_c(const char **s);
