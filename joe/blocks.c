/*
 *	Fast block move/copy subroutines
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
/* This module requires ALIGNED and SIZEOF_INT to be defined correctly */

#include "types.h"

#define BITS 8

#if SIZEOF_INT == 8
#  define SHFT 3
#elif SIZEOF_INT == 4
#  define SHFT 2
#elif SIZEOF_INT == 2
#  define SHFT 1
#endif

/* Set 'sz' 'int's beginning at 'd' to the value 'c' */
/* Returns address of block.  Does nothing if 'sz' equals zero */

int *msetI(int *dest, int c, ptrdiff_t sz)
{
	int	*d = dest;
	int	*orgd = dest;

	while (sz >= 16) {
		d[0] = c;
		d[1] = c;
		d[2] = c;
		d[3] = c;
		d[4] = c;
		d[5] = c;
		d[6] = c;
		d[7] = c;
		d[8] = c;
		d[9] = c;
		d[10] = c;
		d[11] = c;
		d[12] = c;
		d[13] = c;
		d[14] = c;
		d[15] = c;
		d += 16;
		sz -= 16;
	}
	switch (sz) {
	case 15:	d[14] = c;
	case 14:	d[13] = c;
	case 13:	d[12] = c;
	case 12:	d[11] = c;
	case 11:	d[10] = c;
	case 10:	d[9] = c;
	case 9:		d[8] = c;
	case 8:		d[7] = c;
	case 7:		d[6] = c;
	case 6:		d[5] = c;
	case 5:		d[4] = c;
	case 4:		d[3] = c;
	case 3:		d[2] = c;
	case 2:		d[1] = c;
	case 1:		d[0] = c;
	case 0:		/* do nothing */;
	}
	return orgd;
}

ptrdiff_t *msetD(ptrdiff_t *dest, ptrdiff_t c, ptrdiff_t sz)
{
	ptrdiff_t	*d = dest;
	ptrdiff_t	*orgd = dest;

	while (sz >= 16) {
		d[0] = c;
		d[1] = c;
		d[2] = c;
		d[3] = c;
		d[4] = c;
		d[5] = c;
		d[6] = c;
		d[7] = c;
		d[8] = c;
		d[9] = c;
		d[10] = c;
		d[11] = c;
		d[12] = c;
		d[13] = c;
		d[14] = c;
		d[15] = c;
		d += 16;
		sz -= 16;
	}
	switch (sz) {
	case 15:	d[14] = c;
	case 14:	d[13] = c;
	case 13:	d[12] = c;
	case 12:	d[11] = c;
	case 11:	d[10] = c;
	case 10:	d[9] = c;
	case 9:		d[8] = c;
	case 8:		d[7] = c;
	case 7:		d[6] = c;
	case 6:		d[5] = c;
	case 5:		d[4] = c;
	case 4:		d[3] = c;
	case 3:		d[2] = c;
	case 2:		d[1] = c;
	case 1:		d[0] = c;
	case 0:		/* do nothing */;
	}
	return orgd;
}

/* Set 'sz' 'int's beginning at 'd' to the value 'c' */
/* Returns address of block.  Does nothing if 'sz' equals zero */

void **msetP(void **d, void *c, ptrdiff_t sz)
{
	void	**orgd = d;

	while (sz >= 16) {
		d[0] = c;
		d[1] = c;
		d[2] = c;
		d[3] = c;
		d[4] = c;
		d[5] = c;
		d[6] = c;
		d[7] = c;
		d[8] = c;
		d[9] = c;
		d[10] = c;
		d[11] = c;
		d[12] = c;
		d[13] = c;
		d[14] = c;
		d[15] = c;
		d += 16;
		sz -= 16;
	}
	switch (sz) {
	case 15:	d[14] = c;
	case 14:	d[13] = c;
	case 13:	d[12] = c;
	case 12:	d[11] = c;
	case 11:	d[10] = c;
	case 10:	d[9] = c;
	case 9:		d[8] = c;
	case 8:		d[7] = c;
	case 7:		d[6] = c;
	case 6:		d[5] = c;
	case 5:		d[4] = c;
	case 4:		d[3] = c;
	case 3:		d[2] = c;
	case 2:		d[1] = c;
	case 1:		d[0] = c;
	case 0:		/* do nothing */;
	}
	return orgd;
}

