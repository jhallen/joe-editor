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
		unsigned char *r = htfind(gettext_ht, s);
		if (r)
			s = r;
	}
	if (s[0] == '|')
		return ignore_prefix(s);
	else
		return s;
}

/* Load a .po file, convert entries to local character set and add them to
 * hash table */

int load_po(FILE *f)
{
	unsigned char buf[1024];
	unsigned char msgid[1024];
	unsigned char msgstr[1024];
	unsigned char bf[8192];
	struct charmap *po_map = locale_map;
	int preload_flag = 0;
	msgid[0] = 0;
	msgstr[0] = 0;
	while (preload_flag || fgets((char *)buf,sizeof(buf)-1,f)) {
		unsigned char *p;
		preload_flag = 0;
		p = buf;
		parse_ws(&p, '#');
		if (!parse_field(&p, USTR "msgid")) {
			int ofst = 0;
			int len;
			msgid[0] = 0;
			parse_ws(&p, '#');
			while ((len = parse_string(&p, msgid + ofst, sizeof(msgid)-ofst)) >= 0) {
				preload_flag = 0;
				ofst += len;
				parse_ws(&p, '#');
				if (!*p) {
					if (fgets((char *)buf,sizeof(buf) - 1,f)) {
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
			msgstr[0] = 0;
			parse_ws(&p, '#');
			while ((len = parse_string(&p, msgstr + ofst, sizeof(msgstr)-ofst)) >= 0) {
				preload_flag = 0;
				ofst += len;
				parse_ws(&p, '#');
				if (!*p) {
					if (fgets((char *)buf,sizeof(buf) - 1,f)) {
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
				my_iconv(bf, sizeof(bf), locale_map,msgstr,po_map);
				/* Add to hash table */
				htadd(gettext_ht, zdup(msgid), zdup(bf));
			} else if (!msgid[0] && msgstr[0]) {
				unsigned char *p = (unsigned char *)strstr((char *)msgstr, "charset=");
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
	fclose(f);
	return 0;
}

/* Initialize my_gettext().  Call after locale_map has been set. */

void init_gettext(unsigned char *s)
{
	FILE *f;
	unsigned char buf[1024];
	joe_snprintf_2(buf, sizeof(buf), "%slang/%s.po",JOEDATA,s);
	if ((f = fopen((char *)buf, "r"))) {
		/* Try specific language, like en_GB */
		gettext_ht = htmk(256);
		load_po(f);
	} else if (s[0] && s[1]) {
		/* Try generic language, like en */
		joe_snprintf_3(buf, sizeof(buf), "%slang/%c%c.po",JOEDATA,s[0],s[1]);
		if ((f = fopen((char *)buf, "r"))) {
			gettext_ht = htmk(256);
			load_po(f);
		}
	}
}
