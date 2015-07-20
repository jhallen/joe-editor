/* JOE global header file */

#define _FILE_OFFSET_BITS 64
/* #define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#define _GNU_SOURCE 1 */

#define TO_DIFF_OK(a) ((ptrdiff_t)(a)) /* Means it's OK that we are converting off_t to ptrdiff_t in this case */
#define TO_CHAR_OK(a) ((char)(a)) /* Means it's OK that we are converting int to char */
#define SIZEOF(a) ((ptrdiff_t)sizeof(a)) /* Signed version of sizeof() */

#define WIND_BW(x, y) do { \
  if (!((y)->watom->what & (TYPETW | TYPEPW))) \
    return -1; \
  (x) = (BW *)(y)->object; \
  } while(0)

#define WIND_MENU(x, y) do { \
  if ((y)->watom->what != TYPEMENU) \
    return -1; \
  (x) = (MENU *)(y)->object; \
  } while(0)

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

#ifdef HAVE_STDDEF_H
#include <stddef.h>
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

#define joe_gettext(s) my_gettext((s))

/*
#ifdef ENABLE_NLS
#include <libintl.h>
#define joe_gettext(s) (char *)gettext((char *)(s))
#else
#define joe_gettext(s) ((char *)(s))
#endif
*/

/* Strings needing translation are marked with this macro */
#define _(s) (s)

/* Global Defines */

/* Doubly-linked list node */
#define LINK(type) struct { type *next; type *prev; }

#ifdef HAVE_SNPRINTF

#define joe_snprintf_0(buf,len,fmt) zlcpy((buf),(len),(fmt))
#define joe_snprintf_1(buf,len,fmt,a) snprintf((buf),(len),(fmt),(a))
#define joe_snprintf_2(buf,len,fmt,a,b) snprintf((buf),(len),(fmt),(a),(b))
#define joe_snprintf_3(buf,len,fmt,a,b,c) snprintf((buf),(len),(fmt),(a),(b),(c))
#define joe_snprintf_4(buf,len,fmt,a,b,c,d) snprintf((buf),(len),(fmt),(a),(b),(c),(d))
#define joe_snprintf_5(buf,len,fmt,a,b,c,d,e) snprintf((buf),(len),(fmt),(a),(b),(c),(d),(e))
#define joe_snprintf_6(buf,len,fmt,a,b,c,d,e,f) snprintf((buf),(len),(fmt),(a),(b),(c),(d),(e),(f))
#define joe_snprintf_7(buf,len,fmt,a,b,c,d,e,f,g) snprintf((buf),(len),(fmt),(a),(b),(c),(d),(e),(f),(g))
#define joe_snprintf_8(buf,len,fmt,a,b,c,d,e,f,g,h) snprintf((buf),(len),(fmt),(a),(b),(c),(d),(e),(f),(g),(h))
#define joe_snprintf_9(buf,len,fmt,a,b,c,d,e,f,g,h,i) snprintf((buf),(len),(fmt),(a),(b),(c),(d),(e),(f),(g),(h),(i))
#define joe_snprintf_10(buf,len,fmt,a,b,c,d,e,f,g,h,i,j) snprintf((buf),(len),(fmt),(a),(b),(c),(d),(e),(f),(g),(h),(i),(j))

#define logmessage_0(fmt) (zlcpy((i_msg),sizeof(i_msg),(fmt)), internal_msg(i_msg))
#define logmessage_1(fmt,a) (snprintf((i_msg),sizeof(i_msg),(fmt),(a)), internal_msg(i_msg))
#define logmessage_2(fmt,a,b) (snprintf((i_msg),sizeof(i_msg),(fmt),(a),(b)), internal_msg(i_msg))
#define logmessage_3(fmt,a,b,c) (snprintf((i_msg),sizeof(i_msg),(fmt),(a),(b),(c)), internal_msg(i_msg))
#define logmessage_4(fmt,a,b,c,d) (snprintf((i_msg),sizeof(i_msg),(fmt),(a),(b),(c),(d)), internal_msg(i_msg))

#define logerror_0(fmt) (zlcpy((i_msg),sizeof(i_msg),(fmt)), internal_msg(i_msg), setlogerrs())
#define logerror_1(fmt,a) (snprintf((i_msg),sizeof(i_msg),(fmt),(a)), internal_msg(i_msg), setlogerrs())
#define logerror_2(fmt,a,b) (snprintf((i_msg),sizeof(i_msg),(fmt),(a),(b)), internal_msg(i_msg), setlogerrs())
#define logerror_3(fmt,a,b,c) (snprintf((i_msg),sizeof(i_msg),(fmt),(a),(b),(c)), internal_msg(i_msg), setlogerrs())
#define logerror_4(fmt,a,b,c,d) (snprintf((i_msg),sizeof(i_msg),(fmt),(a),(b),(c),(d)), internal_msg(i_msg), setlogerrs())

#else

