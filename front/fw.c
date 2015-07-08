// -*-coding:utf-8-*-
// 生成烧写模板,为了简化开发，还是用会命令行模式
/*
xgettext -o fw.pot fw.c
msginit -l zh_CN -i fw.pot
msgfmt zh_CN.po -o locale/zh_CN/LC_MESSAGES/fw.mo
*/
/*
sample of fw.tpl


*/
#include <stdint.h>// uint8_t
#include <stdio.h>
#include <unistd.h>//getopt()
#include <stdlib.h>//atoi()
#include <time.h>//time(),gmtime()
#include <malloc.h>
#include <libintl.h>

#define LOCALEDIR "locale/"
#define PACKAGE "fw"

#ifndef UNICODE
#define UNICODE
#endif
// 替换串必须固定长度，以简化编码的难度
// 而在匹配前面有混淆字符，这个HOLDER会被忽略，
// 例如 @@@@SIZE@@@, 后面10个字符是匹配的，但是由于前面有1个匹配的前导
//      导致匹配失败
#define SIZE_HOLDER 10
#define MATCH_SIZE 1000
#define MATCH_TAG 1001
#define MATCH_DATE 1002
const char HOLDER_SIZE[]="@@@SIZE@@@";
const char HOLDER_TAG[]= "@@@TAG#@@@";
const char HOLDER_DATE[]="@@@DATE@@@";
int raw(FILE* fp,uint8_t **buf,uint8_t padding,int *size){
  return 0;
}
const char hex[]="0123456789ABCDEF";
int c2d(char c){
  int idx=0;
  while(hex[idx]!=c) idx++;
  return idx;
}
int ihex(FILE* fp,uint8_t **buf,uint8_t padding, int *size){
  int c;
  *size=0;
  *buf=(uint8_t*)malloc(16*1024);
  for(c=0;c<16*1024;c++)(*buf)[c]=padding;
  while(EOF != (c=fgetc(fp))){
	if(c==':'){
	  int sum;
	  int colsize;
	  int address ;
	  int type;
	  int x;
	  int vv=c2d(fgetc(fp));
	  if (vv<0 && vv > 15){
		goto error;
	  }
	  colsize=vv*16;
	  vv=c2d(fgetc(fp));
	  if (vv<0 && vv > 15){
		goto error;
	  }
	  colsize+=vv;
	  sum=colsize;

	  vv=c2d(fgetc(fp));
	  if (vv<0 && vv > 15){
		goto error;
	  }
	  address=vv*16;
	  vv=c2d(fgetc(fp));
	  if (vv<0 && vv > 15){
		goto error;
	  }
	  address+=vv;
	  sum += address;
	  address *= 256;
	  vv=c2d(fgetc(fp));
	  if (vv<0 && vv > 15){
		goto error;
	  }
	  address+=vv*16;
	  vv=c2d(fgetc(fp));
	  if (vv<0 && vv > 15){
		goto error;
	  }

	  address+=vv;
	  sum += address;
	  vv=c2d(fgetc(fp));
	  if (vv<0 && vv > 15){
		goto error;
	  }
	  type=vv*16;
	  vv=c2d(fgetc(fp));
	  if (vv<0 && vv > 15){
		goto error;
	  }
	  type+=vv;

	  sum += type;
	  if (type == 1) break;
	  for (x=0;x<colsize;x++){
		uint8_t data;
		vv=c2d(fgetc(fp));

		if (vv<0 && vv > 15){
		  goto error;
		}
		data=vv*16;

		vv=c2d(fgetc(fp));
		if (vv<0 && vv > 15){
		  goto error;
		}
		data += vv;
		sum += data;
		if (type == 0){
		  int t=address+x;
		  (*buf)[t]=data;
		  if (t > *size) {
			*size=t;
		  }
		}
	  }
	  sum = (-sum) & 0xff;
	  vv=c2d(fgetc(fp));
	  if (vv<0 && vv > 15){
		goto error;
	  }
	  type=vv*16;
	  vv=c2d(fgetc(fp));
	  if (vv<0 && vv > 15){
		goto error;
	  }
	  type+=vv;
	  if (sum!=type){
		printf(gettext("sum check error\n"));
		goto error1;
	  }


	}
  }
  *size = *size / 256 + 1;
  return 0;
 error:
  printf("%s\n",gettext("hex file format error"));
 error1:
  free(*buf);
  return -1;
}
// 返回 吻合当前的holder
int check(FILE *in,FILE *out){
  char keep[SIZE_HOLDER];
  int i,ch;
  ch=fgetc(in);
  if(ch!='@'){
	fputc('@',out);
	fputc(ch,out);
	return -1;
  }
  ch=fgetc(in);
  if(ch!='@'){
	fputc('@',out);
	fputc('@',out);
	fputc(ch,out);
	return -1;
  }
  keep[0]=keep[1]=keep[2]='@';
  for(i=3;i<SIZE_HOLDER;i++){
	ch=fgetc(in);
	keep[i]=ch;
  }
  int m_tag,m_size,m_date;
  m_tag=m_size=m_date=1;
  for(i=3;i<SIZE_HOLDER;i++){
	if(keep[i]!=HOLDER_TAG[i]) m_tag=0;
	if(keep[i]!=HOLDER_SIZE[i]) m_size=0;
	if(keep[i]!=HOLDER_DATE[i]) m_date=0;
  }
  if(m_tag)return MATCH_TAG;
  if(m_size)return MATCH_SIZE;
  if(m_date)return MATCH_DATE;
  for(i=0;i<SIZE_HOLDER;i++)fputc(keep[i],out);
  return -1;
}
void render(FILE *in,FILE *out,uint8_t *buf,const char *tag,int size){
  int ch;
  time_t t=time(NULL);
  struct tm *mt;

  if(t==((time_t)-1)){
	printf(gettext("time() error"));
	return;
  }
  mt=localtime(&t);
  if(NULL==mt){
	printf(gettext("localtime() error"));
	return;
  }
  while((ch=fgetc(in))!=EOF){
	if(ch=='@'){
	  int retval=check(in,out);
	  switch(retval){
	  case MATCH_DATE:
		fprintf(out,"%04d-%02d-%02d %02d:%02d:%02d",
				mt->tm_year+1900,
				mt->tm_mon+1,
				mt->tm_mday+1,
				mt->tm_hour,
				mt->tm_min,
				mt->tm_sec);
		continue;
	  case MATCH_SIZE:
		fprintf(out,"%d",size);
		continue;
	  case MATCH_TAG:
		fprintf(out,"%s",tag);
		continue;
	  default:
		continue;
	  }
	}
	fputc(ch,out);
  }
  fputc('\n',out);
  {
	int cnt;
	for(cnt=0;cnt<size;cnt++){
	  int x,y;
	  fprintf(out,"\n");
	  for(x=0;x<16;x++){
		fprintf(out,".db ");
		for(y=0;y<16;y++){
		  if (y) {
			fprintf(out,",");
		  }
		  fprintf(out," 0x%02x",buf[cnt*256+x*16+y]);
		}
		fprintf(out,"; 0x%04x\n",cnt * 256 + x * 16);
	  }
	}
  }
}
void usage(const char *self){
  printf("%s:\n\t%s %s... %s\n",gettext("usage"),self,gettext("[OPTION]"),gettext("FILE"));
  printf("\
\t-o <file>       %s\n\
\t-r              %s\n\
\t-i              %s\n\
\t-t <template>   %s\n\
\t-p <n>          %s\n\
\t-T <message>    %s\n\
\t-h              %s\n",gettext("Place the output file into <file>"),
		 gettext("Input file format in raw binary"),
		 gettext("Input file format in hex(default)"),
		 gettext("Use <file> as template file. default=fw.tpl"),
		 gettext("Padding empty byte with n(0~255) default = 255"),
		 gettext("Tag remider message"),
		 gettext("Print this message and exit"));
}
int main(int argc, char **argv){
  int (*callback_src)(FILE *,uint8_t **,uint8_t,int *);
  int opt,retval,fwsize;
  uint8_t padding=255;
  const char *tag="No description",// tag message
	*dst_filename,//target asm file, if omited, used standard output
	*tpl_filename,// template asm file
	*src_filename;// frameware binary file
  dst_filename=src_filename=NULL;
  tpl_filename="fw.tpl";
  FILE *fpSrc,*fpDst,*fpTpl;
  uint8_t *fwdata=NULL;
  bindtextdomain(PACKAGE,LOCALEDIR);
  textdomain(PACKAGE);
  if(argc == 1){
	usage(argv[0]);
	return 0;
  }
  callback_src=ihex;
  while((opt=getopt(argc,argv,"o:rit:p:T:h")) != -1){
	switch(opt){
	case 'o':
	  dst_filename=optarg;
	  break;
	case 'r':
	  callback_src=raw;
	  break;
	case 'i':
	  break;
	case 't':
	  tpl_filename=optarg;
	case 'p':
	  padding=atoi(optarg);
	  break;
	case 'h':
	  usage(argv[0]);
	  return 0;
	case 'T':
	  tag=optarg;
	  break;
	case '?':
	  return -1;
	}
  }
  if(optind>=argc){
	printf("%s\n",gettext("Error: Expected a frameware file"));
	return -1;
  }
  src_filename=argv[optind];
  if((fpSrc=fopen(src_filename,"rb"))==NULL){
	printf(gettext("Can't open file %s\n"),src_filename);
	return -1;
  }
  retval=callback_src(fpSrc,&fwdata,padding,&fwsize);

  fclose(fpSrc);
  if(retval){
	printf(gettext("decode source file error"));
	return -1;
  }
  if((fpTpl=fopen(tpl_filename,"r"))==NULL){
	printf(gettext("Can't open file %s\n"),tpl_filename);
	free(fwdata);
	return -1;
  }
  if(dst_filename){
	if((fpDst=fopen(dst_filename,"w+"))==NULL){
	  printf(gettext("Can't open file %s\n"),dst_filename);
	  free(fwdata);
	  fclose(fpTpl);
	  return -1;
	}
	render(fpTpl,fpDst,fwdata,tag,fwsize);
	fclose(fpDst);
  } else {
	render(fpTpl,stdout,fwdata,tag,fwsize);
  }
  free(fwdata);
  fclose(fpTpl);
  return 0;
}
