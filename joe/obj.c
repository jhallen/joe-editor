/*
 * Global object stack, plus dynamic string and dynamic array functions
 * built on top of them.
 *	Copyright
 *		(C) 2006 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

#include "types.h"

/* Global Object stack */

Obj obj_stack[1] = { { { obj_stack, obj_stack } } };

/* Save current stack and create a new one. */

Obj get_obj_stack()
{
	Obj o = obj_stack[0];
	izque(Obj, link, obj_stack);
	return o;
}

/* Free current stack, restore a saved stack. */

void set_obj_stack(Obj o)
{
	Obj *blk, *nxt;
	for (blk = obj_stack->link.next; blk != obj_stack; blk = nxt) {
		nxt = blk->link.next;
		joe_free(deque_f(Obj, link, blk));
	}
	obj_stack[0] = o;
}

/* Allocate an object on the object stack.  If size is 0, a minimum sized
 * object is allocated */

void *obj_malloc(int size)
{
	Obj *blk;
	size = round_up(size, 8);
	blk = joe_malloc(size + obj_hdr_size);
	blk->size = size;
	enqueb(Obj, link, obj_stack, blk);
	return (void *)blk->mem;
}

/* Change allocation size.  If 'ptr' is NULL, a new object is allocated. obj_realloc
 * does not change the stack order of the object. */

void *obj_realloc(void *ptr, int new_size)
{
	if (!ptr)
		return obj_malloc(new_size);
	else {
		Obj *blk = obj_base(ptr);
		new_size = round_up(new_size, 8);
		if (new_size > blk->size) {
			if (obj_isperm(ptr)) {
				/* Object is in heap */
				blk = joe_realloc(blk, new_size + obj_hdr_size);
				blk->size = new_size;
			} else {
				/* Object is on stack */
				Obj *nxt = blk->link.next;
				deque(Obj, link, blk);
				blk = joe_realloc(blk, new_size + obj_hdr_size);
				blk->size = new_size;
				enqueb(Obj, link, nxt, blk);
			}
		}
		return (void *)blk->mem;
	}
}

/* Free an object, and if it is on the stack, free all newer objects as well. */

void obj_free(void *ptr)
{
	if (ptr) {
		Obj *nxt = obj_base(ptr);
		if (obj_isperm(ptr)) {
			joe_free(nxt);
		} else {
			do {
				Obj *blk = nxt;
				nxt = blk->link.next;
				joe_free(deque_f(Obj, link, blk));
			} while (nxt != obj_stack);
		}
	}
}

/* Remove an object from the stack and put it on the heap. */

void obj_perm(void *ptr)
{
	if (ptr) {
		Obj *blk = obj_base(ptr);
		if (blk->link.next) {
			deque(Obj, link, blk);
			blk->link.next = 0;
			blk->link.prev = 0;
		}
	}
}

void obj_temp(void *ptr)
{
	if (ptr) {
		Obj *blk = obj_base(ptr);
		if (!blk->link.next)
			enqueb(Obj, link, obj_stack, blk);
	}
}

/* Return true if object is a heap object. */

int obj_isperm(void *ptr)
{
	if (ptr) {
		Obj *blk = obj_base(ptr);
		return !blk->link.next;
	}
	return 0;
}

/** Dynamic string functions **/

/* Create string, reserve 'len' bytes */

unsigned char *vsmk(int len)
{
	unsigned char *n = (unsigned char *)obj_malloc(len + 1);
	n[0] = 0;
	obj_len(n) = 0;
	return n;
}

/* Get length of string */

int vslen(unsigned char *vary)
{
	if (vary)
		return obj_len(vary);
	else
		return 0;
}

/* Make sure there is enough space for a string of len bytes, but
   don't modify the string. */

