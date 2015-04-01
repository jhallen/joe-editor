/*
 *	tags file symbol lookup
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 * 	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* Forward */

static unsigned char **tag_word_list;
static time_t last_update;

static void get_tag_list();
static int tag_cmplt(BW *bw);

/* Tag database */

int notagsmenu = 0;

typedef struct tag TAG;
static struct tag {
	LINK(TAG) link;
	unsigned char *key;	/* Key */
	unsigned char *file;	/* Target file */
	unsigned char *srch;	/* Target search pattern */
	long line;		/* Target line number */
	unsigned char *cmnt;	/* Comment */
	int last;		/* Set on last record */
} tags = { { &tags, &tags } };

static TAG tagnodes = { { &tagnodes, &tagnodes } };

static B *taghist = NULL;

static void freetag(TAG *n)
{
	obj_free(n->key);
	obj_free(n->file);
	obj_free(n->srch);
	obj_free(n->cmnt);
	enquef(TAG, link, &tagnodes, n);
}

static void clrtags()
{
	while (!qempty(TAG, link, &tags)) {
		freetag(deque_f(TAG, link, tags.link.next));
	}
}

static void addtag(unsigned char *key, unsigned char *file, unsigned char *srch, long line, unsigned char *cmnt)
{
	TAG *n = (TAG *)alitem(&tagnodes, sizeof(TAG));
	n->key = key;
	n->file = file;
	n->srch = srch;
	n->line = line;
	n->cmnt = cmnt;
	n->last = 0;
	enqueb(TAG, link, &tags, n);
	
	obj_perm(key);
	obj_perm(file);
	obj_perm(srch);
	obj_perm(cmnt);
}

static int dotagjump(BW *bw, int flag)
{
	unsigned char *file;
	unsigned char *srch;
	long line;
	int last;
	if (qempty(TAG, link, &tags)) {
		msgnw(bw->parent, joe_gettext(_("Not found")));
		return -1;
	}
	file = tags.link.next->file;
	srch = tags.link.next->srch;
	line = tags.link.next->line;
	last = tags.link.next->last;
	demote(TAG,link,&tags,tags.link.next);
	if (doswitch(bw, vsncpy(NULL, 0, sv(file)))) {
		return -1;
	}
	bw = (BW *)maint->curwin->object;
	p_goto_bof(bw->cursor);
	if (notagsmenu) {
		if (last) {
			msgnw(bw->parent, joe_gettext(_("Last match")));
		} else {
			msgnw(bw->parent, joe_gettext(_("There are more matches")));
		}
	}
	if (!srch) {
		int omid = mid;
		mid = 1;
		pline(bw->cursor, line - 1);
		bw->cursor->xcol = piscol(bw->cursor);
		dofollows();
		mid = omid;
		if (flag)
			smode = 2;
		return 0;
	} else {
		if (flag)
			smode = 2;
		return dopfnext(bw, mksrch(vsncpy(NULL, 0, sv(srch)), NULL, 0, 0, -1, 0, 0, 0));
	}
}

int utagjump(BW *bw)
{
	return dotagjump(bw, 1);
}

static int last_cursor;

static int dotagmenu(MENU *m, int x, unsigned char **s)
{
	unsigned char *file;
	unsigned char *srch;
	long line;
	struct tag *t = tags.link.next;
	BW *bw = m->parent->win->object;
	last_cursor = x;
	if (t == &tags)
		return -1;
	while (x--) {
		t = t->link.next;
		if (t == &tags)
			return -1;
	}
	file = t->file;
	srch = t->srch;
	line = t->line;
	wabort(m->parent);
	if (doswitch(bw, vsncpy(NULL, 0, sv(file)))) {
		return -1;
	}
	bw = (BW *)maint->curwin->object;
	p_goto_bof(bw->cursor);
	if (!srch) {
		int omid = mid;
		mid = 1;
		pline(bw->cursor, line - 1);
		bw->cursor->xcol = piscol(bw->cursor);
		dofollows();
		mid = omid;
		smode = 2;
		return 0;
	} else {
		smode = 2;
		return dopfnext(bw, mksrch(vsncpy(NULL, 0, sv(srch)), NULL, 0, 0, -1, 0, 0, 0));
	}
}

unsigned char *get_word_under_cursor(BW *bw)
{
	P *p = pdup(bw->cursor, USTR "get_word_under_cursor");
	P *q = pdup(p, USTR "get_word_under_cursor");
	unsigned char *s;
	int c;

	while (joe_isalnum_(bw->b->o.charmap,(c = prgetc(p))))
		/* do nothing */;
	if (c != NO_MORE_DATA) {
		pgetc(p);
	}
	pset(q, p);
	while (joe_isalnum_(bw->b->o.charmap,(c = pgetc(q))))
		/* do nothing */;
	if (c != NO_MORE_DATA) {
		prgetc(q);
	}
	s = brvs(NULL, p, q->byte - p->byte);
	prm(p);
	prm(q);
	return s;
}

