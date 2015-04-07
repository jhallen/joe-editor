/* Support for built-in config files */

struct jfile {
	FILE *f;	/* Regular file, or NULL for built-in */
	const char *p;	/* Built-in file pointer */
};

JFILE *jfopen(const char *name, const char *mode);
char *jfgets(char *buf,int len,JFILE *f);
int jfclose(JFILE *f);
char **jgetbuiltins(const char *suffix);

extern const char *builtins[];