/* Set 'sz' 'char's beginning at 'd' to the value 'c' */
/* Returns address of block.  Does nothing if 'sz' equals zero */

char *mset(char *dest, char c, ptrdiff_t sz)
{
	char	*d = dest;
	char	*orgd = dest;

	if (sz < 16) {
		switch (sz) {
		case 15:	d[14] = c;
		case 14:	d[13] = c;
		case 13:	d[12] = c;
		case 12:	d[11] = c;
		case 11:	d[10] = c;
		case 10:	d[9] = c;
		case 9:		d[8] = c;
		case 8:		d[7] = c;
		case 7:		d[6] = c;
		case 6:		d[5] = c;
		case 5:		d[4] = c;
		case 4:		d[3] = c;
		case 3:		d[2] = c;
		case 2:		d[1] = c;
		case 1:		d[0] = c;
		case 0:		/* do nothing */;
		}
	} else {
		ptrdiff_t z = SIZEOF_INT - ((ptrdiff_t)d & (SIZEOF_INT - 1));

		if (z != SIZEOF_INT) {
			switch (z) {
			case 7:		d[6] = c;
			case 6:		d[5] = c;
			case 5:		d[4] = c;
			case 4:		d[3] = c;
			case 3:		d[2] = c;
			case 2:		d[1] = c;
			case 1:		d[0] = c;
			case 0:		/* do nothing */;
			}
			d += z;
			sz -= z;
		}
		msetI((int *)d,
#if SIZEOF_INT >= 8
		      (c << (BITS * 7)) + (c << (BITS * 6)) + (c << (BITS * 5)) + (c << (BITS * 4)) +
#endif
#if SIZEOF_INT >= 4
		      (c << (BITS * 3)) + (c << (BITS * 2)) +
#endif
#if SIZEOF_INT >= 2
		      (c << BITS) +
#endif
		      c, sz >> SHFT);
		d += sz & ~(SIZEOF_INT - 1);
		switch (sz & (SIZEOF_INT - 1)) {
		case 7:		d[6] = c;
		case 6:		d[5] = c;
		case 5:		d[4] = c;
		case 4:		d[3] = c;
		case 3:		d[2] = c;
		case 2:		d[1] = c;
		case 1:		d[0] = c;
		case 0:		/* do nothing */;
		}
	}
	return orgd;
}

/* Copy a block of integers */
/* Copy from highest address to lowest */

static int *mbkwdI(int *dest, const int *src, ptrdiff_t sz)
{
	int	*d = dest;
	const int *s = src;

	if (d == s)
		return d;
	d += sz;
	s += sz;
	while (sz >= 16) {
		d -= 16;
		s -= 16;
		d[15] = s[15];
		d[14] = s[14];
		d[13] = s[13];
		d[12] = s[12];
		d[11] = s[11];
		d[10] = s[10];
		d[9] = s[9];
		d[8] = s[8];
		d[7] = s[7];
		d[6] = s[6];
		d[5] = s[5];
		d[4] = s[4];
		d[3] = s[3];
		d[2] = s[2];
		d[1] = s[1];
		d[0] = s[0];
		sz -= 16;
	}
	d -= sz;
	s -= sz;
	switch (sz) {
	case 15:	d[14] = s[14];
	case 14:	d[13] = s[13];
	case 13:	d[12] = s[12];
	case 12:	d[11] = s[11];
	case 11:	d[10] = s[10];
	case 10:	d[9] = s[9];
	case 9:		d[8] = s[8];
	case 8:		d[7] = s[7];
	case 7:		d[6] = s[6];
	case 6:		d[5] = s[5];
	case 5:		d[4] = s[4];
	case 4:		d[3] = s[3];
	case 3:		d[2] = s[2];
	case 2:		d[1] = s[1];
	case 1:		d[0] = s[0];
	case 0:		/* do nothing */;
	}
	return d;
}

