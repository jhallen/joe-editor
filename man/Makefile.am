sysconf_joedir = $(sysconfdir)/joe
data_joedir = $(datadir)/joe
data_docdir = $(docdir)

SUBDIRS = ru

man_MANS = joe.1

EXTRA_DIST = joe.1.in

joe.1 : joe.1.in
	sed 's,@JOERC\@,$(sysconf_joedir),; s,@JOEDOC\@,$(data_docdir),; s,@JOEDATA\@,$(data_joedir),; s,@SPELL\@,@SPELL@,; s,@BINDIR\@,$(bindir),' $(srcdir)/joe.1.in > $@

MOSTLYCLEANFILES = $(man_MANS)

