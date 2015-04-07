/*
 *	Text editing windows
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* Text window (a BW) */

struct tw {
	char	*stalin;	/* Status line info */
	char	*staright;
	int	staon;		/* Set if status line was on */
	off_t	prevline;	/* Previous cursor line number */
	int	changed;	/* Previous changed value */
	B	*prev_b;	/* Previous buffer (we need to update status line on nbuf/pbuf) */
};

BW *wmktw(Screen *t, B *b);

int usplitw(W *w, int k);
int uduptw(W *w, int k);
int utw0(W *w, int k);
int utw1(W *w, int k);
int uabortbuf(W *w, int k);
int ucancel(W *w, int k);
int upopabort(W *w, int k);
int uabort(W *w, int k);
int uabort1(W *w, int k);
void setline(B *b, off_t line);
int abortit(W *w, int k);
extern int staen;
extern int staupd;
extern int keepup;
extern int bg_stalin;

extern WATOM watomtw;
char *stagen(char *stalin, BW *bw, const char *s, char fill);
