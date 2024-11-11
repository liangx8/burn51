#include <stdio.h>
int read_holder(const char *fname)
{
    FILE *fh=fopen(fname,"r");
    if(fh==NULL){
        return -1;
    }
    return 0;
}