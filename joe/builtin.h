/* Support for built-in config files */

struct jfile {
	FILE *f;		/* Regular file, or NULL for built-in */
	unsigned char *p;	/* Built-in file pointer */
};

JFILE *jfopen(unsigned char *name, const char *mode);
unsigned char *jfgets(unsigned char **buf,JFILE *f);
int jfclose(JFILE *f);
unsigned char **jgetbuiltins(unsigned char *suffix);

extern unsigned char *builtins[];
