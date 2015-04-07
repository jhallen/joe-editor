/*
 *	Basic user edit functions
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

extern int pgamnt; /* Amount of pgup/pgdn lines to keep */

/*
 * Movable functions
 *	return 0 if action was done
 *	return -1 otherwise
 */
int u_goto_bol(W *w, int k);	/* move cursor to beginning of line */
int u_goto_eol(W *w, int k);	/* move cursor to end of line */
int u_goto_bof(W *w, int k);	/* move cursor to beginning of file */
int u_goto_eof(W *w, int k);	/* move cursor to end of file */
int u_goto_left(W *w, int k);	/* move cursor to left (left arrow) */
int u_goto_right(W *w, int k);	/* move cursor to right (right arrow) */
int u_goto_prev(W *w, int k);	/* move cursor to prev. word, edge,
					   or beginning of line */
int u_goto_next(W *w, int k);	/* move cursor to next word, edge,
					   or end of line */

int utomatch(W *w, int k);
int uuparw(W *w, int k);
int udnarw(W *w, int k);
int utos(W *w, int k);
int ubos(W *w, int k);
void scrup(BW *bw, ptrdiff_t n, int flg);
void scrdn(BW *bw, ptrdiff_t n, int flg);
int upgup(W *w, int k);
int upgdn(W *w, int k);
int uupslide(W *w, int k);
int udnslide(W *w, int k);
int uline(W *w, int k);
int udelch(W *w, int k);
int ubacks(W *w, int k);
int u_word_delete(W *w, int k);
int ubackw(W *w, int k);
int udelel(W *w, int k);
int udelbl(W *w, int k);
int udelln(W *w, int k);
int uinsc(W *w, int k);
int utypebw(BW *bw, int k);
int utypew(W *w, int k);
int uquote(W *w, int k);
int uquote8(W *w, int k);
int rtntw(W *w);
int uopen(W *w, int k);
int usetmark(W *w, int c);
int ugomark(W *w, int c);
int ufwrdc(W *w, int k);
int ubkwdc(W *w, int k);
int umsg(W *w, int k);
int uctrl(W *w, int k);
int unedge(W *w, int k);
int upedge(W *w, int k);
int ubyte(W *w, int k);
int ucol(W *w, int k);
int utxt(W *w, int k);
int uhome(W *w, int k);
int uname_joe(W *w, int k);
int upaste(W *w, int k);
int ubrpaste(W *w, int k);
