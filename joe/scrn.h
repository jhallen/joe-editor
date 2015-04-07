/*
 *	Device independant tty interface for JOE
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

struct hentry {
	ptrdiff_t	next;
	ptrdiff_t	loc;
};

/* Each terminal has one of these: terminal capability database */

#ifdef __MSDOS__

struct scrn {
	ptrdiff_t	li;		/* Height of screen */
	ptrdiff_t	co;		/* Width of screen */
	short	*scrn;		/* Buffer */
	int	scroll;
	int	insdel;
	int	*updtab;	/* Lines which need to be updated */
	HIGHLIGHT_STATE *syntax;
	int	*compose;
	ptrdiff_t	*sary;
};

#else

struct scrn {
	CAP	*cap;		/* Termcap/Terminfo data */

	ptrdiff_t	li;		/* Screen height */
        ptrdiff_t	co;		/* Screen width */

	const char	*ti;		/* Initialization string */
	const char	*cl;		/* Home and clear screen... really an
				   init. string */
	const char	*cd;		/* Clear to end of screen */
	const char	*te;		/* Restoration string */
	const char	*brp;		/* Bracketed paste mode */
	const char	*bre;		/* Stop bracketed paste */

	int	haz;		/* Terminal can't print ~s */
	int	os;		/* Terminal overstrikes */
	int	eo;		/* Can use blank to erase even if os */
	int	ul;		/* _ overstrikes */
	int	am;		/* Terminal has autowrap, but not magicwrap */
	int	xn;		/* Terminal has magicwrap */

	const char	*so;		/* Enter standout (inverse) mode */
	const char	*se;		/* Exit standout mode */

	const char	*us;		/* Enter underline mode */
	const char	*ue;		/* Exit underline mode */
	const char	*uc;		/* Single time underline character */

	int	ms;		/* Ok to move when in standout/underline mode */

	const char	*mb;		/* Enter blinking mode */
	const char	*md;		/* Enter bold mode */
	const char	*mh;		/* Enter dim mode */
	const char	*mr;		/* Enter inverse mode */
	const char	*me;		/* Exit above modes */

	const char	*ZH;		/* Enter italic mode */
	const char	*ZR;		/* Exit italic mode */

	const char	*Sb;		/* Set background color */
	const char	*Sf;		/* Set foregrond color */
	int	Co;			/* No. of colors */
	int	ut;		/* Screen erases with background color */

	int	da, db;		/* Extra lines exist above, below */
	const char	*al, *dl, *AL, *DL;	/* Insert/delete lines */
	const char	*cs;		/* Set scrolling region */
	int	rr;		/* Set for scrolling region relative addressing */
	const char	*sf, *SF, *sr, *SR;	/* Scroll */

	const char	*dm, *dc, *DC, *ed;	/* Delete characters */
	const char	*im, *ic, *IC, *ip, *ei;	/* Insert characters */
	int	mi;		/* Set if ok to move while in insert mode */

	const char	*bs;		/* Move cursor left 1 */
	ptrdiff_t	cbs;
	const char	*lf;		/* Move cursor down 1 */
	ptrdiff_t	clf;
	const char	*up;		/* Move cursor up 1 */
	ptrdiff_t	cup;
	const char	*nd;		/* Move cursor right 1 */

	const char	*ta;		/* Move cursor to next tab stop */
	ptrdiff_t	cta;
	const char	*bt;		/* Move cursor to previous tab stop */
	ptrdiff_t	cbt;
	ptrdiff_t	tw;		/* Tab width */

	const char	*ho;		/* Home cursor to upper left */
	ptrdiff_t	cho;
	const char	*ll;		/* Home cursor to lower left */
	ptrdiff_t	cll;
	const char	*cr;		/* Move cursor to left edge */
	ptrdiff_t	ccr;
	const char	*RI;		/* Move cursor right n */
	ptrdiff_t	cRI;
	const char	*LE;		/* Move cursor left n */
        ptrdiff_t	cLE;
	const char	*UP;		/* Move cursor up n */
	ptrdiff_t	cUP;
	const char	*DO;		/* Move cursor down n */
	ptrdiff_t	cDO;
	const char	*ch;		/* Set cursor column */
	ptrdiff_t	cch;
	const char	*cv;		/* Set cursor row */
	ptrdiff_t	ccv;
	const char	*cV;		/* Goto beginning of specified line */
	ptrdiff_t	ccV;
	const char	*cm;		/* Set cursor row and column */
	ptrdiff_t	ccm;

	const char	*ce;		/* Clear to end of line */
	ptrdiff_t	cce;

	int assume_256;		/* Assume terminal has 256 color mode, but use
	                           regular mode for standard colors just in case */

	/* Basic abilities */
	int	scroll;		/* Set to use scrolling */
	int	insdel;		/* Set to use insert/delete within line */

