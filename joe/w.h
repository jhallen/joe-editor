/*
 *	Window management
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_W_H
#define _JOE_W_H 1

struct watom {
	unsigned char	*context;	/* Context name */
	void	(*disp) ();	/* Display window */
	void	(*follow) ();	/* Called to have window follow cursor */
	int	(*abort) ();	/* Common user functions */
	int	(*rtn) ();
	int	(*type) ();
	void	(*resize) ();	/* Called when window changed size */
	void	(*move) ();	/* Called when window moved */
	void	(*ins) ();	/* Called on line insertions */
	void	(*del) ();	/* Called on line deletions */
	int	what;		/* Type of this thing */
};

/* A screen with windows */

struct screen {
	SCRN	*t;		/* Screen data on this screen is output to */

	int	wind;		/* Number of help lines on this screen */

	W	*topwin;	/* Top-most window showing on screen */
	W	*curwin;	/* Window cursor is in */

	int	w, h;		/* Width and height of this screen */
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

	int	x, y, w, h;	/* Position and size of window */
				/* Currently, x = 0, w = width of screen. */
				/* y == -1 if window is not on screen */

	int	ny, nh;		/* Temporary values for wfit */

	int	reqh;		/* Requested new height or 0 for same */
				/* This is an argument for wfit */

	int	fixed;		/* If this is zero, use 'hh'.  If not, this
				   is a fixed size window and this variable
				   gives its height */

	int	hh;		/* Height window would be on a screen with
				   1000 lines.  When the screen size changes
				   this is used to calculate the window's
				   real height */

	W	*win;		/* Window this one operates on */
	W	*main;		/* Main window of this family */
	W	*orgwin;	/* Window where space from this window came */
	int	curx, cury;	/* Cursor position within window */
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

	unsigned char	*msgt;		/* Message at top of window */
	unsigned char	*msgb;		/* Message at bottom of window */
	unsigned char	*huh;		/* Name of window for context sensitive hlp */
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
int getgrouph PARAMS((W *w));

/* W *findtopw(W *);
 * Find first (top-most) window of a family
 */
W *findtopw PARAMS((W *w));

/* W *findbotw(W *);
 * Find last (bottom-most) window a family
 */
W *findbotw PARAMS((W *w));

int demotegroup PARAMS((W *w));

/* W *lastw(Screen *t);
 * Find last window on screen
 */
W *lastw PARAMS((Screen *t));

/* Determine number of main windows
 */
int countmain PARAMS((Screen *t));

/* void wfit(Screen *t);
 *
 * Fit all of the windows onto the screen
 */
void wfit PARAMS((Screen *t));

/* W *watpos(Screen *t, int x, int y);
 * Return the window at the given location, or NULL if there is none
 */
W *watpos PARAMS((Screen *t, int x, int y));

/*****************/
/* Main routines */
/*****************/

/* Screen *screate(SCRN *);
 *
 * Create a screen
 */
Screen *screate PARAMS((SCRN *scrn));

/* void sresize(Screen *t);
 * Screen size changed
 */
void sresize PARAMS((Screen *t));

/* void chsize(Screen *t,int mul,int div)
 * Resize windows: each window is multiplied by the fraction mul/div
 */
/* void chsize PARAMS(()); */

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
W *wcreate PARAMS((Screen *t, WATOM *watom, W *where, W *target, W *original, int height, unsigned char *huh, int *notify));

/* int wabort(W *w);
 *
 * Kill a window and it's children
 */
int wabort PARAMS((W *w));

/* int wnext(Screen *);
 *
 * Switch to next window
 */
int wnext PARAMS((Screen *t));

/* int wprev(Screen *);
 *
 * Switch to previous window
 */
int wprev PARAMS((Screen *t));

/* int wgrow(W *);
 *
 * increase size of window.  Return 0 for success, -1 for fail.
 */
int wgrow PARAMS((W *w));

/* int wshrink(W *);
 *
 * Decrease size of window.  Returns 0 for success, -1 for fail.
 */
int wshrink PARAMS((W *w));

int wgrowup PARAMS((W *w));
int wgrowdown PARAMS((W *w));

/* void wshowone(W *);
 *
 * Show only one window on the screen
 */
void wshowone PARAMS((W *w));

/* void wshowall(Screen *);
 *
 * Show all windows on the screen, including the given one
 */
void wshowall PARAMS((Screen *t));

/* void wredraw(W *);
 *
 * Force complete redraw of window
 */
void wredraw PARAMS((W *w));

/* void updall()
 *
 * Redraw all windows
 */
void updall PARAMS((void));

/* void msgnw[t](W *w, char *s);
 * Display a message which will be eliminated on the next keypress.
 * msgnw displays message on bottom line of window
 * msgnwt displays message on top line of window
 */
void msgnw PARAMS((W *w, unsigned char *s));
void msgnwt PARAMS((W *w, unsigned char *s));

#define JOE_MSGBUFSIZE 300
extern unsigned char msgbuf[JOE_MSGBUFSIZE];	/* Message composition buffer for msgnw/msgnwt */

void msgout PARAMS((W *w));			/* Output msgnw/msgnwt messages */
void msgclr();					/* Clear them */

/* Common user functions */

int urtn PARAMS((BASE *b, int k));		/* User hit return */
int utype PARAMS((BASE *b, int k));		/* User types a character */
int uretyp PARAMS((BASE *bw));			/* Refresh the screen */
int ugroww PARAMS((BASE *bw));			/* Grow current window */
int uexpld PARAMS((BASE *bw));			/* Explode current window or show all windows */
int ushrnk PARAMS((BASE *bw));			/* Shrink current window */
int unextw PARAMS((BASE *bw));			/* Goto next window */
int uprevw PARAMS((BASE *bw));			/* Goto previous window */
int umwind PARAMS((BW *bw));			/* Go to message window */
int umfit PARAMS((BW *bw));			/* Fit two windows on screen */

void scrdel PARAMS((B *b, long int l, long int n, int flg));
void scrins PARAMS((B *b, long int l, long int n, int flg));

extern int bg_msg; /* Background color for messages */

#endif