/* Copy a block of 'int's.  Copy from lowest address to highest */

static int *mfwrdI(int *dest, const int *src, ptrdiff_t sz)
{
	int	*d = dest;
	const int *s = src;
	int	*od = d;

	if (s == d)
		return d;
	while (sz >= 16) {
		d[0] = s[0];
		d[1] = s[1];
		d[2] = s[2];
		d[3] = s[3];
		d[4] = s[4];
		d[5] = s[5];
		d[6] = s[6];
		d[7] = s[7];
		d[8] = s[8];
		d[9] = s[9];
		d[10] = s[10];
		d[11] = s[11];
		d[12] = s[12];
		d[13] = s[13];
		d[14] = s[14];
		d[15] = s[15];
		s += 16;
		d += 16;
		sz -= 16;
	}
	s -= 15 - sz;
	d -= 15 - sz;
	switch (sz) {
	case 15:	d[0] = s[0];
	case 14:	d[1] = s[1];
	case 13:	d[2] = s[2];
	case 12:	d[3] = s[3];
	case 11:	d[4] = s[4];
	case 10:	d[5] = s[5];
	case 9:		d[6] = s[6];
	case 8:		d[7] = s[7];
	case 7:		d[8] = s[8];
	case 6:		d[9] = s[9];
	case 5:		d[10] = s[10];
	case 4:		d[11] = s[11];
	case 3:		d[12] = s[12];
	case 2:		d[13] = s[13];
	case 1:		d[14] = s[14];
	case 0:		/* do nothing */;
	}
	return od;
}

/* Copy the block of 'sz' bytes beginning at 's' to 'd'.  If 'sz' is zero or
 * if 's'=='d', nothing happens.  The bytes at the highest address ('s'+'sz'-1)
 * are copied before the ones at the lowest ('s') are.
 */

static void *mbkwd(register void *orgd, register const void *orgs, register ptrdiff_t sz)
{
	char *d = (char *)orgd;
	const char *s = (const char *)orgs;
	if (s == d)
		return d;
	s += sz;
	d += sz;
#ifdef ALIGNED
	if (sz >= 16)
#else
	if (((ptrdiff_t)s & (SIZEOF_INT - 1)) == ((ptrdiff_t)d & (SIZEOF_INT - 1)) && sz >= 16)
#endif
	{
		ptrdiff_t z = ((ptrdiff_t)s & (SIZEOF_INT - 1));

		s -= z;
		d -= z;
		switch (z) {
		case 7:		d[6] = s[6];
		case 6:		d[5] = s[5];
		case 5:		d[4] = s[4];
		case 4:		d[3] = s[3];
		case 3:		d[2] = s[2];
		case 2:		d[1] = s[1];
		case 1:		d[0] = s[0];
		case 0:		/* do nothing */;
		}
		sz -= z;
		mbkwdI((int *)(d - (sz & ~(SIZEOF_INT - 1))), (const int *)(s - (sz & ~(SIZEOF_INT - 1))), sz >> SHFT);
		d -= sz;
		s -= sz;
		switch (sz & (SIZEOF_INT - 1)) {
		case 7:		d[6] = s[6];
		case 6:		d[5] = s[5];
		case 5:		d[4] = s[4];
		case 4:		d[3] = s[3];
		case 3:		d[2] = s[2];
		case 2:		d[1] = s[1];
		case 1:		d[0] = s[0];
		case 0:		/* do nothing */;
		}
	} else {
		while (sz >= 16) {
			d -= 16;
			s -= 16;
			d[15] = s[15];
			d[14] = s[14];
			d[13] = s[13];
			d[12] = s[12];
			d[11] = s[11];
			d[10] = s[10];
			d[9] = s[9];
			d[8] = s[8];
			d[7] = s[7];
			d[6] = s[6];
			d[5] = s[5];
			d[4] = s[4];
			d[3] = s[3];
			d[2] = s[2];
			d[1] = s[1];
			d[0] = s[0];
			sz -= 16;
		}
		d -= sz;
		s -= sz;
		switch (sz) {
		case 15:	d[14] = s[14];
		case 14:	d[13] = s[13];
		case 13:	d[12] = s[12];
		case 12:	d[11] = s[11];
		case 11:	d[10] = s[10];
		case 10:	d[9] = s[9];
		case 9:		d[8] = s[8];
		case 8:		d[7] = s[7];
		case 7:		d[6] = s[6];
		case 6:		d[5] = s[5];
		case 5:		d[4] = s[4];
		case 4:		d[3] = s[3];
		case 3:		d[2] = s[2];
		case 2:		d[1] = s[1];
		case 1:		d[0] = s[0];
		case 0:		/* do nothing */;
		}
	}
	return d;
}

