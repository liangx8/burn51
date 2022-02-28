#include <windows.h>
#include <stdio.h>
#include <locale.h>
#include "burn.h"

unsigned int expectSize(const PSTR str,int *length)
{
	int len=0;
	unsigned int num=0;
	const char *ptr=str;
//	wprintf(L"[.]");
	while(*ptr==' '){
		len++;
		ptr++;
	}
	while(*ptr && (*ptr != ' ')){
		char c=*ptr;
		if(c>='0' && c <= '9'){
			int nc=c - '0';
			num = num *10 + nc;
		} else if (c=='k' || c=='K'){
			num = num * 1024;
			break;
		} else {
			*length=0;
			return 0;
		}
		ptr ++;
		len ++;
		wprintf(L"[t3]%c",*ptr);
	}
	*length=len;
	return num;
	
}
void usage(void){
    wprintf(
L"Usage:\nC8051 MCU utilities. Copyright by Holy-win, Version 0.1\n\
burn [option] <file>\n\
\t-l\tDon't lock user space\n\
\t-e\tDon't erase before download\n\
\t-d\tdon't show dialog\n\
\t-R\trun target after burn\n\
\t\t<file>, the hex file will be burn to chip.\n\n\n\
\t-i\tinteract mode\n\
\t-r [i,x,c]\n\
\t\tread data from mcu\n\
\t\ti - show sram\n\
\t\tx - show xram\n\
\t\tc - show flash\n\
\t-s <n>k\n"
);
}
wchar_t *ascii2unicode(const char *src);
static inline void parse(PSTR cmdLine,struct data *pd)
{
	char *p=cmdLine;
	char *opt;
	pd->size=0;
	pd->action=0;
	pd->flag.value=0;

	while(1){
		switch(*p){
		case '-':
		{
			p++;
			switch(*p){
				case 'l':pd->flag.bits.lock=1;break;
				case 'e':pd->flag.bits.erase=1;	break;
				case 'd':pd->flag.bits.disableDialog=1;break;
				case 'R':pd->flag.bits.goRun=1;break;
				case 'i':pd->action=INTACT;break;
				case 'w':pd->action=GUI;break;
				case 'r':
					while(1){
						p++;
						if(*p==0){
							return;
						}
						switch(*p){
							case 'x':
								pd->action=XRAM;
								goto inner;
							case 'i':
								pd->action=IRAM;
								goto inner;
							case 'c':
								pd->action=FLASH;
								goto inner;
							case ' ':
							break;
							default:
							pd->flag.bits.error=1;
							pd->wstr=L"only x,i,c available for option -r";
							return;
						}
					}
					inner:
					break;
				case 's':
				{
					int len;
					p++;
					pd->size=expectSize(p,&len);
					if(len == 0){
						pd->flag.bits.error=1;
						pd->wstr=L"[错误的数字]";
						return;
					}
					p += len;
				}
					continue;
				default:
					pd->flag.bits.error=1;
					pd->wstr=L"wrong option";
					return;
			}
		}
			break;
		case ' ':
			break;
		case 0:
			pd->str=0;
			return;
		default:
			pd->str=malloc(128);
			pd->action=BURNIT;
			goto opt_label;
		}
		p++;
	}
	opt_label:
	opt=pd->str;
	while(*p){
		*opt=*p;
		p++;
		opt++;
	}
	*opt=0;
	
}
int WINAPI GuiMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,struct data *pd, int nCmdShow);
int console_main(struct data *pd);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpCmdLine, int nCmdShow)
{
	struct data d;
	setlocale(LC_ALL,"");

	d.flag.value=0;
	parse(lpCmdLine,&d);
	if(d.flag.bits.error){
		usage();
		wprintf(d.wstr);
		return -1;
	}
	if(d.action==GUI){
		GuiMain(hInstance,hPrevInstance,&d,nCmdShow);
		if(d.str)
			free(d.str);
		return 0;
	}
	if(d.action){
		
		console_main(&d);
		if(d.str)
			free(d.str);
	}else {
		usage();
	}
//	wchar_t *wname=ascii2unicode(d.str);
//	wprintf(L"%s\n",wname);
//	free(wname);
	
	return 0;
}
