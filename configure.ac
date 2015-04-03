# Process this file with autoconf to produce a configure script.

# We need AC_PROG_EGREP
AC_PREREQ(2.54)

#### Here's the only place where to change version number ####
AC_INIT(joe, 4.0)
AC_CONFIG_SRCDIR([joe/b.c])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# Init automake
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AM_CONFIG_HEADER(joe/autoconf.h)

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_EGREP
AC_PROG_INSTALL
AC_PROG_LN_S

case "$host" in
	*-*-solaris*)
		CPPFLAGS="$CPPFLAGS -I/usr/local/include"
		LDFLAGS="$LDFLAGS -L/usr/local/lib -R/usr/local/lib"
		;;
	*)	;;
esac

search_libs="$search_libs m"

# Checks for libraries.
AC_ARG_ENABLE(curses,
[  --disable-curses        disable use of curses library],
	curses=$enableval, curses=yes)

if test "$curses" = "yes"; then
	search_libs="ncurses curses"
fi

AC_ARG_ENABLE(termcap,
[  --disable-termcap       disable use of termcap library],
	termcap=$enableval, termcap=yes)

if test "$termcap" = "yes"; then
	search_libs="$search_libs termcap"
fi

AC_SEARCH_LIBS(openpty, util, AC_DEFINE_UNQUOTED(HAVE_OPENPTY, 1, [If we have BSD function openpty()]))
AC_SEARCH_LIBS(login_tty, util, AC_DEFINE_UNQUOTED(HAVE_LOGIN_TTY, 1, [If we have BSD function login_tty()]))
AC_SEARCH_LIBS(tgetflag, $search_libs, AC_DEFINE_UNQUOTED(TERMINFO, 1, [If we have newer terminfo/termcap capabilities]))
AC_SEARCH_LIBS(snprintf, snprintf db, AC_DEFINE_UNQUOTED(HAVE_SNPRINTF, 1, [If we have snprintf]))

# Checks for header files.
AC_SYS_LARGEFILE
AC_HEADER_DIRENT
AC_SYS_POSIX_TERMIOS
if test "$ac_cv_sys_posix_termios" = "yes"; then
	AC_DEFINE(HAVE_POSIX_TERMIOS, 1, [We have <termios.h> and <sys/termios.h>])
else
	AC_CHECK_HEADERS([termio.h sys/termio.h])
	if test "$ac_cv_header_termio_h" = "yes" -a "$ac_cvs_header_sys_termio_h" = "yes" ]; then
		AC_DEFINE(HAVE_SYSV_TERMIO, 1, [We have <termio.h> and <sys/termio.h>])
	fi
fi
AC_HEADER_TIOCGWINSZ
AC_CHECK_HEADERS([errno.h fcntl.h locale.h sgtty.h stdlib.h string.h])
AC_CHECK_HEADERS([sys/ioctl.h sys/param.h sys/time.h unistd.h utime.h])
AC_CHECK_HEADERS([sys/dirent.h time.h pwd.h paths.h pty.h libutil.h])
AC_CHECK_HEADERS([sys/types.h sys/stat.h sys/wait.h limits.h signal.h])
AC_CHECK_HEADERS([curses.h term.h utmp.h sys/utime.h stddef.h])

AC_CHECK_HEADERS([selinux/selinux.h selinux/context.h])
if test "$ac_cv_header_selinux_selinux_h" = "yes" -a "$ac_cv_header_selinux_context_h" = "yes" ; then
	echo "Enabling SELinux context copying!"
	AC_DEFINE(HAVE_SELINUX, 1, [We have SELinux headers])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_PROTOTYPES
AC_C_CONST
if test "$cross_compiling" = "yes" ; then
	AC_MSG_WARN(cross compiling so assuming defaults for most 32 bit machines)
