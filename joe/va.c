/*
 *	Variable length array of strings
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

aELEMENT *vamk(int len)
{
	int *new = (int *) joe_malloc((1 + len) * sizeof(aELEMENT) + 2 * sizeof(int));

	new[0] = len;
	new[1] = 0;
	((aELEMENT *)(new + 2))[0] = aterm;
	return (aELEMENT *)(new + 2);
}

void varm(aELEMENT *vary)
{
	if (vary) {
		vazap(vary, 0, aLen(vary));
		joe_free((int *) vary - 2);
	}
}

int alen(aELEMENT *ary)
{
	if (ary) {
		aELEMENT *beg = ary;
		while (acmp(*ary, aterm))
			++ary;
		return ary - beg;
	} else
		return 0;
}

aELEMENT *vaensure(aELEMENT *vary, int len)
{
	if (!vary)
		vary = vamk(len);
	else if (len > aSiz(vary)) {
		len += (len >> 2);
		vary = (aELEMENT *)(2 + (int *) joe_realloc((int *) vary - 2, (len + 1) * sizeof(aELEMENT) + 2 * sizeof(int)));

		aSiz(vary) = len;
	}
	return vary;
}

aELEMENT *vazap(aELEMENT *vary, int pos, int n)
{
	if (vary) {
		int x;

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

aELEMENT *vatrunc(aELEMENT *vary, int len)
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

aELEMENT *vafill(aELEMENT *vary, int pos, aELEMENT el, int len)
{
	int olen = aLEN(vary), x;

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
aELEMENT *vancpy(aELEMENT *vary, int pos, aELEMENT *array, int len)
{
	int olen = aLEN(vary);

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

aELEMENT *vandup(aELEMENT *vary, int pos, aELEMENT *array, int len)
{
	int olen = aLEN(vary), x;

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

aELEMENT *_vaset(aELEMENT *vary, int pos, aELEMENT el)
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

aELEMENT *vasort(aELEMENT *ary, int len)
{
	if (!ary || !len)
		return ary;
	qsort(ary, len, sizeof(aELEMENT), (int (*)(const void *, const void *))_acmp);
	return ary;
}

aELEMENT *vawords(aELEMENT *a, unsigned char *s, int len, unsigned char *sep, int seplen)
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
