/*
 *	Menu of macros
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

struct rc_menu_entry {
	MACRO *m;
	unsigned char *name;
};

struct rc_menu {
	struct rc_menu *next;	/* Next one in list */
	unsigned char *name;	/* Name of this menu */
	int last_position;	/* Last cursor position */
	int size;		/* Number of entries */
	struct rc_menu_entry **entries;
	MACRO *backs;		/* Macro to execute for backspace */
};

struct menu_instance {
	struct rc_menu *menu;
	unsigned char **s;
};

/* Create a menu */
struct rc_menu *create_menu(unsigned char *name, MACRO *bs);

/* Add entry to a menu */
void add_menu_entry(struct rc_menu *menu, unsigned char *entry_name, MACRO *m);

/* Menu command */
int umenu(BW *bw);

extern int menu_flg;
