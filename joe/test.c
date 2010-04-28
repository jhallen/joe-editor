#include <stdio.h>

main(int argc,char *argv[])
{
  int c;
  FILE *f = fopen(argv[1],"r");
  if (!f) {
    fprintf(stderr,"couldn't open file\n");
    return -1;
  }
  while ((c = fgetc(f)) != -1)
    putchar(c);
  return 0;
}
