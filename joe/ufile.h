/*
 * 	User file operations
 *	Copyright
 *	(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

extern int exask; /* Ask for file name during ^K X */

void genexmsg(BW *bw, int saved, char *name);

int ublksave(BW *bw);
int ushell(BW *bw);
int usys(BW *bw);
int usave(BW *bw);
int usavenow(BW *bw);
int uedit(BW *bw);
int usetcd(BW *bw);
int uswitch(BW *bw);
int uscratch(BW *bw);
int uscratch_push(BW *bw);
int uinsf(BW *bw);
int uexsve(BW *bw);
int unbuf(BW *bw);
int upbuf(BW *bw);
int uask(BW *bw);
int ubufed(BW *bw);
int ulose(BW *bw);
int okrepl(BW *bw);
int doswitch(BW *bw, char *s, void *obj, int *notify);
int uquerysave(BW *bw);
int ukilljoe(BW *bw);
int get_buffer_in_window(BW *bw, B *b);

extern B *filehist; /* History of file names */

extern int nobackups; /* Set to disable backup files */
extern char *backpath; /* Path to backup files if not current directory */
extern int orphan; /* Set to keep orphaned buffers (buffers with no windows)  */

extern char *yes_key;
extern char *no_key;
#define YES_CODE -10
#define NO_CODE -20
int yncheck(char *string, int c);
int ynchecks(char *string, char *s);

int ureload(BW *bw);
int ureload_all(BW *bw);
