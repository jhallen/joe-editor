#include "jwbuiltin.h"

JFILE *jfopen(unsigned char *name, const char *mode);
unsigned char *jfgets(unsigned char **buf, JFILE *f);
unsigned char **jgetbuiltins(unsigned char *suffix);

#define jfclose jwfclose
