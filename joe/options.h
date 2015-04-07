/*
 *	JOE options
 *	Copyright
 *		(C) 1992 Joseph H. Allen; 
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

extern OPTIONS pdefault;
extern OPTIONS fdefault;

/* Set local options depending on file name and contents */
void setopt(B *b, unsigned char *name);

/* Set a global or local option:
 * 's' is option name
 * 'arg' is a possible argument string (taken only if option has an arg)
 * 'options' points to options structure to modify (can be NULL).
 * 'set'==0: set only in 'options' if it's given.
 * 'set'!=0: set global variable option.
 * return value: no. of fields taken (1 or 2), or 0 if option not found.
 *
 * So this function is used both to set options, and to parse over options
 * without setting them.
 *
 * These combinations are used:
 *
 * glopt(name,arg,NULL,1): set global variable option
 * glopt(name,arg,NULL,0): parse over option
 * glopt(name,arg,options,0): set file local option
 * glopt(name,arg,&fdefault,1): set default file options
 * glopt(name,arg,options,1): set file local option
 */

int glopt(unsigned char *s, unsigned char *arg, OPTIONS *options, int set);

/* Option setting user command */
int umode(BW *bw);

/* Update options */
void lazy_opts(B *b, OPTIONS *o);

/* Commands which just convert an option into text and type it in */
int ucharset(BW *bw);
int ulanguage(BW *bw);

/* Get current value of an option (ON / OFF) */
unsigned char *get_status(BW *bw, unsigned char *s);

extern OPTIONS *options;
