/*
 *  This file is part of Joe's Own Editor for Windows.
 *  Copyright (c) 2014 John J. Jordan.
 *
 *  Joe's Own Editor for Windows is free software: you can redistribute it 
 *  and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2 of the 
 *  License, or (at your option) any later version.
 *
 *  Joe's Own Editor for Windows is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Joe's Own Editor for Windows.  If not, see
 *  <http://www.gnu.org/licenses/>.
 */

#include "types.h"

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
			unsigned int i;

			for (i = 0; i < f->sz && f->p[i] != '\n' && f->p[i] != '\r'; i++) b = vsadd(b, f->p[i]);
			if (i < f->sz && f->p[i] == '\r') i++;
			if (i < f->sz && f->p[i] == '\n') i++;

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