	/* Current state of terminal */
	int	*scrn;		/* Characters on screen */
	int	*attr;		/* Attributes on screen */
	ptrdiff_t	x, y;		/* Current cursor position (-1 for unknown) */
	ptrdiff_t	top, bot;	/* Current scrolling region */
	int	attrib;		/* Current character attributes */
	int	ins;		/* Set if we're in insert mode */

	int	*updtab;	/* Dirty lines table */
	int	avattr;		/* Bits set for available attributes */
	ptrdiff_t	*sary;		/* Scroll buffer array */

	int	*compose;	/* Line compose buffer */
	ptrdiff_t	*ofst;		/* stuff for magic */
	struct hentry	*htab;
	struct hentry	*ary;
};

#endif

extern int skiptop;

/* SCRN *nopen(void);
 *
 * Open the screen (sets TTY mode so that screen may be used immediatly after
 * the 'nopen').
 */
SCRN *nopen(CAP *cap);

/* int nresize(SCRN *t,int w,int h);
 *
 * Change size of screen.  For example, call this when you find out that
 * the Xterm changed size.
 */
int nresize(SCRN *t, ptrdiff_t w, ptrdiff_t h);

/* void nredraw(SCRN *t);
 *
 * Invalidate all state variables for the terminal.  This way, everything gets
 * redrawn.
 */
void nredraw(SCRN *t);

void npartial(SCRN *t);
void nescape(SCRN *t);
void nreturn(SCRN *t);

/* void nclose(SCRN *t);
 *
 * Close the screen and restore TTY to initial state.
 *
 * if 'flg' is set, tclose doesn't mess with the signals.
 */
void nclose(SCRN *t);

/* int cpos(SCRN *t,int x,int y);
 *
 * Set cursor position
 */
int cpos(register SCRN *t, register ptrdiff_t x, register ptrdiff_t y);

/* int attr(SCRN *t,int a);
 *
 * Set attributes
 */
int set_attr(SCRN *t, int c);

/* Encode character as utf8 */
void utf8_putc(int c);

/* void outatr(SCRN *t,int *scrn,int *attr,int x,int y,int c,int a);
 *
 * Output a character at the given screen cooridinate.  The cursor position
 * after this function is executed is indeterminate.
 */

/* Character attribute bits */

#ifdef __MSDOS__

#define INVERSE 1
#define UNDERLINE 2
#define BOLD 4
#define BLINK 8
#define DIM 16
#define ITALIC 32
extern unsigned atab[];

#define outatr(t,scrn,attr,x,y,c,a) do { \
	(t); \
	(x); \
	(y); \
	*(scrn) = ((unsigned)(c) | atab[a]); \
} while(0)

#else

#define CONTEXT_COMMENT	1
#define CONTEXT_STRING	2
#define CONTEXT_MASK	(CONTEXT_COMMENT+CONTEXT_STRING)

#define ITALIC		 128
#define INVERSE		 256
#define UNDERLINE	 512
#define BOLD		1024
#define BLINK		2048
#define DIM		4096
#define AT_MASK		(INVERSE+UNDERLINE+BOLD+BLINK+DIM+ITALIC)

#define BG_SHIFT	13
#define BG_VALUE	(255<<BG_SHIFT)
#define BG_NOT_DEFAULT	(256<<BG_SHIFT)
#define BG_MASK		(511<<BG_SHIFT)

#define BG_DEFAULT	(0<<BG_SHIFT)

/* #define BG_COLOR(color)	(BG_NOT_DEFAULT^(color)<<BG_SHIFT) */
#define BG_COLOR(color)	(color)

#define BG_BLACK	(BG_NOT_DEFAULT|(0<<BG_SHIFT))
#define BG_RED		(BG_NOT_DEFAULT|(1<<BG_SHIFT))
#define BG_GREEN	(BG_NOT_DEFAULT|(2<<BG_SHIFT))
#define BG_YELLOW	(BG_NOT_DEFAULT|(3<<BG_SHIFT))
#define BG_BLUE		(BG_NOT_DEFAULT|(4<<BG_SHIFT))
#define BG_MAGENTA	(BG_NOT_DEFAULT|(5<<BG_SHIFT))
#define BG_CYAN		(BG_NOT_DEFAULT|(6<<BG_SHIFT))
#define BG_WHITE	(BG_NOT_DEFAULT|(7<<BG_SHIFT))
#define BG_BBLACK	(BG_NOT_DEFAULT|(8<<BG_SHIFT))
#define BG_BRED		(BG_NOT_DEFAULT|(9<<BG_SHIFT))
#define BG_BGREEN	(BG_NOT_DEFAULT|(10<<BG_SHIFT))
#define BG_BYELLOW	(BG_NOT_DEFAULT|(11<<BG_SHIFT))
#define BG_BBLUE	(BG_NOT_DEFAULT|(12<<BG_SHIFT))
#define BG_BMAGENTA	(BG_NOT_DEFAULT|(13<<BG_SHIFT))
#define BG_BCYAN	(BG_NOT_DEFAULT|(14<<BG_SHIFT))
#define BG_BWHITE	(BG_NOT_DEFAULT|(15<<BG_SHIFT))

