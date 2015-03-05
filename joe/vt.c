/* Terminal emulator */

#include "types.h"

VT *mkvt(B *b, int top, int height, int width)
{
	VT *vt = joe_malloc(sizeof(VT));
	vt->b = b;
	vt->vtcur = pdup(b->eof, USTR "vt");
	vt->state = vt_idle;
	vt->top = top;
	vt->regn_top = 0;
	vt->regn_bot = height;
	vt->height = height;
	vt->width = width;
	vt->argc = 0;
	vt->xn = 0;
	vt->kbd = mkkbd(kmap_getcontext("shell"));
	return vt;
}

void vt_resize(VT *vt, int top, int height, int width)
{
	int flag = 0;
	if (vt->regn_bot == vt->height)
		flag = 1;
	vt->height = height;
	vt->width = width;
	vt->top = top;
	if (vt->regn_top > height)
		vt->regn_top = height;
	if (vt->regn_bot > height)
		vt->regn_bot = height;
	if (flag)
		vt->regn_bot = height;
	if (vt->vtcur->line >= vt->top + height)
		vt->top = vt->vtcur->line - height + 1;
}

void vtrm(VT *vt)
{
	if (vt->vtcur)
		prm(vt->vtcur);
	rmkbd(vt->kbd);
	joe_free(vt);
}

void vt_beep(VT *bw)
{
	ttputc(7);
	bw->xn = 0;
}

void vt_type(VT *bw, int c)
{
	int col;
	bw->xn = 0;
	if (piseol(bw->vtcur)) {
		binsc(bw->vtcur, c);
		pgetc(bw->vtcur);
	} else {
		P *q = pdup(bw->vtcur, USTR "vt_type");
		int col = piscol(q);
		pcol(q, col + 1);
		bdel(bw->vtcur, q);
		prm(q);
		binsc(bw->vtcur, c);
		pgetc(bw->vtcur);
	}
	col = piscol(bw->vtcur);
	if (col >= bw->width) {
		if (bw->b->eof->line != bw->vtcur->line) {
			pnextl(bw->vtcur);
		} else {
			pnextl(bw->vtcur);
			binsc(bw->vtcur, '\n');
			pgetc(bw->vtcur);
		}
		bw->xn = 1; /* Ignore next newline */
	}
}

void vt_lf(VT *bw)
{
	int col;
	if (bw->xn) {
		bw->xn = 0;
		return;
	}
	col = piscol(bw->vtcur);
	if (bw->vtcur->line == bw->top + bw->regn_bot - 1) {
		if (bw->regn_top != 0) {
			/* Delete top line */
			P *p = pdup(bw->vtcur, USTR "vt_lf");
			P *q;
			pline(p, bw->top + bw->regn_top);
			q = pdup(p, USTR "vt_lf");
			pnextl(q);
			bdel(p, q);
			prm(q);
			prm(p);
		} else {
			W *w;
			/* Save top line in buffer */
			++bw->top;
			vt_scrdn();
		}
		/* Move to next line: add line if we are at end */
		if (!pnextl(bw->vtcur)) {
			binsc(bw->vtcur, '\n');
			pgetc(bw->vtcur);
		} else { /* Insert if we are not at end */
			p_goto_bol(bw->vtcur);
			binsc(bw->vtcur, '\n');
		}
	} else {
		/* Move to next line: add line if we are at end */
		if (!pnextl(bw->vtcur)) {
			binsc(bw->vtcur, '\n');
			pgetc(bw->vtcur);
		}
	}
	pcol(bw->vtcur, col);
	pfill(bw->vtcur, col, ' ');
	if (bw->vtcur->line >= bw->top + bw->height)
		bw->top = bw->vtcur->line - bw->height + 1;
}

void vt_insert_spaces(VT *bw, int n)
{
	bw->xn = 0;
	while (n--)
		binsc(bw->vtcur, ' ');
}

void vt_cr(VT *bw)
{
	/* Note: No bw->xn = 0; Here. */
	p_goto_bol(bw->vtcur);
}

void vt_tab(VT *bw)
{
	bw->xn = 0;
	if (piseol(bw->vtcur)) {
		binsc(bw->vtcur, '\t');
		pgetc(bw->vtcur);
	} else {
		int col = piscol(bw->vtcur);
		col += bw->vtcur->b->o.tab - col % bw->vtcur->b->o.tab;
		pcol(bw->vtcur, col);
		pfill(bw->vtcur, col, ' ');
	}
}

void vt_left(VT *bw, int n)
{
	int col = piscol(bw->vtcur);
	bw->xn = 0;
	if (n > col)
		col = 0;
	else
		col -= n;
	pcol(bw->vtcur, col);
	pfill(bw->vtcur, col, ' ');
}

