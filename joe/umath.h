/*
 *	Math
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
extern const char *merr; /* Math error message: set if calc returned an error */
double calc(BW *bw, char *s,int secure);
int umath(W *w, int k);
int usmath(W *w, int k);

extern B *mathhist;
