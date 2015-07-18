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

/* JOE's version of zero-terminated string (z-string) functions.
   In older versions of JOE we had these because the library versions were slow. 
   In older versions of JOE we had these because we globally used "unsigned char *" strings.
   Now we have them for ptrdiff_t lengths (instead of size_t), to avoid mixed signed / unsigned warnings. */

/* These are exactly the same as the library versions: for consistent naming */
#define zstr(a, b) strstr((a), (b))
#define zchr(s, c) strchr((s), (c))
#define zrchr(s, c) strrchr((s), (c))
#define zcat(a, b) strcat((a), (b))
#define zdup(s) strdup(s)
#define zcpy(a, b) strcpy((a), (b))
#define zcmp(a, b) strcmp((a), (b))

/* char *zstr(const char *a, const char *b); */
/* char *zchr(const char *s, int c); */
/* char *zrchr(const char *s, int c); */
/* char *zcat(char *a, char *b); */
/* char *zdup(const char *s); */
/* char *zcpy(char *a, char *b); */
/* int zcmp(char *a, char *b); */

#define zlen(s) (ptrdiff_t)strlen(s)

/* Case insensitive compare, but only for ASCII */
int zicmp(const char *a, const char *b);

/* String compare for ctags file strings */
int zmcmp(const char *a, const char *b);

/* Same strncmp */
int zncmp(const char *a, const char *b, ptrdiff_t len);

/* Same as memcpy */
void *mcpy(void *a, const void *b, ptrdiff_t len);

/* Same as strncpy */
char *zncpy(char *a, const char *b, ptrdiff_t len);

/* Like strncpy, but string always ends up zero-terminated */
char *zlcpy(char *a, ptrdiff_t siz, const char *b);

/* Like strncat, but result is always zero-terminated */ 
char *zlcat(char *a, ptrdiff_t siz, const char *b);

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
ptrdiff_t joe_write(int fd, const void *buf, ptrdiff_t siz);
int joe_ioctl(int fd, unsigned long req, void *ptr);

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
int parse_ws(const char * *p,int cmt);
int parse_ident(const char **p,char *buf,ptrdiff_t len);
int parse_kw(const char **p,const char *kw);
int parse_tows(const char **p,char *buf);
int parse_field(const char **p,const char *field);
int parse_char(const char **p,char c);
int parse_int(const char **p,int *buf);
int parse_diff(const char **p,ptrdiff_t *buf);
int parse_off_t(const char **p,off_t *buf);
off_t ztoo(const char *s);
off_t zhtoo(const char *s);
long ztol(const char *s);
long zhtol(const char *s);
ptrdiff_t ztodiff(const char *s);
ptrdiff_t zhtodiff(const char *s);
int ztoi(const char *s);
int zhtoi(const char *s);
ptrdiff_t parse_string(const char **p,char *buf,ptrdiff_t len);
int parse_range(const char **p,int *first,int *second);
void emit_string(FILE *f,const char *s,ptrdiff_t len);

void jsort(void *base, ptrdiff_t num, ptrdiff_t size, int (*compar)(const void *a, const void *b));

off_t oabs(off_t);
