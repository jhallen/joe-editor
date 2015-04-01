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
MACRO *mkmacro(int k, int arg, int n, CMD *cmd);
void addmacro(MACRO *macro, MACRO *m);
MACRO *dupmacro(MACRO *mac);
void rmmacro(MACRO *macro);
MACRO *macstk(MACRO *m, int k);
MACRO *macsta(MACRO *m, int a);

void chmac(void);

/* Text to macro / Macro to text */
MACRO *mparse(MACRO *m, unsigned char *buf, int *sta, int secure);
unsigned char *mtext(unsigned char *s, MACRO *m);

/* Execute a macro */
extern MACRO *curmacro; /* Current macro being executed */
int exemac(MACRO *m);
int exmacro(MACRO *m, int u);

/* Keyboard macros user interface */
int uplay(BW *bw, int c);
int ustop(void);
int urecord(BW *bw, int c);
int uquery(BW *bw);
int umacros(BW *bw);
int utimer(BW *bw);

/* Repeat prefix user command */
int uarg(BW *bw);
int uuarg(BW *bw, int c);
int uif(BW *bw);
int uelsif(BW *bw);
int uelse(BW *bw);
int uendif(BW *bw);

unsigned char *unescape(unsigned char *ptr,int c);

void load_macros(FILE *f);
void save_macros(FILE *f);

extern int current_arg; /* Current macro repeat argument */
extern int current_arg_set; /* Set if repeat arg was given */

#endif