void vt_right(VT *bw, int n)
{
	int col = piscol(bw->vtcur);
	bw->xn = 0;
	col += n;
	if (col >= bw->width)
		col = bw->width - 1;
	pcol(bw->vtcur, col);
	pfill(bw->vtcur, col, ' ');
}

void vt_up(VT *bw, int n)
{
	long line = bw->vtcur->line;
	int col = piscol(bw->vtcur);
	bw->xn = 0;
	line -= n;
	if (line < bw->top) {
		line = bw->top;
	}
	pline(bw->vtcur, line);
	pcol(bw->vtcur, col);
	pfill(bw->vtcur, col, ' ');
}

void vt_reverse_lf(VT *bw)
{
	bw->xn = 0;
	if (bw->vtcur->line >= bw->top) {
		if (bw->vtcur->line != bw->top + bw->regn_top)
			vt_up(bw, 1);
		else {
			int col;

			/* Delete last line */
			if (bw->vtcur->b->eof->line >= bw->top + bw->regn_bot - 1) {
				P *q = pdup(bw->vtcur, USTR "vt_reverse_lf");
				P *r;
				pline(q, bw->top + bw->regn_bot - 1);
				r = pdup(q, USTR "vt_reverse_lf1");
				pnextl(r);
				bdel(q, r);
				prm(r);
				prm(q);
			}

			/* Scroll up */
			col = piscol(bw->vtcur);
			p_goto_bol(bw->vtcur);
			binsc(bw->vtcur, '\n');
			pcol(bw->vtcur, col);
			pfill(bw->vtcur, col, ' ');
		}
	}
}

void vt_down(VT *bw, int n)
{
	long line = bw->vtcur->line;
	int col = piscol(bw->vtcur);
	bw->xn = 0;
	line += n;
	if (line >= bw->top + bw->height)
		line = bw->top + bw->height - 1;
	while (line > bw->b->eof->line) {
		p_goto_eof(bw->vtcur);
		binsc(bw->vtcur, '\n');
		pgetc(bw->vtcur);
	}
	pline(bw->vtcur, line);
	pcol(bw->vtcur, col);
	pfill(bw->vtcur, col, ' ');
}

void vt_col(VT *bw, int col)
{
	bw->xn = 0;
	pcol(bw->vtcur, col);
	pfill(bw->vtcur, col, ' ');
}

void vt_row(VT *bw, int row)
{
	int col = piscol(bw->vtcur);
	long line = bw->top + row;
	bw->xn = 0;
	if (line >= bw->top + bw->height)
		line = bw->top + bw->height - 1;
	while (line > bw->b->eof->line) {
		p_goto_eof(bw->vtcur);
		binsc(bw->vtcur, '\n');
		pgetc(bw->vtcur);
	}
	pline(bw->vtcur, line);
	pcol(bw->vtcur, col);
	pfill(bw->vtcur, col, ' ');
}

void vt_erase_eos(VT *bw)
{
	P *p = pdup(bw->b->eof, USTR "vt_erase_line");
	bw->xn = 0;
	bdel(bw->vtcur, p);
	prm(p);
}

void vt_erase_bos(VT *bw)
{
}

void vt_erase_screen(VT *bw)
{
	long li = bw->vtcur->line;
	int col = piscol(bw->vtcur);
	bw->xn = 0;
	pline(bw->vtcur, bw->top);
	bdel(bw->vtcur, bw->b->eof);
	while (bw->vtcur->line < li) {
		binsc(bw->vtcur, '\n');
		pgetc(bw->vtcur);
	}
	pcol(bw->vtcur, col);
	pfill(bw->vtcur, col, ' ');
}

void vt_erase_line(VT *bw)
{
	P *p = pdup(bw->vtcur, USTR "vt_erase_line");
	int col = piscol(bw->vtcur);
	bw->xn = 0;
	p_goto_bol(bw->vtcur);
	pnextl(p);
	if (bw->vtcur->byte == p->byte) {
		/* Do nothing */
	} else {
		bdel(bw->vtcur, p);
	}
	prm(p);
	pcol(bw->vtcur, col);
	pfill(bw->vtcur, col, ' ');
}

void vt_erase_bol(VT *bw)
{
}

void vt_erase_eol(VT *bw)
{
	P *p = p_goto_eol(pdup(bw->vtcur, USTR "vt_erase_eol"));
	bw->xn = 0;
	if (bw->vtcur->byte == p->byte) {
		/* Do nothing */
	} else
		bdel(bw->vtcur, p);
	prm(p);
}

