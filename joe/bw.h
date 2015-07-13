/*
 *	Edit buffer window generation
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* A buffer window: there are several kinds, depending on what is in 'object' */

struct bw {
	W	*parent;
	B	*b;
	P	*top;
	P	*cursor;
	off_t	offset;
	Screen	*t;
	ptrdiff_t	h, w, x, y;

	OPTIONS	o;
	void	*object;

	int	linums;
	int	top_changed;	/* Top changed */
	struct lattr_db *db;	/* line attribute database */
	int	shell_flag;	/* Cursor should follow shell cursor in this window */
};

extern int dspasis;	/* Display characters above 127 as-is */
extern int opt_mid;	/* Controls how window scrolls: when set, scroll window enough so that line with cursor becomes centered */

void bwfllw(W *w);
void bwfllwt(W *w);
void bwfllwh(W *w);
void bwins(BW *w, off_t l, off_t n, int flg);
void bwdel(BW *w, off_t l, off_t n, int flg);
void bwgen(BW *w, int linums);
void bwgenh(BW *w);
BW *bwmk(W *window, B *b, int prompt);
void bwmove(BW *w, ptrdiff_t x, ptrdiff_t y);
void bwresz(BW *w, ptrdiff_t wi, ptrdiff_t he);
void bwrm(BW *w);
int ustat(W *w, int k);
int ucrawll(W *w, int k);
int ucrawlr(W *w, int k);
void orphit(BW *bw);

extern int marking;	/* Anchored block marking mode */

void save_file_pos(FILE *f);
void load_file_pos(FILE *f);
off_t get_file_pos(const char *name);
void set_file_pos(const char *name, off_t pos);

extern int restore_file_pos;

void set_file_pos_all(Screen *t);

BW *vtmaster(Screen *t, B *b);
