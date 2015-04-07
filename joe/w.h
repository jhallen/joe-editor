/*
 *	Window management
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

struct watom {
	const char *context;	/* Context name */
	void	(*disp)(W *w, int flg);	/* Display window */
	void	(*follow)(W *w);/* Called to have window follow cursor */
	int	(*abort)(W *w);	/* Common user functions */
	int	(*rtn)(W *w);
	int	(*type)(W *w, int k);
	void	(*resize)(W *w,ptrdiff_t width, ptrdiff_t height);	/* Called when window changed size */
	void	(*move)(W *w, ptrdiff_t x, ptrdiff_t y);	/* Called when window moved */
	void	(*ins)(W *w, B *b,off_t l,off_t n,int flg);	/* Called on line insertions */
	void	(*del)(W *w, B *b,off_t l,off_t n,int flg);	/* Called on line deletions */
	int	what;		/* Type of this thing */
};

/* A screen with windows */

struct screen {
	SCRN	*t;		/* Screen data on this screen is output to */

	ptrdiff_t	wind;		/* Number of help lines on this screen */

	W	*topwin;	/* Top-most window showing on screen */
	W	*curwin;	/* Window cursor is in */

	ptrdiff_t	w, h;		/* Width and height of this screen */
};

/* Buffer stack entry */

struct bstack {
	struct bstack *next;
	B *b;
	P *top;
	P *cursor;
};

/* A window (base class) */

struct window {
	LINK(W)	link;		/* Linked list of windows in order they
				   appear on the screen */

	Screen	*t;		/* Screen this thing is on */

	ptrdiff_t	x, y, w, h;	/* Position and size of window */
				/* Currently, x = 0, w = width of screen. */
				/* y == -1 if window is not on screen */

	ptrdiff_t	ny, nh;		/* Temporary values for wfit */

	ptrdiff_t	reqh;		/* Requested new height or 0 for same */
				/* This is an argument for wfit */

	ptrdiff_t	fixed;		/* If this is zero, use 'hh'.  If not, this
				   is a fixed size window and this variable
				   gives its height */

	ptrdiff_t	hh;		/* Height window would be on a screen with
				   1000 lines.  When the screen size changes
				   this is used to calculate the window's
				   real height */

	W	*win;		/* Window this one operates on */
	W	*main;		/* Main window of this family */
	W	*orgwin;	/* Window where space from this window came */
	ptrdiff_t	curx, cury;	/* Cursor position within window */
	KBD	*kbd;		/* Keyboard handler for this window */
	WATOM	*watom;		/* The type of this window */
	void	*object;	/* Object which inherits this */
#if 0
	union {			/* FIXME: instead of void *object we should */
		BW	*bw;	/* use this union to get strict type checking */
		PW	*pw;	/* from C compiler (need to check and change */
		QW	*qw;	/* all of the occurrencies of ->object) */
		TW	*tw;
		MENU	*menu;
		BASE	*base;
	} object;
#endif

	const char	*msgt;		/* Message at top of window */
	const char	*msgb;		/* Message at bottom of window */
	const char	*huh;		/* Name of window for context sensitive hlp */
	int	*notify;	/* Address of kill notification flag */
	struct bstack *bstack;	/* Pushed buffer stack */
};

/* Anything which goes in window.object must start like this: */
struct base {
	W	*parent;
};

/***************/
/* Subroutines */
/***************/

/* int getgrouph(W *);
 * Get height of a family of windows
 */
ptrdiff_t getgrouph(W *w);

/* W *findtopw(W *);
 * Find first (top-most) window of a family
 */
W *findtopw(W *w);

/* W *findbotw(W *);
 * Find last (bottom-most) window a family
 */
W *findbotw(W *w);

int demotegroup(W *w);

/* W *lastw(Screen *t);
 * Find last window on screen
 */
W *lastw(Screen *t);

/* Determine number of main windows
 */
int countmain(Screen *t);

/* void wfit(Screen *t);
 *
 * Fit all of the windows onto the screen
 */
