/*
 *	Compiler error handler
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

int unxterr(W *w, int k);
int uprverr(W *w, int k);
int parserrb(B *b);
int uparserr(W *w, int k);
int ugparse(W *w, int k);
int urelease(W *w, int k);
int ujump(W *w, int k);
void inserr(const char *name, off_t where, off_t n, int bol);
void delerr(const char *name, off_t where, off_t n);
void abrerr(const char *name);
void saverr(const char *name);
B *beafter(B *);
extern B *errbuf; /* Buffer which has error messages */
void parseone_grep(struct charmap *map, const char *s, char **rtn_name, off_t *rtn_line);
int ucurrent_msg(W *w, int k);
