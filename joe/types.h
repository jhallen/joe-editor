/* JOE global header file */

#include "config.h"

/* Common header files */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
typedef int pid_t;
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#define joe_gettext(s) my_gettext((unsigned char *)(s))

/*
#ifdef ENABLE_NLS
#include <libintl.h>
#define joe_gettext(s) (unsigned char *)gettext((char *)(s))
#else
#define joe_gettext(s) ((unsigned char *)(s))
#endif
*/

/* Strings needing translation are marked with this macro */
#define _(s) (s)

/* Global Defines */

/* Prefix to make string constants unsigned */
#define USTR (unsigned char *)

/* Doubly-linked list node */
#define LINK(type) struct { type *next; type *prev; }

#ifdef HAVE_SNPRINTF

#define joe_snprintf_0(buf,len,fmt) zlcpy((buf),(len),(unsigned char *)(fmt))
#define joe_snprintf_1(buf,len,fmt,a) snprintf((char *)(buf),(len),(char *)(fmt),(a))
#define joe_snprintf_2(buf,len,fmt,a,b) snprintf((char *)(buf),(len),(char *)(fmt),(a),(b))
#define joe_snprintf_3(buf,len,fmt,a,b,c) snprintf((char *)(buf),(len),(char *)(fmt),(a),(b),(c))
#define joe_snprintf_4(buf,len,fmt,a,b,c,d) snprintf((char *)(buf),(len),(char *)(fmt),(a),(b),(c),(d))
#define joe_snprintf_5(buf,len,fmt,a,b,c,d,e) snprintf((char *)(buf),(len),(char *)(fmt),(a),(b),(c),(d),(e))
#define joe_snprintf_6(buf,len,fmt,a,b,c,d,e,f) snprintf((char *)(buf),(len),(char *)(fmt),(a),(b),(c),(d),(e),(f))
#define joe_snprintf_7(buf,len,fmt,a,b,c,d,e,f,g) snprintf((char *)(buf),(len),(char *)(fmt),(a),(b),(c),(d),(e),(f),(g))
#define joe_snprintf_8(buf,len,fmt,a,b,c,d,e,f,g,h) snprintf((char *)(buf),(len),(char *)(fmt),(a),(b),(c),(d),(e),(f),(g),(h))
#define joe_snprintf_9(buf,len,fmt,a,b,c,d,e,f,g,h,i) snprintf((char *)(buf),(len),(char *)(fmt),(a),(b),(c),(d),(e),(f),(g),(h),(i))
#define joe_snprintf_10(buf,len,fmt,a,b,c,d,e,f,g,h,i,j) snprintf((char *)(buf),(len),(char *)(fmt),(a),(b),(c),(d),(e),(f),(g),(h),(i),(j))

#define logmessage_0(fmt) (zlcpy((i_msg),sizeof(i_msg),(unsigned char *)(fmt)), internal_msg(i_msg))
#define logmessage_1(fmt,a) (snprintf((char *)(i_msg),sizeof(i_msg),(char *)(fmt),(a)), internal_msg(i_msg))
#define logmessage_2(fmt,a,b) (snprintf((char *)(i_msg),sizeof(i_msg),(char *)(fmt),(a),(b)), internal_msg(i_msg))
#define logmessage_3(fmt,a,b,c) (snprintf((char *)(i_msg),sizeof(i_msg),(char *)(fmt),(a),(b),(c)), internal_msg(i_msg))
#define logmessage_4(fmt,a,b,c,d) (snprintf((char *)(i_msg),sizeof(i_msg),(char *)(fmt),(a),(b),(c),(d)), internal_msg(i_msg))

#define logerror_0(fmt) (zlcpy((i_msg),sizeof(i_msg),(unsigned char *)(fmt)), internal_msg(i_msg), setlogerrs())
#define logerror_1(fmt,a) (snprintf((char *)(i_msg),sizeof(i_msg),(char *)(fmt),(a)), internal_msg(i_msg), setlogerrs())
#define logerror_2(fmt,a,b) (snprintf((char *)(i_msg),sizeof(i_msg),(char *)(fmt),(a),(b)), internal_msg(i_msg), setlogerrs())
#define logerror_3(fmt,a,b,c) (snprintf((char *)(i_msg),sizeof(i_msg),(char *)(fmt),(a),(b),(c)), internal_msg(i_msg), setlogerrs())
#define logerror_4(fmt,a,b,c,d) (snprintf((char *)(i_msg),sizeof(i_msg),(char *)(fmt),(a),(b),(c),(d)), internal_msg(i_msg), setlogerrs())

#else

