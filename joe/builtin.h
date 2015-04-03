/* Support for built-in config files */

struct jfile {
	FILE *f;		/* Regular file, or NULL for built-in */
	char *p;	/* Built-in file pointer */
};

JFILE *jfopen(char *name, char *mode);
char *jfgets(char *buf,int len,JFILE *f);
int jfclose(JFILE *f);
char **jgetbuiltins(char *suffix);

extern char *builtins[];
