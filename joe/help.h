/*
 *	Help system
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *		(C) 2001 Marek 'Marx' Grac
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

void help_display(Screen *t);		/* display text in help window */
int help_on(Screen *t);		/* turn help on */
int help_init(JFILE *fd,char *bf,int line); /* read help from rc file */

int u_help(W *w, int k);		/* toggle help on/off */
int u_help_next(W *w, int k);		/* goto next help screen */
int u_help_prev(W *w, int k);		/* goto prev help screen */
extern int bg_help;				/* Background color for help */
extern int help_is_utf8;			/* Set if help text is UTF-8 */
