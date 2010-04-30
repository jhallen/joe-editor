/*
 *	tags file symbol lookup
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 * 	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

static unsigned char **get_tag_list()
{
	unsigned char buf[512];
	unsigned char tag[512];
	int i,pos;
	FILE *f;
	unsigned char **lst = NULL;
	
	f = fopen("tags", "r");
	if (f) {
		while (fgets((char *)buf, 512, f)) {
			pos = 0;
			for (i=0; i<512; i++) {
				if (buf[i] == ' ' || buf[i] == '\t') {
					pos = i;
					i = 512;
				}
			}
			if (pos > 0) {
				zncpy(tag, buf, pos);
				tag[pos] = '\0';
			}
			lst = vaadd(lst, vsncpy(NULL, 0, sz(tag)));
		}
		fclose(f);	
	}
	return lst;
}

static unsigned char **tag_word_list;

static int tag_cmplt(BW *bw)
{
	/* Reload every time: we should really check date of tags file...
	if (tag_word_list)
		varm(tag_word_list); */

	if (!tag_word_list) {
		tag_word_list = get_tag_list();
		vaperm(tag_word_list);
	}

	if (!tag_word_list) {
		ttputc(7);
		return 0;
	}

	return simple_cmplt(bw,tag_word_list);
}

static B *taghist = NULL;

unsigned char *get_word_under_cursor(BW *bw)
{
	P *p = pdup(bw->cursor, USTR "utag");
	P *q = pdup(p, USTR "utag");
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

int utag(BW *bw)
{
	unsigned char *s = get_word_under_cursor(bw);

	s = ask(bw->parent, joe_gettext(_("Tag search: ")), &taghist, NULL, tag_cmplt, locale_map, 0, 0, s);

	if (s) {
		unsigned char buf[512];
		unsigned char buf1[512];
		FILE *f;
		unsigned char *t = NULL;

		if (bw->b->name) {
			t = vsncpy(t, 0, sz(bw->b->name));
			t = vsncpy(sv(t), sc(":"));
			t = vsncpy(sv(t), sv(s));
		}
		f = fopen("tags", "r");
		if (!f) {
			char *tagspath = getenv("TAGS");
			if (tagspath)
				f = fopen(tagspath, "r");
			if (!f) {
				msgnw(bw->parent, joe_gettext(_("Couldn't open tags file")));
				return -1;
			}
		}
		while (fgets((char *)buf, 512, f)) {
			int x, y, c, z;

			for (x = 0; buf[x] && buf[x] != ' ' && buf[x] != '\t'; ++x) ;
			c = buf[x];
			buf[x] = 0;
			if (!zcmp(s, buf) || (t && !zcmp(t, buf))) {
				buf[x] = c;
				while (buf[x] == ' ' || buf[x] == '\t') {
					++x;
				}
				for (y = x; buf[y] && buf[y] != ' ' && buf[y] != '\t' && buf[y] != '\n'; ++y) ;
				if (x != y) {
					c = buf[y];
					buf[y] = 0;
					if (doswitch(bw, vsncpy(NULL, 0, sz(buf + x)))) {
						fclose(f);
						return -1;
					}
					bw = (BW *) maint->curwin->object;
					p_goto_bof(bw->cursor);
					buf[y] = c;
					while (buf[y] == ' ' || buf[y] == '\t') {
						++y;
					}
					for (x = y; buf[x] && buf[x] != '\n'; ++x) ;
					buf[x] = 0;
					if (x != y) {
						long line = 0;
						if (buf[y] >= '0' && buf[y] <= '9') {
							/* It's a line number */
							sscanf((char *)(buf + y), "%ld", &line);
							if (line >= 1) {
								int omid = mid;

								mid = 1;
								pline(bw->cursor, line - 1), bw->cursor->xcol = piscol(bw->cursor);
								dofollows();
								mid = omid;
							} else {
								msgnw(bw->parent, joe_gettext(_("Invalid line number")));
							}
						} else {
							z = 0;
							/* It's a search string.  New versions of
							   ctags have real regex with vi command.  Old
							   ones do not always quote / and depend on it
							   being last char on line. */
							if (buf[y] == '/' || buf[y] == '?') {
								int ch = buf[y++];
								/* Find terminating / or ? */
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
								fclose(f);
								buf1[z] = 0;
								return dopfnext(bw, mksrch(vsncpy(NULL, 0, sz(buf1)), NULL, 0, 0, -1, 0, 0, 0));
							}
						}
					}
					fclose(f);
					return 0;
				}
			}
		}
		msgnw(bw->parent, joe_gettext(_("Not found")));
		fclose(f);
		return -1;
	} else {
		return -1;
	}
}
