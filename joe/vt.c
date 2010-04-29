/* Terminal emulator */

#define MAXARGS 2

enum vt_state {
	vt_idle,
	vt_esc,
	vt_args,
};



struct vt_context {
	enum vt_state state;

	/* Numeric arguments */
	int argv[MAXARGS + 1];
	int argc;

	BW *bw;
};

void vt_beep(BW *bw)
{
	ttputc(7);
}

void vt_type(BW *bw, int c)
{
	int o = bw->o.overtype;
	int pid = bw->pid;

	/* Set proper modes.. */
	bw->pid = 0;
	bw->o.overtype = 1;

	utypebw(bw, c);

	/* Restore user modes */
	bw->o.overtype = o;
	bw->pid = pid;
}

void vt_lf(BW *bw)
{
	int col = bw->cursor->xcol;
	if (!pnextl(bw->cursor)) {
		binsc(bw->cursor, '\n');
		pgetc(bw->cursor);
	}
	pcol(bw->cursor, col);
}

void vt_reverse_lf(BW *bw)
{
	if (bw->cursor->line) {
		int col = bw->cursor->xcol;
		pprevl(bw->cursor);
		pcol(bw->cursor, col);
	} else {
		int col = bw->cursor->xcol;
		p_goto_bol(bw->cursor);
		binsc(bw->cursor, '\n');
		pcol(bw->cursor, col);
	}
}

void vt_insert_spaces(BW *bw, int n)
{
	while (n--)
		binsc(bw->cursor, ' ');
}

void vt_cr(BW *bw)
{
	p_goto_bol(bw->cursor);
}

void vt_tab(BW *bw)
{
	vt_type(vt, '\t');
}

void vt_left(BW *bw, int n)
{
	if (n > bw->cursor->xcol)
		bw->cursor->xcol = 0;
	else
		bw->cursor->xcol -= n;
	pcol(bw->cursor, bw->cursor->xcol);
}

void vt_right(BW *bw, int n)
{
	bw->cursor->xcol += n;
	pcol(bw->cursor, bw->cursor->xcol);
}

void vt_up(BW *bw, int n)
{
	if (n > bw->cursor->line)
		n = bw->cursor->line;
	while (n--) {
		pprevl(bw->cursor);
	}
	pcol(bw->cursor, bw->cursor->xcol);
}

void vt_down(BW *bw, int n)
{
	if (n > bw->b->eof->line - bw->cursor->line)
		n = bw->b->eof->line - bw->cursor->line;
	while (n--) {
		pnextl(bw->cursor);
	}
	pcol(bw->cursor, bw->cursor->xcol);
}

void vt_col(BW *bw, int col)
{
	bw->cursor->xcol = col;
	pcol(bw->cursor, bw->cursor->xcol);
}

void vt_row(BW *bw, int row)
{
	// Hmm...
}

void vt_erase_eos(BW *bw)
{
}

void vt_erase_bos(BW *bw)
{
}

void vt_erase_screen(BW *bw)
{
}

void vt_erase_line(BW *bw)
{
	P *p = pdup(bw->cursor, USTR "vt_erase_line");
	int col = bw->cursor->xcol;

	p_goto_bol(bw->cursor);
	pnextl(p);
	if (bw->cursor->byte == p->byte) {
		/* Do nothing */
	} else {
		bdel(bw->cursor, p);
	}
	prm(p);
	pcol(bw->cursor, col);
}

void vt_erase_bol(BW *bw)
{
}

void vt_erase_eol(BW *bw)
{
	P *p = p_goto_eol(pdup(bw->cursor, USTR "vt_erase_eol"));

	if (bw->cursor->byte == p->byte) {
		/* Do nothing */
	} else
		bdel(bw->cursor, p);
	prm(p);
	return 0;
}

void vt_insert_lines(BW *bw, int n)
{
}

void vt_delete_lines(BW *bw, int n)
{
}

void vt_delete_chars(BW *bw, int n)
{
}

void vt_scroll_up(BW *bw, int n)
{
}

void vt_scroll_down(BW *bw, int n)
{
}

void vt_erase_chars(BW *bw, int n)
{
}

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


void vt_data(struct vt_context *vt, unsigned char *dat, int siz)
{
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
						vt->args[0] = 0;
						vt->state = vt_args;
						break;
					} case 0x9C; { /* Same as ESC \ */
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
				} else if (c == 'H') { /* Set tab */
					break;
				} else if (c == '7') { /* vt100: save position and attributes */
					break;
				} else if (c == '8') { /* vt100: restore position and attributes (home, initially) */
					break;
				} else if (c == '[') { /* CSI */
					vt->argc = 0;
					vt->args[0] = 0;
					vt->state = vt_args;
				} else { /* Ignore the rest... */
					vt->state = vt_idle;
				}
				break;
			} case vt_args: {
				if (c >= '0' && c <= '9') {
					if (vt->argc < MAXARGS)
						vt->argv[vt->argc] = vt->argv[vt->argc] * 10 + c - '0';
				} else if (c == ';') {
					if (vt->argc < MAXARGS) {
						++vt->argc;
						vt->argv[vt->argc] = 0;
					}
				} else {
					int x;
					vt->state = vt_idle;
					if (vt->argc < MAXARGS) {
						++vt->argc;
					}
					/* Set default value of arguments */
					for (x = 0; x != vt->argc; ++x)
						if (vt->argv[x] == 0)
							vt->argv[x] = 1;
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
							break;
						} case 'n': { /* 6: send cursor position as ESC [ row ; col R */
							break;
						} case 'r': { /* vt100: Set scrolling region */
							vt_set_region(vt, vt_arg(vt, 0, 1) - 1, vt_arg(vt, 1, vt->height) - 1);
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
}