#define joe_snprintf_0(buf,len,fmt) sprintf((char *)(buf),(char *)(fmt))
#define joe_snprintf_1(buf,len,fmt,a) sprintf((char *)(buf),(char *)(fmt),(a))
#define joe_snprintf_2(buf,len,fmt,a,b) sprintf((char *)(buf),(char *)(fmt),(a),(b))
#define joe_snprintf_3(buf,len,fmt,a,b,c) sprintf((char *)(buf),(char *)(fmt),(a),(b),(c))
#define joe_snprintf_4(buf,len,fmt,a,b,c,d) sprintf((char *)(buf),(char *)(fmt),(a),(b),(c),(d))
#define joe_snprintf_5(buf,len,fmt,a,b,c,d,e) sprintf((char *)(buf),(char *)(fmt),(a),(b),(c),(d),(e))
#define joe_snprintf_6(buf,len,fmt,a,b,c,d,e,f) sprintf((char *)(buf),(char *)(fmt),(a),(b),(c),(d),(e),(f))
#define joe_snprintf_7(buf,len,fmt,a,b,c,d,e,f,g) sprintf((char *)(buf),(char *)(fmt),(a),(b),(c),(d),(e),(f),(g))
#define joe_snprintf_8(buf,len,fmt,a,b,c,d,e,f,g,h) sprintf((char *)(buf),(char *)(fmt),(a),(b),(c),(d),(e),(f),(g),(h))
#define joe_snprintf_9(buf,len,fmt,a,b,c,d,e,f,g,h,i) sprintf((char *)(buf),(char *)(fmt),(a),(b),(c),(d),(e),(f),(g),(h),(i))
#define joe_snprintf_10(buf,len,fmt,a,b,c,d,e,f,g,h,i,j) sprintf((char *)(buf),(char *)(fmt),(a),(b),(c),(d),(e),(f),(g),(h),(i),(j))

#define logmessage_0(fmt) (sprintf((char *)(i_msg),(char *)(fmt)), internal_msg(i_msg))
#define logmessage_1(fmt,a) (sprintf((char *)(i_msg),(char *)(fmt),(a)), internal_msg(i_msg))
#define logmessage_2(fmt,a,b) (sprintf((char *)(i_msg),(char *)(fmt),(a),(b)), internal_msg(i_msg))
#define logmessage_3(fmt,a,b,c) (sprintf((char *)(i_msg),(char *)(fmt),(a),(b),(c)), internal_msg(i_msg))
#define logmessage_4(fmt,a,b,c,d) (sprintf((char *)(i_msg),(char *)(fmt),(a),(b),(c),(d)), internal_msg(i_msg))

#define logerror_0(fmt) (sprintf((char *)(i_msg),(char *)(fmt)), internal_msg(i_msg), setlogerrs())
#define logerror_1(fmt,a) (sprintf((char *)(i_msg),(char *)(fmt),(a)), internal_msg(i_msg), setlogerrs())
#define logerror_2(fmt,a,b) (sprintf((char *)(i_msg),(char *)(fmt),(a),(b)), internal_msg(i_msg), setlogerrs())
#define logerror_3(fmt,a,b,c) (sprintf((char *)(i_msg),(char *)(fmt),(a),(b),(c)), internal_msg(i_msg), setlogerrs())
#define logerror_4(fmt,a,b,c,d) (sprintf((char *)(i_msg),(char *)(fmt),(a),(b),(c),(d)), internal_msg(i_msg), setlogerrs())

#endif

/* Largest signed integer */
#define MAXINT  ((((unsigned int)-1)/2)-1)

/* Largest signed long */
#define MAXLONG ((((unsigned long)-1L)/2)-1)

/* Largest signed long long */
#define MAXLONGLONG ((((unsigned long long)-1L)/2)-1)

/* Largest off_t */
/* BSD provides a correct OFF_MAX macro, but AIX provides a broken one,
   so do it ourselves. */
#if (SIZEOF_OFF_T == SIZEOF_INT)
#define MAXOFF MAXINT
#elif (SIZEOF_OFF_T == SIZEOF_LONG)
#define MAXOFF MAXLONG
#elif (SIZEOF_OFF_T == SIZEOF_LONG_LONG)
#define MAXOFF MAXLONGLONG
#else
#error off_t is not an int, long, or long long?
#endif

#include <stdio.h>
#ifndef EOF
#define EOF -1
#endif
#define NO_MORE_DATA EOF

