/*
 *	Various utilities
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *		(C) 2001 Marek 'Marx' Grac
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#if 0
int joe_ispunct(int wide,struct charmap *map,int c)
{
	if (joe_isspace(c))
		return 0;

	if (c=='_')
		return 1;

	if (isalnum_(wide,map,c))
		return 0;

	return joe_isprint(wide,map,c);
}
#endif

/*
 * return minimum/maximum of two numbers
 */
unsigned int uns_min(unsigned int a, unsigned int b)
{
	return a < b ? a : b;
}

signed int int_min(signed int a, signed int b)
{
	return a < b ? a : b;
}

signed long int long_max(signed long int a, signed long int b)
{
	return a > b ? a : b;
}

signed long int long_min(signed long int a, signed long int b)
{
	return a < b ? a : b;
}

off_t off_max(off_t a, off_t b)
{
	return a > b ? a : b;
}

off_t off_min(off_t a, off_t b)
{
	return a < b ? a : b;
}

ptrdiff_t diff_max(ptrdiff_t a, ptrdiff_t b)
{
	return a > b ? a : b;
}

ptrdiff_t diff_min(ptrdiff_t a, ptrdiff_t b)
{
	return a < b ? a : b;
}

#if 0
/* 
 * Characters which are considered as word characters 
 * 	_ is considered as word character because is often used 
 *	in the names of C/C++ functions
 */
int isalnum_(int wide,struct charmap *map,int c)
{
	/* Fast... */
	if (c>='0' && c<='9' ||
	    c>='a' && c<='z' ||
	    c>='A' && c<='Z' ||
	    c=='_')
	  return 1;
	else if(c<128)
	  return 0;

	/* Slow... */
	if (wide)
		return joe_iswalpha(c);
	else
		return joe_iswalpha(to_uni(map,c));
}

int isalpha_(int wide,struct charmap *map,int c)
{
	/* Fast... */
	if (c>='a' && c<='z' ||
	    c>='A' && c<='Z' ||
	    c=='_')
	  return 1;
	else if(c<128)
	  return 0;

	/* Slow... */
	if (wide)
		return joe_iswalpha(c);
	else
		return joe_iswalpha(to_uni(map,c));
}
#endif

/* Versions of 'read' and 'write' which automatically retry when interrupted */
ptrdiff_t joe_read(int fd, void *buf, ptrdiff_t size)
{
	ptrdiff_t rt;

	do {
		rt = read(fd, buf, (size_t)size);
	} while (rt < 0 && errno == EINTR);
	return rt;
}

ptrdiff_t joe_write(int fd, const void *buf, ptrdiff_t size)
{
	ptrdiff_t rt;

	do {
		rt = write(fd, buf, (size_t)size);
	} while (rt < 0 && errno == EINTR);
	return rt;
}

int joe_ioctl(int fd, int req, void *ptr)
{
	int rt;
	do {
		rt = ioctl(fd, req, ptr);
	} while (rt == -1 && errno == EINTR);
	return rt;
}

/* Heap checking versions of malloc() */

/* #define HEAP_CHECK 1 */

#ifdef HEAP_CHECK

struct mcheck {
	struct mcheck *next;
	int state;		/* 0=malloced, 1=free */
	int size;		/* size in bytes */
	int magic;
};

struct mcheck *first;
struct mcheck *last;

void check_malloc()
{
	struct mcheck *m;
	int y = 0;
	for(m=first;m;m=m->next) {
		char *ptr = (char *)m+SIZEOF(struct mcheck);
		int x;
		if(m->magic!=0x55AA55AA) {
			printf("corrupt heap: head %x\n",ptr);
			*(int *)0=0;
		}
		for(x=0;x!=16384;++x)
			if(ptr[x+m->size]!=0xAA) {
				printf("Corrupt heap: tail %x\n",ptr);
				*(int *)0=0;
			}
		if(m->state)
			for(x=0;x!=m->size;++x)
				if(ptr[x]!=0x41) {
					printf("Corrupt heap: modified free block %x\n",ptr);
					*(int *)0=0;
				}
		++y;
	}
}

