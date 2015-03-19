/*
 * 	Shell-window functions
 *	Copyright (C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_USHELL_H
#define _JOE_USHELL_H 1

int ubknd PARAMS((BW *bw));
int uvtbknd PARAMS((BW *bw));
int ukillpid PARAMS((BW *bw));
int urun PARAMS((BW *bw));
int ubuild PARAMS((BW *bw));
int ugrep PARAMS((BW *bw));
int cstart PARAMS((BW *bw, unsigned char *name, unsigned char **s, void *obj, int *notify, int build, int out_only, unsigned char *first_command, int vt));

extern B *runhist; /* Shell command history */
extern B *buildhist; /* Build command history */
extern B *grephist; /* Grep command history */

void vt_scrdn();

#endif
