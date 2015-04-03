/*
 *	Compiler error handler
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

int unxterr(BW *bw);
int uprverr(BW *bw);
int parserrb(B *b);
int uparserr(BW *bw);
int ugparse(BW *bw);
int urelease(BW *bw);
int ujump(BW *bw);
void inserr(char *name, off_t where, off_t n, int bol);
void delerr(char *name, off_t where, off_t n);
void abrerr(char *name);
void saverr(char *name);
B *beafter(B *);
extern B *errbuf; /* Buffer which has error messages */
void parseone_grep(struct charmap *map, char *s, char **rtn_name, off_t *rtn_line);
int ucurrent_msg(BW *bw);