void *joe_malloc(int size)
{
	struct mcheck *m = (struct mcheck *)malloc(size+SIZEOF(struct mcheck)+16384);
	char *ptr = (char *)m+SIZEOF(struct mcheck);
	int x;
	m->state = 0;
	m->size = size;
	m->magic=0x55AA55AA;
	m->next = 0;

	if(!size) {
		printf("0 passed to malloc\n");
		*(int *)0=0;
	}
	for(x=0;x!=16384;++x)
		ptr[x+size]=0xAA;
	if(first) {
		last->next = m;
		last = m;
	} else {
		first = last = m;
	}
	check_malloc();
	/* printf("malloc size=%d addr=%x\n",size,ptr); fflush(stdout); */
	return ptr;
}

void *joe_calloc(int nmemb, int size)
{
	int sz = nmemb*size;
	int x;
	char *ptr;
	ptr = joe_malloc(sz);
	for(x=0;x!=sz;++x)
		ptr[x] = 0;
	return ptr;
}

void *joe_realloc(void *ptr, int size)
{
	struct mcheck *n;
	char *np;
	struct mcheck *m = (struct mcheck *)((char *)ptr-SIZEOF(struct mcheck));

	if(!size) {
		printf("0 passed to realloc\n");
		*(int *)0=0;
	}

	np = joe_malloc(size);

	n = (struct mcheck *)(np-SIZEOF(struct mcheck));

	if(m->size>size)
		memcpy(np,ptr,size);
	else
		memcpy(np,ptr,m->size);

	joe_free(ptr);

	return np;
}

void joe_free(void *ptr)
{
	struct mcheck *m = (struct mcheck *)((char *)ptr-SIZEOF(struct mcheck));
	int x;
	if (m->magic!=0x55AA55AA) {
		printf("Free non-malloc block %x\n",ptr);
		*(int *)0=0x0;
	}
	if (m->state) {
		printf("Double-free %x\n",ptr);
		*(int *)0=0x0;
	}
	for(x=0;x!=m->size;++x)
		((char *)ptr)[x]=0x41;
	m->state = 1;
	check_malloc();
	/* printf("free=%x\n",ptr); */
}

#else

/* Normal malloc() */

void *joe_malloc(ptrdiff_t size)
{
	void *p = malloc((size_t)size);
	if (!p)
		ttsig(-1);
	return p;
}

void *joe_calloc(ptrdiff_t nmemb,ptrdiff_t size)
{
	void *p = calloc((size_t)nmemb, (size_t)size);
	if (!p)
		ttsig(-1);
	return p;
}

void *joe_realloc(void *ptr,ptrdiff_t size)
{
	void *p = realloc(ptr, (size_t)size);
	if (!p)
		ttsig(-1);
	return p;
}

void joe_free(void *ptr)
{
	free(ptr);
}

#endif

ptrdiff_t zlen(const char *s)
{
	return (ptrdiff_t)strlen(s);
}

int zcmp(const char *a, const char *b)
{
	return strcmp(a, b);
}

/* Only if you know for sure that the text is ASCII */

int zicmp(const char *a, const char *b)
{
	char ca;
	char cb;
	while (*a && *b) {
		ca = *a;
		cb = *b;
		if (ca >= 'a' && ca <= 'z')
			ca = (char)(ca + ('A' - 'a'));
		if (cb >= 'a' && cb <= 'z')
			cb = (char)(cb + ('A' - 'a'));
		if (ca > cb)
			return 1;
		if (ca < cb)
			return -1;
		++a;
		++b;
	}
	if (*a)
		return 1;
	if (*b)
		return -1;
	return 0;
}

/* Tags file string matcher */
/* a can be something like: "class::subclass::member"
 * b can be:
 *    member
 *    ::member
 *    subclass::member
 *    ::subclass::member
 *    class::subclass::member
 */

int zmcmp(const char *a, const char *b)
{
	ptrdiff_t x = zlen(a);
	do {
		if (a[x] == ':' && a[x + 1] == ':')
			if ((b[0] == ':' && !zcmp(a + x, b)) || !zcmp(a + x + 2, b))
				return 0;
	} while (x--);
	return zcmp(a, b);
}

