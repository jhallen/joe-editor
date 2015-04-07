/*
 * 	Highlighted block functions
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

extern int square; /* Column / rectangular block mode */
extern int lightoff; /* Automatic turn off highlighting */
extern P *markb, *markk; /* ^KB and ^KK positions */

void pinsrect(P *cur, B *tmp, off_t width, int usetabs);
int ptabrect(P *org, off_t height, off_t right);
void pclrrect(P *org, off_t height, off_t right, int usetabs);
void pdelrect(P *org, off_t height, off_t right);
B *pextrect(P *org, off_t height, off_t right);
int markv(int r);
int umarkb(W *w, int k);
int umarkk(W *w, int k);
int uswap(W *w, int k);
int umarkl(W *w, int k);
int utomarkb(W *w, int k);
int utomarkk(W *w, int k);
int utomarkbk(W *w, int k);
int ublkdel(W *w, int k);
int upicokill(W *w, int k);
int ublkmove(W *w, int k);
int ublkcpy(W *w, int k);
int dowrite(W *w, char *s, void *object, int *notify);
int doinsf(W *w, char *s, void *object, int *notify);
void setindent(BW *bw);
int urindent(W *w, int k);
int ulindent(W *w, int k);
int ufilt(W *w, int k);
int unmark(W *w, int k);
int udrop(W *w, int k);
int utoggle_marking(W *w, int k);
int ubegin_marking(W *w, int k);
int uselect(W *w, int k);
int upsh(W *w, int k);
int upop(W *w, int k);
int ulower(W *w, int k);
int uupper(W *w, int k);
extern int nstack; /* No. block markers pushed on stack */
int blksum(double *,double *);
char *blkget();
extern int autoswap; /* Automatically swap markb and markk if need to make a valid block */
extern int nowmarking; /* Now marking flag (so make block bigger) for Ctrl-arrow key block selection */
