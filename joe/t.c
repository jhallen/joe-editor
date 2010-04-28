#include <sys/types.h>
#include <sys/stat.h>

main(int argc,char *argv[])
{
struct stat sbuf;
stat(argv[1], &sbuf);
printf("stat ok %o %o\n", sbuf.st_mode,S_IFLNK);
}
