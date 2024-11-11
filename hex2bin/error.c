#include <errno.h>
#include <string.h>
#include <stdio.h>

void show_error(const char *srcname,int line)
{
    printf("%s(%d):%s\n",srcname,line,strerror(errno));
}