#define joe_snprintf_0(buf,len,fmt) sprintf((buf),(fmt))
#define joe_snprintf_1(buf,len,fmt,a) sprintf((buf),(fmt),(a))
#define joe_snprintf_2(buf,len,fmt,a,b) sprintf((buf),(fmt),(a),(b))
#define joe_snprintf_3(buf,len,fmt,a,b,c) sprintf((buf),(fmt),(a),(b),(c))
#define joe_snprintf_4(buf,len,fmt,a,b,c,d) sprintf((buf),(fmt),(a),(b),(c),(d))
#define joe_snprintf_5(buf,len,fmt,a,b,c,d,e) sprintf((buf),(fmt),(a),(b),(c),(d),(e))
#define joe_snprintf_6(buf,len,fmt,a,b,c,d,e,f) sprintf((buf),(fmt),(a),(b),(c),(d),(e),(f))
#define joe_snprintf_7(buf,len,fmt,a,b,c,d,e,f,g) sprintf((buf),(fmt),(a),(b),(c),(d),(e),(f),(g))
#define joe_snprintf_8(buf,len,fmt,a,b,c,d,e,f,g,h) sprintf((buf),(fmt),(a),(b),(c),(d),(e),(f),(g),(h))
#define joe_snprintf_9(buf,len,fmt,a,b,c,d,e,f,g,h,i) sprintf((buf),(fmt),(a),(b),(c),(d),(e),(f),(g),(h),(i))
#define joe_snprintf_10(buf,len,fmt,a,b,c,d,e,f,g,h,i,j) sprintf((buf),(fmt),(a),(b),(c),(d),(e),(f),(g),(h),(i),(j))

#define logmessage_0(fmt) (sprintf((i_msg),(fmt)), internal_msg(i_msg))
#define logmessage_1(fmt,a) (sprintf((i_msg),(fmt),(a)), internal_msg(i_msg))
#define logmessage_2(fmt,a,b) (sprintf((i_msg),(fmt),(a),(b)), internal_msg(i_msg))
#define logmessage_3(fmt,a,b,c) (sprintf((i_msg),(fmt),(a),(b),(c)), internal_msg(i_msg))
#define logmessage_4(fmt,a,b,c,d) (sprintf((i_msg),(fmt),(a),(b),(c),(d)), internal_msg(i_msg))

#define logerror_0(fmt) (sprintf((i_msg),(fmt)), internal_msg(i_msg), setlogerrs())
#define logerror_1(fmt,a) (sprintf((i_msg),(fmt),(a)), internal_msg(i_msg), setlogerrs())
#define logerror_2(fmt,a,b) (sprintf((i_msg),(fmt),(a),(b)), internal_msg(i_msg), setlogerrs())
#define logerror_3(fmt,a,b,c) (sprintf((i_msg),(fmt),(a),(b),(c)), internal_msg(i_msg), setlogerrs())
#define logerror_4(fmt,a,b,c,d) (sprintf((i_msg),(fmt),(a),(b),(c),(d)), internal_msg(i_msg), setlogerrs())

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

/* We use -256 so that it's still unique even if we promoted a signed char to an int */
#define NO_MORE_DATA (-256)

#if defined __MSDOS__ && SIZEOF_INT == 2 /* real mode ms-dos compilers */
#if SIZEOF_VOID_P == 4 /* real mode ms-dos compilers with 'far' memory model or something like that */
#define physical(a)  (((unsigned long)(a)&0xFFFF)+(((unsigned long)(a)&0xFFFF0000)>>12))
#define normalize(a) ((void *)(((unsigned long)(a)&0xFFFF000F)+(((unsigned long)(a)&0x0000FFF0)<<12)))
#else
#define physical(a) ((size_t)(a))
#define normalize(a) (a)
#endif /* sizeof(void *) == 4 */

#define SEGSIZ 1024
#define PGSIZE 1024
#define LPGSIZE 10
#define ILIMIT (PGSIZE*96L)
#define HTSIZE 128

#else /* not real mode ms-dos */

#define physical(a) ((size_t)(a))
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

#define KEY_MDOWN	0x200000
#define KEY_MUP		0x200001
#define KEY_MDRAG	0x200002
#define KEY_M2DOWN	0x200003
#define KEY_M2UP	0x200004
#define KEY_M2DRAG	0x200005
#define KEY_M3DOWN	0x200006
#define KEY_M3UP	0x200007
#define KEY_M3DRAG	0x200008
#define KEY_MWUP	0x200009
#define KEY_MWDOWN	0x20000A

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
typedef struct Hash HASH;
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
typedef struct Phash PHASH;

/* Structure which are passed by value */

#define SAVED_SIZE 24

struct highlight_state {
	struct high_frame *stack; /* Pointer to the current frame in the call stack */
	ptrdiff_t state; /* Current state in the current subroutine */
	int saved_s[SAVED_SIZE]; /* Buffer for saved delimiters */
};

/* Include files */

#include "cmap.h"
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
#include "mmenu.h"
#include "state.h"
#include "options.h"
#include "selinux.h"
#include "phash.h"
#include "unicode.h"
