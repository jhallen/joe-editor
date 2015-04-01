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
int u_goto_bol(BW *bw);	/* move cursor to beginning of line */
int u_goto_eol(BW *bw);	/* move cursor to end of line */
int u_goto_bof(BW *bw);	/* move cursor to beginning of file */
int u_goto_eof(BW *bw);	/* move cursor to end of file */
int u_goto_left(BW *bw);	/* move cursor to left (left arrow) */
int u_goto_right(BW *bw);	/* move cursor to right (right arrow) */
int u_goto_prev(BW *bw);	/* move cursor to prev. word, edge,
					   or beginning of line */
int u_goto_next(BW *bw);	/* move cursor to next word, edge,
					   or end of line */

int utomatch(BW *bw);
int uuparw(BW *bw);
int udnarw(BW *bw);
int utos(BW *bw);
int ubos(BW *bw);
void scrup(BW *bw, int n, int flg);
void scrdn(BW *bw, int n, int flg);
int upgup(BW *bw);
int upgdn(BW *bw);
int uupslide(BW *bw);
int udnslide(BW *bw);
int uline(BW *bw);
int udelch(BW *bw);
int ubacks(BW *bw, int k);
int u_word_delete(BW *bw);
int ubackw(BW *bw);
int udelel(BW *bw);
int udelbl(BW *bw);
int udelln(BW *bw);
int uinsc(BW *bw);
int utypebw(BW *bw, int k);
int uquote(BW *bw);
int uquote8(BW *bw);
int rtntw(BW *bw);
int uopen(BW *bw);
int usetmark(BW *bw, int c);
int ugomark(BW *bw, int c);
int ufwrdc(BW *bw, int k);
int ubkwdc(BW *bw, int k);
int umsg(BASE *b);
int uctrl(BW *bw);
int unedge(BW *bw);
int upedge(BW *bw);
int ubyte(BW *bw);
int ucol(BW *bw);
int utxt(BW *bw);
int uhome(BW *bw);
int uname_joe(BW *bw);
int upaste(BW *bw, int k);
int ubrpaste(BW *bw, int k);

#endif
