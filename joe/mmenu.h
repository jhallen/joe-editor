/*
 *	Menu of macros
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

struct rc_menu_entry {
	MACRO *m;
	char *name;
};

struct rc_menu {
	struct rc_menu *next;	/* Next one in list */
	char *name;	/* Name of this menu */
	ptrdiff_t last_position;	/* Last cursor position */
	ptrdiff_t size;		/* Number of entries */
	struct rc_menu_entry **entries;
	MACRO *backs;		/* Macro to execute for backspace */
};

struct menu_instance {
	struct rc_menu *menu;
	char **s;
};

/* Create a menu */
struct rc_menu *create_menu(char *name, MACRO *bs);

/* Add entry to a menu */
void add_menu_entry(struct rc_menu *menu, char *entry_name, MACRO *m);

/* Menu command */
int umenu(W *w, int k);

extern int menu_flg;
