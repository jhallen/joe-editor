/*
 *	Various utilities
 *	
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *		(C) 2001 Marek 'Marx' Grac
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* Destructors */

#define AUTO_DESTRUCT GC *gc = 0;

typedef struct gc GC;

struct gc {
	struct gc *next;	/* List */
	void **var;		/* Address of pointer variable */
	void (*rm)(void *val);	/* Destructor which takes pointer variable */
};

/* Add a variable to GC list */

void gc_add(GC **gc, void **var, void (*rm)(void *val));

/* Call destructors */

void gc_collect(GC **gc);

/* Version of return which calls destructors before returning */

#define RETURN(val) do { \
	if (gc) gc_collect(&gc); \
	return (val); \
	} while(0)

/* Pool allocation functions using singly-linked lists */

extern void *ITEM; /* Temporary global variable (from queue.c) */

/* Allocate item from free-list.  If free-list empty, replenish it. */

void *replenish(void **list,ptrdiff_t size);

#define al_single(list,type) ( \
	(ITEM = *(void **)(list)) ? \
	  ( (*(void **)(list) = *(void **)ITEM), ITEM ) \
	: \
	  replenish((void **)(list),sizeof(type)) \
)

/* Put item on free list */

#define fr_single(list,item) do { \
	*(void **)(item) = *(void **)(list); \
	*(void **)(list) = (void *)(item); \
} while(0)

/* Unsigned versions of regular string functions */

ptrdiff_t zlen(char *s);
int zcmp(char *a, char *b);
int zicmp(char *a, char *b);
int zmcmp(char *a, char *b);
int zncmp(char *a, char *b, ptrdiff_t len);
char *zdup(char *s);
/* char *zcpy(char *a, char *b); */
void *mcpy(void *a, void *b, ptrdiff_t len);
char *zncpy(char *a, char *b, ptrdiff_t len);
char *zlcpy(char *a, ptrdiff_t siz, char *b);
char *zstr(char *a, char *b);
char *zchr(char *s, int c);
char *zrchr(char *s, int c);
/* char *zcat(char *a, char *b); */
char *zlcat(char *a, ptrdiff_t siz, char *b);

/*
 * Functions which return minimum/maximum of two numbers  
 */
unsigned int uns_min(unsigned int a, unsigned int b);
signed int int_min(signed int a, int signed b);
signed long long_max(signed long a, signed long b);
signed long long_min(signed long a, signed long b);
off_t off_max(off_t a, off_t b);
off_t off_min(off_t a, off_t b);
ptrdiff_t diff_max(ptrdiff_t a, ptrdiff_t b);
ptrdiff_t diff_min(ptrdiff_t a, ptrdiff_t b);

/* Versions of 'read' and 'write' which automatically retry when interrupted */
ptrdiff_t joe_read(int fd, void *buf, ptrdiff_t siz);
ptrdiff_t joe_write(int fd, void *buf, ptrdiff_t siz);
int joe_ioctl(int fd, int req, void *ptr);

/* wrappers to *alloc routines */
void *joe_malloc(ptrdiff_t size);
char *joe_strdup(char *ptr);
void *joe_calloc(ptrdiff_t nmemb, ptrdiff_t size);
void *joe_realloc(void *ptr, ptrdiff_t size);
void joe_free(void *ptr);

#ifndef HAVE_SIGHANDLER_T
typedef RETSIGTYPE (*sighandler_t)(int);
#endif

#ifdef NEED_TO_REINSTALL_SIGNAL
#define REINSTALL_SIGHANDLER(sig, handler) joe_set_signal(sig, handler)
#else
#define REINSTALL_SIGHANDLER(sig, handler) do {} while(0)
#endif

/* wrapper to hide signal interface differrencies */
int joe_set_signal(int signum, sighandler_t handler);

/* Simple parsers */
int parse_ws(char **p,int cmt);
int parse_ident(char **p,char *buf,ptrdiff_t len);
int parse_kw(char **p,char *kw);
int parse_tows(char **p,char *buf);
int parse_field(char **p,char *field);
int parse_char(char  **p,char c);
int parse_int(char **p,int *buf);
int parse_diff(char **p,ptrdiff_t *buf);
int parse_off_t(char **p,off_t *buf);
off_t ztoo(char *s);
off_t zhtoo(char *s);
long ztol(char *s);
long zhtol(char *s);
ptrdiff_t ztodiff(char *s);
ptrdiff_t zhtodiff(char *s);
int ztoi(char *s);
int zhtoi(char *s);
ptrdiff_t parse_string(char **p,char *buf,ptrdiff_t len);
int parse_range(char **p,int *first,int *second);
void emit_string(FILE *f,char *s,ptrdiff_t len);

void jsort(void *base, ptrdiff_t num, ptrdiff_t size, int (*compar)(const void *a, const void *b));