void vt_insert_lines(VT *bw, int n)
{
	bw->xn = 0;
	if (bw->vtcur->line < bw->top + bw->regn_bot)
		while (n--) {
			P *p = pdup(bw->vtcur, USTR "vt_insert_lines");
			p_goto_bol(p);
			binsc(p, '\n');
			pline(p, bw->top + bw->regn_bot);
			if (p->line == bw->top + bw->regn_bot) {
				P *q = pdup(p, USTR "vt_insert_lines");
				if (pnextl(q)) {
					bdel(p, q);
				}
				prm(q);
			}
			prm(p);
		}
}

void vt_delete_lines(VT *bw, int n)
{
	bw->xn = 0;
	if (bw->vtcur->line < bw->top + bw->regn_bot)
		while (n--) {
			int col = piscol(bw->vtcur);
			P *a = pdup(bw->vtcur, USTR "vt_delete_lines");
			P *p;
			P *q;
			pline(a, bw->top + bw->regn_bot);
			if (a->line == bw->top + bw->regn_bot) {
				binsc(a, '\n');
			}
			prm(a);
			p = pdup(bw->vtcur, USTR "vt_delete_lines");
			q = pdup(p, USTR "vt_delete_lines");
			p_goto_bol(p);
			if (pnextl(q)) {
				bdel(p, q);
			}
			prm(q);
			prm(p);
			pcol(bw->vtcur, col);
		}
}

void vt_delete_chars(VT *bw, int n)
{
	bw->xn = 0;
	while (n && !piseol(bw->vtcur)) {
		P *q = pdup(bw->vtcur, USTR "vt_delete_chars");
		pgetc(q);
		bdel(bw->vtcur, q);
		prm(q);
		n--;
	}
}

void vt_scroll_up(VT *bw, int n)
{
}

void vt_scroll_down(VT *bw, int n)
{
}

void vt_erase_chars(VT *bw, int n)
{
}

void vt_set_region(VT *bw, int top, int bot)
{
	bw->xn = 0;
	if (top < bw->height && bot < bw->height && top <= bot) {
		bw->regn_top = top;
		bw->regn_bot = bot + 1;
	}
}

/*
Odd VT100 codes:

ESC (	default font
ESC )	alternate font

ESC H		Set tab
ESC [ g		Clear tab at current position
ESC [ 3 g	Clear all tabs

ESC [ i		Print screen
ESC [ 1 i	Print line
ESC [ 4 i	Stop print log
ESC [ 5 i	Start print log

VT100 reset sequence:
ESC [ ? 8 h
ESC [ ? 7 h
ESC [ ? 5 l
ESC [ ? 4 l
ESC [ ? 3 l
ESC >

ks
ESC [ ? 1 h	application cursor keys
ESC =		application keypad

ke
ESC [ ? 1 l	normal cursor keys
ESC >		normal keypad

eA
ESC ( B
ESC ) 0

ct
ESC [ 3 g

as  alternate font
^N

ae
^O

SA (ANSI.SYS)
ESC [ ? 7 h	enable line wrap

RA (ANSI.SYS)
ESC [ ? 7 l	disable line wrap

ANSI.SYS

u7
ESC [ 6 n

u6
ESC [ %d ; %d R

pk
ESC [ key-ascii ; %s p	define key

is
ESC [ m
ESC [ ? 7 h

ESC [ ? 25 h  show cursor
ESC [ ? 25 l  hide cursor
*/

int vt_arg(VT *vt, int argn, int dflt)
{
	while (vt->argc <= argn) {
		vt->argv[vt->argc++] = 0;
	}
	if (!vt->argv[argn])
		vt->argv[argn] = dflt;
	return vt->argv[argn];
}

