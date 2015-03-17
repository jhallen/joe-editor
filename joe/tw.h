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

BW *wmktw PARAMS((Screen *t, B *b));

int usplitw PARAMS((BW *bw));
int uduptw PARAMS((BW *bw));
int utw0 PARAMS((BASE *b));
int utw1 PARAMS((BASE *b));
int uabortbuf PARAMS((BW *bw));
int ucancel PARAMS((BW *bw, int k));
int upopabort PARAMS((BW *bw));
int uabort PARAMS((BW *bw, int k));
int uabort1 PARAMS((BW *bw, int k));
void setline PARAMS((B *b, long int line));
int abortit PARAMS((BW *bw));
extern int staen;
extern int staupd;
extern int keepup;
extern int bg_stalin;

extern WATOM watomtw;
unsigned char *stagen(unsigned char *stalin, BW *bw, unsigned char *s, int fill);

#endif
