/*
hex格式说明
:BBAAAATTHHH...HHHCC
BB   数据长度
AAAA 地址
TT   格式 一般是00， 01文件结束
CC   校验码　整行的数据相加的值的补码
*/
#include <malloc.h>
#include <stdio.h>
#define DEFAULT_SIZE 65536

void show_error(const char *srcname,int line);
/**
 * @brief 把一行hex数据转换为裸数据
 * @param raw 存放结果
 * @param hexbuf hex数据一行的开始
 * @param hexsize 存储返回转换一行后，hexbuf使用的字节
 */
int hexline(char *raw,char *hexbuf, int *hexsize)
{
    int idx=0;
    char *p=hexbuf;
    if(*p!=':'){
        printf("行首必须是':'\n");
        return -1;
    }
    while(*p != '\r' || *p != '\n'){
        putchar(*p);
        idx++;
        p++;
    }
}
int toraw(char *dst,char *hexbuf,int hexsize)
{

    printf("正在开发\n");
    return -1;
}

char *readhex(const char *hexname)
{
    FILE *hexf=fopen(hexname,"r");
    char *raw;
    if(hexf==NULL){
        show_error(__FILE__,__LINE__);
        return NULL;
    }
    fseek(hexf,0,SEEK_END);
    long size=ftell(hexf);
    char *hexbuf=malloc(size);
    rewind(hexf);
    long siz=fread(hexbuf,1,size,hexf);
    if(siz != size ){
        printf("不能把源hex文件读完%ld,%ld\n",siz,size);
        free(hexbuf);
        fclose(hexf);
        return NULL;
    }
    fclose(hexf);
    printf("读取%ld字节\n",size);
    raw=malloc(DEFAULT_SIZE);
    int rawsize=toraw(raw,hexbuf,size);
    free(hexbuf);
    if(rawsize<0){
        free(raw);
        return NULL;
    }
    return raw;
}