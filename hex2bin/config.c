#include <string.h>
#include <stdio.h>
#include "config.h"
char linebuf[256];
int readline(FILE *fp)
{
    int ix=0;
    int c;
    while(1){
        if(feof(fp)){
            if(ix>0)
                break;
            else {
                return -1;
            }
        }
        c=getc(fp);
        if(c<0){
            return -1;
        }
        if(c=='\n'){
            break;
        }
        if(c=='\r'){
            continue;
        }
        linebuf[ix]=c;
        ix++;
    }
    return ix;
}
int str2int(const char *str,int len);
int read_pad(FILE *fp,struct PAD_DATA *pd)
{
    int lnum=readline(fp);
    if(lnum<=0){
        return -1;
    }
    int div=-1;
    for(int ix=0;ix<lnum;ix++){
        if(linebuf[ix]==':'){
            div=ix;
        }
    }
    if(div==-1){
        goto on_error;
    }
    int ival=str2int(linebuf,div);
    if(ival<0){
        goto on_error;
    }
    pd->offset=ival;
    ival=str2int(&linebuf[div+1],lnum-div-1);
    if(ival<0){
        goto on_error;
    }
    pd->count=ival;
    return 0;
on_error:
    printf("错误行:");
    for(int ix=0;ix<lnum;ix++){
        putchar(linebuf[ix]);
    }
    printf("\n");
    return -1;
}
struct PAD_DATA pads[100];
int read_config(const char *fname,struct CONFIG *cfg)
{
    FILE *fcfg=fopen(fname,"r");
    if(fcfg==NULL){
        return -1;
    }
    int len;
    if((len=readline(fcfg))<0){
        if(feof(fcfg)){
            fclose(fcfg);
            return -1;
        }
    }
    linebuf[len]='\0';
    cfg->src=strdup(linebuf);
    if((len=readline(fcfg))<0){
        if(feof(fcfg)){
            fclose(fcfg);
            return -1;
        }
    }
    linebuf[len]='\0';
     cfg->dst=strdup(linebuf);
     printf("src:%s, dst:%s\n",cfg->src,cfg->dst);
     int ix=0;
     while(!read_pad(fcfg,&pads[ix])){
        printf("在偏移0x%04x处填%d个零\n",pads[ix].offset,pads[ix].count);
        ix++;
     }
     fclose(fcfg);
     cfg->pad_count=ix;
     cfg->pad=&pads[0];
     return 0;
}