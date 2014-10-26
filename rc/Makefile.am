sysconf_joedir = $(sysconfdir)/joe
data_joedir = $(datadir)/joe
data_docdir = $(docdir)

sysconf_joe_built = joerc jicerc.ru jmacsrc jstarrc rjoerc jpicorc

sysconf_joe_DATA = $(sysconf_joe_built) ftyperc shell.sh shell.csh

EXTRA_DIST = joerc.in jicerc.ru.in jmacsrc.in jstarrc.in jpicorc.in rjoerc.in ftyperc shell.sh shell.csh

joerc : joerc.in
	sed 's,@JOERC\@,$(sysconf_joedir),; s,@JOEDOC\@,$(data_docdir),; s,@JOEDATA\@,$(data_joedir),; s,@SPELL\@,@SPELL@,' $(srcdir)/joerc.in > $@

jicerc.ru : jicerc.ru.in
	sed 's,@JOERC\@,$(sysconf_joedir),; s,@JOEDOC\@,$(data_docdir),; s,@JOEDATA\@,$(data_joedir),; s,@SPELL\@,@SPELL@,' $(srcdir)/jicerc.ru.in > $@

jmacsrc : jmacsrc.in
	sed 's,@JOERC\@,$(sysconf_joedir),; s,@JOEDOC\@,$(data_docdir),; s,@JOEDATA\@,$(data_joedir),; s,@SPELL\@,@SPELL@,' $(srcdir)/jmacsrc.in > $@

jstarrc : jstarrc.in
	sed 's,@JOERC\@,$(sysconf_joedir),; s,@JOEDOC\@,$(data_docdir),; s,@JOEDATA\@,$(data_joedir),; s,@SPELL\@,@SPELL@,' $(srcdir)/jstarrc.in > $@

jpicorc : jpicorc.in
	sed 's,@JOERC\@,$(sysconf_joedir),; s,@JOEDOC\@,$(data_docdir),; s,@JOEDATA\@,$(data_joedir),; s,@SPELL\@,@SPELL@,' $(srcdir)/jpicorc.in > $@

rjoerc : rjoerc.in
	sed 's,@JOERC\@,$(sysconf_joedir),; s,@JOEDOC\@,$(data_docdir),; s,@JOEDATA\@,$(data_joedir),; s,@SPELL\@,@SPELL@,' $(srcdir)/rjoerc.in > $@

MOSTLYCLEANFILES = $(sysconf_joe_built)
