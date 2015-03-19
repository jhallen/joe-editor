/*
 *  This file is part of Joe's Own Editor for Windows.
 *  Copyright (c) 2014 John J. Jordan.
 *
 *  Joe's Own Editor for Windows is free software: you can redistribute it 
 *  and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2 of the 
 *  License, or (at your option) any later version.
 *
 *  Joe's Own Editor for Windows is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Joe's Own Editor for Windows.  If not, see
 *  <http://www.gnu.org/licenses/>.
 */

#ifndef _JOEWIN_GLUE_H
#define _JOEWIN_GLUE_H

/*
** Redefinitions to wrap system-specific JOE code to use either the appropriate
** Windows equivalent, or our own (lightweight) glue layer.
*/

#define random rand
#define sleep(x) Sleep(1000*(x))
#define snprintf _snprintf
#define getenv glue_getenv
#define _exit jwexit
#define exit jwexit

#define open glue_open
#define fopen glue_fopen
#define creat glue_creat
#define getcwd glue_getcwd
#define chdir glue_chdir
#define chddir glue_chdir
#define unlink glue_unlink
#define mkdir glue_mkdir

/* TODO: long is 32 bits on 64 bit Windows and JOE does not use time_t or off_t (it uses longs to
   store these internally).  time_t is 64 bits on both platforms, but off_t is 32 bits.  This needs
   to be fixed but it is an extensive change that touches all platforms.  For now, we will just live
   with 32-bit types for both values. */

#define lstat glue_stat
#define stat _stat32
#define _stat32(x,y) glue_stat((x),(y))		/* simultaneously ugly and clever :-) */
#define jwstatfunc _wstat32
#define fstat _fstat32

/* UTF-8 encoding = ~3x MAX_PATH */
#define PATH_MAX (MAX_PATH*3)

/* UNIX-y directory functions */

struct dirent {
    char    d_name[PATH_MAX + 1]; /* PATH_MAX is defined inside of JOE and = 3 * MAX_PATH */
    wchar_t _startname[MAX_PATH + 1];
    HANDLE  find_handle;
};

void *opendir(unsigned char*);
void closedir(void*);
struct dirent *readdir(void*);

/* Definitions of those functions */

char* glue_getenv(const char*);
int glue_open(const char *, int, ...);
int glue_stat(const char *, struct stat *);
int glue_creat(const char *, int);
FILE *glue_fopen(const char *, const char *);
char *glue_getcwd(char *output, size_t outsz);
int glue_chdir(const char *path);
int glue_fprintf(FILE *, const char *, ...);
int glue_unlink(const char *path);
int glue_mkdir(const char *path, int);

#endif // _JOEWIN_GLUE_H
