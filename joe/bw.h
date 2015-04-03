/*
 *	Edit buffer window generation
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_BW_H
#define _JOE_BW_H 1

/* A buffer window: there are several kinds, depending on what is in 'object' */

struct bw {
	W	*parent;
	B	*b;
	P	*top;
	P	*cursor;
	long	offset;
	Screen	*t;
	int	h, w, x, y;

	OPTIONS	o;
	void	*object;

	int	linums;
	int	top_changed;	/* Top changed */
	struct lattr_db *db;	/* line attribute database */
	int	shell_flag;	/* Cursor should follow shell cursor in this window */
};

extern int dspasis;	/* Display characters above 127 as-is */
extern int mid;		/* Controls how window scrolls: when set, scroll window enough so that line with cursor becomes centered */

void bwfllw(BW *w);
void bwfllwt(BW *w);
void bwfllwh(BW *w);
void bwins(BW *w, long int l, long int n, int flg);
void bwdel(BW *w, long int l, long int n, int flg);
void bwgen(BW *w, int linums);
void bwgenh(BW *w);
BW *bwmk(W *window, B *b, int prompt);
void bwmove(BW *w, int x, int y);
void bwresz(BW *w, int wi, int he);
void bwrm(BW *w);
int ustat(BW *bw);
int ucrawll(BW *bw);
int ucrawlr(BW *bw);
void orphit(BW *bw);

extern int marking;	/* Anchored block marking mode */

void save_file_pos(FILE *f);
void load_file_pos(FILE *f);
long get_file_pos(unsigned char *name);
void set_file_pos(unsigned char *name, long pos);

extern int restore_file_pos;

void set_file_pos_all(Screen *t);

BW *vtmaster(Screen *t, B *b);

#endif