void wfit(Screen *t);

/* W *watpos(Screen *t, int x, int y);
 * Return the window at the given location, or NULL if there is none
 */
W *watpos(Screen *t, ptrdiff_t x, ptrdiff_t y);

/*****************/
/* Main routines */
/*****************/

/* Screen *screate(SCRN *);
 *
 * Create a screen
 */
Screen *screate(SCRN *scrn);

/* void sresize(Screen *t);
 * Screen size changed
 */
void sresize(Screen *t);

/* void chsize(Screen *t,int mul,int div)
 * Resize windows: each window is multiplied by the fraction mul/div
 */
/* void chsize(); */

/* W *wcreate(Screen *t,WATOM *watom,W *where,W *target,W *original,int height);
 *
 * Try to create a window
 *
 * 't'		Is the screen the window is placed on
 * 'watom'	Type of new window
 * 'where'	The window is placed after this window, or if 'where'==0, the
 *		window is placed on the end of the screen
 * 'target'	The window operates on this window.  The window becomes a
 *		member of 'target's family or starts a new family if
 *		'target'==0.
 * 'original'	Attempt to get 'height' from this window.  When the window is
 *              aborted, the space gets returned to 'original' if it still
 *		exists.  If 'original'==0, the window will force other
 *		windows to go off of the screen.
 * 'height'	The height of the window
 *
 * Returns the new window or returns 0 if there was not enough space to
 * create the window and maintain family integrity.
 */
W *wcreate(Screen *t, WATOM *watom, W *where, W *target, W *original, ptrdiff_t height, const char *huh, int *notify);

/* int wabort(W *w);
 *
 * Kill a window and it's children
 */
int wabort(W *w);

/* int wnext(Screen *);
 *
 * Switch to next window
 */
int wnext(Screen *t);

/* int wprev(Screen *);
 *
 * Switch to previous window
 */
int wprev(Screen *t);

/* int wgrow(W *);
 *
 * increase size of window.  Return 0 for success, -1 for fail.
 */
int wgrow(W *w);

/* int wshrink(W *);
 *
 * Decrease size of window.  Returns 0 for success, -1 for fail.
 */
int wshrink(W *w);

int wgrowup(W *w);
int wgrowdown(W *w);

/* void wshowone(W *);
 *
 * Show only one window on the screen
 */
void wshowone(W *w);

/* void wshowall(Screen *);
 *
 * Show all windows on the screen, including the given one
 */
void wshowall(Screen *t);

/* void wredraw(W *);
 *
 * Force complete redraw of window
 */
void wredraw(W *w);

/* void updall()
 *
 * Redraw all windows
 */
void updall(void);

/* void msgnw[t](W *w, char *s);
 * Display a message which will be eliminated on the next keypress.
 * msgnw displays message on bottom line of window
 * msgnwt displays message on top line of window
 */
void msgnw(W *w, const char *s);
void msgnwt(W *w, const char *s);

#define JOE_MSGBUFSIZE 300
extern char msgbuf[JOE_MSGBUFSIZE];	/* Message composition buffer for msgnw/msgnwt */

void msgout(W *w);		/* Output msgnw/msgnwt messages */
void msgclr(W *w);			/* Clear them */

/* Common user functions */

int urtn(W *w, int k);		/* User hit return */
int utype(W *w, int k);		/* User types a character */
int uretyp(W *w, int k);	/* Refresh the screen */
int ugroww(W *w, int k);	/* Grow current window */
int uexpld(W *w, int k);	/* Explode current window or show all windows */
int ushrnk(W *w, int k);	/* Shrink current window */
int unextw(W *w, int k);	/* Goto next window */
int uprevw(W *w, int k);	/* Goto previous window */
int umwind(W *w, int k);	/* Go to message window */
int umfit(W *w, int k);		/* Fit two windows on screen */

void scrdel(B *b, off_t l, off_t n, int flg);
void scrins(B *b, off_t l, off_t n, int flg);

extern int bg_msg; /* Background color for messages */
