/*
-c 使用配置文件来转换hex文件
格式
(第一行)源hex文件
(第二行)目标bin文件
文件偏移:填充０的数量
*/


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "config.h"
#include "hexfile.h"
char pch[256];
const char *optstr="c:o:";
char *modify_ext(char *src)
{
	char *dot=NULL;
	char *p=src;
	while(*p){
		if(*p == '.'){
			dot=p;
		}
		p++;
	}
	if(dot==NULL){
		dot=p;
		*dot='.';
	}
	*(dot + 1)='b';
	*(dot + 2)='i';
	*(dot + 3)='n';
	*(dot + 4)=0;
	return src;
	
}
int read_config(const char *fname,struct CONFIG *cfg);
int parse(struct CONFIG *opts,int argc,char **argv)
{
	int opt;
	opts->dst=NULL;
	opts->pad_count=0;
	while((opt=getopt(argc,argv,optstr))!=-1){
		switch(opt){
		case 'o':
			opts->dst=optarg;
			break;
		case 'c':
			return read_config(optarg,opts);
		}
	}
	if(optind >= argc){
		wprintf(L"提供输入hex文件\n");
		return -1;
	}
	opts->src=argv[optind];
	if(opts->dst==NULL){
		strcpy(&pch[0],opts->src);
		opts->dst=modify_ext(&pch[0]);
	}
	return 0;
}


int find_end(unsigned char *data,int max)
{
	int end=0;
	for(int ix=0;ix<max;ix++){
		if(*(data+ix)){
			end=ix;
		}
	}
	return end;
}
#define BUFSIZE 65536
/**
 * @brief 在指定区域填０，同时把被填充的内容显示出来
 */
void disp_asm(unsigned char *data,int offset,int num)
{
	if(num>0){
		unsigned char *base=data+offset;
		wprintf(L".org 0x%04x: 0x%02x",offset,*base);
		*base = '\0';
		for(int ix=1;ix<num;ix++){
			unsigned char *ptr=base+ix;
			wprintf(L",0x%02x",*ptr);
			*ptr='\0';
		}
		wprintf(L"\n");
	}
}
int write_bin(const char *dst,unsigned char *mem,int size)
{
	FILE *fdst=fopen(dst,"w");
	if(fdst==NULL){
		return -1;
	}
	size_t wsz= fwrite(mem,1,size,fdst);
	if(wsz!=size){
		wprintf(L"写入数据不对\n");
		fclose(fdst);
		return -1;
	}
	fclose(fdst);
	return 0;
}
void show_error(const char *srcname,int line);
void usage(const char*);
int main(int argc, char **argv)
{
	setlocale(LC_ALL,"");
	struct CONFIG opts;
	if(parse(&opts,argc,argv)){
		usage(argv[0]);
		return -1;
	}
	//wprintf(L"src:%s,dst:%s\n",opts.src,opts.dst);
	struct hexfile hf;
	if(init_hexfile(&hf,BUFSIZE,NULL,opts.src)){
		show_error(__FILE__,__LINE__);
		return -1;
	}
	read_hex(&hf);
	int hexsize=find_end(hf.mem,BUFSIZE)+1;
	wprintf(L"size:%d,hex end:%x,修改:%d\n",hf.memsize,hexsize,opts.pad_count);

	for(int ix=0;ix<opts.pad_count;ix++){
		int offset=opts.pad[ix].offset;
		int count=opts.pad[ix].count;
		disp_asm(hf.mem,offset,count);
	}
	if(write_bin(opts.dst,hf.mem,hexsize)){
		show_error(__FILE__,__LINE__);
	}
	destroy_hexfile(&hf);
	return 0;
}
