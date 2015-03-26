/*
 *	*rc file parser
 *	Copyright
 *		(C) 1992 Joseph H. Allen; 
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* Validate joerc file */

int validate_rc()
{
	KMAP *k;
	if (!(k = ngetcontext(USTR "main")) || kmap_empty(k)) {
		logerror_0(joe_gettext(_("Missing or empty :main keymap\n")));
		return -1;
	}

	if (!(k = ngetcontext(USTR "prompt")) || kmap_empty(k)) {
		logerror_0(joe_gettext(_("Missing or empty :prompt keymap\n")));
		return -1;
	}

	if (!(k = ngetcontext(USTR "query")) || kmap_empty(k)) {
		logerror_0(joe_gettext(_("Missing or empty :query keymap\n")));
		return -1;
	}

	if (!(k = ngetcontext(USTR "querya")) || kmap_empty(k)) {
		logerror_0(joe_gettext(_("Missing or empty :querya keymap\n")));
		return -1;
	}

	if (!(k = ngetcontext(USTR "querysr")) || kmap_empty(k)) {
		logerror_0(joe_gettext(_("Missing or empty :querysr keymap\n")));
		return -1;
	}

	if (!(k = ngetcontext(USTR "shell")) || kmap_empty(k)) {
		logerror_0(joe_gettext(_("Missing or empty :shell keymap\n")));
	}

	if (!(k = ngetcontext(USTR "vtshell")) || kmap_empty(k)) {
		logerror_0(joe_gettext(_("Missing or empty :vtshell keymap\n")));
	}

	return 0;
}

/* Parse a macro- allow it to cross lines */

MACRO *multiparse(JFILE *fd, int *refline, unsigned char *buf, int *ofst, int *referr, unsigned char *name)
{
	MACRO *m;
	int x = *ofst;
	int err = *referr;
	int line = *refline;
	m = 0;
	for (;;) {
		m = mparse(m, buf + x, &x, 0);
		if (x == -1) { /* Error */
			err = -1;
			logerror_2((char *)joe_gettext(_("%s %d: Unknown command in macro\n")), name, line);
			break;
		} else if (x == -2) { /* Get more input */
			jfgets(buf, 1024, fd);
			++line;
			x = 0;
		} else /* We're done */
			break;
	}
	*referr = err;
	*refline = line;
	*ofst = x;
	return m; 
}

/* Process rc file
 * Returns 0 if the rc file was succefully processed
 *        -1 if the rc file couldn't be opened
 *         1 if there was a syntax error in the file
 */

