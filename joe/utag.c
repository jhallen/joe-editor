/*
 *	tags file symbol lookup
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 * 	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

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

static void freetag(TAG *n)
{
	vsrm(n->key);
	vsrm(n->file);
	vsrm(n->srch);
	vsrm(n->cmnt);
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
	if (doswitch(bw, vsncpy(NULL, 0, sv(file)), NULL, NULL)) {
		return -1;
	}
	bw = (BW *)maint->curwin->object;
	p_goto_bof(bw->cursor);
	if (notagsmenu)
		if (last) {
			msgnw(bw->parent, joe_gettext(_("Last match")));
		} else {
			msgnw(bw->parent, joe_gettext(_("There are more matches")));
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
		return dopfnext(bw, mksrch(vsncpy(NULL, 0, sv(srch)), NULL, 0, 0, -1, 0, 0, 0), NULL);
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
	if (doswitch(bw, vsncpy(NULL, 0, sv(file)), NULL, NULL)) {
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
		return dopfnext(bw, mksrch(vsncpy(NULL, 0, sv(srch)), NULL, 0, 0, -1, 0, 0, 0), NULL);
	}
}

unsigned char **tag_array;

static int dotag(BW *bw, unsigned char *s, void *obj, int *notify)
{
	unsigned char buf[512];
	unsigned char buf1[512];
	FILE *f;
	unsigned char *prefix = NULL;
	unsigned char *t = NULL;
	struct tag *ta;

	if (notify) {
		*notify = 1;
	}
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
			prefix = dirprt(tagspath);
		}
		if(!f) {
			msgnw(bw->parent, joe_gettext(_("Couldn't open tags file")));
			vsrm(s);
			vsrm(t);
			return -1;
		}
	}
	clrtags();
	while (fgets((char *)buf, sizeof(buf), f)) {
		int x, y, c;
		for (x = 0; buf[x] && buf[x] != ' ' && buf[x] !='\t'; ++x) ;
		c = buf[x];
		buf[x] = 0;
		/* We look for these things:
		         string
		         buffer-name:string
		         .*::string */
		if (!zmcmp(buf, s) || (t && !zcmp(t, buf))) {
			unsigned char *key = vsncpy(NULL, 0, sz(buf));
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
						} else {
							vsrm(key);
							vsrm(file);
						}
					} else {
						int z = 0;
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
								buf1[z++] = '\\';
								buf1[z++] = '^';
								++y;
							}
							
							while (buf[y] && buf[y] != '\n' && !(buf[y] == ch && y == x)) {
								if (buf[y] == '$' && buf[y+1] == ch) {
									++y;
									buf1[z++] = '\\';
									buf1[z++] = '$';
								} else if (buf[y] == '\\' && buf[y+1]) {
									/* This is going to cause problem...
									   old ctags did not have escape */
									++y;
									if (buf[y] == '\\')
										buf1[z++] = '\\';
									buf1[z++] = buf[y++];
								} else {
									buf1[z++] = buf[y++];
								}
							}
						}
						if (z) {
							int q;
							unsigned char *srch;
							srch = vsncpy(NULL, 0, buf1, z);
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
						} else {
							vsrm(key);
							vsrm(file);
						}
					}
				} else {
					vsrm(key);
					vsrm(file);
				}
			} else {
				vsrm(key);
			}
		}
	}
	fclose(f);
	vsrm(prefix);
	if (!qempty(TAG, link, &tags)) {
		tags.link.prev->last = 1;
	}
	/* Build menu */
	varm(tag_array);
	tag_array = vamk(10);
	for (ta = tags.link.next; ta != &tags; ta=ta->link.next) {
		unsigned char buf[1024];
		if (ta->srch) {
			char *a;
			joe_snprintf_2(buf, sizeof(buf), "%s%s",ta->file,ta->srch /* ,(ta->cmnt ? ta->cmnt : USTR "") */);
			a = zstr(buf, "\\^");
			if (a) {
				a[0] = ':';
				a[1] = '"';
			}
			a = zstr(buf, "\\$");
			if (a) {
				a[0] = '"';
				a[1] = ' ';
			}
		} else
			joe_snprintf_2(buf, sizeof(buf), "%s:%ld",ta->file,ta->line /* ,(ta->cmnt ? ta->cmnt : USTR "") */);
		tag_array = vaadd(tag_array, vsncpy(NULL, 0, sz(buf)));
	}
	last_cursor = 0;
	/* Jump if only one result */
	if (notagsmenu || aLEN(tag_array) == 1)
		return dotagjump(bw, notagsmenu);
	if (mkmenu(bw->parent, bw->parent, tag_array, dotagmenu, NULL, NULL, 0, tag_array, NULL))
		return 0;
	else
		return -1;
}

static unsigned char **tag_word_list;
static time_t last_update;

static void get_tag_list()
{
	unsigned char buf[512];
	unsigned char tag[512];
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
		ht = htmk(256);
		varm(tag_word_list);
		tag_word_list = 0;
		while (fgets((char *)buf, sizeof(buf), f)) {
			pos = 0;
			for (i=0; i<sizeof(buf); i++) {
				if (buf[i] == ' ' || buf[i] == '\t') {
					pos = i;
					i = sizeof(buf);
				}
			}
			if (pos > 0) {
				zncpy(tag, buf, pos);
				tag[pos] = '\0';
				if (!htfind(ht, tag)) {
					unsigned char *s = vsncpy(NULL, 0, sz(tag));
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

static B *taghist = NULL;

int utag(BW *bw)
{
	BW *pbw;

	/* Repeat previous search */
	if (smode && !qempty(TAG, link, &tags)) {
		if (notagsmenu)
			return utagjump(bw);
		if (mkmenu(bw->parent, bw->parent, tag_array, dotagmenu, NULL, NULL, last_cursor, tag_array, NULL))
			return 0;
		else
			return -1;
	}

	pbw = wmkpw(bw->parent, joe_gettext(_("Tag search: ")), &taghist, dotag, NULL, NULL, tag_cmplt, NULL, NULL, locale_map, 0);
	if (pbw && joe_isalnum_(bw->b->o.charmap,brch(bw->cursor))) {
		P *p = pdup(bw->cursor, USTR "utag");
		P *q = pdup(p, USTR "utag");
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
		binsb(pbw->cursor, bcpy(p, q));
		pset(pbw->cursor, pbw->b->eof);
		pbw->cursor->xcol = piscol(pbw->cursor);
		prm(p);
		prm(q);
	}
	if (pbw) {
		return 0;
	} else {
		return -1;
	}
}
