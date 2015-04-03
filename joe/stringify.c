/* Convert files into a C strings */
/* Use to create built in rc and .jsf files in JOE with: ./stringify ../rc/ *rc ../syntax/ *.jsf >builtins.c */

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int x;
    int first_file = 1;
    printf("/* Built-in files */\n");
    printf("\n");
    printf("#include \"types.h\"\n");
    printf("\n");
    printf("char *builtins[]=\n");
    printf("{\n");
    for (x = 1; argv[x]; ++x) {
        int c;
        int instring = 0;
        int first_string = 1;
        size_t z;
        int type;
        FILE *f;
        f = fopen(argv[x], "r");
        if (!f) {
            fprintf(stderr, "Couldn't open file '%s'\n", argv[x]);
            return -1;
        }
        for (z = strlen(argv[x]); z && argv[x][z - 1] != '/'; --z);
        if (first_file) {
            printf("	\"%s\",\n", argv[x] + z);
            first_file = 0;
        } else
            printf(",	\"%s\",\n", argv[x] + z);
        if (strstr(argv[x] + z, ".jsf"))
            type = 1; /* .jsf file: delete # comments */
        else
            type = 0; /* rc file: delete SPACE comments */
        while ((c = fgetc(f)) != -1) {
            if (!instring) {
                if ((type && c == '#') || (!type && (c == ' ' || c == '\t' || c == '\n'))) {
		    do
                        if (c == '\n')
                            break;
                    while ((c = fgetc(f)) != -1);
                    continue;
                }
                if (first_string) {
                    printf("		\"");
                    first_string = 0;
                } else
                    printf("		\"");
                instring = 1;
            }
            if (c == '"')
                printf("\\\"");
            else if (c == '\\')
                printf("\\\\"); 
            else if (c == '\n')
                printf("\\n");
            else if ((c >= 32 && c <= 126) || c == '\t')
                putchar(c);
            else
                printf("\\x%2.2x", c);
            if (c == '\n') {
                printf("\"\n");
                instring = 0;
            }
        }
        if (instring) {
            printf("\"\n");
            instring = 0;
        }
        fclose(f);
    }
    if (first_file)
        printf("	NULL\n");
    else
        printf(",	NULL\n");
    printf("};\n");
    return 0;
}