int procrc(CAP *cap, unsigned char *name)
{
	OPTIONS *o = &fdefault;	/* Current options */
	KMAP *context = NULL;	/* Current context */
	struct rc_menu *current_menu = NULL;
	unsigned char buf[1024];	/* Input buffer */
	unsigned char buf1[1024];	/* Input buffer */
	JFILE *fd;		/* rc file */
	int line = 0;		/* Line number */
	int err = 0;		/* Set to 1 if there was a syntax error */

	strncpy((char *)buf, (char *)name, sizeof(buf) - 1);
	buf[sizeof(buf)-1] = '\0';
#ifdef __MSDOS__
	fd = jfopen(buf, "rt");
#else
	fd = jfopen(buf, "r");
#endif

	if (!fd)
		return -1;	/* Return if we couldn't open the rc file */

	logmessage_1((char *)joe_gettext(_("Processing '%s'...\n")), name);

	while (jfgets(buf, sizeof(buf), fd)) {
		line++;
		switch (buf[0]) {
		case ' ':
		case '\t':
		case '\n':
		case '\f':
		case 0:
			break;	/* Skip comment lines */

		case '=':	/* Define a global color */
			{ /* # introduces comment */
			parse_color_def(&global_colors,buf+1,name,line);
			}
			break;

		case '*':	/* Select file types for file-type dependant options */
			{ /* Space and tab introduce comments- which means we can't have them in the regex */
				int x;

				o = (OPTIONS *) joe_malloc(sizeof(OPTIONS));
				*o = fdefault;
				for (x = 0; buf[x] && buf[x] != '\n' && buf[x] != ' ' && buf[x] != '\t'; ++x) ;
				buf[x] = 0;
				o->next = options;
				options = o;
				o->name_regex = zdup(buf);
			}
			break;
		case '+':	/* Set file contents match regex */
			{ /* No comments allowed- entire line used. */
				int x;

				for (x = 0; buf[x] && buf[x] != '\n' && buf[x] != '\r'; ++x) ;
				buf[x] = 0;
				if (o)
					o->contents_regex = zdup(buf+1);
			}
			break;
		case '-':	/* Set an option */
			{ /* parse option and arg.  arg goes to end of line.  This is bad. */
				unsigned char *opt = buf + 1;
				int x;
				unsigned char *arg = NULL;

				for (x = 0; buf[x] && buf[x] != '\n' && buf[x] != ' ' && buf[x] != '\t'; ++x) ;
				if (buf[x] && buf[x] != '\n') {
					buf[x] = 0;
					for (arg = buf + ++x; buf[x] && buf[x] != '\n'; ++x) ;
				}
				buf[x] = 0;
				if (!glopt(opt, arg, o, 2)) {
					err = 1;
					logerror_3((char *)joe_gettext(_("%s %d: Unknown option %s\n")), name, line, opt);
				}
			}
			break;
		case '{':	/* Process help text.  No comment allowed after {name */
			{	/* everything after } is ignored. */
				line = help_init(fd,buf,line);
			}
			break;
		case ':':	/* Select context */
			{
				int x, c;

				for (x = 1; !joe_isspace_eof(locale_map,buf[x]); ++x) ;
				c = buf[x];
				buf[x] = 0;
				if (x != 1)
					if (!zcmp(buf + 1, USTR "def")) {
						int y;

						for (buf[x] = c; joe_isblank(locale_map,buf[x]); ++x) ;
						for (y = x; !joe_isspace_eof(locale_map,buf[y]); ++y) ;
						c = buf[y];
						buf[y] = 0;
						zlcpy(buf1, sizeof(buf1), buf + x);
						if (y != x) {
							int sta = y + 1;
							MACRO *m;

							if (joe_isblank(locale_map,c)
							    && (m = multiparse(fd, &line, buf, &sta, &err, name)))
								addcmd(buf1, m);
							else {
								err = 1;
								logerror_2((char *)joe_gettext(_("%s %d: macro missing from :def\n")), name, line);
							}
						} else {
							err = 1;
							logerror_2((char *)joe_gettext(_("%s %d: command name missing from :def\n")), name, line);
						}
					} else if (!zcmp(buf + 1, USTR "inherit")) {
						if (context) {
							for (buf[x] = c; joe_isblank(locale_map,buf[x]); ++x) ;
							for (c = x; !joe_isspace_eof(locale_map,buf[c]); ++c) ;
							buf[c] = 0;
							if (c != x)
								kcpy(context, kmap_getcontext(buf + x));
							else {
								err = 1;
								logerror_2((char *)joe_gettext(_("%s %d: context name missing from :inherit\n")), name, line);
							}
						} else {
							err = 1;
							logerror_2((char *)joe_gettext(_("%s %d: No context selected for :inherit\n")), name, line);
						}
					} else if (!zcmp(buf + 1, USTR "include")) {
						for (buf[x] = c; joe_isblank(locale_map,buf[x]); ++x) ;
						for (c = x; !joe_isspace_eof(locale_map,buf[c]); ++c) ;
						buf[c] = 0;
						if (c != x) {
							unsigned char bf[1024];
							unsigned char *p = (unsigned char *)getenv("HOME");
							int rtn = -1;
							bf[0] = 0;
							if (p && buf[x] != '/') {
								joe_snprintf_2(bf,sizeof(bf),"%s/.joe/%s",p,buf + x);
								rtn = procrc(cap, bf);
							}
							if (rtn == -1 && buf[x] != '/') {
								joe_snprintf_2(bf,sizeof(bf),"%s%s",JOERC,buf + x);
								rtn = procrc(cap, bf);
							}
							if (rtn == -1 && buf[x] != '/') {
								joe_snprintf_1(bf,sizeof(bf),"*%s",buf + x);
								rtn = procrc(cap, bf);
							}
							if (rtn == -1 && buf[x] == '/') {
								joe_snprintf_1(bf,sizeof(bf),"%s",buf + x);
								rtn = procrc(cap, bf);
							}
							switch (rtn) {
							case 1:
								err = 1;
								break;
							case -1:
								logerror_3((char *)joe_gettext(_("%s %d: Couldn't open %s\n")), name, line, bf);
								err = 1;
								break;
							}
							context = 0;
							o = &fdefault;
						} else {
							err = 1;
							logerror_2((char *)joe_gettext(_("%s %d: :include missing file name\n")), name, line);
						}
					} else if (!zcmp(buf + 1, USTR "delete")) {
						if (context) {
							int y;

							for (buf[x] = c; joe_isblank(locale_map,buf[x]); ++x) ;
							for (y = x; buf[y] != 0 && buf[y] != '\t' && buf[y] != '\n' && (buf[y] != ' ' || buf[y + 1]
															!= ' '); ++y) ;
							buf[y] = 0;
							kdel(context, buf + x);
						} else {
							err = 1;
							logerror_2((char *)joe_gettext(_("%s %d: No context selected for :delete\n")), name, line);
						}
					} else if (!zcmp(buf + 1, USTR "defmap")) {
						for (buf[x] = c; joe_isblank(locale_map,buf[x]); ++x) ;
						for (c = x; !joe_isspace_eof(locale_map,buf[c]); ++c) ;
						buf[c] = 0;
						if (c != x) {
							context = kmap_getcontext(buf + x);
							current_menu = 0;
						} else {
							err = 1;
							logerror_2((char *)joe_gettext(_("%s %d: :defmap missing name\n")), name, line);
						}
					} else if (!zcmp(buf + 1, USTR "defmenu")) {
						MACRO *m = 0;
						int y, d;
						for (buf[x] = c; joe_isblank(locale_map,buf[x]); ++x) ;
						for (c = x; !joe_isspace_eof(locale_map,buf[c]); ++c) ;
						d = buf[c];
						buf[c] = 0;
						zlcpy(buf1, sizeof(buf1), buf + x);
						buf[c] = d;
						for (y = c; joe_isblank(locale_map, buf[y]); ++y);
						if (!joe_isspace_eof(locale_map, buf[y]))
							m = multiparse(fd, &line, buf, &y, &err, name);
						current_menu = create_menu(buf1, m);
						context = 0;
					} else {
						context = kmap_getcontext(buf + 1);
						current_menu = 0;
						/* err = 1;
						logerror_2((char *)joe_gettext(_("%s %d: unknown :command\n")), name, line);*/
					}
				else {
					err = 1;
					logerror_2((char *)joe_gettext(_("%s %d: Invalid context name\n")), name, line);
				}
			}
			break;
		default:	/* Get key-sequence to macro binding */
			{
				int x, y;
				MACRO *m;

				if (!context && !current_menu) {
					err = 1;
					logerror_2((char *)joe_gettext(_("%s %d: No context selected for macro to key-sequence binding\n")), name, line);
					break;
				}

				x = 0;
				m = multiparse(fd, &line, buf, &x, &err, name);
				if (x == -1)
					break;
				if (!m)
					break;

				/* Skip to end of key sequence */
				for (y = x; buf[y] != 0 && buf[y] != '\t' && buf[y] != '\n' && (buf[y] != ' ' || buf[y + 1] != ' '); ++y) ;
				buf[y] = 0;

				if (current_menu) {
					/* Add menu entry */
					add_menu_entry(current_menu, buf + x, m);
				} else {
					/* Add binding to context */
					if (kadd(cap, context, buf + x, m) == -1) {
						logerror_3((char *)joe_gettext(_("%s %d: Bad key sequence '%s'\n")), name, line, buf + x);
						err = 1;
					}
				}
			}
			break;
		}
	}
	jfclose(fd);		/* Close rc file */

	/* Print proper ending string */
	logmessage_1((char *)joe_gettext(_("Finished processing %s\n")), name);

	return err;		/* 0 for success, 1 for syntax error */
}
