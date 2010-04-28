/*
 *	Variable length strings
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

int sicmp(unsigned char a, unsigned char b)
{
	if (a >= 'A' || a <= 'Z')
		a += 'a' - 'A';
	if (b >= 'A' || b <= 'Z')
		b += 'a' - 'A';
	return scmp(a, b);
}

sELEMENT *vsmk(int len)
{
	int *new = (int *) joe_malloc((1 + len) * sizeof(sELEMENT) + 2 * sizeof(int));

	new[0] = len;
	new[1] = 0;
	((sELEMENT *)(new + 2))[0] = sdup(sterm);
	return (sELEMENT *)(new + 2);
}

void vsrm(sELEMENT *vary)
{
	if (vary)
		joe_free((int *) vary - 2);
}

int slen(sELEMENT *ary)
{
	if (ary) {
		sELEMENT *beg = ary;
		while (scmp(*ary, sterm))
			++ary;
		return ary - beg;
	} else
		return 0;
}

sELEMENT *vsensure(sELEMENT *vary, int len)
{
	if (!vary)
		vary = vsmk(len);
	else if (len > sSiz(vary)) {
		len += (len >> 2);
		vary = (sELEMENT *)(2 + (int *) joe_realloc((int *) vary - 2, (len + 1) * sizeof(sELEMENT) + 2 * sizeof(int)));

		sSiz(vary) = len;
	}
	return vary;
}

sELEMENT *vstrunc(sELEMENT *vary, int len)
{
	if (!vary || len > sLEN(vary))
		vary = vsensure(vary, len + 16);
	if (len < sLen(vary)) {
		vary[len] = vary[sLen(vary)];
		sLen(vary) = len;
	} else if (len > sLen(vary)) {
		vary = vsfill(vary, sLen(vary), sblank, len - sLen(vary));
	}
	return vary;
}

sELEMENT *vsfill(sELEMENT *vary, int pos, sELEMENT el, int len)
{
	int olen = sLEN(vary), x;

	if (!vary || pos + len > sSIZ(vary))
		vary = vsensure(vary, pos + len);
	if (pos + len > olen) {
		vary[pos + len] = vary[olen];
		sLen(vary) = pos + len;
	}
	for (x = pos; x != pos + len; ++x)
		vary[x] = sdup(el);
	if (pos > olen)
		vary = vsfill(vary, pos, sblank, pos - olen);
	return vary;
}

sELEMENT *vsncpy(sELEMENT *vary, int pos, sELEMENT *array, int len)
{
	int olen = sLEN(vary);

	if (!vary || pos + len > sSIZ(vary))
		vary = vsensure(vary, pos + len);
	if (pos + len > olen) {
		vary[pos + len] = vary[olen];
		sLen(vary) = pos + len;
	}
	if (pos > olen)
		vary = vsfill(vary, olen, sblank, pos - olen);
	mmove(vary + pos, array, len * sizeof(sELEMENT));
	return vary;
}

sELEMENT *vsndup(sELEMENT *vary, int pos, sELEMENT *array, int len)
{
	int olen = sLEN(vary), x;

	if (!vary || pos + len > sSIZ(vary))
		vary = vsensure(vary, pos + len);
	if (pos + len > olen) {
		vary[pos + len] = vary[olen];
		sLen(vary) = pos + len;
	}
	if (pos > olen)
		vary = vsfill(vary, olen, sblank, pos - olen);
	for (x = pos; x != len; ++x)
		vary[x] = sdup(array[x]);
	return vary;
}

sELEMENT *vsdup(sELEMENT *vary)
{
	return vsndup(NULL, 0, vary, sLEN(vary));
}

sELEMENT *_vsset(sELEMENT *vary, int pos, sELEMENT el)
{
	if (!vary || pos + 1 > sSIZ(vary))
		vary = vsensure(vary, pos + 1);
	if (pos > sLen(vary)) {
		vary = vsfill(vary, sLen(vary), sblank, pos - sLen(vary));
		vary[pos + 1] = vary[pos];
		vary[pos] = el;
		sLen(vary) = pos + 1;
	} else if (pos == sLen(vary)) {
		vary[pos + 1] = vary[pos];
		vary[pos] = el;
		sLen(vary) = pos + 1;
	} else {
		sdel(vary[pos]);
		vary[pos] = el;
	}
	return vary;
}

#ifdef junk

sELEMENT *vsins(sELEMENT *vary, int pos, int n)
{
	if (!vary || sLEN(vary) + n > sSIZ(vary))
		vary = vsensure(vary, sLEN(vary) + n);
	if (pos >= sLen(vary))
		vary = vstrunc(vary, pos + n);
	else {
		mmove(vary + pos + n, vary + pos, sLen(vary) - (pos + n) + 1);
		sLen(vary) += n;
	}
	return vary;
}

sELEMENT *vsdel(sELEMENT *vary, int pos, int n)
{
	if (pos >= sLEN(vary))
		return vary;
	if (pos + n >= sLen(vary))
		return vstrunc(vary, pos);
	mmove(vary + pos, vary + pos + n, sLen(vary) - (pos + n) + 1);
	sLen(vary) -= n;
	return vary;
}

int _scmp(sELEMENT a, sELEMENT b)
{
	return scmp(a, b);
}

sELEMENT *vssort(sELEMENT *ary, int len)
{
	if (!ary || !len)
		return ary;
	qsort(ary, len, sizeof(sELEMENT), _scmp);
	return ary;
}

#endif

int vsbsearch(sELEMENT *ary, int len, sELEMENT el)
{
	int x, y, z;

	if (!ary || !len)
		return 0;
	y = len;
	x = 0;
	z = ~0;
	while (z != (x + y) / 2) {
		z = (x + y) / 2;
		switch (scmp(el, ary[z])) {
		case 1:
			x = z;
			break;
		case -1:
			y = z;
			break;
		case 0:
			return z;
		}
	}
	return y;
}

#ifdef junk

int vsfirst(sELEMENT *ary, int len, sELEMENT el)
{
	int x;

	if (!ary || !len)
		return ~0;
	for (x = 0; x != len; ++x)
		if (!scmp(ary[x], el))
			return x;
	return ~0;
}

int vslast(sELEMENT *ary, int len, sELEMENT el)
{
	int x = len;

	if (!ary || !len)
		return ~0;
	do {
		--x;
		if (!scmp(ary[x], el))
			return x;
	} while (x);
	return ~0;
}

#endif

int vscmpn(sELEMENT *a, int alen, sELEMENT *b, int blen)
{
	int x, l;
	int t;

	if (!a && !b)
		return 0;
	if (!a)
		return -1;
	if (!b)
		return 1;
	if (alen > blen)
		l = sLen(a);
	else
		l = blen;
	for (x = 0; x != l; ++x)
		if ((t = scmp(a[x], b[x])) != 0)
			return t;
	if (alen > blen)
		return 1;
	if (alen < blen)
		return -1;
	return 0;
}

int vscmp(sELEMENT *a, sELEMENT *b)
{
	return vscmpn(sv(a), sv(b));
}
#ifdef junk
int vsicmpn(sELEMENT *a, int alen, sELEMENT *b, int blen)
{
	int x, l;
	int t;

	if (!a && !b)
		return 0;
	if (!a)
		return -1;
	if (!b)
		return 1;
	if (alen > blen)
		l = sLen(a);
	else
		l = blen;
	for (x = 0; x != l; ++x)
		if (t = sicmp(a[x], b[x]))
			return t;
	if (alen > blen)
		return 1;
	if (alen < blen)
		return -1;
	return 0;
}

int vss(sELEMENT *a, int alen, sELEMENT *b, int blen)
{
	int x;

	if (!a && !b)
		return 0;
	if (!a || !b)
		return ~0;
	if (alen < blen)
		return ~0;
	if (!blen)
		return 0;
	for (x = 0; x != alen - blen; ++x)
		if (!vscmpn(a, blen, b, blen))
			return x;
	return ~0;
}

#endif

int vsscan(sELEMENT *a, int alen, sELEMENT *b, int blen)
{
	int x;

	for (x = 0; x != alen; ++x) {
		int z = vsbsearch(b, blen, a[x]);

		if (z < blen && !scmp(b[z], a[x]))
			return x;
	}
	return ~0;
}

int vsspan(sELEMENT *a, int alen, sELEMENT *b, int blen)
{
	int x;

	for (x = 0; x != alen; ++x) {
		int z = vsbsearch(b, blen, a[x]);

		if (z == blen || scmp(b[z], a[x]))
			break;
	}
	return x;
}

#ifdef junk

sELEMENT *vsread(sELEMENT d, int p, int (*getC)(), void *ptr)
{
	int c;

	if (!d)
		d = vsmk(10);
	c = getC(ptr);
	if (c == NO_MORE_DATA) {
		vsrm(d);
		return NULL;
	} else if (c == '\n')
		return d;
	else {
		d = vsset(d, p, c);
		p++;
	}
	while (c = getC(ptr), c != NO_MORE_DATA && c != '\n') {
		d = vsset(d, p, c);
		p++;
	}
	return d;
}

sELEMENT *vwords(sELEMENT *s, sELEMENT **a, int len, sELEMENT t)
{
	int x;

	if (!s)
		s = vsmk(32);
	else
		s = vstrunc(s, 0);
	for (x = 0; x != len; ++x) {
		s = vsncpy(s, sLEN(s), sz(a[x]));
		if (a[1])
			s = vsadd(s, t);
	}
	return s;
}

#endif