#define FG_SHIFT	22
#define FG_VALUE	(255<<FG_SHIFT)
#define FG_NOT_DEFAULT	(256<<FG_SHIFT)
#define FG_MASK		(511<<FG_SHIFT)

#define FG_DEFAULT	(0<<FG_SHIFT)
#define FG_BWHITE	(FG_NOT_DEFAULT|(15<<FG_SHIFT))
#define FG_BCYAN	(FG_NOT_DEFAULT|(14<<FG_SHIFT))
#define FG_BMAGENTA	(FG_NOT_DEFAULT|(13<<FG_SHIFT))
#define FG_BBLUE	(FG_NOT_DEFAULT|(12<<FG_SHIFT))
#define FG_BYELLOW	(FG_NOT_DEFAULT|(11<<FG_SHIFT))
#define FG_BGREEN	(FG_NOT_DEFAULT|(10<<FG_SHIFT))
#define FG_BRED		(FG_NOT_DEFAULT|(9<<FG_SHIFT))
#define FG_BBLACK	(FG_NOT_DEFAULT|(8<<FG_SHIFT))
#define FG_WHITE	(FG_NOT_DEFAULT|(7<<FG_SHIFT))
#define FG_CYAN		(FG_NOT_DEFAULT|(6<<FG_SHIFT))
#define FG_MAGENTA	(FG_NOT_DEFAULT|(5<<FG_SHIFT))
#define FG_BLUE		(FG_NOT_DEFAULT|(4<<FG_SHIFT))
#define FG_YELLOW	(FG_NOT_DEFAULT|(3<<FG_SHIFT))
#define FG_GREEN	(FG_NOT_DEFAULT|(2<<FG_SHIFT))
#define FG_RED		(FG_NOT_DEFAULT|(1<<FG_SHIFT))
#define FG_BLACK	(FG_NOT_DEFAULT|(0<<FG_SHIFT))

void outatr(struct charmap *map,SCRN *t,int *scrn,int *attrf,ptrdiff_t xx,ptrdiff_t yy,int c,int a);

#endif

/* int eraeol(SCRN *t,int x,int y);
 *
 * Erase from screen coordinate to end of line.
 */
int eraeol(SCRN *t, ptrdiff_t x, ptrdiff_t y, int atr);

/* void nscrlup(SCRN *t,int top,int bot,int amnt);
 *
 * Buffered scroll request.  Request that some lines up.  'top' and 'bot'
 * indicate which lines to scroll.  'bot' is the last line to scroll + 1.
 * 'amnt' is distance in lines to scroll.
 */
void nscrlup(SCRN *t, ptrdiff_t top, ptrdiff_t bot, ptrdiff_t amnt);

/* void nscrldn(SCRN *t,int top,int bot,int amnt);
 *
 * Buffered scroll request.  Scroll some lines down.  'top' and 'bot'
 * indicate which lines to scroll.  'bot' is the last line to scroll + 1.
 * 'amnt' is distance in lines to scroll.
 */
void nscrldn(SCRN *t, ptrdiff_t top, ptrdiff_t bot, ptrdiff_t amnt);

/* void nscroll(SCRN *t);
 *
 * Execute buffered scroll requests
 */
void nscroll(SCRN *t, int atr);

/* void magic(SCRN *t,int y,int *cur,int *new);
 *
 * Figure out and execute line shifting
 */
void magic(SCRN *t, ptrdiff_t y, int *cs, int *ca, int *s, int *a,ptrdiff_t placex);

int clrins(SCRN *t);

int meta_color(const char *s);

/* Generate a field */
void genfield(SCRN *t,int *scrn,int *attr,ptrdiff_t x,ptrdiff_t y,ptrdiff_t ofst,const char *s,ptrdiff_t len,int atr,ptrdiff_t width,int flg,int *fmt);

/* Column width of a string takes into account utf-8) */
ptrdiff_t txtwidth(const char *s,ptrdiff_t len);

off_t txtwidth1(struct charmap *map, off_t tabwidth, const char *s, ptrdiff_t len);

/* Generate a field: formatted */
void genfmt(SCRN *t, ptrdiff_t x, ptrdiff_t y, ptrdiff_t ofst, const char *s, int atr, int flg);

/* Column width of formatted string */
ptrdiff_t fmtlen(const char *s);

/* Offset within formatted string of particular column */
ptrdiff_t fmtpos(const char *s, ptrdiff_t goal);

extern int bg_text;
extern int columns;
extern int lines;
extern int notite;
extern int nolinefeeds;
extern int usetabs;
extern int assume_color;
extern int assume_256color;