int zncmp(const char *a, const char *b, ptrdiff_t len)
{
	return strncmp(a, b, (size_t)len);
}

char *zdup(const char *bf)
{
	ptrdiff_t size = zlen(bf);
	char *p = (char *)joe_malloc(size+1);
	memcpy(p, bf, (size_t)(size + 1));
	return p;
}

#if 0
char *zcpy(char *a, char *b)
{
	strcpy(a,b);
	return a;
}
#endif

char *zstr(const char *a, const char *b)
{
	return (char *)strstr((char *)a,b);
}

char *zncpy(char *a, const char *b, ptrdiff_t len)
{
	strncpy(a,b,(size_t)len);
	return a;
}

char *zlcpy(char *a, ptrdiff_t len, const char *b)
{
	char *org = a;
	if (!len) {
		fprintf(stderr, "zlcpy called with len == 0\n");
		exit(1);
	}
	--len;
	while (len && *b) {
		*a++ = *b++;
		--len;
	}
	*a = 0;
	return org;
}

void *mcpy(void *a, const void *b, ptrdiff_t len)
{
	if (len)
		memcpy(a, b, (size_t)len);
	return a;
}

#if 0
char *zcat(char *a, char *b)
{
	strcat(a,b);
	return a;
}
#endif

char *zlcat(char *a, ptrdiff_t siz, const char *b)
{
	char *org = a;
	while (*a && siz) {
		++a;
		--siz;
	}
	zlcpy(a, siz, b);
	return org;
}

char *zchr(const char *s, int c)
{
	return strchr((char *)s,c);
}

char *zrchr(const char *s, int c)
{
	return strrchr((char *)s,c);
}

off_t zhtoo(const char *s)
{
	off_t val = 0;
	int flg = 0;
	if (s) {
		if (*s == '-') {
			++s;
			flg = 1;
		} else if (*s == '+') {
			++s;
		}
		while ((*s >= '0' && *s <= '9') || (*s >= 'a' && *s <= 'f') || (*s >= 'A' && *s <= 'F')) {
			if (*s >= '0' && *s <= '9')
				val = val * 16 + (off_t)(*s - '0');
			else if (*s >= 'a' && *s <= 'f')
				val = val * 16 + (off_t)(*s - 'a' + 10);
			else if (*s >= 'A' && *s <= 'F')
				val = val * 16 + (off_t)(*s - 'A' + 10);
			++s;
		}
	}
	if (flg)
		return -val;
	else
		return val;
}

off_t ztoo(const char *s)
{
	off_t val = 0;
	int flg = 0;
	if (s) {
		if (*s == '-') {
			++s;
			flg = 1;
		} else if (*s == '+') {
			++s;
		}
		if (s[0] == '0' && s[1] == 'x') {
			s += 2;
			while ((*s >= '0' && *s <= '9') || (*s >= 'a' && *s <= 'f') || (*s >= 'A' && *s <= 'F')) {
				if (*s >= '0' && *s <= '9')
					val = val * 16 + (off_t)(*s - '0');
				else if (*s >= 'a' && *s <= 'f')
					val = val * 16 + (off_t)(*s - 'a' + 10);
				else if (*s >= 'A' && *s <= 'F')
					val = val * 16 + (off_t)(*s - 'A' + 10);
				++s;
			}
		} else if (s[0] == '0') {
			while (*s >= '0' && *s <= '7') {
				val = val * 8 + (off_t)(*s - '0');
				++s;
			}
		} else {
			while (*s >= '0' && *s <= '9') {
				val = val * 10 + (off_t)(*s - '0');
				++s;
			}
		}
	}
	if (flg)
		return -val;
	else
		return val;
}

long ztol(const char *s)
{
	off_t val = ztoo(s);
	return (long)val;
}

int ztoi(const char *s)
{
	off_t val = ztoo(s);
	return (int)val;
}

ptrdiff_t ztodiff(const char *s)
{
	off_t val = ztoo(s);
	return (ptrdiff_t)val;
}

