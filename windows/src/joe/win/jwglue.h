#ifndef _JOEWIN_GLUE_H
#define _JOEWIN_GLUE_H

/*
** Redefinitions to wrap system-specific JOE code to use either the appropriate
** Windows equivalent, or our own (lightweight) glue layer.
*/

#define random rand
#define sleep(x) Sleep(1000*(x))
#define getenv glue_getenv
#define _exit jwexit
#define exit jwexit

#define lstat glue_stat
#define stat(x,y) glue_stat((x),(y))	/* Don't confuse with struct stat */
#define open glue_open
#define fopen glue_fopen
#define creat glue_creat
#define getcwd glue_getcwd
#define chdir glue_chdir
#define fprintf glue_fprintf

/* Definitions of those functions */

char* glue_getenv(const char*);

/* UTF-8 encoding = ~3x MAX_PATH */
#define PATH_MAX (MAX_PATH*3)

struct dirent {
    char    d_name[PATH_MAX + 1]; /* PATH_MAX is defined inside of JOE and = 3 * MAX_PATH */
    wchar_t _startname[MAX_PATH + 1];
    HANDLE  find_handle;
};

void *opendir(unsigned char*);
void closedir(void*);
struct dirent *readdir(void*);

int glue_open(const char *, int);
int glue_stat(const char *, struct _stat *);
int glue_creat(const char *, int);
FILE *glue_fopen(const char *, const char *);
char *glue_getcwd(char *output, size_t outsz);
int glue_chdir(const char *path);
int glue_fprintf(FILE *, const char *, ...);


#endif // _JOEWIN_GLUE_H