/* Copy the block of 'sz' bytes beginning at 's' to 'd'.  If 'sz' is zero or
 * if 's'=='d', nothing happens.  The bytes at the lowest address ('s')
 * are copied before the ones at the highest ('s'+'sz'-1) are.
 */

static void *mfwrd(register void *orgd, register const void *orgs, register ptrdiff_t sz)
{
	char *d = (char *)orgd;
	const char *s = (const char *)orgs;

	if (d == s)
		return d;
#ifdef ALIGNED
	if (sz >= 16)
#else
	if (((ptrdiff_t)d & (SIZEOF_INT - 1)) == ((ptrdiff_t)s & (SIZEOF_INT - 1)) && sz >= 16)
#endif
	{
		ptrdiff_t z = ((ptrdiff_t)s & (SIZEOF_INT - 1));

		if (z) {
			s -= z;
			d -= z;
			switch (SIZEOF_INT - z) {
#if SIZEOF_INT == 8
			case 7:		d[1] = s[1];
			case 6:		d[2] = s[2];
			case 5:		d[3] = s[3];
			case 4:		d[4] = s[4];
			case 3:		d[5] = s[5];
			case 2:		d[6] = s[6];
			case 1:		d[7] = s[7];
			case 0:		/* do nothing */;
#else
#if SIZEOF_INT == 4
			case 3:		d[1] = s[1];
			case 2:		d[2] = s[2];
			case 1:		d[3] = s[3];
			case 0:		/* do nothing */;
#else
#if SIZEOF_INT == 2
			case 1:		d[1] = s[1];
			case 0:		/* do nothing */;
#endif
#endif
#endif
			}
			s += SIZEOF_INT;
			d += SIZEOF_INT;
			sz -= SIZEOF_INT - z;
		}
		mfwrdI((int *)d, (const int *)s, sz >> SHFT);
		s += sz - (SIZEOF_INT - 1);
		d += sz - (SIZEOF_INT - 1);
		switch (sz & (SIZEOF_INT - 1)) {
#if SIZEOF_INT == 8
		case 7:		d[0] = s[0];
		case 6:		d[1] = s[1];
		case 5:		d[2] = s[2];
		case 4:		d[3] = s[3];
		case 3:		d[4] = s[4];
		case 2:		d[5] = s[5];
		case 1:		d[6] = s[6];
		case 0:		/* do nothing */;
#else
#if SIZEOF_INT == 4
		case 3:		d[0] = s[0];
		case 2:		d[1] = s[1];
		case 1:		d[2] = s[2];
		case 0:		/* do nothing */;
#else
#if SIZEOF_INT == 2
		case 1:		d[0] = s[0];
		case 0:		/* do nothing */;
#endif
#endif
#endif
		}
	} else {
		while (sz >= 16) {
			d[0] = s[0];
			d[1] = s[1];
			d[2] = s[2];
			d[3] = s[3];
			d[4] = s[4];
			d[5] = s[5];
			d[6] = s[6];
			d[7] = s[7];
			d[8] = s[8];
			d[9] = s[9];
			d[10] = s[10];
			d[11] = s[11];
			d[12] = s[12];
			d[13] = s[13];
			d[14] = s[14];
			d[15] = s[15];
			s += 16;
			d += 16;
			sz -= 16;
		}
		s -= 15 - sz;
		d -= 15 - sz;
		switch (sz) {
		case 15:	d[0] = s[0];
		case 14:	d[1] = s[1];
		case 13:	d[2] = s[2];
		case 12:	d[3] = s[3];
		case 11:	d[4] = s[4];
		case 10:	d[5] = s[5];
		case 9:		d[6] = s[6];
		case 8:		d[7] = s[7];
		case 7:		d[8] = s[8];
		case 6:		d[9] = s[9];
		case 5:		d[10] = s[10];
		case 4:		d[11] = s[11];
		case 3:		d[12] = s[12];
		case 2:		d[13] = s[13];
		case 1:		d[14] = s[14];
		case 0:		/* do nothing */;
		}
	}
	return orgd;
}

