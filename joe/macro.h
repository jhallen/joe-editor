/*
 *	Keyboard macros
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* A macro or macro step */

struct macro {
	int k; /* Keycode */
	int flg; /* Flags: bit 0: this step wants the negative arg,
	                   bit 1: ignore return value of this step, but use it as return value of macro */
	CMD *cmd; /* Command address */
	ptrdiff_t n; /* Number of steps */
	ptrdiff_t size; /* Malloc size of steps */
	MACRO **steps; /* Block */
};

/* Macro being recorded */

struct recmac {
	struct recmac *next;	/* Stack of macros: in case user records a macro from within a macro */
	int	n;		/* Keyboard macro number */
	MACRO	*m;		/* Macro being recorded */
};

extern struct recmac *recmac; /* Set when macro is recording: for status line */

/* Macro construction functions */

/* Create a macro
 *  k is key to feed to command
 *  flg has flag bits
 *  n is number of steps (but it does not allocate space for them)
 *  cmd is command for this step
 */
MACRO *mkmacro(int k, int flg, ptrdiff_t n, CMD *cmd);

/* Append step m to macro */
void addmacro(MACRO *macro, MACRO *m);

/* Recursively duplicate a macro */
MACRO *dupmacro(MACRO *mac);

/* Recursively delete a macro */
void rmmacro(MACRO *macro);

/* Set key part of macro step */
MACRO *macstk(MACRO *m, int k);

/* Set flag part of macro step */
MACRO *macsta(MACRO *m, int a);

/* Stuff Ctrl-C into previous step: used in nungetc() */
void chmac(void);

/* Parse a macro:
     m is NULL: start a new macro
     m is not NULL: append to existing macro
     buf: is text to parse
     sta: is filled with offset in buf where parsing stopped or
        -1 for error (unknown command)
        -2 more input needed (macro ended with , )
     secure: if set, only allow commands which begin with "shell_"
*/
MACRO *mparse(MACRO *m, char *buf, ptrdiff_t *sta, int secure);

/* Convert macro to text.  Provide a buffer to write to in 's'. */
char *mtext(char *s, MACRO *m);

/* Execute a macro */
extern MACRO *curmacro; /* Current macro being executed */
extern int current_arg; /* Current macro repeat argument */
extern int current_arg_set; /* Set if repeat arg was given */

/* Execute a macro: for user typing.
     Records step if we are recording.
*/
int exemac(MACRO *m);

/* Execute a macro as a subroutine
    u set to treat as single undo step
*/
int exmacro(MACRO *m, int u);

/* Keyboard macros user interface */
int uplay(W *w, int c);		/* Play a keyboard macro */
int ustop(W *w, int c);		/* Stop recording */
int urecord(W *w, int c);	/* Start recording */
int uquery(W *w, int c);	/* Suspend recording/playing for user input */
int umacros(W *w, int c);	/* Convert keyboard macros to text and insert them */
int utimer(W *w, int c);	/* Execute a macro periodically */

/* Repeat prefix user command */
int uarg(W *w, int k);		/* Prompt for repeat argument */
int uuarg(W *w, int c);		/* Emacs way of setting repeat argument */

int uif(W *w, int k);		/* Prompt for expression, execute following steps if true */
int uelsif(W *w, int k);	/* Prompt for expression, execute following steps if true */
int uelse(W *w, int k);		/* Execute following steps if last expression was false */
int uendif(W *w, int k);	/* End of conditional */

void load_macros(FILE *f);	/* Load keyboard macros from .joe_state file */
void save_macros(FILE *f);	/* Save keyboard macros in .joe_state file */

/* Append escaped version of character c to string ptr
   Used by mtext.
*/
char *unescape(char *ptr,int c);