unsigned char *vsensure(unsigned char *vary, int len)
{
	if (!vary) {
		vary = (unsigned char *)obj_malloc(len + 1);
		vary[0] = 0;
		obj_len(vary) = 0;
	} else {
		++len; /* For terminator */
		if (len > obj_size(vary)) {
			len += (len >> 1);
			vary = (unsigned char *)obj_realloc(vary, len);
		}
	}
	return vary;
}

/* Truncate or expand string to specified length.  The string is expanded
 * with spaces if necessary. */

unsigned char *vstrunc(unsigned char *vary, int len)
{
	int olen;
	if (!vary) {
		olen = 0;
		vary = (unsigned char *)obj_malloc(len + 1);
		vary[0] = 0;
		obj_len(vary) = 0;
	} else {
		olen = obj_len(vary);
	}
	if (len < olen) {
		vary[len] = 0;
		obj_len(vary) = len;
	} else if (len > olen) {
		vary = vsfill(vary, olen, ' ', len - olen);
	}
	return vary;
}

/* Write spaces to an area of a string */

unsigned char *vsfill(unsigned char *vary, int pos, int el, int len)
{
	int olen;
	int x;
	if (!vary) {
		olen = 0;
		vary = (unsigned char *)obj_malloc(pos + len + 1);
		vary[0] = 0;
		obj_len(vary) = 0;
	} else {
		olen = obj_len(vary);
		if (pos + len + 1 > obj_size(vary))
			vary = (unsigned char *)obj_realloc(vary, pos + len + 1);
	}
	if (pos + len > olen) {
		vary[pos + len] = 0;
		obj_len(vary) = pos + len;
	}
	if (pos > olen) {
		int x;
		for (x = olen; x != pos; ++x)
			vary[x] = ' ';
	}
	for (x = pos; x != pos + len; ++x)
		vary[x] = el;
	return vary;
}

/* Copy memory block to an area of a string */

unsigned char *vsncpy(unsigned char *vary, int pos, unsigned char *array, int len)
{
	int olen;
	if (!vary) {
		olen = 0;
		vary = (unsigned char *)obj_malloc(pos + len + 1);
		vary[0] = 0;
		obj_len(vary) = 0;
	} else {
		olen = obj_len(vary);
		if (pos + len + 1 > obj_size(vary))
			vary = (unsigned char *)obj_realloc(vary, pos + len + 1);
	}
	if (pos + len > olen) {
		vary[pos + len] = 0;
		obj_len(vary) = pos + len;
	}
	if (pos > olen)
		vary = vsfill(vary, olen, ' ', pos - olen);
	mmove(vary + pos, array, len);
	return vary;
}

unsigned char *vsdup(unsigned char *vary)
{
	return vsncpy(NULL, 0, vary, vslen(vary));
}

unsigned char *vsdupz(unsigned char *z)
{
	return vsncpy(NULL, 0, z, zlen(z));
}

/* Overwrite a string from a block */

unsigned char *vscpy(unsigned char *vary, unsigned char *s, int len)
{
	vary = vstrunc(vary, 0);
	return vsncpy(vary, 0, s, len);
}

/* Overwrite a string from a zstring */

unsigned char *vscpyz(unsigned char *vary, unsigned char *z)
{
	vary = vstrunc(vary, 0);
	return vsncpy(vary, 0, z, zlen(z));
}

/* Append a block to a string */

unsigned char *vscat(unsigned char *vary, unsigned char *s, int len)
{
	return vsncpy(vary, vslen(vary), s, len);
}

/* Append a zstring to a string */

unsigned char *vscatz(unsigned char *vary, unsigned char *z)
{
	return vsncpy(vary, vslen(vary), z, zlen(z));
}

/* Append a single character to a string */

unsigned char *_vsadd(unsigned char *vary, unsigned char c)
{
	if (vary && (obj_len(vary) + 1) < obj_size(vary)) {
		vary[obj_len(vary)++] = c;
		vary[obj_len(vary)] = 0;
		return vary;
	} else
		return vsncpy(vary, vslen(vary), &c, 1);
}

