/*
 *	Math
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
extern char *merr; /* Math error message: set if calc returned an error */
double calc(BW *bw, char *s,int secure);
int umath(BW *bw);
int usmath(BW *bw);

extern B *mathhist;
