/* Process UnicodeData.txt into category tables */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct cat {
    struct cat *next;
    char *name;
    int size;
} *cats;

void addcat(char *s)
{
    struct cat *c;
    for (c = cats; c; c = c->next)
        if (!strcmp(c->name, s))
            break;
    if (!c) {
        c = (struct cat *)malloc(sizeof(struct cat));
        c->next = cats;
        cats = c;
        c->name = strdup(s);
        /* printf("New categry %s\n", s); */
    }
}

int main(int argc, char *argv[])
{
    FILE  *f;
    char buf[1024];
    struct cat *cat;
    int low, high;

    /** Create table of categories from UnicodeData.txt **/

    f = fopen("UnicodeData.txt", "r");
    if (!f) {
        fprintf(stderr, "Couldn't open UnicodeData.txt\n");
        return -1;
    }
    /* First pass: find all category codes */
    while (fgets(buf, sizeof(buf), f)) {
        int x, y;
        for (x = 0; buf[x] && buf[x] != ';'; ++x); /* Skip to first ; */
        if (buf[x] == ';') {
            for (x = x + 1; buf[x] && buf[x] != ';'; ++x); /* Skip to second ; */
            if (buf[x] == ';') {
                ++x;
                for (y = x; buf[y] && buf[y] != ';'; ++y); /* Skip to third ; */
                buf[y] = 0;
                addcat(buf + x);
            }
        }
    }

    printf("/* Unicode categories */\n");
    printf("\n");
    printf("#include \"unicat.h\"\n");

    /* Generate a table for each category */
    for (cat = cats; cat; cat = cat->next) {
        rewind(f);
        int count = 0;
        low = high = -2;
        printf("\n");
        printf("struct interval %s_table[] = {\n", cat->name);
        while (fgets(buf, sizeof(buf), f)) {
            int x, y;
            int val;
            sscanf(buf, "%x", &val);
            for (x = 0; buf[x] && buf[x] != ';'; ++x); /* Skip to first ; */
            if (buf[x] == ';') {
                for (x = x + 1; buf[x] && buf[x] != ';'; ++x); /* Skip to second ; */
                if (buf[x] == ';') {
                    ++x;
                    for (y = x; buf[y] && buf[y] != ';'; ++y); /* Skip to third ; */
                    buf[y] = 0;
                    if (!strcmp(buf + x, cat->name)) {
                        if (val == high + 1) {
                            high = val;
                        } else {
                            if (low != -2) {
                                ++count;
                                printf("	{ 0x%x, 0x%x },\n", low, high);
                            }
                            low = high = val;
                        }
                    }
                }
            }
        }
        if (low != -2) {
            printf("	{ 0x%x, 0x%x }\n", low, high);
            ++count;
        }
        printf("};\n");
        cat->size = count;
    }

    /* Generate lookup table */
    printf("\n");
    printf("struct unicat unicat[] = {\n");
    for (cat = cats; cat; cat = cat->next) {
        printf("	{ \"%s\", %d, %s_table },\n", cat->name, cat->size, cat->name);
    }
    printf("	{ 0, 0, 0 }\n");
    printf("};\n");
    fclose(f);

    /** Create table of block names **/
    f = fopen("Blocks.txt", "r");
    if (!f) {
        fprintf(stderr,"Couldn't open Blocks.txt\n");
        return -1;
    }
    while (fgets(buf, sizeof(buf), f)) {
        if (buf[0] >= '0' && buf[0] <= '9' ||
            buf[0] >= 'A' && buf[0] <= 'F' ||
            buf[0] >= 'a' && buf[0] <= 'f') {
                int first;
                int last;
                char buf1[name];
                sscanf("%x..%x; %s", &first, &last, buf1);
        }
    }
}
