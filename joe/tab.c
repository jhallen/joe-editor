/*
 *	File selection menu
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

typedef struct tab TAB;

int menu_explorer = 0;		/* Stay in menu system when directory selected */
int menu_jump = 0;		/* Jump into menu */

extern WATOM watommenu;

struct tab {
	ptrdiff_t first_len;			/* Original size of path */
	off_t ofst;			/* Starting offset to path */
	char *path;		/* current directory */
	char *pattern;		/* search pattern */
	ptrdiff_t len;		/* no. entries in files */
	char **files;		/* array of file names */
	char **list;
	char *type;		/* file type array */
	ino_t prv;
	char *orgpath;
	char *orgnam;
};

#define F_DIR		1	/* type codes for file type array */
#define F_NORMAL	2
#define F_EXEC		4

/* Read matching files from a directory
 *  Directory is given in tab.path
 *  Pattern is given in tab.pattern
 *
 * Returns with -1 if there was an error
 * Otherwise returns index to file with inode given in prv
 * len and files are set with the file names
 * type is set with the file types
 */

static int get_entries(TAB *tab, ino_t prv)
{
	int a;
	int which = 0;
	char *oldpwd = pwd();
	char **files;
	char *tmp;
	int users_flg = 0;

	tmp = vsncpy(NULL,0,sv(tab->path));
	tmp = canonical(tmp);

	if (chpwd(tmp)) {
		vsrm(tmp);
		return -1;
	}
	vsrm(tmp);
	if (!tab->path[0] && tab->pattern[0]=='~') {
		files = rexpnd_users(tab->pattern);
		users_flg = 1;
	} else
		files = rexpnd(tab->pattern);
	if (!files) {
		chpwd(oldpwd);
		return -1;
	}
	if (!aLEN(files)) {
		chpwd(oldpwd);
		return -1;
	}
	tab->len = aLEN(files);
	varm(tab->files);
	tab->files = files;
	vasort(files, tab->len);
	if (tab->type)
		joe_free(tab->type);
	tab->type = (char *)joe_malloc(tab->len);
	for (a = 0; a != tab->len; a++)
		if(users_flg) {
			tab->type[a] = F_DIR;
		} else {
			struct stat buf;
			mset((char *)&buf, 0, SIZEOF(struct stat));

			stat((files[a]), &buf);
			if (buf.st_ino == prv)
				which = a;
			if ((buf.st_mode & S_IFMT) == S_IFDIR)
				tab->type[a] = F_DIR;
			else if (buf.st_mode & (0100 | 0010 | 0001))
				tab->type[a] = F_EXEC;
			else
				tab->type[a] = F_NORMAL;
		}
	chpwd(oldpwd);
	return which;
}

static void insnam(BW *bw, char *path, char *nam, int dir, off_t ofst)
{
	P *p = pdup(bw->cursor, "insnam");

	pgoto(p, ofst);
	p_goto_eol(bw->cursor);
	bdel(p, bw->cursor);
	if (sLEN(path)) {
		binsm(bw->cursor, sv(path));
		p_goto_eol(bw->cursor);
		if (path[sLEN(path) - 1] != '/') {
			binsm(bw->cursor, sc("/"));
			p_goto_eol(bw->cursor);
		}
	}
	binsm(bw->cursor, sv(nam));
	p_goto_eol(bw->cursor);
	if (dir) {
		binsm(bw->cursor, sc("/"));
		p_goto_eol(bw->cursor);
	}
	prm(p);
	bw->cursor->xcol = piscol(bw->cursor);
}

/* Given a menu structure with a tab structure as its object,
 * a pattern and path set in the tab structure:
 *
 * Load the menu with a list of file names and set the file name in
 * the prompt window to the directory the menu was read in from.
 * If flg is set, treload attempts to position to the previous directory
 * which was visited.
 *
 * Returns with -1 if there was an error
 * Returns with 0 for success
 */

