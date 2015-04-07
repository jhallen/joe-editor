/*
 *	Variable length array of strings
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

aELEMENT *vamk(ptrdiff_t len)
{
	ptrdiff_t *newa = (ptrdiff_t *) joe_malloc((1 + len) * SIZEOF(aELEMENT) + 2 * SIZEOF(ptrdiff_t));

	newa[0] = len;
	newa[1] = 0;
	((aELEMENT *)(newa + 2))[0] = aterm;
	return (aELEMENT *)(newa + 2);
}

void varm(aELEMENT *vary)
{
	if (vary) {
		vazap(vary, 0, aLen(vary));
		joe_free((ptrdiff_t *) vary - 2);
	}
}

ptrdiff_t alen(aELEMENT *ary)
{
	if (ary) {
		aELEMENT *beg = ary;
		while (acmp(*ary, aterm))
			++ary;
		return ary - beg;
	} else
		return 0;
}

aELEMENT *vaensure(aELEMENT *vary, ptrdiff_t len)
{
	if (!vary)
		vary = vamk(len);
	else if (len > aSiz(vary)) {
		len += (len >> 2);
		vary = (aELEMENT *)(2 + (ptrdiff_t *) joe_realloc((ptrdiff_t *) vary - 2, (len + 1) * SIZEOF(aELEMENT) + 2 * SIZEOF(ptrdiff_t)));

		aSiz(vary) = len;
	}
	return vary;
}

aELEMENT *vazap(aELEMENT *vary, ptrdiff_t pos, ptrdiff_t n)
{
	if (vary) {
		ptrdiff_t x;

		if (pos < aLen(vary)) {
			if (pos + n <= aLen(vary)) {
				for (x = pos; x != pos + n; ++x)
					adel(vary[x]);
			} else {
				for (x = pos; x != aLen(vary); ++x)
					adel(vary[x]);
			}
		}
	}
	return vary;
}

aELEMENT *vatrunc(aELEMENT *vary, ptrdiff_t len)
{
	if (!vary || len > aLEN(vary))
		vary = vaensure(vary, len);
	if (len < aLen(vary)) {
		vary = vazap(vary, len, aLen(vary) - len);
		vary[len] = vary[aLen(vary)];
		aLen(vary) = len;
	} else if (len > aLen(vary)) {
		vary = vafill(vary, aLen(vary), ablank, len - aLen(vary));
	}
	return vary;
}

aELEMENT *vafill(aELEMENT *vary, ptrdiff_t pos, aELEMENT el, ptrdiff_t len)
{
	ptrdiff_t olen = aLEN(vary), x;

	if (!vary || pos + len > aSIZ(vary))
		vary = vaensure(vary, pos + len);
	if (pos + len > olen) {
		vary[pos + len] = vary[olen];
		aLen(vary) = pos + len;
	}
	for (x = pos; x != pos + len; ++x)
		vary[x] = adup(el);
	if (pos > olen)
		vary = vafill(vary, pos, ablank, pos - olen);
	return vary;
}

#ifdef junk
aELEMENT *vancpy(aELEMENT *vary, ptrdiff_t pos, aELEMENT *array, ptrdiff_t len)
{
	ptrdiff_t olen = aLEN(vary);

	if (!vary || pos + len > aSIZ(vary))
		vary = vaensure(vary, pos + len);
	if (pos + len > olen) {
		vary[pos + len] = vary[olen];
		aLen(vary) = pos + len;
	}
	if (pos > olen)
		vary = vafill(vary, olen, ablank, pos - olen);
	mfwrd(vary + pos, array, len * SIZEOF(aELEMENT));
	return vary;
}
#endif

aELEMENT *vandup(aELEMENT *vary, ptrdiff_t pos, aELEMENT *array, ptrdiff_t len)
{
	ptrdiff_t olen = aLEN(vary), x;

	if (!vary || pos + len > aSIZ(vary))
		vary = vaensure(vary, pos + len);
	if (pos + len > olen) {
		vary[pos + len] = vary[olen];
		aLen(vary) = pos + len;
	}
	if (pos > olen)
		vary = vafill(vary, olen, ablank, pos - olen);
	for (x = 0; x != len; ++x)
		vary[x + pos] = adup(array[x]);
	return vary;
}

aELEMENT *vadup(aELEMENT *vary)
{
	return vandup(NULL, 0, vary, aLEN(vary));
}

aELEMENT *_vaset(aELEMENT *vary, ptrdiff_t pos, aELEMENT el)
{
	if (!vary || pos + 1 > aSIZ(vary))
		vary = vaensure(vary, pos + 1);
	if (pos > aLen(vary)) {
		vary = vafill(vary, aLen(vary), ablank, pos - aLen(vary));
		vary[pos + 1] = vary[pos];
		vary[pos] = el;
		aLen(vary) = pos + 1;
	} else if (pos == aLen(vary)) {
		vary[pos + 1] = vary[pos];
		vary[pos] = el;
		aLen(vary) = pos + 1;
	} else {
		adel(vary[pos]);
		vary[pos] = el;
	}
	return vary;
}

static int _acmp(aELEMENT *a, aELEMENT *b)
{
	return acmp(*a, *b);
}

aELEMENT *vasort(aELEMENT *ary, ptrdiff_t len)
{
	if (!ary || !len)
		return ary;
	jsort(ary, len, SIZEOF(aELEMENT), (int (*)(const void *, const void *))_acmp);
	return ary;
}

aELEMENT *vawords(aELEMENT *a, const char *s, ptrdiff_t len, const char *sep, ptrdiff_t seplen)
{
	ptrdiff_t x;

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
