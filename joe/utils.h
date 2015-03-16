/*
 *	Various utilities
 *	
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *		(C) 2001 Marek 'Marx' Grac
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_UTILS_H
#define _JOE_UTILS_H 1

/* Unsigned versions of regular string functions */

/* JOE uses 'unsigned char *', never 'char *'.  This is so that when a
   character is loaded from a string into an 'int', the codes 0-255 are
   used, not -128 - 127. */

size_t zlen PARAMS((unsigned char *s));
int zcmp PARAMS((unsigned char *a, unsigned char *b));
int zncmp PARAMS((unsigned char *a, unsigned char *b, size_t len));
unsigned char *zdup PARAMS((unsigned char *s));
/* unsigned char *zcpy PARAMS((unsigned char *a, unsigned char *b)); */
unsigned char *mcpy PARAMS((unsigned char *a, unsigned char *b, size_t len));
unsigned char *zncpy PARAMS((unsigned char *a, unsigned char *b,size_t len));
unsigned char *zlcpy PARAMS((unsigned char *a, size_t siz, unsigned char *b));
unsigned char *zstr PARAMS((unsigned char *a, unsigned char *b));
unsigned char *zchr PARAMS((unsigned char *s, int c));
unsigned char *zrchr PARAMS((unsigned char *s, int c));
/* unsigned char *zcat PARAMS((unsigned char *a, unsigned char *b)); */
unsigned char *zlcat PARAMS((unsigned char *a, size_t siz, unsigned char *b));

int filecmp PARAMS((unsigned char *a, unsigned char *b));
int fullfilecmp PARAMS((unsigned char *a, unsigned char *b));

/*
 * Functions which return minimum/maximum of two numbers  
 */
unsigned int uns_min PARAMS((unsigned int a, unsigned int b));
signed int int_min PARAMS((signed int a, int signed b));
signed long long_max PARAMS((signed long a, signed long b));
signed long long_min PARAMS((signed long a, signed long b));

/* Versions of 'read' and 'write' which automatically retry when interrupted */
ssize_t joe_read PARAMS((int fd, void *buf, size_t siz));
ssize_t joe_write PARAMS((int fd, void *buf, size_t siz));
int joe_ioctl PARAMS((int fd, int req, void *ptr));

/* wrappers to *alloc routines */
void *joe_malloc PARAMS((size_t size));
unsigned char *joe_strdup PARAMS((unsigned char *ptr));
void *joe_calloc PARAMS((size_t nmemb, size_t size));
void *joe_realloc PARAMS((void *ptr, size_t size));
void joe_free PARAMS((void *ptr));

#ifndef HAVE_SIGHANDLER_T
typedef RETSIGTYPE (*sighandler_t)(int);
#endif

#ifdef NEED_TO_REINSTALL_SIGNAL
#define REINSTALL_SIGHANDLER(sig, handler) joe_set_signal(sig, handler)
#else
#define REINSTALL_SIGHANDLER(sig, handler) do {} while(0)
#endif

/* wrapper to hide signal interface differrencies */
int joe_set_signal PARAMS((int signum, sighandler_t handler));

/* Simple parsers */
int parse_ws PARAMS((unsigned char **p,int cmt));
int parse_ident PARAMS((unsigned char **p,unsigned char **buf));
int parse_kw PARAMS((unsigned char **p,unsigned char *kw));
long parse_num PARAMS((unsigned char **p));
int parse_tows PARAMS((unsigned char **p,unsigned char **buf));
int parse_field PARAMS((unsigned char **p,unsigned char *field));
int parse_char PARAMS((unsigned char  **p,unsigned char c));
int parse_int PARAMS((unsigned char **p,int *buf));
int parse_long PARAMS((unsigned char **p,long  *buf));
int parse_string PARAMS((unsigned char **p,unsigned char **dst));
int parse_range PARAMS((unsigned char **p,int *first,int *second));
void emit_string PARAMS((FILE *f,unsigned char *s,int len));
int escape PARAMS((int utf8,unsigned char **a, int *b));

#endif
