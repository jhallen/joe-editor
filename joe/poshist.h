/*
 *	Position history
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_POSHIST_H
#define _JOE_POSHIST_H 1

void afterpos PARAMS((void));
void aftermove PARAMS((W *w, P *p));
void windie PARAMS((W *w));
int uprevpos PARAMS((BW *bw));
int unextpos PARAMS((BW *bw));

#endif
