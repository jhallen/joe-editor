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
int umarkb(BW *bw);
int umarkk(BW *bw);
int uswap(BW *bw);
int umarkl(BW *bw);
int utomarkb(BW *bw);
int utomarkk(BW *bw);
int utomarkbk(BW *bw);
int ublkdel(BW *bw);
int upicokill(BW *bw);
int ublkmove(BW *bw);
int ublkcpy(BW *bw);
int dowrite(BW *bw, char *s, void *object, int *notify);
int doinsf(BW *bw, char *s, void *object, int *notify);
void setindent(BW *bw);
int urindent(BW *bw);
int ulindent(BW *bw);
int ufilt(BW *bw);
int unmark(BW *bw);
int udrop(BW *bw);
int utoggle_marking(BW *bw);
int ubegin_marking(BW *bw);
int uselect(BW *bw);
int upsh(BW *bw);
int upop(BW *bw);
int ulower(BW *bw);
int uupper(BW *bw);
extern int nstack; /* No. block markers pushed on stack */
int blksum(double *,double *);
char *blkget();
extern int autoswap; /* Automatically swap markb and markk if need to make a valid block */
extern int nowmarking; /* Now marking flag (so make block bigger) for Ctrl-arrow key block selection */