static char **treload(TAB *tab,MENU *m, BW *bw, int flg,int *defer)
{
	int x;
	int which;
	struct stat buf;

	if ((which = get_entries(tab, tab->prv)) < 0)
		return 0;
	if (tab->path && tab->path[0])
		stat(tab->path, &buf);
	else
		stat(".", &buf);
	tab->prv = buf.st_ino;
	if (!flg)
		which = 0;

	tab->list = vatrunc(tab->list, aLEN(tab->files));

	for (x = 0; tab->files[x]; ++x) {
		char *s = vsncpy(NULL, 0, sv(tab->files[x]));

		tab->list = vaset(tab->list, x, s);
		if (tab->type[x] == F_DIR)
			tab->list[x] = vsadd(tab->list[x], '/');
		else if (tab->type[x] == F_EXEC)
			tab->list[x] = vsadd(tab->list[x], '*');
	}
	if (defer) {
		*defer = which;
		/* bash */
		/* insnam(bw, tab->path, tab->pattern, 0, tab->ofst); */
		return tab->list;
	} else {
		ldmenu(m, tab->list, which);
		/* bash */
		/* insnam(bw, tab->path, tab->pattern, 0, tab->ofst); */
		return tab->list;
	}
}

static void rmtab(TAB *tab)
{
	vsrm(tab->orgpath);
	vsrm(tab->orgnam);
	varm(tab->list);
	vsrm(tab->path);
	vsrm(tab->pattern);
	varm(tab->files);
	if (tab->type)
		joe_free(tab->type);
	joe_free(tab);
}
/*****************************************************************************/
/****************** The user hit return **************************************/
/*****************************************************************************/
static int tabrtn(MENU *m, ptrdiff_t cursor, void *object, int op)
{
	TAB *tab = (TAB *)object;
	if (menu_explorer && tab->type[cursor] == F_DIR) {	/* Switch directories */
		char *orgpath = tab->path;
		char *orgpattern = tab->pattern;
		char *e = endprt(tab->path);

		/* if (!zcmp(tab->files[cursor], "..") && sLEN(e)
		    && !(e[0] == '.' && e[1] == '.' && (!e[2] || e[2] == '/')))
			tab->path = begprt(tab->path);
		else */ {
			tab->path = vsncpy(NULL, 0, sv(tab->path));
			tab->path = vsncpy(sv(tab->path), sv(m->list[cursor]));
		}
		vsrm(e);
		tab->pattern = vsncpy(NULL, 0, sc("*"));
		if (!treload((TAB *)m->object, m, (BW *)m->parent->win->object, 0, NULL)) {
			msgnw(m->parent, joe_gettext(_("Couldn't read directory ")));
			vsrm(tab->pattern);
			tab->pattern = orgpattern;
			vsrm(tab->path);
			tab->path = orgpath;
			return -1;
		} else {
			vsrm(orgpattern);
			vsrm(orgpath);
			return 0;
		}
	} else {		/* Select name */
		BW *bw = (BW *)m->parent->win->object;

		insnam(bw, tab->path, tab->files[cursor], (tab->type[cursor]==F_DIR), tab->ofst);
		rmtab(tab);
		m->object = NULL;
		m->abrt = NULL;
		wabort(m->parent);
		return 0;
	}
}

/* Like above, but treats directories as files (adds them to path instead of
 * traverse hierarchy) */

static int tabrtn1(MENU *m, int cursor, TAB *tab)
{
	/* New way: just add directory to path */
	BW *bw = (BW *)m->parent->win->object;

	insnam(bw, tab->path, tab->files[cursor], (tab->type[cursor]==F_DIR ? 1 : 0), tab->ofst);
	rmtab(tab);
	m->object = NULL;
	m->abrt = NULL;
	wabort(m->parent);
	return 0;
}


/*****************************************************************************/
/****************** The user hit backspace ***********************************/
/*****************************************************************************/
static int tabbacks(MENU *m, ptrdiff_t cursor, void *object)
{
	TAB *tab = (TAB *)object;
	char *orgpath = tab->path;
	char *orgpattern = tab->pattern;
	char *e = endprt(tab->path);

	if (sLEN(e) && sLEN(tab->path)!=tab->first_len)
		tab->path = begprt(tab->path);
	else {
		wabort(m->parent);
		return 0;
	}
	vsrm(e);
	tab->pattern = vsncpy(NULL, 0, sc("*"));

	if (!treload((TAB *)m->object, m, (BW *)m->parent->win->object, 1, NULL)) {
		msgnw(m->parent, joe_gettext(_("Couldn't read directory ")));
		vsrm(tab->pattern);
		tab->pattern = orgpattern;
		vsrm(tab->path);
		tab->path = orgpath;
		return -1;
	} else {
		vsrm(orgpattern);
		vsrm(orgpath);
		return 0;
	}
}
/*****************************************************************************/
/* This should verify that bw still exists... */
static int tababrt(W *w, ptrdiff_t cursor, void *object)
{
	TAB *tab = (TAB *)object;
	/* bash */
	/* insnam(bw, tab->orgpath, tab->orgnam, 0, tab->ofst); */
	rmtab(tab);
	return -1;
}

