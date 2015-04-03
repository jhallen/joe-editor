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
MENU *mkmenu(W *loc, W *targ, char **s, int (*func) (/* ??? */), int (*abrt) (/* ??? */), int (*backs) (/* ??? */), int cursor, void *object, int *notify);

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

void ldmenu(MENU *m, char **s, int cursor);

char *mcomplete(MENU *m);
char *find_longest(char **lst);

void menujump(MENU *m, int x, int y);

extern WATOM watommenu; /* Menu WATOM */

extern int menu_above; /* Menu position: above or below */
extern int bg_menu; /* Background color for menu */
extern int transpose;