void *mmove(void *d, const void *s, ptrdiff_t sz)
{
	if (d > s)
		mbkwd(d, s, sz);
	else
		mfwrd(d, s, sz);
	return d;
}

/* Utility to count number of lines within a segment */

ptrdiff_t mcnt(register const char *blk, register char c, ptrdiff_t size)
{
	register ptrdiff_t nlines = 0;

	while (size >= 16) {
		if (blk[0] == c) ++nlines;
		if (blk[1] == c) ++nlines;
		if (blk[2] == c) ++nlines;
		if (blk[3] == c) ++nlines;
		if (blk[4] == c) ++nlines;
		if (blk[5] == c) ++nlines;
		if (blk[6] == c) ++nlines;
		if (blk[7] == c) ++nlines;
		if (blk[8] == c) ++nlines;
		if (blk[9] == c) ++nlines;
		if (blk[10] == c) ++nlines;
		if (blk[11] == c) ++nlines;
		if (blk[12] == c) ++nlines;
		if (blk[13] == c) ++nlines;
		if (blk[14] == c) ++nlines;
		if (blk[15] == c) ++nlines;
		blk += 16;
		size -= 16;
	}
	switch (size) {
	case 15:	if (blk[14] == c) ++nlines;
	case 14:	if (blk[13] == c) ++nlines;
	case 13:	if (blk[12] == c) ++nlines;
	case 12:	if (blk[11] == c) ++nlines;
	case 11:	if (blk[10] == c) ++nlines;
	case 10:	if (blk[9] == c) ++nlines;
	case 9:		if (blk[8] == c) ++nlines;
	case 8:		if (blk[7] == c) ++nlines;
	case 7:		if (blk[6] == c) ++nlines;
	case 6:		if (blk[5] == c) ++nlines;
	case 5:		if (blk[4] == c) ++nlines;
	case 4:		if (blk[3] == c) ++nlines;
	case 3:		if (blk[2] == c) ++nlines;
	case 2:		if (blk[1] == c) ++nlines;
	case 1:		if (blk[0] == c) ++nlines;
	case 0:		/* do nothing */;
	}
	return nlines;
}

#ifdef junk

char *mchr(register char *blk, char c)
{
    loop:
	if (blk[0] == c) return blk + 0;
	if (blk[1] == c) return blk + 1;
	if (blk[2] == c) return blk + 2;
	if (blk[3] == c) return blk + 3;
	if (blk[4] == c) return blk + 4;
	if (blk[5] == c) return blk + 5;
	if (blk[6] == c) return blk + 6;
	if (blk[7] == c) return blk + 7;
	if (blk[8] == c) return blk + 8;
	if (blk[9] == c) return blk + 9;
	if (blk[10] == c) return blk + 10;
	if (blk[11] == c) return blk + 11;
	if (blk[12] == c) return blk + 12;
	if (blk[13] == c) return blk + 13;
	if (blk[14] == c) return blk + 14;
	if (blk[15] == c) return blk + 15;
	blk += 15;
	goto loop;
}

#endif
