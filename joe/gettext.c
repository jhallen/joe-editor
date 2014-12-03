/* JOE's gettext() library.  Why?  Once again we can not rely on the
 * system's or GNU's gettext being installed properly */

/* One modification from standard gettext: comments are allowed at
 * the start of strings: "|comment|comment|comment|foo".  The leading
 * '|' is required to indicate the comment.
 *
 * Comments can be used to make two otherwise identical strings distinct.
 */

#include "types.h"

HASH *gettext_ht;

unsigned char *ignore_prefix(unsigned char *set)
{
	unsigned char *s = zrchr(set, '|');
	if (s)
		++s;
	else
		s = set;
	return s;
}

unsigned char *my_gettext(unsigned char *s)
{
	if (gettext_ht) {
		unsigned char *r = (unsigned char *)htfind(gettext_ht, s);
		if (r)
			s = r;
	}
	if (s[0] == '|')
		return ignore_prefix(s);
	else
		return s;
}

int load_po(JFILE *f)
{
	unsigned char *buf = 0;
	unsigned char *msgid = vsdupz(USTR "");
	unsigned char *msgstr = vsdupz(USTR "");
	struct charmap *po_map = locale_map;
	int preload_flag = 0;

	while (preload_flag || jfgets(isfree(&buf) ,f)) {
		unsigned char *p;
		preload_flag = 0;
		p = buf;
		parse_ws(&p, '#');
		if (!parse_field(&p, USTR "msgid")) {
			int ofst = 0;
			int len;
			unsigned char *bf = 0;
			msgid = vscpyz(msgid, USTR "");
			parse_ws(&p, '#');
			while ((len = parse_string(&p, &bf)) >= 0) {
				msgid = vscat(msgid, sv(bf));
				preload_flag = 0;
				ofst += len;
				parse_ws(&p, '#');
				if (!*p) {
					if (jfgets(&buf,f)) {
						p = buf;
						preload_flag = 1;
						parse_ws(&p, '#');
					} else {
						goto bye;
					}
				}
			}
		} else if (!parse_field(&p, USTR "msgstr")) {
			int ofst = 0;
			int len;
			unsigned char *bf = 0;
			msgstr = vscpyz(msgstr, USTR "");
			parse_ws(&p, '#');
			while ((len = parse_string(&p, &bf)) >= 0) {
				msgstr = vscat(msgstr, sv(bf));
				preload_flag = 0;
				ofst += len;
				parse_ws(&p, '#');
				if (!*p) {
					if (jfgets(&buf,f)) {
						p = buf;
						preload_flag = 1;
						parse_ws(&p, '#');
					} else {
						break;
					}
				}
			}
			if (msgid[0] && msgstr[0]) {
				/* Convert to locale character map */
				unsigned char *bf = my_iconv(NULL,locale_map,msgstr,po_map);
				/* Add to hash table */
				htadd(gettext_ht, zdup(msgid), zdup(bf));
			} else if (!msgid[0] && msgstr[0]) {
				unsigned char *p = (unsigned char *)strstr((char *)msgstr, "charset=");
				msgid = vscpyz(msgid, msgstr); /* Make sure msgid is long enough */
				msgid = vscpyz(msgid, USTR ""); /* Truncate it */
				if (p) {
					/* Copy character set name up to next delimiter */
					int x;
					p += sizeof("charset=") - 1;
					while (*p == ' ' || *p == '\t') ++p;
					for (x = 0; p[x] && p[x] !='\n' && p[x] != '\r' && p[x] != ' ' &&
					            p[x] != '\t' && p[x] != ';' && p[x] != ','; ++x)
					            msgid[x] = p[x];
					msgid[x] = 0;
					po_map = find_charmap(msgid);
					if (!po_map)
						po_map = locale_map;
				}
			}
		}
	}
	bye:
	jfclose(f);
	obj_free(msgid);
	return 0;
}

/* Initialize my_gettext().  Call after locale_map has been set. */

static JFILE *find_po(unsigned char *s)
{
	unsigned char *buf = NULL;
	JFILE *f = NULL;

	buf = vsfmt(buf, 0, USTR "%slang/%s.po", JOEDATA, s);
	if ((f = jfopen(buf, "r"))) goto found;

	if (s[0] && s[1]) {
		buf = vsfmt(buf, 0, USTR "%slang/%c%c.po", JOEDATA, s[0], s[1]);
		if ((f = jfopen(buf, "r"))) goto found;
	}

	buf = vsfmt(buf, 0, USTR "*%s.po", s);
	if ((f = jfopen(buf, "r"))) goto found;

	if (s[0] && s[1]) {
		buf = vsfmt(buf, 0, USTR "*%c%c.po", s[0], s[1]);
		if ((f = jfopen(buf, "r"))) goto found;
	}

found:
	obj_free(buf);
	return f;
}

void init_gettext(unsigned char *s)
{
	JFILE *f = find_po(s);

	if (f) {
		gettext_ht = htmk(256);
		load_po(f);
	}
}
