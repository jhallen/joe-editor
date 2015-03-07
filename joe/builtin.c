/*
 *	Built-in config files
 *	Copyright
 *		(C) 2006 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

JFILE *jfopen(unsigned char *name, char *mode)
{
	if (name[0] == '*') {
		int x;
		for (x = 0; builtins[x]; x += 2) {
			if (!zcmp(builtins[x], name + 1)) {
				JFILE *j = (JFILE *)joe_malloc(sizeof(JFILE));
				j->f = 0;
				j->p = builtins[x + 1];
				return j;
			}
		}
		return 0;
	} else {
		FILE *f = fopen((char *)name, (char *)mode);
		if (f) {
			JFILE *j = (JFILE *)joe_malloc(sizeof(JFILE));
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

unsigned char *jfgets(unsigned char *buf,int len,JFILE *f)
{
	if (f->f)
		return (unsigned char *)fgets((char *)buf, len, f->f);
	else {
		if (f->p[0]) {
			int x;
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

unsigned char **jgetbuiltins(unsigned char *suffix)
{
	int x;
	int sflen = 0;
	unsigned char **result = NULL;
	
	if (suffix)
		sflen = zlen(suffix);
	
	for (x = 0; builtins[x]; x += 2) {
		unsigned char *name = builtins[x];
		int nlen = zlen(name);
		
		if (!suffix || (sflen <= nlen && !zcmp(suffix, &name[nlen - sflen]))) {
			result = vaadd(result, vsncpy(NULL, 0, sz(name)));
		}
	}
	
	return result;
}
