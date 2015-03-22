AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = TODO

sysconf_joedir = $(sysconfdir)/joe
data_joedir = $(datadir)/joe

noinst_HEADERS = b.h blocks.h bw.h cmd.h config.h hash.h help.h kbd.h \
	macro.h main.h menu.h path.h poshist.h pw.h queue.h qw.h rc.h regex.h \
	scrn.h tab.h termcapj.h tty.h tw.h types.h ublock.h uedit.h uerror.h \
	ufile.h uformat.h uisrch.h umath.h undo.h usearch.h ushell.h utag.h \
	utils.h va.h vfile.h vs.h w.h utf8.h syntax.h i18n.h charmap.h mouse.h \
	lattr.h gettext.h builtin.h vt.h

bin_PROGRAMS = joe termidx stringify
INCLUDES = -DJOERC="\"$(sysconf_joedir)/\"" -DJOEDATA="\"$(data_joedir)/\""

# joe_LDADD = -lm

joe_SOURCES = b.c blocks.c bw.c cmd.c hash.c help.c kbd.c macro.c main.c menu.c \
	path.c poshist.c pw.c queue.c qw.c rc.c regex.c scrn.c tab.c termcap.c \
	tty.c tw.c ublock.c uedit.c uerror.c ufile.c uformat.c uisrch.c umath.c \
	undo.c usearch.c ushell.c utag.c va.c vfile.c vs.c w.c utils.c syntax.c \
	utf8.c selinux.c i18n.c charmap.c mouse.c lattr.c gettext.c builtin.c \
	builtins.c vt.c

termidx_SOURCES = termidx.c

stringify_SOURCES = stringify.c

distclean-local:
	rm -f *~

.PHONY: termcap