fi
AC_CHECK_TYPES([long long])
AC_CHECK_SIZEOF(char, 1)
AC_CHECK_SIZEOF(short, 2)
AC_CHECK_SIZEOF(int, 4)
AC_CHECK_SIZEOF(long, 4)
AC_CHECK_SIZEOF(long long, 4)
AC_CHECK_SIZEOF(void *, 4)
AC_CHECK_TYPE([sighandler_t],
	[AC_DEFINE([HAVE_SIGHANDLER_T], 1, [Define if you have the `sighandler_t' type.])],
	,
	[/* if you have signal.h header, include it */
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif])

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_LSTAT
joe_SETPGRP
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_OFF_T
AC_CHECK_SIZEOF(off_t, 4)
AC_CHECK_TYPE([ssize_t], [], [
	AC_DEFINE([ssize_t], [int], [Define to `int' if <sys/types.h> does not define.])
	], [])
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_FORK
AC_CHECK_FUNCS([getcwd getwd])
if test x"$ac_cv_func_getcwd" != xyes; then
	if test x"$ac_cv_func_getwd" != xyes; then
		AC_MSG_ERROR([don't know how to get current working directory])
	fi
fi
AC_CHECK_FUNCS([isblank])
if test x"$ac_cv_func_isblank" = xyes; then
	joe_ISBLANK
fi
AC_CHECK_FUNCS([alarm mkdir mkstemp putenv setlocale strchr strdup utime setpgid])
AC_CHECK_FUNCS([setitimer sigaction sigvec siginterrupt sigprocmask])

dnl Math functions... "-lm" doesn't always have them all on embedded systems

AC_SEARCH_LIBS(sin, m, AC_DEFINE_UNQUOTED(HAVE_SIN, 1, [If we have sin()]))
AC_SEARCH_LIBS(cos, m, AC_DEFINE_UNQUOTED(HAVE_COS, 1, [If we have cos()]))
AC_SEARCH_LIBS(tan, m, AC_DEFINE_UNQUOTED(HAVE_TAN, 1, [If we have tan()]))
AC_SEARCH_LIBS(exp, m, AC_DEFINE_UNQUOTED(HAVE_EXP, 1, [If we have exp()]))
AC_SEARCH_LIBS(sqrt, m, AC_DEFINE_UNQUOTED(HAVE_SQRT, 1, [If we have sqrt()]))
AC_SEARCH_LIBS(cbrt, m, AC_DEFINE_UNQUOTED(HAVE_CBRT, 1, [If we have cbrt()]))
AC_SEARCH_LIBS(log, m, AC_DEFINE_UNQUOTED(HAVE_LOG, 1, [If we have log()]))
AC_SEARCH_LIBS(log10, m, AC_DEFINE_UNQUOTED(HAVE_LOG10, 1, [If we have log10()]))
AC_SEARCH_LIBS(asin, m, AC_DEFINE_UNQUOTED(HAVE_ASIN, 1, [If we have asin()]))
AC_SEARCH_LIBS(acos, m, AC_DEFINE_UNQUOTED(HAVE_ACOS, 1, [If we have acos()]))
AC_SEARCH_LIBS(atan, m, AC_DEFINE_UNQUOTED(HAVE_ATAN, 1, [If we have atan()]))
AC_SEARCH_LIBS(sinh, m, AC_DEFINE_UNQUOTED(HAVE_SINH, 1, [If we have sinh()]))
AC_SEARCH_LIBS(cosh, m, AC_DEFINE_UNQUOTED(HAVE_COSH, 1, [If we have cosh()]))
AC_SEARCH_LIBS(tanh, m, AC_DEFINE_UNQUOTED(HAVE_TANH, 1, [If we have tanh()]))
AC_SEARCH_LIBS(asinh, m, AC_DEFINE_UNQUOTED(HAVE_ASINH, 1, [If we have asinh()]))
AC_SEARCH_LIBS(acosh, m, AC_DEFINE_UNQUOTED(HAVE_ACOSH, 1, [If we have acosh()]))
AC_SEARCH_LIBS(atanh, m, AC_DEFINE_UNQUOTED(HAVE_ATANH, 1, [If we have atanh()]))
AC_SEARCH_LIBS(floor, m, AC_DEFINE_UNQUOTED(HAVE_FLOOR, 1, [If we have floor()]))
AC_SEARCH_LIBS(ceil, m, AC_DEFINE_UNQUOTED(HAVE_CEIL, 1, [If we have ceil()]))
AC_SEARCH_LIBS(fabs, m, AC_DEFINE_UNQUOTED(HAVE_FABS, 1, [If we have fabs()]))
AC_SEARCH_LIBS(erf, m, AC_DEFINE_UNQUOTED(HAVE_ERF, 1, [If we have erf()]))
AC_SEARCH_LIBS(erfc, m, AC_DEFINE_UNQUOTED(HAVE_ERFC, 1, [If we have erfc()]))
AC_SEARCH_LIBS(j0, m, AC_DEFINE_UNQUOTED(HAVE_J0, 1, [If we have j0()]))
AC_SEARCH_LIBS(j1, m, AC_DEFINE_UNQUOTED(HAVE_J1, 1, [If we have j1()]))
AC_SEARCH_LIBS(y0, m, AC_DEFINE_UNQUOTED(HAVE_Y0, 1, [If we have y0()]))
AC_SEARCH_LIBS(y1, m, AC_DEFINE_UNQUOTED(HAVE_Y1, 1, [If we have y1()]))

joe_REINSTALL_SIGHANDLERS

# check if -liconv is necessary
# if iconv() is in libc, don't bother with -liconv
#AC_CHECK_LIB(c,iconv,
#  [REQRD_LIBS= ],
#  [REQRD_LIBS=-liconv])
#
#AC_SUBST(REQRD_LIBS)

dnl OK lets try it...
dnl AM_GNU_GETTEXT([external])

AH_VERBATIM([_PARAMS],[/* Check to see if we can use strict prototypes */
#ifndef PARAMS
#  ifdef PROTOTYPES
#    define PARAMS(protos) protos
#  else
#    define PARAMS(protos) ()
#  endif
#endif])

# Check for programs..
AC_CHECK_PROG(SPELL,aspell,aspell,ispell)

AC_CONFIG_FILES([Makefile joe/Makefile rc/Makefile man/Makefile man/ru/Makefile
syntax/Makefile po/Makefile charmaps/klingon
syntax/4gl.jsf syntax/ada.jsf syntax/ant.jsf syntax/asm.jsf syntax/awk.jsf
syntax/batch.jsf syntax/c.jsf syntax/cobol.jsf syntax/comment_todo.jsf
syntax/conf.jsf syntax/csharp.jsf syntax/csh.jsf syntax/css.jsf
syntax/debian.jsf syntax/diff.jsf syntax/elixir.jsf syntax/erb.jsf
syntax/erlang.jsf syntax/fortran.jsf syntax/git-commit.jsf syntax/go.jsf
syntax/haml.jsf syntax/haskell.jsf syntax/html.jsf syntax/htmlerb.jsf
syntax/ini.jsf syntax/iptables.jsf syntax/java.jsf syntax/joerc.jsf
syntax/js.jsf syntax/jsf_check.jsf syntax/jsf.jsf syntax/json.jsf
syntax/lisp.jsf syntax/lua.jsf syntax/m4.jsf syntax/mail.jsf
syntax/mason.jsf syntax/matlab.jsf syntax/md.jsf syntax/ocaml.jsf
syntax/pascal.jsf syntax/perl.jsf syntax/php.jsf syntax/powershell.jsf
syntax/prolog.jsf syntax/properties.jsf syntax/ps.jsf syntax/puppet.jsf
syntax/python.jsf syntax/rexx.jsf syntax/ruby.jsf syntax/sed.jsf
syntax/sh.jsf syntax/sieve.jsf syntax/skill.jsf syntax/sml.jsf
syntax/spec.jsf syntax/sql.jsf syntax/tcl.jsf syntax/tex.jsf
syntax/troff.jsf syntax/typescript.jsf syntax/verilog.jsf syntax/vhdl.jsf
syntax/whitespace.jsf syntax/xml.jsf syntax/yaml.jsf syntax/filename.jsf])
AC_OUTPUT