unsigned char **tag_array;

int utag(BW *bw)
{
	unsigned char *buf;
	FILE *f;
	unsigned char *prefix = NULL;
	unsigned char *t = NULL;
	struct tag *ta;
	unsigned char *s;

	/* Repeat previous search */
	if (smode && !qempty(TAG, link, &tags)) {
		if (notagsmenu)
			return utagjump(bw);
		if (mkmenu(bw->parent, bw->parent, tag_array, dotagmenu, NULL, NULL, last_cursor, tag_array)) // TODO
			return 0;
		else
			return -1;
	}

	s = get_word_under_cursor(bw);
	s = ask(bw->parent, joe_gettext(_("Tag search: ")), &taghist, NULL, tag_cmplt, locale_map, 0, 0, s);
	
	if (!s)
		return -1;
	
	if (bw->b->name) {
		t = vsncpy(t, 0, sz(bw->b->name));
		t = vsncpy(sv(t), sc(":"));
		t = vsncpy(sv(t), sv(s));
	}
	/* first try to open the tags file in the current directory */
	f = fopen("tags", "r");
	if (!f) {
		/* if there's no tags file in the current dir, then query
		   for the environment variable TAGS.
		*/
		char *tagspath = getenv("TAGS");
		if(tagspath) {
			f = fopen(tagspath, "r");
			prefix = dirprt(USTR tagspath);
		}
		if(!f) {
			msgnw(bw->parent, joe_gettext(_("Couldn't open tags file")));
			return -1;
		}
	}
	clrtags();
	buf = vsmk(128);
	while (vsgets(&buf, f)) {
		int x, y, c;
		
		for (x = 0; buf[x] && buf[x] != ' ' && buf[x] !='\t'; ++x) ;
		c = buf[x];
		buf[x] = 0;
		/* We look for these things:
		         string
		         buffer-name:string
		         .*::string */
		if (!zmcmp(buf, s) || (t && !zcmp(t, buf))) {
			unsigned char *key = vsdup(buf);
			buf[x] = c;
			while (buf[x] == ' ' || buf[x] == '\t') {
				++x;
			}
			for (y = x; buf[y] && buf[y] != ' ' && buf[y] != '\t' && buf[y] != '\n'; ++y) ;
			if (x != y) {
				unsigned char *file = 0;
				c = buf[y];
				buf[y] = 0;
				if (prefix)
					file = vsncpy(NULL, 0, sv(prefix));
				file = vsncpy(sv(file), sz(buf + x));
				buf[y] = c;
				while (buf[y] == ' ' || buf[y] == '\t') {
					++y;
				}
				for (x = y; buf[x] && buf[x] != '\n'; ++x) ;
				buf[x] = 0;
				if (x != y) {
					if (buf[y] >= '0' && buf[y] <= '9')  {
						long line = 1;
						/* It's a line number */
						sscanf((char *)(buf + y), "%ld", &line);
						if (line >= 1) {
							/* Comment */
							int q;
							while (buf[y] >= '0' && buf[y] <= '9')
								++y;
							/* Comment (skip vi junk) */
							while (buf[y] == ' ' || buf[y] == '\t' ||
							       buf[y] == ';' || buf[y] == '"' ||
							       (buf[y] && (buf[y + 1] == ' ' ||
							                   buf[y + 1] == '\t' ||
							                   buf[y + 1] == '\r' ||
							                   buf[y + 1] == '\n' ||
							                   !buf[y + 1])))
								++y;
							q = y + zlen(buf + y);
							if (q > y && buf[q - 1] == '\n') {
								buf[q - 1] = 0;
								--q;
								if (q > y && buf[q - 1] == '\r') {
									buf[q - 1] = 0;
									--q;
								}
							}
							addtag(key, file, NULL, line, q > y ? vsncpy(NULL, 0, buf + y, q - y) : 0);
						}
					} else {
						unsigned char *buf1 = NULL;
						/* It's a search string. New versions of
						   ctags have real regex with vi command.  Old
						   ones do not always quote / and depend on it
						   being last char on line. */
						if (buf[y] == '/' || buf[y] == '?') {
							int ch = buf[y++];
							/* Find last / or ? on line... */
							for (x = y + zlen(buf + y); x != y; --x)
								if (buf[x] == ch)
									break;
							/* Copy characters, convert to JOE regex... */
							if (buf[y] == '^') {
								buf1 = vscat(buf1, sc("\\^"));
								++y;
							}
							
							while (buf[y] && buf[y] != '\n' && !(buf[y] == ch && y == x)) {
								if (buf[y] == '$' && buf[y+1] == ch) {
									++y;
									buf1 = vscat(buf1, sc("\\$"));
								} else if (buf[y] == '\\' && buf[y+1]) {
									/* This is going to cause problem...
									   old ctags did not have escape */
									++y;
									if (buf[y] == '\\')
										buf1 = vscat(buf1, sc("\\"));
									buf1 = vsadd(buf1, buf[y++]);
								} else {
									buf1 = vsadd(buf1, buf[y++]);
								}
							}
						}
						if (buf1 && vslen(buf1)) {
							int q;
							unsigned char *srch = buf1;
							if (buf[y]) ++y;
							/* Comment (skip vi junk) */
							while (buf[y] == ' ' || buf[y] == '\t' ||
							       buf[y] == ';' || buf[y] == '"' ||
							       (buf[y] && (buf[y + 1] == ' ' ||
							                   buf[y + 1] == '\t' ||
							                   buf[y + 1] == '\r' ||
							                   buf[y + 1] == '\n' ||
							                   !buf[y + 1])))
								++y;
							q = y + zlen(buf + y);
							if (q > y && buf[q - 1] == '\n') {
								buf[q - 1] = 0;
								--q;
								if (q > y && buf[q - 1] == '\r') {
									buf[q - 1] = 0;
									--q;
								}
							}
							addtag(key, file, srch, 1, q > y ? vsncpy(NULL, 0, buf + y, q - y) : 0);
						}
					}
				}
			}
		}
	}
	fclose(f);
	obj_free(buf);
	if (!qempty(TAG, link, &tags)) {
		tags.link.prev->last = 1;
	}
	/* Build menu */
	varm(tag_array);
	tag_array = vamk(10);
	vaperm(tag_array);
	for (ta = tags.link.next; ta != &tags; ta=ta->link.next) {
		unsigned char *buf = NULL;
		if (ta->srch) {
			unsigned char *a;
			buf = vsfmt(buf, 0, USTR "%s%s", ta->file, ta->srch);
			a = zstr(buf, USTR "\\^");
			if (a) {
				a[0] = ':';
				a[1] = '"';
			}
			a = zstr(buf, USTR "\\$");
			if (a) {
				a[0] = '"';
				a[1] = ' ';
			}
		} else
			buf = vsfmt(buf, 0, USTR "%s:%ld",ta->file,ta->line /* ,(ta->cmnt ? ta->cmnt : USTR "") */);
		tag_array = vaadd(tag_array, buf);
	}
	last_cursor = 0;
	/* Jump if only one result */
	if (notagsmenu || valen(tag_array) == 1)
		return dotagjump(bw, notagsmenu);
	if (mkmenu(bw->parent, bw->parent, tag_array, dotagmenu, NULL, NULL, 0, tag_array))
		return 0;
	else
		return -1;
}