MACRO *vt_data(VT *vt, unsigned char **indat, int *insiz)
{
	unsigned char *dat = *indat;
	int siz = *insiz;
	while (siz--) {
		unsigned char c = *dat++;
		switch (vt->state) {
			case vt_idle: {
				switch (c) {
					case 5: { /* ENQ- Return terminal status */
						break;
					} case 7: { /* BEL- vt100, linux: Beep */
						vt_beep(vt);
						break;
					} case 8: { /* BS- vt100, linux: Backspace */
						vt_left(vt, 1);
						break;
					} case 9: { /* HT- vt100, linux: Tab */
						vt_tab(vt);
						break;
					} case 10: { /* LF- vt100, linux: LF */
						vt_lf(vt);
						break;
					} case 11: { /* VT- vt100: VT, same as LF */
						vt_lf(vt);
					} case 12: { /* FF- linux: FF, same as LF */
						vt_lf(vt);
						break;
					} case 13: { /* CR- vt100, linux; Return */
						vt_cr(vt);
						break;
					} case 14: { /* SO- vt100: Alt char set */
						break;
					} case 15: { /* SI- vt100: Regular char set */
						break;
					} case 27: { /* ESC */
						vt->bufx = 0;
						vt->buf[vt->bufx++] = 27;
						vt->state = vt_esc;
						break;
					} case 0x84: { /* Same as ESC D */
						vt_lf(vt);
						break;
					} case 0x85: { /* Same as ESC E */
						vt_cr(vt);
						vt_lf(vt);
						break;
					} case 0x88: { /* Same as ESC H */
						break;
					} case 0x8D: { /* Same as ESC M */
						vt_reverse_lf(vt);
						break;
					} case 0x8E: { /* Same as ESC N */
						break;
					} case 0x8F: { /* Same as ESC O */
						break;
					} case 0x90: { /* Same as ESC P */
						break;
					} case 0x96: { /* Same as ESC V */
						break;
					} case 0x97: { /* Same as ESC W */
						break;
					} case 0x98: { /* Same as ESC X */
						break;
					} case 0x9A: { /* Same as ESC [ c    (return terminal id) */
						break;
					} case 0x9B: { /* Same as ESC [, CSI */
						vt->argc = 0;
						vt->argv[0] = 0;
						vt->state = vt_args;
						vt->bufx = 0;
						vt->buf[vt->bufx++] = 27;
						vt->buf[vt->bufx++] = '[';
						break;
					} case 0x9C: { /* Same as ESC \ */
						break;
					} case 0x9D: { /* Same as ESC ] */
						break;
					} case 0x9E: { /* Same as ESC ^ */
						break;
					} case 0x9F: { /* Same as ESC _ */
						break;
					} default: { /* Type regular character */
						if (c >= 32 && c <= 126)
							vt_type(vt, c);
						break;
					}
				}
				break;
			} case vt_esc: {
				if (c == 'c') { /* vt100: Reset */
					vt->state = vt_idle;
					break;
				} else if (c == 'M') { /* vt100: Go up, scroll if at edge */
					vt_reverse_lf(vt);
					vt->state = vt_idle;
					break;
				} else if (c == 'D') { /* vt100: Go dowm, scroll if at edge */
					vt_lf(vt);
					vt->state = vt_idle;
					break;
				} else if (c == 'E') { /* Goto down and to start of line.  Scroll if at edge */
					vt_cr(vt);
					vt_lf(vt);
					vt->state = vt_idle;
					break;
				} else if (c == 'F') { /* Goto lower left of screen */
					break;
				} else if (c == 'H') { /* Set tab */
					break;
				} else if (c == '7') { /* vt100: save position and attributes */
					break;
				} else if (c == '8') { /* vt100: restore position and attributes (home, initially) */
					break;
				} else if (c == '[') { /* CSI */
					vt->argc = 0;
					vt->argv[0] = 0;
					vt->state = vt_args;
					vt->buf[vt->bufx++] = '[';
				} else if (c == '{') {
					vt->bufx = 0;
					vt->state = vt_cmd;
				} else { /* Ignore the rest... */
					vt->state = vt_idle;
				}
				break;
			} case vt_cmd: {
				if (c != '}') {
				/* if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == ',' || c == '_') { */
					if (vt->bufx < sizeof(vt->buf) - 1)
						vt->buf[vt->bufx++] = c;
				} else {
					if (c == '}') {
						MACRO *m;
						int rtn;
						vt->buf[vt->bufx] = 0;
						/* printf("Got command '%s'\r\n", vt->buf); */
						m = mparse(NULL, vt->buf, &rtn);
						if (rtn >= 0) {
							*insiz = siz;
							*indat = dat;
							vt->state = vt_idle;
							return m;
							/* FIXME should only do this if cursor is on window */
							exmacro(m, 1);
							edupd(1);
						}
						rmmacro(m);
					}
					vt->state = vt_idle;
				}
				break;
			} case vt_args: {
				if (vt->bufx < sizeof(vt->buf) - 1)
					vt->buf[vt->bufx++] = c;
				if (c >= '0' && c <= '9') {
					if (vt->argc < MAXARGS)
						vt->argv[vt->argc] = vt->argv[vt->argc] * 10 + c - '0';
				} else if (c == ';') {
					if (vt->argc < MAXARGS) {
						++vt->argc;
						vt->argv[vt->argc] = 0;
					}
				} else if (c == '?') {
					/* Options */
				} else {
					int x;
					vt->state = vt_idle;
					if (vt->argc < MAXARGS) {
						++vt->argc;
					}
					switch (c) {
						case '@': { /* Insert spaces */
							vt_insert_spaces(vt, vt_arg(vt, 0, 1));
							break;
						} case 'A': { /* VT100, ansi.sys Go up */
							vt_up(vt, vt_arg(vt, 0, 1));
							break;
						} case 'B': { /* Go down */
							vt_down(vt, vt_arg(vt, 0, 1));
							break;
						} case 'C': { /* Go right */
							vt_right(vt, vt_arg(vt, 0, 1));
							break;
						} case 'D': { /* Go left */
							vt_left(vt, vt_arg(vt, 0, 1));
							break;
						} case 'E': { /* Beginning of line, n lines down */
							vt_cr(vt);
							vt_down(vt, vt_arg(vt, 0, 1));
							break;
						} case 'F': { /* Beginning of line, n line up */
							vt_cr(vt);
							vt_up(vt, vt_arg(vt, 0, 1));
							break;
						} case 'G': { /* Goto col, keep row */
							vt_col(vt, vt_arg(vt, 0, 1) - 1);
							break;
						} case 'f': case 'H': { /* With one args: left edge of row, with two; goto row, col */
							vt_row(vt, vt_arg(vt, 0, 1) - 1);
							vt_col(vt, vt_arg(vt, 1, 1) - 1);
							break;
						} case 'J': { /* Erase screen: default=to end of screen, 1=to beg. of screen, 2=entire screen */
							switch (vt_arg(vt, 0, 0)) {
								case 0: {
									vt_erase_eos(vt);
									break;
								} case 1: {
									vt_erase_bos(vt);
									break;
								} case 2: {
									vt_erase_screen(vt);
									break;
								}
							}
							break;
						} case 'K': { /* Erase line: default=to end of line, 1=to beg. of line, 2=entire line */
							switch (vt_arg(vt, 0, 0)) {
								case 0: {
									vt_erase_eol(vt);
									break;
								} case 1: {
									vt_erase_bol(vt);
									break;
								} case 2: {
									vt_erase_line(vt);
									break;
								}
							}
							break;
						} case 'L': { /* Insert lines */
							vt_insert_lines(vt, vt_arg(vt, 0, 1));
							break;
						} case 'M': { /* Delete lines */
							vt_delete_lines(vt, vt_arg(vt, 0, 1));
							break;
						} case 'P': { /* Delete characters */
							vt_delete_chars(vt, vt_arg(vt, 0, 1));
							break;
						} case 'S': { /* Scroll screen n lines up (no cursor move) */
							vt_scroll_up(vt, vt_arg(vt, 0, 1));
							break;
						} case 'T': { /* Scroll screen n lines down (no cursor move) */
							vt_scroll_down(vt, vt_arg(vt, 0, 1));
							break;
						} case 'X': { /* Erase n characters to right */
							vt_erase_chars(vt, vt_arg(vt, 0, 1));
							break;
						} case 'd': { /* Goto row, keep col */
							vt_row(vt, vt_arg(vt, 0, 1) - 1);
							break;
						} case 'h': { /* Set mode: 4 for insert mode */
							break;
						} case 'k': { /* Ansi.sys: ce */
							break;
						} case 'l': { /* Clear mode: 4 for insert mode */
							break;
						} case 'm': { /* Attributes:
									ansi.sys: 1=bold
									vt100 ansi.sys: 4=understrike,
									vt100 5=blink,
									vt100 ansi.sys: 7=inverse,
									ansi.sys: 8= conceal?
									ansi.sys: 10= primary font?
									ansi.sys: 11-19= alternate font?
									vt100 ansi.sys: default=understrike,bold,inverse off
									ansi.sys: 30 - 37: foreground color
									ansi.sys: 40 - 47: background color */
							int x;
							for (x = 0; x != vt->bufx; ++x) {
								binsc(vt->vtcur, vt->buf[x]);
								pgetb(vt->vtcur);
								vt->vtcur->valcol = 0;
							}
							break;
						} case 'n': { /* 6: send cursor position as ESC [ row ; col R */
							break;
						} case 'r': { /* vt100: Set scrolling region */
							if (vt->argc < 2)
								vt_set_region(vt, 0, vt->height - 1);
							else
								vt_set_region(vt, vt_arg(vt, 0, 1) - 1, vt_arg(vt, 1, vt->height) - 1);
							vt_row(vt, 0);
							vt_col(vt, 0);
							break;
						} case 's': { /* ansi.sys: save cursor position */
							break;
						} case 'u': { /* ansi.sys: restore cursor position */
							break;
						}
					}
				}
				break;
			}
		}
	}
	return 0;
}
