/*
 * 	User text formatting functions
 * 	Copyright
 *		(C) 1992 Joseph H. Allen
 * 
 *	This file is part of JOE (Joe's Own Editor)
 */

int ucenter(BW *bw);
P *pbop(BW *bw, P *p);
P *peop(BW *bw, P *p);
int ubop(BW *bw);
int ueop(BW *bw);
void wrapword(BW *bw, P *p, off_t indent, int french, int no_over, char *indents);
int uformat(BW *bw);
int ufmtblk(BW *bw);
