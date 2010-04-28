/*
 *	Menu selection window
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_MENU_H
#define _JOE_MENU_H 1

/* A menu window */

struct menu {
	W	*parent;	/* Window we're in */
	unsigned char	**list;		/* List of items */
	int	top;		/* First item on screen */
	int	cursor;		/* Item cursor is on */
	int	width;		/* Width of widest item, up to 'w' max */
	int 	fitline;	/* Number of items we can fit on each line */
	int	perline;	/* Number of items we place on each line */
	int	lines;		/* Total no. of lines */
	int	nitems;		/* No. items in list */
	Screen	*t;		/* Screen we're on */
	int	h, w, x, y;
	int	(*abrt) ();	/* Abort callback function */
	int	(*func) ();	/* Return callback function */
	int	(*backs) ();	/* Backspace callback function */
	void	*object;
};

/* Create a menu */
/* FIXME: ??? ---> */
MENU *mkmenu PARAMS((W *loc, W *targ, unsigned char **s, int (*func) (/* ??? */), int (*abrt) (/* ??? */), int (*backs) (/* ??? */), int cursor, void *object, int *notify));

/* Menu user functions */

int umuparw PARAMS((MENU *m));
int umdnarw PARAMS((MENU *m));
int umpgup PARAMS((MENU *m));
int umpgdn PARAMS((MENU *m));
int umscrup PARAMS((MENU *m));
int umscrdn PARAMS((MENU *m));
int umltarw PARAMS((MENU *m));
int umrtarw PARAMS((MENU *m));
int umtab PARAMS((MENU *m));
int umbof PARAMS((MENU *m));
int umeof PARAMS((MENU *m));
int umbol PARAMS((MENU *m));
int umeol PARAMS((MENU *m));
int umbacks PARAMS((MENU *m));

void ldmenu PARAMS((MENU *m, unsigned char **s, int cursor));

unsigned char *mcomplete PARAMS((MENU *m));
unsigned char *find_longest PARAMS((unsigned char **lst));

void menujump PARAMS((MENU *m, int x, int y));

extern int lines; /* Number of menu lines */

extern WATOM watommenu; /* Menu WATOM */

extern int menu_above; /* Menu position: above or below */
extern int bg_menu; /* Background color for menu */
extern int transpose;

#endif
