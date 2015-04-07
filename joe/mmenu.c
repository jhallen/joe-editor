/*
 *	Menu of macros
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

#include "types.h"

static B *menuhist = NULL; /* Menu history */
static struct rc_menu *menus; /* All menus */
static unsigned char **smenus = NULL; /* Completion list */

/* Find a menu */

static struct rc_menu *find_menu(unsigned char *s)
{
	struct rc_menu *m;
	for (m = menus; m; m = m->next)
		if (!zcmp(m->name, s))
			break;
	return m;
}

/* Create a new menu, push it on menus list */

struct rc_menu *create_menu(unsigned char *name, MACRO *bs)
{
	struct rc_menu *menu = find_menu(name);
	if (menu)
		return menu;
	menu = (struct rc_menu *)joe_malloc(sizeof(struct rc_menu));
	menu->name = zdup(name);
	menu->next = menus;
	menus = menu;
	menu->last_position = 0;
	menu->size = 0;
	menu->entries = 0;
	menu->backs = bs;
	return menu;
}

/* Add an entry to a menu */

void add_menu_entry(struct rc_menu *menu, unsigned char *entry_name, MACRO *m)
{
	struct rc_menu_entry *e = (struct rc_menu_entry *)joe_malloc(sizeof(struct rc_menu_entry));
	e->m = m;
	e->name = zdup(entry_name);
	++menu->size;
	if (!menu->entries) {
		menu->entries = (struct rc_menu_entry **)joe_malloc(menu->size * sizeof(struct rc_menu_entry *));
	} else {
		menu->entries = (struct rc_menu_entry **)joe_realloc(menu->entries, menu->size * sizeof(struct rc_menu_entry *));
	}
	menu->entries[menu->size - 1] = e;
}

int menu_flg; /* Record of key used to select menu entry */

/* Display macro menu if it exists */

int display_menu(BW *bw, struct rc_menu *menu)
{
	unsigned char **s = vamk(20);
	int x;
	
	for (x = 0; x != menu->size; ++x) {
		s = vaadd(s, stagen(NULL, bw, menu->entries[x]->name, ' '));
	}
	
	x = choose(bw->parent, bw->parent, s, &menu->last_position);
	if (x == -1) {
		/* Abort */
		return -1;
	} else if (x == 3) {
		/* Backspace */
		if (menu->backs)
			return exmacro(menu->backs, 1);
		else
			return 0;
	}
	
	menu_flg = x;
	return exmacro(menu->entries[menu->last_position]->m, 1);
}

/* Build array of menu name from list of menus for completion */

unsigned char **getmenus(void)
{
	unsigned char **s = vaensure(NULL, 20);
	struct rc_menu *m;
	vaperm(s);

	for (m = menus; m; m = m->next)
		s = vaadd(s, vsncpy(NULL, 0, sz(m->name)));
	vasort(av(s));
	return s;
}

/* When user hits tab */

static int menucmplt(BW *bw)
{
	if (!smenus)
		smenus = getmenus();
	return simple_cmplt(bw,smenus);
}

/* Menu of macros command: prompt for a menu to bring up */

int umenu(BW *bw)
{
	unsigned char *s = ask(bw->parent, joe_gettext(USTR _("Menu: ")), &menuhist, USTR "menu", menucmplt, locale_map, 0, 0, NULL);
	struct rc_menu *menu;
	
	if (!s)
		return -1;
	
	menu = find_menu(s);
	if (!menu) {
		msgnw(bw->parent, joe_gettext(_("No such menu")));
		return -1;
	} else {
		bw->b->o.readonly = bw->o.readonly = bw->b->rdonly;
		return display_menu(bw, menu);
	}
}