static P *p_goto_start_of_path(P *p)
{
	int c;
	
	do
		c = prgetc(p);
	while (c != NO_MORE_DATA && c != ' ' && c != '\n');
	
	if (c == ' ') {
		P *q = pdup(p, "p_goto_start_of_path");
		
		do
			c = prgetc(q);
		while (c != NO_MORE_DATA && c != '\n');
		
		if (c != NO_MORE_DATA)
			pgetc(q);
		
		if (brch(q) == '!') {
			/* If piping from a command, then expand path starting past space */
			prm(q);
			pgetc(p);
			return p;
		} else {
			/* Otherwise, go to front of line. */
			pset(p, q);
			prm(q);
			return p;
		}
	}

	if (c != NO_MORE_DATA)
		pgetc(p);

	return p;
}

/*****************************************************************************/
/****************** Create a tab window **************************************/
/*****************************************************************************/

int cmplt(BW *bw, int k)
{
	MENU *newmenu;
	TAB *tab;
	P *p, *q;
	char *cline;
	int which;
	char **l;
	off_t ofst;

	tab = (TAB *) joe_malloc(SIZEOF(TAB));
	tab->files = NULL;
	tab->type = NULL;
	tab->list = NULL;
	tab->prv = 0;
	tab->len = 0;

	q = pdup(bw->cursor, "cmplt");
	p_goto_eol(q);
	p = pdup(q, "cmplt");
	p_goto_start_of_path(p);
	ofst = p->byte;

	cline = brvs(p, q->byte - p->byte); /* Risky */
	/* Don't do it so soon... */
	/* cline = canonical(cline); */
	prm(p);
	prm(q);

	tab->ofst = ofst;
	tab->pattern = namprt(cline);
	tab->path = dirprt(cline);
	tab->first_len = sLEN(tab->path);
	tab->orgnam = vsncpy(NULL, 0, sv(tab->pattern));
	tab->orgpath = vsncpy(NULL, 0, sv(tab->path));
	tab->pattern = vsadd(tab->pattern, '*');
	vsrm(cline);

	l = treload(tab, 0, bw, 0, &which);

	/* bash */
	/* TRY */
	if (menu_above) {
		if (bw->parent->link.prev->watom==&watommenu) {
			wabort(bw->parent->link.prev);
			/* smode=2; */
		}
	} else {
		if (bw->parent->link.next->watom==&watommenu) {
			wabort(bw->parent->link.next);
			/* smode=2; */
		}
	}

	if (l && (newmenu = mkmenu((menu_above ? bw->parent->link.prev : bw->parent), bw->parent, l, tabrtn, tababrt, tabbacks, which, tab, NULL))) {
		if (sLEN(tab->files) == 1)
			/* Only one file found, so select it */
			return tabrtn1(newmenu, 0, tab);
		else if (smode || isreg(tab->orgnam)) {
			/* User tried to complete twice (see smode=2 below), so leave menu on */
			/* bash */
			if (!menu_jump)
				bw->parent->t->curwin=bw->parent;
			return 0;
		} else {
			/* Complete name as much as possible, turn menu off */
			char *com = mcomplete(newmenu);

			vsrm(tab->orgnam);
			tab->orgnam = com;
			/* wabort causes tab->orgnam to be copied to prompt */
			insnam(bw, tab->orgpath, tab->orgnam, 0, tab->ofst);
			wabort(newmenu->parent);
			smode = 2;
			/* if(joe_beep) */
				ttputc(7);
			return 0;
		}
	} else {
		/* if(joe_beep) */
			ttputc(7);
		rmtab(tab);
		return -1;
	}
}
