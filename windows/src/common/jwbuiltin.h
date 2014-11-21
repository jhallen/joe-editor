/* Support for built-in config files */

struct jfile {
	FILE *f;		/* Regular file, or NULL for built-in */
	unsigned char *p;	/* Built-in file pointer */
	unsigned int sz;
	void *dealloc;
};

typedef struct jfile JFILE;

JFILE *jwfopen(wchar_t *name, wchar_t *mode);
unsigned char *jwfgets(unsigned char *buf, int size, JFILE *f);
int jwfclose(JFILE *f);
const wchar_t *jwnextbuiltin(const wchar_t *prev, const wchar_t *suffix);

void jwAddResourceHandle(HMODULE module);
