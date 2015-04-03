/*
 * 	User text formatting functions
 * 	Copyright
 *		(C) 1992 Joseph H. Allen
 * 
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_UFORMAT_H
#define _JOE_UFORMAT_H 1

int ucenter(BW *bw);
P *pbop(BW *bw, P *p);
P *peop(BW *bw, P *p);
int ubop(BW *bw);
int ueop(BW *bw);
void wrapword(BW *bw, P *p, long int indent, int french, int no_over, unsigned char *indents);
int uformat(BW *bw);
int ufmtblk(BW *bw);

#endif
