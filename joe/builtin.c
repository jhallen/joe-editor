/*
 *	Built-in config files
 *	Copyright
 *		(C) 2006 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

JFILE *jfopen(const char *name, const char *mode)
{
	if (name[0] == '*') {
		int x;
		for (x = 0; builtins[x]; x += 2) {
			if (!zcmp(builtins[x], name + 1)) {
				JFILE *j = (JFILE *)joe_malloc(SIZEOF(JFILE));
				j->f = 0;
				j->p = builtins[x + 1];
				return j;
			}
		}
		return 0;
	} else {
		FILE *f = fopen(name, mode);
		if (f) {
			JFILE *j = (JFILE *)joe_malloc(SIZEOF(JFILE));
			j->f = f;
			j->p = 0;
			return j;
		} else {
			return 0;
		}
	}
}

int jfclose(JFILE *f)
{
	int rtn = 0;
	if (f->f)
		rtn = fclose(f->f);
	joe_free(f);
	return rtn;
}

char *jfgets(char *buf,int len,JFILE *f)
{
	if (f->f)
		return fgets(buf, len, f->f);
	else {
		if (f->p[0]) {
			ptrdiff_t x;
			for (x = 0; f->p[x] && f->p[x] != '\n'; ++x)
				buf[x] = f->p[x];
			if (f->p[x] == '\n') {
				buf[x++] = '\n';
			}
			buf[x] = 0;
			f->p += x;
			return buf;
		} else
			return 0;
	}
}

char **jgetbuiltins(const char *suffix)
{
	ptrdiff_t x;
	ptrdiff_t sflen = 0;
	char **result = NULL;
	
	if (suffix)
		sflen = zlen(suffix);
	
	for (x = 0; builtins[x]; x += 2) {
		const char *name = builtins[x];
		ptrdiff_t nlen = zlen(name);
		
		if (!suffix || (sflen <= nlen && !zcmp(suffix, &name[nlen - sflen]))) {
			result = vaadd(result, vsncpy(NULL, 0, sz(name)));
		}
	}
	
	return result;
}
