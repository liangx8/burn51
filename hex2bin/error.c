#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>

void show_error(const char *srcname,int line)
{
    wprintf(L"%s(%d):%s\n",srcname,line,strerror(errno));
}