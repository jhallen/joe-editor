/*
 *	Basic user edit functions
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_UEDIT_H
#define _JOE_UEDIT_H 1

extern int pgamnt; /* Amount of pgup/pgdn lines to keep */

/*
 * Movable functions
 *	return 0 if action was done
 *	return -1 otherwise
 */
int u_goto_bol PARAMS((BW *bw));	/* move cursor to beginning of line */
int u_goto_eol PARAMS((BW *bw));	/* move cursor to end of line */
int u_goto_bof PARAMS((BW *bw));	/* move cursor to beginning of file */
int u_goto_eof PARAMS((BW *bw));	/* move cursor to end of file */
int u_goto_left PARAMS((BW *bw));	/* move cursor to left (left arrow) */
int u_goto_right PARAMS((BW *bw));	/* move cursor to right (right arrow) */
int u_goto_prev PARAMS((BW *bw));	/* move cursor to prev. word, edge,
					   or beginning of line */
int u_goto_next PARAMS((BW *bw));	/* move cursor to next word, edge,
					   or end of line */

int utomatch PARAMS((BW *bw));
int uuparw PARAMS((BW *bw));
int udnarw PARAMS((BW *bw));
int utos PARAMS((BW *bw));
int ubos PARAMS((BW *bw));
void scrup PARAMS((BW *bw, int n, int flg));
void scrdn PARAMS((BW *bw, int n, int flg));
int upgup PARAMS((BW *bw));
int upgdn PARAMS((BW *bw));
int uupslide PARAMS((BW *bw));
int udnslide PARAMS((BW *bw));
int uline PARAMS((BW *bw));
int udelch PARAMS((BW *bw));
int ubacks PARAMS((BW *bw, int k));
int u_word_delete PARAMS((BW *bw));
int ubackw PARAMS((BW *bw));
int udelel PARAMS((BW *bw));
int udelbl PARAMS((BW *bw));
int udelln PARAMS((BW *bw));
int uinsc PARAMS((BW *bw));
int utypebw PARAMS((BW *bw, int k));
int uquote PARAMS((BW *bw));
int uquote8 PARAMS((BW *bw));
int rtntw PARAMS((BW *bw));
int uopen PARAMS((BW *bw));
int usetmark PARAMS((BW *bw, int c));
int ugomark PARAMS((BW *bw, int c));
int ufwrdc PARAMS((BW *bw, int k));
int ubkwdc PARAMS((BW *bw, int k));
int umsg PARAMS((BASE *b));
int uctrl PARAMS((BW *bw));
int unedge PARAMS((BW *bw));
int upedge PARAMS((BW *bw));
int ubyte PARAMS((BW *bw));
int ucol PARAMS((BW *bw));
int utxt PARAMS((BW *bw));
int uhome PARAMS((BW *bw));
int uname_joe PARAMS((BW *bw));
int upaste PARAMS((BW *bw, int k));
int ubrpaste PARAMS((BW *bw, int k));

#endif