unsigned char *_vsset(unsigned char *vary, int pos, unsigned char el)
{
	if (!vary || pos + 1 > obj_size(vary))
		vary = vsensure(vary, pos + 1);
	if (pos > obj_len(vary)) {
		vary = vsfill(vary, obj_len(vary), ' ', pos - obj_len(vary));
		vary[pos + 1] = 0;
		vary[pos] = el;
		obj_len(vary) = pos + 1;
	} else if (pos == obj_len(vary)) {
		vary[pos + 1] = 0;
		vary[pos] = el;
		obj_len(vary) = pos + 1;
	} else {
		vary[pos] = el;
	}
	return vary;
}

/* Get line of input */

unsigned char *vsgets(unsigned char **sp, FILE *f)
{
	unsigned char *s = *sp;
	int s_size;
	int i;
	int c = 0;

	if (!s)
		s = vsmk(128);

	obj_len(s) = 0;

	for (i = 0;;) {
		s_size = obj_size(s);
		for (; i != s_size && ((c = getc(f), (c != -1 && c != '\n' && c != '\r'))); ++i)
			s[i] = c;
		if (i == s_size) {
			s = vsensure(s, s_size * 2);
		} else
			break;
	}

	if (i == s_size) {
		s = vsensure(s, i + 1);
	}

	s[i] = 0;
	obj_len(s) = i;

	*sp = s;

	if (!i && c == -1)
		return 0;
	else
		return s;
}

/* Sprintf to a variable string. */

#ifdef junk

unsigned char *vsfmt(unsigned char *vary, int pos, const unsigned char *format, ...)
{
	unsigned char buf[10240];
	va_list ap;

	/* Traverse list to guess size */
	va_start(ap, format);
	vsnprintf((char *)buf, sizeof(buf), format, ap);
	va_end(ap);

	/* Allocate buffer */
	return vsncpy(vary, pos, buf, zlen(buf));
}
#endif

#define _upper 1
#define _signed 2
#define _plus 4
#define _space 8
#define _zero 16
#define _alt 32
#define _left 64

#if SIZEOF_LONG_LONG
static int _cvt(unsigned char *ts,int base,int prec,int flag,unsigned long long n)
#else
static int _cvt(unsigned char *ts,int base,int prec,int flag,unsigned long n)
#endif
  {
  int x,y=0;
  char ary[128], *cv=flag&_upper?"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ":"0123456789abcdefghijklmnopqrstuvwxyz";

  if(flag&_signed)
    {
#if SIZEOF_LONG_LONG
    if((long long)n<0)
#else
    if((long)n<0)
#endif
      {
      ts[y++]='-';
      n= -n;
      goto skip;
      }
    }
  if(flag&_plus)
    {
    ts[y++]='+';
    goto skip;
    }
  if(flag&_space)
    {
    ts[y++]=' ';
    goto skip;
    }
  skip:

  x=0;
  if (base == 10)
    do ary[x++]=cv[n%10]; while(n/=10);
  else if (base == 16)
    do ary[x++]=cv[n%16]; while(n/=16);
  else if (base == 8)
    do ary[x++]=cv[n%8]; while(n/=8);
  else
    do ary[x++]=cv[n%base]; while(n/=base);
  prec -= x;
  while (prec > 0)
    {
    ts[y++] = '0';
    --prec;
    }
  while (x)
    ts[y++]=ary[--x];
  return y;
  }

