/*
 *	Compiler error handler
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_UERROR_H
#define _JOE_UERROR_H 1

int unxterr(BW *bw);
int uprverr(BW *bw);
int parserrb(B *b);
int uparserr(BW *bw);
int ugparse(BW *bw);
int urelease(BW *bw);
int ujump(BW *bw);
void inserr(unsigned char *name, long int where, long int n, int bol);
void delerr(unsigned char *name, long int where, long int n);
void abrerr(unsigned char *name);
void saverr(unsigned char *name);
B *beafter(B *);
extern B *errbuf; /* Buffer which has error messages */
void parseone_grep(struct charmap *map, unsigned char *s, unsigned char **rtn_name, long *rtn_line);
int ucurrent_msg(BW *bw);

#endif
