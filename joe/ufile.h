/*
 * 	User file operations
 *	Copyright
 *	(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

extern int exask; /* Ask for file name during ^K X */

void genexmsg(BW *bw, int saved, char *name);

int ublksave(W *w, int k);
int ushell(W *w, int k);
int usys(W *w, int k);
int usave(W *w, int k);
int usavenow(W *w, int k);
int uedit(W *w, int k);
int usetcd(W *w, int k);
int uswitch(W *w, int k);
int uscratch(W *w, int k);
int uscratch_push(W *w, int k);
int uinsf(W *w, int k);
int uexsve(W *w, int k);
int unbuf(W *w, int k);
int upbuf(W *w, int k);
int uask(W *w, int k);
int ubufed(W *w, int k);
int ulose(W *w, int k);
int okrepl(BW *bw);
int doswitch(W *w, char *s, void *obj, int *notify);
int uquerysave(W *w, int k);
int ukilljoe(W *w, int k);
int get_buffer_in_window(BW *bw, B *b);

extern B *filehist; /* History of file names */

extern int nobackups; /* Set to disable backup files */
extern char *backpath; /* Path to backup files if not current directory */
extern int orphan; /* Set to keep orphaned buffers (buffers with no windows)  */

extern const char *yes_key;
extern const char *no_key;
#define YES_CODE -10
#define NO_CODE -20
int yncheck(const char *string, int c);
int ynchecks(const char *string, const char *s);

int ureload(W *w, int k);
int ureload_all(W *w, int k);