static void get_tag_list()
{
	unsigned char *buf = NULL, *tag = NULL;
	int i,pos;
	FILE *f;
	HASH *ht; /* Used to prevent duplicates in list */
	struct stat stat;

	/* first try to open the tags file in the current directory */
	f = fopen("tags", "r");
	if (!f) {
		/* if there's no tags file in the current dir, then query
		   for the environment variable TAGS.
		*/
		char *tagspath = getenv("TAGS");
		if(tagspath) {
			f = fopen(tagspath, "r");    
		}
	}
	if (f) {
		if (!fstat(fileno(f), &stat)) {
			if (last_update == stat.st_mtime) {
				fclose(f);
				return;
			} else {
				last_update = stat.st_mtime;
			}
		}
		buf = vsmk(128);
		ht = htmk(256);
		varm(tag_word_list);
		tag_word_list = vamk(16);
		vaperm(tag_word_list);
		while (vsgets(&buf, f)) {
			pos = 0;
			for (i=0; i<vslen(buf); i++) {
				if (buf[i] == ' ' || buf[i] == '\t') {
					pos = i;
					i = vslen(buf);
				}
			}
			if (pos > 0) {
				tag = vsncpy(NULL, 0, buf, pos);
				if (!htfind(ht, tag)) {
					unsigned char *s = vsdup(tag);
					/* Add class::member */
					htadd(ht, s, s);
					tag_word_list = vaadd(tag_word_list, s);
					i = zlen(tag);
					do {
						if (tag[i] == ':' && tag[i + 1] == ':') {
							/* Add ::member */
							if (!htfind(ht, tag + i)) {
								s = vsncpy(NULL, 0, sz(tag + i));
								htadd(ht, s, s);
								tag_word_list = vaadd(tag_word_list, s);
							}
							/* Add member */
							if (!htfind(ht, tag + i + 2)) {
								s = vsncpy(NULL, 0, sz(tag + i + 2));
								htadd(ht, s, s);
								tag_word_list = vaadd(tag_word_list, s);
							}
						}
					} while(i--);
				}
			}
		}
		fclose(f);
		htrm(ht);
		obj_free(buf);
	}
}

static int tag_cmplt(BW *bw)
{
	get_tag_list();

	if (!tag_word_list) {
		ttputc(7);
		return 0;
	}

	return simple_cmplt(bw,tag_word_list);
}
