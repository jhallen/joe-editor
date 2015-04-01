/*
 * 	Highlighted block functions
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_UBLOCK_H
#define _JOE_UBLOCK_H 1

extern int square; /* Column / rectangular block mode */
extern int lightoff; /* Automatic turn off highlighting */
extern P *markb, *markk; /* ^KB and ^KK positions */

void pinsrect(P *cur, B *tmp, long int width, int usetabs);
int ptabrect(P *org, long int height, long int right);
void pclrrect(P *org, long int height, long int right, int usetabs);
void pdelrect(P *org, long int height, long int right);
B *pextrect(P *org, long int height, long int right);
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
int dowrite(BW *bw, unsigned char *s, void *object, int *notify);
int doinsf(BW *bw, unsigned char *s, void *object, int *notify);
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
unsigned char *blkget();
extern int autoswap; /* Automatically swap markb and markk if need to make a valid block */
extern int nowmarking; /* Now marking flag (so make block bigger) for Ctrl-arrow key block selection */

#endif
