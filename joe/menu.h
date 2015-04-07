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
	int (*abrt)(W *w, ptrdiff_t cursor, void *object); /* Abort callback function */
	int (*func)(MENU *m, ptrdiff_t cursor, void *object, int k); /* Return callback function */
	int (*backs)(MENU *m, ptrdiff_t cursor, void *object); /* Backspace callback function */
	void	*object;
};

/* Create a menu */
MENU *mkmenu(W *loc, W *targ, char **s, int (*func)(MENU *m, ptrdiff_t cursor, void *object, int k),
             int (*abrt)(W *w, ptrdiff_t cursor, void *object),
             int (*backs)(MENU *m, ptrdiff_t cursor, void *object),
             ptrdiff_t cursor, void *object, int *notify);

/* Menu user functions */

int umuparw(W *w, int k);
int umdnarw(W *w, int k);
int umpgup(W *w, int k);
int umpgdn(W *w, int k);
int umscrup(W *w, int k);
int umscrdn(W *w, int k);
int umltarw(W *w, int k);
int umrtarw(W *w, int k);
int umtab(W *w, int k);
int umbof(W *w, int k);
int umeof(W *w, int k);
int umbol(W *w, int k);
int umeol(W *w, int k);
int umbacks(W *w, int k);

void ldmenu(MENU *m, char **s, ptrdiff_t cursor);

char *mcomplete(MENU *m);
char *find_longest(char **lst);

void menujump(MENU *m, ptrdiff_t x, ptrdiff_t y);

extern WATOM watommenu; /* Menu WATOM */

extern int menu_above; /* Menu position: above or below */
extern int bg_menu; /* Background color for menu */
extern int transpose;