unsigned char *vsfmt(unsigned char *vary, int pos, const unsigned char *format, ...)
  {
  unsigned char buf[128];
  va_list ap;

  vary = vstrunc(vary, pos);

  va_start(ap, format);

  while (*format)
    {
    if (*format == '%')
      {
      /* Flags */
      int flags = 0;

      int width = 0;
      int prec = 0;
      int arg_size = sizeof(int);

      ++format;

      /* Parse flags */
      for (;;) {
        switch (*format) {
          case '0':
            flags |= _zero;
            ++format;
            break;

          case '#':
            flags |= _alt;
            ++format;
            break;

          case '-':
            flags |= _left;
            ++format;
            break;

          case '+':
            flags |= _plus;
            ++format;
            break;

          case ' ':
            flags |= _space;
            ++format;
            break;

          default:
            goto done_flags;
        }
      }
      done_flags:

      /* Field width */
      while (*format >= '0' && *format <= '9')
        width = width * 10 + *format++ - '0';

      /* Precision */
      if (*format == '.')
        {
        ++format;
        while (*format >= '0' && *format <= '9')
          prec = prec * 10 + *format++ - '0';
        }

      /* Argument size */
      switch (*format)
        {
        case 'z': /* size_t */
        case 't': /* ptrdiff_t: this is wrong, but ptrdiff_t is not portably defined */
          {
          arg_size = sizeof(size_t);
          ++format;
          break;
          }
#ifdef junk
        case 't': /* ptrdiff_t */
          {
          arg_size = sizeof(ptrdiff_t);
          ++format;
          break;
          }
#endif
        case 'l': /* long */
          {
          arg_size = sizeof(long);
          ++format;
          break;
          }

        case 'O': /* off_t */
          {
          arg_size = sizeof(off_t);
          ++format;
          break;
          }
        }

      /* Argument and print type */
      switch (*format)
        {
        int base;
        int actual;
        unsigned char c;
        unsigned char *s;
        
        case 's': /* String */
          {
          /* Get string */
          s = va_arg(ap, unsigned char *);

          if (!s)
            s = USTR "(null)";

          if (!prec)
            actual = zlen(s);
          else
            for (actual = 0; actual != prec && s[actual]; ++actual);

          goto print_it;
          }

        case '%': /* Print % */
          {
          /* Copy character */
          if (obj_len(vary) == obj_size(vary))
            vary = vsensure(vary, obj_size(vary) + 2);

          vary[obj_len(vary)++] = '%';
          break;
          }

        case 'c': /* Character */
          {
          /* Get character */
          c = va_arg(ap, int);

          /* Point to it */
          s = &c;
          actual = 1;

          goto print_it;
          }

        case 'u': /* Unsigned int */
          {
          base = 10;
          goto print_number;
          }

        case 'x': /* Lowercase hex */
          {
          base = 16;
          goto print_number;
          }

        case 'X': /* Uppercase hex */
          {
          base = 16;
          flags |= _upper;
          goto print_number;
          }

        case 'p': /* Pointer */
          {
          arg_size = sizeof(void *);
          base = 16;
          goto print_number;
          }

        case 'o': /* Octal */
          {
          base = 8;
          goto print_number;
          }

        case 'd': /* Signed int */
          {
          int fill;
#if SIZEOF_LONG_LONG
          long long n;
#else
          long n;
#endif

          flags |= _signed;

          base = 10;

          print_number:

          n = 0;

          /* Read numeric argument */
          if (flags & _signed)
            {
            if (arg_size == sizeof(int))
              n = va_arg(ap, int);
	    else if (arg_size == sizeof(long))
	      n = va_arg(ap, long);
#if SIZEOF_LONG_LONG
	    else if (arg_size == sizeof(long long))
	      n = va_arg(ap, long long);
#endif
	    }
	  else
	    {
            if (arg_size == sizeof(int))
              n = va_arg(ap, unsigned int);
	    else if (arg_size == sizeof(long))
	      n = va_arg(ap, unsigned long);
#if SIZEOF_LONG_LONG
	    else if (arg_size == sizeof(long long))
	      n = va_arg(ap, unsigned long long);
#endif
	    }

          /* Generate number, return width */
          actual = _cvt(buf,base,prec,flags,n);
          s = buf;

          print_it:

          /* Figure out fill */
          if (width > actual)
            fill = width - actual;
          else
            fill = 0;
          
          if (!(flags & _left) && fill)
            /* Fill for right justified */
            vary = vsfill(vary, obj_len(vary), (flags & _zero ? '0': ' '), fill);

          /* Print field */
          vary = vscat(vary, s, actual);

          if ((flags & _left) && fill)
            /* Fill for left justified */
            vary = vsfill(vary, obj_len(vary), (flags & _zero ? '0': ' '), fill);

          break;
          }
        }
      if (*format)
        ++format;
      }
    else
      {
      /* Copy character */
      if (obj_len(vary) == obj_size(vary))
        vary = vsensure(vary, obj_size(vary) + 2);

      vary[obj_len(vary)++] = *format++;
      }
    }

  /* Add terminator */
  if (obj_len(vary) == obj_size(vary))
    vary = vsensure(vary, obj_size(vary) + 1);

  vary[obj_len(vary)] = 0;

  va_end(ap);

  return vary;
  }


