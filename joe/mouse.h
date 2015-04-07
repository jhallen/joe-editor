/* GPM/xterm mouse functions
   Copyright (C) 1999 Jesse McGrew

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
JOE; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

/* maximum number of milliseconds that can elapse between
   double/triple clicks */
#define MOUSE_MULTI_THRESH	300

#ifdef MOUSE_GPM
int gpmopen();		/* initialize the connection. returns 0 on failure. */
void gpmclose();	/* close the connection. */
#endif

void mouseopen();	/* initialize mouse */
void mouseclose();	/* de-initialize mouse */

/* mousedn(int x, int y) - handle a mouse-down event */
void mousedn(ptrdiff_t x, ptrdiff_t y);

/* mouseup(int x, int y) - handle a mouse-up event */
void mouseup(ptrdiff_t x, ptrdiff_t y);

/* mousedrag(int x, int y) - handle a mouse drag event */
void mousedrag(ptrdiff_t x, ptrdiff_t y);

/* user command handlers */
int uxtmouse(W *, int);		/* handle an xterm mouse control sequence */
int uextmouse(W *, int);		/* handle an extended xterm mouse control sequence */
int utomouse(W *, int);		/* move the pointer to the mouse */
int udefmdown(W *, int);	/* default mouse click handlers */
int udefmup(W *, int);
int udefmdrag(W *, int);
int udefm2down(W *, int);
int udefm2up(W *, int);
int udefm2drag(W *, int);
int udefm3down(W *, int);
int udefm3up(W *, int);
int udefm3drag(W *, int);

long mnow();
void reset_trig_time();

/* options */
extern int floatmouse;	/* Allow mouse to set cursor past end of lines */
extern int rtbutton; /* Use button 3 instead of button 1 */

extern int auto_scroll; /* Set for autoscroll */
extern long auto_trig_time; /* Time of next scroll */
extern int joexterm; /* Set if xterm can do base64 paste */
