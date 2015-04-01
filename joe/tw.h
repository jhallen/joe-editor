/*
 *	Text editing windows
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_TW_H
#define _JOE_TW_H 1

/* Text window (a BW) */

struct tw {
	unsigned char	*stalin;	/* Status line info */
	unsigned char	*staright;
	int	staon;		/* Set if status line was on */
	long	prevline;	/* Previous cursor line number */
	int	changed;	/* Previous changed value */
	B	*prev_b;	/* Previous buffer (we need to update status line on nbuf/pbuf) */
};

BW *wmktw(Screen *t, B *b);

int usplitw(BW *bw);
int uduptw(BW *bw);
int utw0(BASE *b);
int utw1(BASE *b);
int uabortbuf(BW *bw);
int ucancel(BW *bw, int k);
int upopabort(BW *bw);
int uabort(BW *bw, int k);
int uabort1(BW *bw, int k);
void setline(B *b, long int line);
int abortit(BW *bw);
extern int staen;
extern int staupd;
extern int keepup;
extern int bg_stalin;

extern WATOM watomtw;
unsigned char *stagen(unsigned char *stalin, BW *bw, unsigned char *s, int fill);

#endif
