/*
 *	Regular expression subroutines
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

int escape(int utf8,char **a, ptrdiff_t *b);
int pmatch(char **pieces, char *regex, ptrdiff_t len, P *p, ptrdiff_t n, int icase);
