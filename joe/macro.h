/*
 *	Keyboard macros
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_MACRO_H
#define _JOE_MACRO_H 1

struct macro {
	int k; /* Keycode */
	int flg; /* Flags: bit 0: this step wants the negative arg,
	                   bit 1: ignore return value of this step, but use it as return value of macro */
	CMD *cmd; /* Command address */
	int n; /* Number of steps */
	int size; /* Malloc size of steps */
	MACRO **steps; /* Block */
};

struct recmac {
	struct recmac *next;
	int	n;
	MACRO	*m;
};

extern struct recmac *recmac; /* Set when macro is recording: for status line */

/* Macro construction functions */
MACRO *mkmacro PARAMS((int k, int arg, int n, CMD *cmd));
void addmacro PARAMS((MACRO *macro, MACRO *m));
MACRO *dupmacro PARAMS((MACRO *mac));
void rmmacro PARAMS((MACRO *macro));
MACRO *macstk PARAMS((MACRO *m, int k));
MACRO *macsta PARAMS((MACRO *m, int a));

void chmac PARAMS((void));

/* Text to macro / Macro to text */
MACRO *mparse PARAMS((MACRO *m, unsigned char *buf, int *sta, int secure));
unsigned char *mtext PARAMS((unsigned char *s, MACRO *m));

/* Execute a macro */
extern MACRO *curmacro; /* Current macro being executed */
int exemac PARAMS((MACRO *m));
int exmacro PARAMS((MACRO *m, int u));

/* Keyboard macros user interface */
int uplay PARAMS((BW *bw, int c));
int ustop PARAMS((void));
int urecord PARAMS((BW *bw, int c));
int uquery PARAMS((BW *bw));
int umacros PARAMS((BW *bw));
int utimer PARAMS((BW *bw));

/* Repeat prefix user command */
int uarg PARAMS((BW *bw));
int uuarg PARAMS((BW *bw, int c));
int uif PARAMS((BW *bw));
int uelsif PARAMS((BW *bw));
int uelse PARAMS((BW *bw));
int uendif PARAMS((BW *bw));

unsigned char *unescape PARAMS((unsigned char *ptr,int c));

void load_macros PARAMS((FILE *f));
void save_macros PARAMS((FILE *f));

extern int current_arg; /* Current macro repeat argument */
extern int current_arg_set; /* Set if repeat arg was given */

#endif
