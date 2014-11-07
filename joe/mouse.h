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

#ifndef _Imouse
#define _Imouse 1

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
void mousedn PARAMS((int x, int y));

/* mouseup(int x, int y) - handle a mouse-up event */
void mouseup PARAMS((int x, int y));

/* mousedrag(int x, int y) - handle a mouse drag event */
void mousedrag PARAMS((int x, int y));

/* user command handlers */
int uxtmouse PARAMS((BW *));		/* handle an xterm mouse control sequence */
int uextmouse PARAMS((BW *));		/* handle an extended xterm mouse control sequence */
int utomouse PARAMS((BW *));		/* move the pointer to the mouse */
int udefmdown PARAMS((BW *));	/* default mouse click handlers */
int udefmup PARAMS((BW *));
int udefmdrag PARAMS((BW *));
int udefm2down PARAMS((BW *));
int udefm2up PARAMS((BW *));
int udefm2drag PARAMS((BW *));
int udefm3down PARAMS((BW *));
int udefm3up PARAMS((BW *));
int udefm3drag PARAMS((BW *));

int mnow();
void reset_trig_time();

/* options */
extern int floatmouse;	/* Allow mouse to set cursor past end of lines */
extern int rtbutton; /* Use button 3 instead of button 1 */

extern int auto_scroll; /* Set for autoscroll */
extern int auto_trig_time; /* Time of next scroll */
extern int joexterm; /* Set if xterm can do base64 paste */

#endif
