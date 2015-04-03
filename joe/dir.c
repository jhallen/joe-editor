/*
 *	Directory package for older UNIXs
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

struct direct {
	short d_ino;
	char d_name[14];
};

void *opendir(char *name)
{
	return fopen(name, "r");
}

struct direct *readdir(void *f)
{
	static struct direct direct;

	while (1 == fread(&direct, SIZEOF(struct direct), 1, (FILE *) f)) {
		if (direct.d_ino) {
			return &direct;
		}
	}
	return 0;
}

void closedir(FILE *f)
{
	fclose(f);
}

int mkdir(char *s)
{
	char *y = NULL;
	int rtval;

	y = vsncpy(sv(y), sc("/bin/mkdir "));
	y = vsncpy(sv(y), sz(s));
	y = vsncpy(sv(y), sc(" 2>/dev/null"));
	tickoff();
	rtval = system(y);
	tickon();
	vsrm(y);
	return rtval;
}
