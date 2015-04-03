/* 
 *	Directory and path functions
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#ifdef HAVE_PATHS_H
#  include <paths.h>	/* for _PATH_TMP */
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif


#ifdef HAVE_DIRENT_H
#  include <dirent.h>
#  define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#  ifdef HAVE_SYS_DIRENT_H
#    include <sys/dirent.h>
#    define NAMLEN(dirent) strlen((dirent)->d_name)
#  else
#    define direct dirent
#    define NAMLEN(dirent) (dirent)->d_namlen
#    ifdef HAVE_SYS_NDIR_H
#      include <sys/ndir.h>
#    else
#      ifdef HAVE_SYS_DIR_H
#        include <sys/dir.h>
#      else
#        ifdef HAVE_NDIR_H
#          include <ndir.h>
#        else
#          ifndef __MSDOS__
#            include "dir.c"
#          endif
#        endif
#      endif
#    endif
#  endif
#endif

#ifdef __MSDOS__	/* paths in MS-DOS can include a drive letter followed by semicolon */
#define	do_if_drive_letter(path, command) do { \
						if ((path)[0] && (path)[1] == ':') { \
							command; \
						} \
					} while(0)
#else
#define do_if_drive_letter(path, command)	do { } while(0)
#endif
#define skip_drive_letter(path)	do_if_drive_letter((path), (path) += 2)

#ifndef		_PATH_TMP
#  ifdef __MSDOS__
#    define	_PATH_TMP	""
#  else
#    define	_PATH_TMP	"/tmp/"
#  endif
#endif

#ifndef PATH_MAX
/* #warning is gcc extension
  #warning What should we include to have PATH_MAX defined?
*/
#define PATH_MAX	4096
#endif

/********************************************************************/
char *joesep(char *path)
{
#ifdef __MSDOS__
	int x;

	for (x = 0; path[x]; ++x)
		if (path[x] == '\\')
			path[x] = '/';
#endif
	return path;
}
/********************************************************************/
char *namprt(char *path)
{
	char *z;

	skip_drive_letter(path);
	z = path + slen(path);
	while ((z != path) && (z[-1] != '/'))
		--z;
	return vsncpy(NULL, 0, sz(z));
}
/********************************************************************/
char *namepart(char *tmp, int tmpsiz, char *path)
{
	char *z;

	skip_drive_letter(path);
	z = path + zlen(path);
	while ((z != path) && (z[-1] != '/'))
		--z;
	return zlcpy(tmp, tmpsiz, z);
}
/********************************************************************/
char *dirprt(char *path)
{
	char *b = path;
	char *z = path + slen(path);

	skip_drive_letter(b);
	while ((z != b) && (z[-1] != '/'))
		--z;
	return vsncpy(NULL, 0, path, z - path);
}
/********************************************************************/
char *begprt(char *path)
{
	char *z = path + slen(path);
	int drv = 0;

	do_if_drive_letter(path, drv = 2);
	while ((z != path + drv) && (z[-1] == '/'))
		--z;
	if (z == path + drv)
		return vsncpy(NULL, 0, sz(path));
	else {
		while ((z != path + drv) && (z[-1] != '/'))
			--z;
		return vsncpy(NULL, 0, path, z - path);
	}
}
/********************************************************************/
char *endprt(char *path)
{
	char *z = path + slen(path);
	int drv = 0;

	do_if_drive_letter(path, drv = 2);
	while ((z != path + drv) && (z[-1] == '/'))
		--z;
	if (z == path + drv)
		return vsncpy(NULL, 0, sc(""));
	else {
		while (z != path + drv && z[-1] != '/')
			--z;
		return vsncpy(NULL, 0, sz(z));
	}
}
/********************************************************************/
int mkpath(char *path)
{
	char *s;

	if (path[0] == '/') {
		if (chddir("/"))
			return 1;
		s = path;
		goto in;
	}

	while (path[0]) {
		char c;

		for (s = path; (*s) && (*s != '/'); s++) ;
		c = *s;
		*s = 0;
		if (chddir(path)) {
			if (mkdir(path, 0777))
				return 1;
			if (chddir(path))
				return 1;
		}
		*s = c;
	      in:
		while (*s == '/')
			++s;
		path = s;
	}
	return 0;
}
/********************************************************************/
/* Create a temporary file */
/********************************************************************/
char *mktmp(char *where)
{
#ifndef HAVE_MKSTEMP
	static unsigned seq = 0;
#endif
	char *name;
	int fd;
	int namesize;

	if (!where)
		where = getenv("TEMP");
	if (!where)
		where = _PATH_TMP;

	namesize = zlen(where) + 16;
	name = vsmk(namesize);	/* [G.Ghibo'] we need to use vsmk() and not malloc() as
				   area returned by mktmp() is destroyed later with
				   vsrm(); */
#ifdef HAVE_MKSTEMP
	joe_snprintf_1(name, namesize, "%s/joe.tmp.XXXXXX", where);
	if((fd = mkstemp(name)) == -1)
		return NULL;	/* FIXME: vflsh() and vflshf() */
				/* expect mktmp() always succeed!!! */

	fchmod(fd, 0600);       /* Linux glibc 2.0 mkstemp() creates it with */
				/* 0666 mode --> change it to 0600, so nobody */
				/* else sees content of temporary file */
	close(fd);

#else
      loop:
	seq = (seq + 1) % 1000;
	joe_snprintf_3(name, namesize, "%s/joe.tmp.%03u%03u", where, seq, (unsigned) time(NULL) % 1000);
	if ((fd = open(name, O_RDONLY)) != -1) {
		close(fd);
		goto loop;	/* FIXME: possible endless loop --> DoS attack */
	}
	if ((fd = open(name, O_RDWR | O_CREAT | O_EXCL, 0600)) == -1)
		return NULL;	/* FIXME: see above */
	else
		close(fd);
#endif
	return name;
}
/********************************************************************/
int rmatch(char *a, char *b)
{
	int flag, inv, c;

	for (;;)
		switch (*a) {
		case '*':
			++a;
			do {
				if (rmatch(a, b))
					return 1;
			} while (*b++);
			return 0;
		case '[':
			++a;
			flag = 0;
			if (*a == '^') {
				++a;
				inv = 1;
			} else
				inv = 0;
			if (*a == ']')
				if (*b == *a++)
					flag = 1;
			while (*a && (c = *a++) != ']')
				if ((c == '-') && (a[-2] != '[') && (*a)) {
					if ((*b >= a[-2]) && (*b <= *a))
						flag = 1;
				} else if (*b == c)
					flag = 1;
			if ((!flag && !inv) || (flag && inv) || (!*b))
				return 0;
			++b;
			break;
		case '?':
			++a;
			if (!*b)
				return 0;
			++b;
			break;
		case 0:
			if (!*b)
				return 1;
			else
				return 0;
		default:
			if (*a++ != *b++)
				return 0;
		}
}
/********************************************************************/
int isreg(char *s)
{
	int x;

	for (x = 0; s[x]; ++x)
		if ((s[x] == '*') || (s[x] == '?') || (s[x] == '['))
			return 1;
	return 0;
}
/********************************************************************/
#ifdef __MSDOS__
#include <dos.h>
#include <dir.h>