#if defined __MSDOS__ && SIZEOF_INT == 2 /* real mode ms-dos compilers */
#if SIZEOF_VOID_P == 4 /* real mode ms-dos compilers with 'far' memory model or something like that */
#define physical(a)  (((unsigned long)(a)&0xFFFF)+(((unsigned long)(a)&0xFFFF0000)>>12))
#define normalize(a) ((void *)(((unsigned long)(a)&0xFFFF000F)+(((unsigned long)(a)&0x0000FFF0)<<12)))
#else
#define physical(a) ((unsigned long)(a))
#define normalize(a) (a)
#endif /* sizeof(void *) == 4 */

#define SEGSIZ 1024
#define PGSIZE 1024
#define LPGSIZE 10
#define ILIMIT (PGSIZE*96L)
#define HTSIZE 128

#else /* not real mode ms-dos */

#define physical(a) ((unsigned long)(a))
#define normalize(a) (a)

/* Log2 of page size */
#define LPGSIZE 12
/* No. bytes in page */
#define PGSIZE (1<<LPGSIZE)
/* Gap buffer size (must be same as page size) */
#define SEGSIZ PGSIZE

/* Max number of pages allowed in core */
#define NPAGES 8192
/* Max core memory used in bytes */
#define ILIMIT (PGSIZE*NPAGES)
/* Hash table size (should be double the max number of pages) */
#define HTSIZE (NPAGES*2)

#endif /* real mode ms-dos */


/* These do not belong here. */

/* #define KEYS		256 */
#define KEYS 267	/* 256 ascii + mdown, mup, mdrag, m2down, m2up, m2drag,
                                        m3down, m3up, m3drag */
#define KEY_MDOWN	256
#define KEY_MUP		257
#define KEY_MDRAG	258
#define KEY_M2DOWN	259
#define KEY_M2UP	260
#define KEY_M2DRAG	261
#define KEY_M3DOWN	262
#define KEY_M3UP	263
#define KEY_M3DRAG	264
#define KEY_MWUP	265
#define KEY_MWDOWN	266

#define stdsiz		8192
#define FITHEIGHT	4		/* Minimum text window height */
#define LINCOLS		10
#define NPROC		8		/* Number of processes we keep track of */
#define INC		16		/* Pages to allocate each time */

#define TYPETW		0x0100
#define TYPEPW		0x0200
#define TYPEMENU	0x0800
#define TYPEQW		0x1000

/* Typedefs */

typedef struct header H;
typedef struct buffer B;
typedef struct point P;
typedef struct options OPTIONS;
typedef struct macro MACRO;
typedef struct cmd CMD;
typedef struct entry HENTRY;
typedef struct hash HASH;
typedef struct kmap KMAP;
typedef struct kbd KBD;
typedef struct key KEY;
typedef struct watom WATOM;
typedef struct screen Screen;
typedef struct window W;
typedef struct base BASE;
typedef struct bw BW;
typedef struct menu MENU;
typedef struct scrn SCRN;
typedef struct cap CAP;
typedef struct pw PW;
typedef struct stditem STDITEM;
typedef struct query QW;
typedef struct tw TW;
typedef struct undo UNDO;
typedef struct undorec UNDOREC;
typedef struct search SRCH;
typedef struct srchrec SRCHREC;
typedef struct vpage VPAGE;
typedef struct vfile VFILE;
typedef struct highlight_state HIGHLIGHT_STATE;
typedef struct mpx MPX;
typedef struct jfile JFILE;
typedef struct vt_context VT;

/* Structure which are passed by value */

struct highlight_state {
	struct high_frame *stack;   /* Pointer to the current frame in the call stack */
	int state;                  /* Current state in the current subroutine */
	unsigned char saved_s[24];  /* Buffer for saved delimiters */
};

/* Include files */

#include "b.h"
#include "blocks.h"
#include "bw.h"
#include "charmap.h"
#include "cmd.h"
#include "hash.h"
#include "help.h"
#include "i18n.h"
#include "kbd.h"
#include "lattr.h"
#include "macro.h"
#include "main.h"
#include "menu.h"
#include "mouse.h"
#include "path.h"
#include "poshist.h"
#include "pw.h"
#include "queue.h"
#include "qw.h"
#include "rc.h"
#include "regex.h"
#include "scrn.h"
#include "syntax.h"
#include "tab.h"
#include "termcapj.h"
#include "tty.h"
#include "tw.h"
#include "ublock.h"
#include "uedit.h"
#include "uerror.h"
#include "ufile.h"
#include "uformat.h"
#include "uisrch.h"
#include "umath.h"
#include "undo.h"
#include "usearch.h"
#include "ushell.h"
#include "utag.h"
#include "utf8.h"
#include "utils.h"
#include "va.h"
#include "vfile.h"
#include "vs.h"
#include "w.h"
#include "gettext.h"
#include "builtin.h"
#include "vt.h"