long zhtol(const char *s)
{
	off_t val = zhtoo(s);
	return (long)val;
}

int zhtoi(const char *s)
{
	off_t val = zhtoo(s);
	return (int)val;
}

ptrdiff_t zhtodiff(const char *s)
{
	off_t val = zhtoo(s);
	return (ptrdiff_t)val;
}

#ifndef SIG_ERR
#define SIG_ERR ((sighandler_t) -1)
#endif

/* wrapper to hide signal interface differrencies */
int joe_set_signal(int signum, sighandler_t handler)
{
	int retval;
#ifdef HAVE_SIGACTION
	struct sigaction sact;

	mset((char *)&sact, 0, SIZEOF(sact));
	sact.sa_handler = handler;
#ifdef SA_INTERRUPT
	sact.sa_flags = SA_INTERRUPT;
#endif
	retval = sigaction(signum, &sact, NULL);
#elif defined(HAVE_SIGVEC)
	struct sigvec svec;

	mset(&svec, 0, SIZEOF(svec));
	svec.sv_handler = handler;
#ifdef HAVE_SV_INTERRUPT
	svec.sv_flags = SV_INTERRUPT;
#endif
	retval = sigvec(signum, &svec, NULL);
#else
	retval = (signal(signum, handler) != SIG_ERR) ? 0 : -1;
#ifdef HAVE_SIGINTERRUPT
	siginterrupt(signum, 1);
#endif
#endif
	return(retval);
}

/* Helpful little parsing utilities */

/* Skip whitespace and return first non-whitespace character */

int parse_ws(char **pp,int cmt)
{
	char *p = *pp;
	while (*p==' ' || *p=='\t')
		++p;
 	if (*p=='\r' || *p=='\n' || *p==cmt)
		*p = 0;
	*pp = p;
	return *p;
}

/* Parse an identifier into a buffer.  Identifier is truncated to a maximum of len-1 chars. */

int parse_ident(char * *pp, char *buf, ptrdiff_t len)
{
	char *p = *pp;
	if (joe_isalpha_(locale_map,*p)) {
		while(len > 1 && joe_isalnum_(locale_map,*p))
			*buf++= *p++, --len;
		*buf=0;
		while(joe_isalnum_(locale_map,*p))
			++p;
		*pp = p;
		return 0;
	} else
		return -1;
}

/* Parse to next whitespace */

int parse_tows(char * *pp, char *buf)
{
	char *p = *pp;
	while (*p && *p!=' ' && *p!='\t' && *p!='\n' && *p!='\r' && *p!='#')
		*buf++ = *p++;

	*pp = p;
	*buf = 0;
	return 0;
}

/* Parse over a specific keyword */

int parse_kw(char * *pp, const char *kw)
{
	char *p = *pp;
	while(*kw && *kw==*p)
		++kw, ++p;
	if(!*kw && !joe_isalnum_(locale_map,*p)) {
		*pp = p;
		return 0;
	} else
		return -1;
}

/* Parse a field (same as parse_kw, but string must be terminated with whitespace) */

int parse_field(char **pp, const char *kw)
{
	char *p = *pp;
	while(*kw && *kw==*p)
		++kw, ++p;
	if(!*kw && (!*p || *p==' ' || *p=='\t' || *p=='#' || *p=='\n' || *p=='\r')) {
		*pp = p;
		return 0;
	} else
		return -1;
}

/* Parse a specific character */

int parse_char(char **pp, char c)
{
	char *p = *pp;
	if (*p == c) {
		*pp = p+1;
		return 0;
	} else
		return -1;
}

/* Parse an integer.  Returns 0 for success. */

int parse_int(char **pp, int *buf)
{
	char *p = *pp;
	if ((*p>='0' && *p<='9') || *p=='-') {
		*buf = ztoi(p);
		if(*p=='-')
			++p;
		while(*p>='0' && *p<='9')
			++p;
		*pp = p;
		return 0;
	} else
		return -1;
}