int vsbsearch(unsigned char *ary, int len, unsigned char el)
{
	int x, y, z;

	if (!ary || !len)
		return 0;
	y = len;
	x = 0;
	z = ~0;
	while (z != (x + y) / 2) {
		z = (x + y) / 2;
		if (el > ary[z])
			x = z;
		else if (el < ary[z])
			y = z;
		else
			return z;
	}
	return y;
}

int vsscan(unsigned char *a, int alen, unsigned char *b, int blen)
{
	int x;

	for (x = 0; x != alen; ++x) {
		int z = vsbsearch(b, blen, a[x]);

		if (z < blen && b[z]==a[x])
			return x;
	}
	return ~0;
}

int vsspan(unsigned char *a, int alen, unsigned char *b, int blen)
{
	int x;

	for (x = 0; x != alen; ++x) {
		int z = vsbsearch(b, blen, a[x]);

		if (z == blen || b[z] != a[x])
			break;
	}
	return x;
}


/** Dynamic arrays of strings **/

/* Return length, NULL allowed for vary */

int valen(unsigned char **vary)
{
	if (vary)
		return obj_len(vary);
	else
		return 0;
}

/* Create array with enough space preallocated for len strings */

unsigned char **vamk(int len)
{
	unsigned char **n = obj_malloc((len + 1) * sizeof(unsigned char *));
	obj_len(n) = 0;
	n[0] = 0;
	return n;
}

/* Move array into heap */

void vaperm(unsigned char **a)
{
	int x;
	obj_perm(a);
	for (x = 0; x != valen(a); ++x)
		obj_perm(a[x]);
}

/* Delete an array */

void varm(unsigned char **vary)
{
	if (vary) {
		vazap(vary, 0, obj_len(vary));
		obj_free(vary);
	}
}

/* Make sure there is enough space for len strings */

unsigned char **vaensure(unsigned char **vary, int len)
{
	if (!vary)
		vary = vamk(len);
	else if ((len + 1) * sizeof(unsigned char *) > obj_size(vary)) {
		len += (len >> 1);
		vary = obj_realloc(vary, (len + 1) * sizeof(unsigned char *));
	}
	return vary;
}

/* Delete strings */

unsigned char **vazap(unsigned char **vary, int pos, int n)
{
	if (vary) {
		int x;
		int isperm = obj_isperm(vary);

		if (pos < obj_len(vary)) {
			if (pos + n <= obj_len(vary)) {
				for (x = pos; x != pos + n; ++x) {
					if (isperm) obj_free(vary[x]); 
					vary[x] = 0;
				}
			} else {
				for (x = pos; x != obj_len(vary); ++x) {
					if (isperm) obj_free(vary[x]);
					vary[x] = 0;
				}
			}
		}
	}
	return vary;
}

