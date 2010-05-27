/*
 * 	User file operations
 *	Copyright
 *	(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_UFILE_H
#define _JOE_UFILE_H 1

extern int exask; /* Ask for file name during ^K X */

void genexmsg PARAMS((BW *bw, int saved, unsigned char *name));

int ublksave PARAMS((BW *bw));
int ushell PARAMS((BW *bw));
int usys PARAMS((BW *bw));
int usave PARAMS((BW *bw));
int usavenow PARAMS((BW *bw));
int uedit PARAMS((BW *bw));
int usetcd PARAMS((BW *bw));
int uswitch PARAMS((BW *bw));
int uscratch PARAMS((BW *bw));
int uinsf PARAMS((BW *bw));
int uexsve PARAMS((BW *bw));
int unbuf PARAMS((BW *bw));
int upbuf PARAMS((BW *bw));
int uask PARAMS((BW *bw));
int ubufed PARAMS((BW *bw));
int ulose PARAMS((BW *bw));
int okrepl PARAMS((BW *bw));
int doswitch PARAMS((BW *bw, unsigned char *s, void *obj, int *notify));
int uquerysave PARAMS((BW *bw));
int ukilljoe PARAMS((BW *bw));
int get_buffer_in_window PARAMS((BW *bw, B *b));

extern B *filehist; /* History of file names */

extern int nobackups; /* Set to disable backup files */
extern unsigned char *backpath; /* Path to backup files if not current directory */
extern int orphan; /* Set to keep orphaned buffers (buffers with no windows)  */

extern unsigned char *yes_key;
extern unsigned char *no_key;
#define YES_CODE -10
#define NO_CODE -20
int yncheck PARAMS((unsigned char *string, int c));
int ynchecks PARAMS((unsigned char *string, unsigned char *s));

int ureload(BW *bw);
int ureload_all(BW *bw);

#endif
