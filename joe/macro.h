/*
 *	Keyboard macros
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

struct macro {
	int k; /* Keycode */
	int flg; /* Flags: bit 0: this step wants the negative arg,
	                   bit 1: ignore return value of this step, but use it as return value of macro */
	CMD *cmd; /* Command address */
	ptrdiff_t n; /* Number of steps */
	ptrdiff_t size; /* Malloc size of steps */
	MACRO **steps; /* Block */
};

struct recmac {
	struct recmac *next;
	int	n;
	MACRO	*m;
};

extern struct recmac *recmac; /* Set when macro is recording: for status line */

/* Macro construction functions */
MACRO *mkmacro(int k, int arg, ptrdiff_t n, CMD *cmd);
void addmacro(MACRO *macro, MACRO *m);
MACRO *dupmacro(MACRO *mac);
void rmmacro(MACRO *macro);
MACRO *macstk(MACRO *m, int k);
MACRO *macsta(MACRO *m, int a);

void chmac(void);

/* Text to macro / Macro to text */
MACRO *mparse(MACRO *m, char *buf, ptrdiff_t *sta, int secure);
char *mtext(char *s, MACRO *m);

/* Execute a macro */
extern MACRO *curmacro; /* Current macro being executed */
int exemac(MACRO *m);
int exmacro(MACRO *m, int u);

/* Keyboard macros user interface */
int uplay(W *w, int c);
int ustop(W *w, int c);
int urecord(W *w, int c);
int uquery(W *w, int c);
int umacros(W *w, int c);
int utimer(W *w, int c);

/* Repeat prefix user command */
int uarg(W *w, int k);
int uuarg(W *w, int c);
int uif(W *w, int k);
int uelsif(W *w, int k);
int uelse(W *w, int k);
int uendif(W *w, int k);

char *unescape(char *ptr,int c);

void load_macros(FILE *f);
void save_macros(FILE *f);

extern int current_arg; /* Current macro repeat argument */
extern int current_arg_set; /* Set if repeat arg was given */