struct direct {
	char d_name[16];
} direc;
int dirstate = 0;
struct ffblk ffblk;
char *dirpath = NULL;

void *opendir(char *path)
{
	dirstate = 0;
	return &direc;
}

void closedir()
{
}

struct direct *readdir()
{
	int x;

	if (dirstate) {
		if (findnext(&ffblk))
			return NULL;
	} else {
		if (findfirst("*.*", &ffblk, FA_DIREC))
			return NULL;
		dirstate = 1;
	}

	zlcpy(direc.d_name, SIZEOF(direc.d_name), ffblk.ff_name);
	for (x = 0; direc.d_name[x]; ++x)
		direc.d_name[x] = tolower(direc.d_name[x]);
	return &direc;
}
#endif
/********************************************************************/
char **rexpnd(char *word)
{
	void *dir;
	char **lst = NULL;

	struct dirent *de;
	dir = opendir(".");
	if (dir) {
		while ((de = readdir(dir)) != NULL)
			if (strcmp(".", de->d_name))
				if (rmatch(word, de->d_name))
					lst = vaadd(lst, vsncpy(NULL, 0, sz(de->d_name)));
		closedir(dir);
	}
	return lst;
}
/********************************************************************/
char **rexpnd_users(char *word)
{
	char **lst = NULL;
	struct passwd *pw;

	while((pw=getpwent()))
		if (rmatch(word+1, pw->pw_name)) {
			char *t = vsncpy(NULL,0,sc("~"));
			lst = vaadd(lst, vsncpy(sv(t),sz(pw->pw_name)));
			}
	endpwent();

	return lst;
}
/********************************************************************/
int chpwd(char *path)
{
#ifdef __MSDOS__
	char buf[256];
	int x;

	if (!path)
		return 0;
	if ((path[0]) && (path[1] == ':')) {
		if (_chdrive(path[0] & 0x1F))
			return -1;
		path += 2;
	}
	if (!path[0])
		return 0;
	zlcpy(buf, SIZEOF(buf), path);
	x = zlen(buf);
	while (x > 1) {
		--x;
		if ((buf[x] == '/') || (buf[x] == '\\'))
			buf[x] = 0;
		else
			break;
	}
	return chdir(buf);
#else
	if ((!path) || (!path[0]))
		return 0;
	return chdir(path);
#endif
}

/* The pwd function */
char *pwd(void)
{
	static char buf[PATH_MAX];
	char	*ret;

#ifdef HAVE_GETCWD
	ret = getcwd(buf, PATH_MAX - 1);
#else
	ret = getwd(buf);
#endif
	buf[PATH_MAX - 1] = '\0';

	return ret;
}

/* Simplify prefix by using ~ */
/* Expects s to have trailing / */

char *simplify_prefix(char *s)
{
	char *t = getenv("HOME");
	char *n;

#ifdef junk
	char *org = pwd();
	/* Normalize home */
	if (t && !chpwd(t)) {
		t = pwd();
	} else {
		t = 0;
	}
	chpwd(org);
#endif

	/* If current directory is prefixed with home directory, use ~... */
	if (t && !zncmp(s,t,zlen(t)) && (!s[zlen(t)] || s[zlen(t)]=='/')) {
		n = vsncpy(NULL,0,sc("~/"));
		/* If anything more than just the home directory, add it */
		if (s[zlen(t)]) {
			n = vsncpy(sv(n),s+zlen(t)+1,zlen(s+zlen(t)+1));
		}
	} else {
		n = vsncpy(NULL,0,sz(s));
	}
	return n;
}
