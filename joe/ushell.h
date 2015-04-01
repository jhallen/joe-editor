/*
 * 	Shell-window functions
 *	Copyright (C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_USHELL_H
#define _JOE_USHELL_H 1

int ubknd(BW *bw);
int uvtbknd(BW *bw);
int ukillpid(BW *bw);
int urun(BW *bw);
int ubuild(BW *bw);
int ugrep(BW *bw);
int cstart(BW *bw, unsigned char *name, unsigned char **s, void *obj, int *notify, int build, int out_only, unsigned char *first_command, int vt);

extern B *runhist; /* Shell command history */
extern B *buildhist; /* Build command history */
extern B *grephist; /* Grep command history */

void vt_scrdn();

#endif
