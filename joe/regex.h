/*
 *	Regular expression subroutines
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

int escape(int utf8,char **a, int *b);
int pmatch(char **pieces, char *regex, int len, P *p, int n, int icase);
