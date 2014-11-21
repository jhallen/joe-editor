
#include "types.h"
#include "jwglobals.h"

const char* joedata_plus(const char* input)
{
    static char buffer[PATH_MAX];
    strcpy(buffer, jw_joedata);
    strcat(buffer, input);
    return buffer;
}