unsigned char **vatrunc(unsigned char **vary, int len)
{
	if (!vary || len > obj_len(vary))
		vary = vaensure(vary, len);
	if (len < obj_len(vary)) {
		vary = vazap(vary, len, obj_len(vary) - len);
		vary[len] = 0;
		obj_len(vary) = len;
	} else if (len > obj_len(vary)) {
		int x;
		for (x = obj_len(vary); x != len; ++x)
			vary[x] = 0;
		vary[x] = 0;
		obj_len(vary) = x;
	}
	return vary;
}

unsigned char **vafill(unsigned char **vary, int pos, unsigned char *el, int len)
{
	int olen = valen(vary), x;
	int isperm;

	if (!vary || (pos + len) * sizeof(unsigned char *) > obj_size(vary))
		vary = vaensure(vary, pos + len);
	isperm = obj_isperm(vary);
	if (pos + len > olen) {
		int x;
		for (x = olen; x != pos + len; ++x)
			vary[x] = 0;
		vary[x] = 0;
		obj_len(vary) = pos + len;
	}
	for (x = pos; x != pos + len; ++x) {
		if (vary[x] && isperm)
			obj_free(vary[x]);
		/* Not sure about this... */
		if (el) {
			vary[x] = vsdup(el);
			if (isperm)
				obj_perm(vary[x]);
		} else
			vary[x] = 0;
	}
	return vary;
}

unsigned char **vandup(unsigned char **vary, int pos, unsigned char **array, int len)
{
	int isperm;
	int olen = valen(vary), x;

	if (!vary || (pos + len) * sizeof(unsigned char *) > obj_size(vary))
		vary = vaensure(vary, pos + len);
	isperm = obj_isperm(vary);
	if (pos + len > olen) {
		int x;
		for (x = olen; x != pos + len; ++x)
			vary[x] = 0;
		vary[x] = 0;
		obj_len(vary) = pos + len;
	}
	for (x = 0; x != len; ++x) {
		if (vary[x + pos] && isperm)
			obj_free(vary[x + pos]);
		vary[x + pos] = vsdup(array[x]);
		if (isperm)
			obj_perm(vary[x + pos]);
	}
	return vary;
}

unsigned char **vadup(unsigned char **vary)
{
	return vandup(NULL, 0, vary, valen(vary));
}

unsigned char **vaadd(unsigned char **vary, unsigned char *s)
{
	vary = vaensure(vary, valen(vary) + 1);
	if (obj_isperm(vary))
		obj_perm(s);
	vary[obj_len(vary)] = s;
	++obj_len(vary);
	vary[obj_len(vary)] = 0;
	return vary;
}

static int _acmp(unsigned char **a, unsigned char **b)
{
	return zcmp(*a, *b);
}

void vasort(unsigned char **ary, int len)
{
	if (!ary || !len)
		return;
	qsort(ary, len, sizeof(unsigned char *), (int (*)(const void *, const void *))_acmp);
}

static int _aicmp(unsigned char **a, unsigned char **b)
{
	return stricmp(*a, *b);
}

void vaisort(unsigned char **ary, int len)
{
	if (!ary || !len)
		return;
	qsort(ary, len, sizeof(unsigned char *), (int (*)(const void *, const void *))_aicmp);
}

unsigned char **vawords(unsigned char **a, unsigned char *s, int len, unsigned char *sep, int seplen)
{
	int x;

	if (!a)
		a = vamk(10);
	else
		a = vatrunc(a, 0);
      loop:
	x = vsspan(s, len, sep, seplen);
	s += x;
	len -= x;
	if (len) {
		x = vsscan(s, len, sep, seplen);
		if (x != ~0) {
			a = vaadd(a, vsncpy(vsmk(x), 0, s, x));
			s += x;
			len -= x;
			if (len)
				goto loop;
		} else
			a = vaadd(a, vsncpy(vsmk(len), 0, s, len));
	}
	return a;
}

unsigned char **isfree(unsigned char **s)
{
	obj_free(*s);
	*s = 0;
	return s;
}
