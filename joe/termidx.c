/*
 *	Program to generate termcap index file
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 * This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

static void gen(unsigned char *s, FILE *fd)
{
	int c, x;
	long addr = 0, oaddr;

      loop:
	while (c = getc(fd), c == ' ' || c == '\t' || c == '#')
		do {
			c = getc(fd);
		} while (!(c == -1 || c == '\n'));
	if (c == -1)
		return;
	if (c == '\n')
		goto loop;
	oaddr = addr;
	addr = ftell(fd) - 1;
	ungetc(c, fd);
	s[x = 0] = 0;
	while (1) {
		c = getc(fd);
		if (c == -1 || c == '\n') {
			if (x != 0 && s[x - 1] == '\\')
				--x;
			if (x) {
				int y, z, flg;

				s[x] = 0;
				z = 0;
				flg = 0;
				do {
					for (y = z; s[y] && s[y] != '|' && s[y] != ':'; ++y) ;
					c = s[y];
					s[y] = 0;
					if (strlen((char *)(s + z)) > 2 && !strchr((char *)(s + z), ' ') && !strchr((char *)(s + z), '\t')) {
						if(flg)
							putchar(' ');
						fputs((char *)(s + z), stdout);
						flg = 1;
					}
					s[y] = c;
					z = y + 1;
				} while (c && c != ':');
				if (flg)
					printf(" %lx\n", addr - oaddr);
			}
			goto loop;
		} else if (c == '\r')
			/* do nothing */ ;
		else
			s[x++] = c;
	}
}

int main(int argc, char *argv[])
{
	unsigned char array[65536];

	gen(array, stdin);
	return(0);
}
