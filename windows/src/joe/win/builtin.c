/*
 *	Built-in config files
 *	Copyright
 *		(C) 2006 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"
#include <assert.h>

JFILE *jfopen(unsigned char *filename, const char *mode)
{
	wchar_t wfilename[MAX_PATH + 1];
	wchar_t wmode[20];

	if (utf8towcs(wfilename, (char*)filename, MAX_PATH) || utf8towcs(wmode, mode, 20))
	{
		assert(FALSE);
		return NULL;
	}

	return jwfopen(wfilename, wmode);
}

unsigned char *jfgets(unsigned char **buf, JFILE *f)
{
	if (f->f) {
		return vsgets(buf, f->f);
	} else {
		unsigned char *b = *buf;
		b = vstrunc(b, 0);
		if (f->sz && *f->p) {
			int i;

			for (i = 0; i < f->sz && f->p[i] != '\n' && f->p[i] != '\r'; i++) b = vsadd(b, f->p[i]);
			while (i < f->sz && (f->p[i] == '\r' || f->p[i] == '\n')) i++;

			f->p += i;
			f->sz -= i;
			*buf = b;

			return b;
		} else {
			*buf = b;
			return NULL;
		}
	}
}

unsigned char **jgetbuiltins(unsigned char *suffix)
{
	unsigned char tmp[1024];
	wchar_t wsuffix[80];
	wchar_t *wsuffixp;
	const wchar_t *ptr = NULL;
	unsigned char **result;

	result = vamk(4);

	if (suffix) {
		if (utf8towcs(wsuffix, (char *)suffix, 79)) {
			assert(FALSE);
			return NULL;
		}

		wsuffixp = wsuffix;
	} else {
		wsuffixp = NULL;
	}

	while ((ptr = jwnextbuiltin(ptr, wsuffixp))) {
		int i = 0;

		if (wcstoutf8((char *)tmp, ptr, 1023)) {
			assert(FALSE);
			return NULL;
		}

		/* Lowercase (since resources always stored in upper!) */
		for (i = 0; tmp[i]; i++) {
			tmp[i] = tolower(tmp[i]);
		}

		result = vaadd(result, vsdupz(USTR tmp));
	}

	return result;
}
