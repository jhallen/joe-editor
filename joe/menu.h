/*
 *	Menu selection window
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* A menu window */

struct menu {
	W	*parent;	/* Window we're in */
	char	**list;		/* List of items */
	ptrdiff_t	top;		/* First item on screen */
	ptrdiff_t	cursor;		/* Item cursor is on */
	ptrdiff_t	width;		/* Width of widest item, up to 'w' max */
	ptrdiff_t	fitline;	/* Number of items we can fit on each line */
	ptrdiff_t	perline;	/* Number of items we place on each line */
	ptrdiff_t	lines;		/* Total no. of lines */
	ptrdiff_t	nitems;		/* No. items in list */
	Screen	*t;		/* Screen we're on */
	ptrdiff_t	h, w, x, y;
	int	(*abrt) ();	/* Abort callback function */
	int	(*func) ();	/* Return callback function */
	int	(*backs) ();	/* Backspace callback function */
	void	*object;
};

/* Create a menu */
/* FIXME: ??? ---> */
MENU *mkmenu(W *loc, W *targ, char **s, int (*func) (/* ??? */), int (*abrt) (/* ??? */), int (*backs) (/* ??? */), ptrdiff_t cursor, void *object, int *notify);

/* Menu user functions */

int umuparw(MENU *m);
int umdnarw(MENU *m);
int umpgup(MENU *m);
int umpgdn(MENU *m);
int umscrup(MENU *m);
int umscrdn(MENU *m);
int umltarw(MENU *m);
int umrtarw(MENU *m);
int umtab(MENU *m);
int umbof(MENU *m);
int umeof(MENU *m);
int umbol(MENU *m);
int umeol(MENU *m);
int umbacks(MENU *m);

void ldmenu(MENU *m, char **s, ptrdiff_t cursor);

char *mcomplete(MENU *m);
char *find_longest(char **lst);

void menujump(MENU *m, ptrdiff_t x, ptrdiff_t y);

extern WATOM watommenu; /* Menu WATOM */

extern int menu_above; /* Menu position: above or below */
extern int bg_menu; /* Background color for menu */
extern int transpose;
