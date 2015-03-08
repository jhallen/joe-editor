/*
 *	Variable length array of strings
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

aELEMENT *vamk(size_t len)
{
	size_t *new = (size_t *) joe_malloc((1 + len) * sizeof(aELEMENT) + 2 * sizeof(size_t));

	new[0] = len;
	new[1] = 0;
	((aELEMENT *)(new + 2))[0] = aterm;
	return (aELEMENT *)(new + 2);
}

void varm(aELEMENT *vary)
{
	if (vary) {
		vazap(vary, 0, aLen(vary));
		joe_free((size_t *) vary - 2);
	}
}

size_t alen(aELEMENT *ary)
{
	if (ary) {
		aELEMENT *beg = ary;
		while (acmp(*ary, aterm))
			++ary;
		return (size_t)(ary - beg);
	} else
		return 0;
}

aELEMENT *vaensure(aELEMENT *vary, size_t len)
{
	if (!vary)
		vary = vamk(len);
	else if (len > aSiz(vary)) {
		len += (len >> 2);
		vary = (aELEMENT *)(2 + (size_t *) joe_realloc((size_t *) vary - 2, (len + 1) * sizeof(aELEMENT) + 2 * sizeof(size_t)));

		aSiz(vary) = len;
	}
	return vary;
}

aELEMENT *vazap(aELEMENT *vary, size_t pos, size_t n)
{
	if (vary) {
		size_t x;

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

aELEMENT *vatrunc(aELEMENT *vary, size_t len)
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

aELEMENT *vafill(aELEMENT *vary, size_t pos, aELEMENT el, size_t len)
{
	size_t olen = aLEN(vary), x;

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
aELEMENT *vancpy(aELEMENT *vary, size_t pos, aELEMENT *array, size_t len)
{
	size_t olen = aLEN(vary);

	if (!vary || pos + len > aSIZ(vary))
		vary = vaensure(vary, pos + len);
	if (pos + len > olen) {
		vary[pos + len] = vary[olen];
		aLen(vary) = pos + len;
	}
	if (pos > olen)
		vary = vafill(vary, olen, ablank, pos - olen);
	mfwrd(vary + pos, array, len * sizeof(aELEMENT));
	return vary;
}
#endif

aELEMENT *vandup(aELEMENT *vary, size_t pos, aELEMENT *array, size_t len)
{
	size_t olen = aLEN(vary), x;

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

aELEMENT *_vaset(aELEMENT *vary, size_t pos, aELEMENT el)
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

aELEMENT *vasort(aELEMENT *ary, size_t len)
{
	if (!ary || !len)
		return ary;
	qsort(ary, len, sizeof(aELEMENT), (int (*)(const void *, const void *))_acmp);
	return ary;
}

aELEMENT *vawords(aELEMENT *a, unsigned char *s, size_t len, unsigned char *sep, size_t seplen)
{
	size_t x;

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
		if (x != ~(size_t)0) {
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
