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
static char **smenus = NULL; /* Completion list */

/* Find a menu */

static struct rc_menu *find_menu(char *s)
{
	struct rc_menu *m;
	for (m = menus; m; m = m->next)
		if (!zcmp(m->name, s))
			break;
	return m;
}

/* Create a new menu, push it on menus list */

struct rc_menu *create_menu(char *name, MACRO *bs)
{
	struct rc_menu *menu = find_menu(name);
	if (menu)
		return menu;
	menu = (struct rc_menu *)joe_malloc(SIZEOF(struct rc_menu));
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

void add_menu_entry(struct rc_menu *menu, char *entry_name, MACRO *m)
{
	struct rc_menu_entry *e = (struct rc_menu_entry *)joe_malloc(SIZEOF(struct rc_menu_entry));
	e->m = m;
	e->name = zdup(entry_name);
	++menu->size;
	if (!menu->entries) {
		menu->entries = (struct rc_menu_entry **)joe_malloc(menu->size * SIZEOF(struct rc_menu_entry *));
	} else {
		menu->entries = (struct rc_menu_entry **)joe_realloc(menu->entries, menu->size * SIZEOF(struct rc_menu_entry *));
	}
	menu->entries[menu->size - 1] = e;
}

/* When user hits backspace */

static int backsmenu(MENU *m, ptrdiff_t x, void *obj)
{
	struct menu_instance *mi = (struct menu_instance *)obj;
	struct rc_menu *menu = mi->menu;
	int *notify = m->parent->notify;
	if (notify)
		*notify = 1;
	wabort(m->parent);
	if (menu->backs)
		return exmacro(menu->backs, 1, -1);
	else
		return 0;
}

static int doabrt(W *w, ptrdiff_t x, void *obj)
{
	struct menu_instance *mi = (struct menu_instance *)obj;
	mi->menu->last_position = x;
	for (x = 0; mi->s[x]; ++x)
		vsrm(mi->s[x]);
		/* joe_free(mi->s[x]); */
	joe_free(mi->s);
	joe_free(mi);
	return -1;
}

/* When user selects a menu entry */

int menu_flg; /* Record of key used to select menu entry */

static int execmenu(MENU *m, ptrdiff_t x, void *obj, int flg)
{
	struct menu_instance *mi = (struct menu_instance *)obj;
	struct rc_menu *menu = mi->menu;
	int *notify = m->parent->notify;
	if (notify)
		*notify = 1;
	wabort(m->parent);
	menu_flg = flg; /* Hack to pass key to umode command */
	return exmacro(menu->entries[x]->m, 1, -1);
}

/* Display macro menu if it exists */

static int display_menu(BW *bw, struct rc_menu *menu, int *notify)
{
	struct menu_instance *m = (struct menu_instance *)joe_malloc(SIZEOF(struct menu_instance));
	char **s = (char **)joe_malloc(SIZEOF(char *) * (menu->size + 1));
	int x;
	for (x = 0; x != menu->size; ++x) {
		s[x] = stagen(NULL, bw, menu->entries[x]->name, ' ');
	}
	s[x] = 0;
	m->menu = menu;
	m->s = s;
	if (mkmenu(bw->parent, bw->parent, m->s, execmenu, doabrt, backsmenu, menu->last_position, m, notify))
		return 0;
	else
		return -1;
}

/* Build array of menu name from list of menus for completion */

static char **getmenus(void)
{
	char **s = vaensure(NULL, 20);
	struct rc_menu *m;

	for (m = menus; m; m = m->next)
		s = vaadd(s, vsncpy(NULL, 0, sz(m->name)));
	vasort(s, aLen(s));
	return s;
}

/* When user hits tab */

static int menucmplt(BW *bw, int k)
{
	if (!smenus)
		smenus = getmenus();
	return simple_cmplt(bw,smenus);
}

/* When user selects a menu to bring up */

static int domenu(W *w, char *s, void *object, int *notify)
{
	BW *bw;
	struct rc_menu *menu;
	WIND_BW(bw, w);
	menu = find_menu(s);
	vsrm(s);
	if (!menu) {
		msgnw(w, joe_gettext(_("No such menu")));
		if (notify)
			*notify = 1;
		return -1;
	} else {
		bw->b->o.readonly = bw->o.readonly = bw->b->rdonly;
		return display_menu(bw, menu, notify);
	}
}

/* Menu of macros command: prompt for a menu to bring up */

int umenu(W *w, int k)
{
	if (wmkpw(w, joe_gettext(_("Menu: ")), &menuhist, domenu, "menu", NULL, menucmplt, NULL, NULL, locale_map, 0)) {
		return 0;
	} else {
		return -1;
	}
}
