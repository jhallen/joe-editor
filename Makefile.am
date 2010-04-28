AUTOMAKE_OPTIONS = foreign

SUBDIRS = man rc po syntax joe

EXTRA_DIST = COPYING docs/LIST docs/HACKING ChangeLog NEWS README INSTALL INSTALL.AMIGA \
	docs/help-system.html charmaps/klingon.in docs/HINTS \
	cygbuild setup.hint xterm-patch xterm-readme autojoe

sysconf_joedir = $(sysconfdir)/joe
data_joedir = $(datadir)/joe

data_charmapsdir = $(data_joedir)/charmaps
data_charmaps_DATA = charmaps/klingon

data_docdir = $(docdir)
data_doc_DATA = docs/LIST README docs/HINTS ChangeLog docs/HACKING NEWS

install-exec-hook:
	rm -f $(DESTDIR)$(bindir)/jmacs $(DESTDIR)$(bindir)/jstar $(DESTDIR)$(bindir)/rjoe $(DESTDIR)$(bindir)/jpico
	rm -f $(DESTDIR)$(bindir)/jmacs.exe $(DESTDIR)$(bindir)/jstar.exe $(DESTDIR)$(bindir)/rjoe.exe $(DESTDIR)$(bindir)/jpico.exe
	for i in jmacs jstar rjoe jpico ; do $(LN_S) joe $(DESTDIR)$(bindir)/$$i ; done

uninstall-local:
	rm -f $(DESTDIR)$(bindir)/jmacs $(DESTDIR)$(bindir)/jstar $(DESTDIR)$(bindir)/rjoe $(DESTDIR)$(bindir)/jpico
	rm -f $(DESTDIR)$(bindir)/jmacs.exe $(DESTDIR)$(bindir)/jstar.exe $(DESTDIR)$(bindir)/rjoe.exe $(DESTDIR)$(bindir)/jpico.exe
