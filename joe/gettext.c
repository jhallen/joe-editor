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

static const char *ignore_prefix(const char *set)
{
	const char *s = zrchr(set, '|');
	if (s)
		++s;
	else
		s = set;
	return s;
}

const char *my_gettext(const char *s)
{
	if (gettext_ht) {
		const char *r = (const char *)htfind(gettext_ht, s);
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

static int load_po(FILE *f)
{
	char buf[1024];
	char msgid[1024];
	char msgstr[1024];
	char bf[8192];
	struct charmap *po_map = locale_map;
	int preload_flag = 0;
	msgid[0] = 0;
	msgstr[0] = 0;
	while (preload_flag || fgets(buf,SIZEOF(buf)-1,f)) {
		const char *p;
		preload_flag = 0;
		p = buf;
		parse_ws(&p, '#');
		if (!parse_field(&p, "msgid")) {
			ptrdiff_t ofst = 0;
			ptrdiff_t len;
			msgid[0] = 0;
			parse_ws(&p, '#');
			while ((len = parse_string(&p, msgid + ofst, SIZEOF(msgid)-ofst)) >= 0) {
				preload_flag = 0;
				ofst += len;
				parse_ws(&p, '#');
				if (!*p) {
					if (fgets(buf,SIZEOF(buf) - 1,f)) {
						p = buf;
						preload_flag = 1;
						parse_ws(&p, '#');
					} else {
						goto bye;
					}
				}
			}
		} else if (!parse_field(&p, "msgstr")) {
			ptrdiff_t ofst = 0;
			ptrdiff_t len;
			msgstr[0] = 0;
			parse_ws(&p, '#');
			while ((len = parse_string(&p, msgstr + ofst, SIZEOF(msgstr)-ofst)) >= 0) {
				preload_flag = 0;
				ofst += len;
				parse_ws(&p, '#');
				if (!*p) {
					if (fgets(buf,SIZEOF(buf) - 1,f)) {
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
				my_iconv(bf, SIZEOF(bf), locale_map,msgstr,po_map);
				/* Add to hash table */
				htadd(gettext_ht, zdup(msgid), zdup(bf));
			} else if (!msgid[0] && msgstr[0]) {
				char *tp = zstr(msgstr, "charset=");
				if (tp) {
					/* Copy character set name up to next delimiter */
					int x;
					tp += SIZEOF("charset=") - 1;
					while (*tp == ' ' || *tp == '\t') ++tp;
					for (x = 0; tp[x] && tp[x] !='\n' && tp[x] != '\r' && tp[x] != ' ' &&
					            tp[x] != '\t' && tp[x] != ';' && tp[x] != ','; ++x)
					            msgid[x] = tp[x];
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

void init_gettext(const char *s)
{
	FILE *f;
	char buf[1024];
	joe_snprintf_2(buf, SIZEOF(buf), "%slang/%s.po",JOEDATA,s);
	if ((f = fopen(buf, "r"))) {
		/* Try specific language, like en_GB */
		gettext_ht = htmk(256);
		load_po(f);
	} else if (s[0] && s[1]) {
		/* Try generic language, like en */
		joe_snprintf_3(buf, SIZEOF(buf), "%slang/%c%c.po",JOEDATA,s[0],s[1]);
		if ((f = fopen(buf, "r"))) {
			gettext_ht = htmk(256);
			load_po(f);
		}
	}
}