int parse_diff(char **pp, ptrdiff_t *buf)
{
	char *p = *pp;
	if ((*p>='0' && *p<='9') || *p=='-') {
		*buf = ztodiff(p);
		if(*p=='-')
			++p;
		while(*p>='0' && *p<='9')
			++p;
		*pp = p;
		return 0;
	} else
		return -1;
}

/* Parse a long */

int parse_off_t(char * *pp, off_t *buf)
{
	char *p = *pp;
	off_t val = 0;
	int flg = 0;
	if ((*p>='0' && *p<='9') || *p=='-') {
		if(*p=='-') {
			++p;
			flg = 1;
		}
		while(*p >= '0' && *p <= '9') {
			val = val * 10 + (int)(*p - '0');
			++p;
		}
		if (flg)
			val = -val;
		*buf = val;
		*pp = p;
		return 0;
	} else
		return -1;
}

/* Parse a string of the form "xxxxx" into a fixed-length buffer.  The
 * address of the buffer is 'buf'.  The length of this buffer is 'len'.  A
 * terminating NUL is added to the parsed string.  If the string is larger
 * than the buffer, the string is truncated.
 *
 * C string escape sequences are handled.
 *
 * 'p' holds an address of the input string pointer.  The pointer
 * is updated to point right after the parsed string if the function
 * succeeds.
 *
 * Returns the length of the string (not including the added NUL), or
 * -1 if there is no string or if the input ended before the terminating ".
 */

ptrdiff_t parse_string(char **pp, char *buf, ptrdiff_t len)
{
	char *start = buf;
	char *p= *pp;
	if(*p=='\"') {
		++p;
		while(len > 1 && *p && *p!='\"') {
			ptrdiff_t x = 50;
			int c = escape(0, (const char **)&p, &x);
			*buf++ = TO_CHAR_OK(c);
			--len;
		}
		*buf = 0;
		while(*p && *p!='\"')
			if(*p=='\\' && p[1])
				p += 2;
			else
				p++;
		if(*p == '\"') {
			*pp = p + 1;
			return buf - start;
		}
	}
	return -1;
}

/* Emit a string with escape sequences */

#ifdef junk

/* Used originally for printing macros */

void emit_string(FILE *f,const char *s,ptrdiff_t len)
{
	char buf[8];
	char *p, *q;
	fputc('\"',f);
	while(len) {
		p = unescape(buf,*s++);
		for(q=buf;q!=p;++q)
			fputc(*q,f);
		--len;
	}
	fputc('\"',f);
}
#endif

/* Emit a string */

void emit_string(FILE *f,const char *s,ptrdiff_t len)
{
	fputc('"',f);
	while(len) {
		if (*s=='"' || *s=='\\')
			fputc('\\',f), fputc(*s,f);
		else if(*s=='\n')
			fputc('\\',f), fputc('n',f);
		else if(*s=='\r')
			fputc('\\',f), fputc('r',f);
		else if(*s==0)
			fputc('\\',f), fputc('0',f), fputc('0',f), fputc('0',f);
		else
			fputc(*s,f);
		++s;
		--len;
	}
	fputc('"',f);
}

/* Parse a character range: a-z */

int parse_range(char * *pp, int *first, int *second)
{
	char *p= *pp;
	int a, b;
	if(!*p)
		return -1;
	if(*p=='\\' && p[1]) {
		++p;
		if(*p=='n')
			a = '\n';
		else if(*p=='t')
  			a = '\t';
		else
			a = *p;
		++p;
	} else
		a = *p++;
	if(*p=='-' && p[1]) {
		++p;
		if(*p=='\\' && p[1]) {
			++p;
			if(*p=='n')
				b = '\n';
			else if(*p=='t')
				b = '\t';
			else
				b = *p;
			++p;
		} else
			b = *p++;
	} else
		b = a;
	*first = a;
	*second = b;
	*pp = p;
	return 0;
}

void jsort(void *base, ptrdiff_t num, ptrdiff_t size, int (*compar)(const void *a, const void *b))
{
	return qsort(base, (size_t)num, (size_t)size, compar);
}

off_t oabs(off_t a)
{
	if (a < 0)
		return -a;
	else
		return a;
}
