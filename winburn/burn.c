/*
$Id$
$Time-stamp$

2013-6-3
快速烧写8051芯片
-d 不显示对话框
-e 烧写前先删除flash,对于被保护的芯片，如果不用-e先删除的话，会
   导致烧写失败
-l 不对芯片加读保护

2013-7-17
refer to an117.pdf
compiling depend on siutil.lib
runing depend on SiUtl.dll and USBHID.dll,
2015-09-24
依赖文件上传了一份在百度云盘上
*/

#include <stdio.h>
#include <sys/stat.h>
#include "burn.h"
#include "dll.h"


void readCodeMemory(const char *file,DWORD size);

void readXMemory(size_t);
void readMemory(void);
void interAction(void);

HRESULT connectC2(const char *serialName,int powerTarget,int disableDialogBox){
    HRESULT result;
    result=ConnectUSB(serialName,C2_PROTOCOL,powerTarget,disableDialogBox);
    if(FAILED(result)){
        wprintf(L"connect failure!(%x)\n",result);
    } else {
        wprintf(L"connect established!\n");
    }
    return result;
}
HRESULT disconnectC2(void){
    HRESULT result;
    result=DisconnectUSB();
    if(FAILED(result)){
        wprintf(L"disconnect isn't completed!(%x)\n",result);
    }
    return result;
}
HRESULT downloadC2(const char *hexFile,int codeErase,int disableDialog, int flag,int bank,int lock,int persistFlash){
    wprintf(L"正在烧写 ...\n");
    return Download((char*)hexFile,codeErase,disableDialog,flag,bank,lock,persistFlash);
}
wchar_t *ascii2unicode(const char *src){
    int c=strlen(src);
    int i;
    wchar_t *wc=malloc(sizeof(wchar_t)*(c+1));
    for(i=0;i<c;i++){
        wc[i]=src[i];
    }
    wc[c]=0;
    return wc;
}
void listUSB(int count,struct data *d){
    DWORD i;
    wchar_t *wc;
    HRESULT result;
    for(i=0;i<count;i++){
        const char *pChar;
        result=GetUSBDeviceSN(i, &pChar);
        if(SUCCEEDED(result)){
            if(i==0){
                if(SUCCEEDED(connectC2(pChar,0,1))){
                    result=SetTargetHalt();
                    if(FAILED(result)){
                        wprintf(L"Failed to halt target MCU(%x)\n",result);
                    } else {
                        result=downloadC2(
							d->str,
							d->flag.bits.erase?1:0,
							d->flag.bits.disableDialog?1:0,
							0,-1,
							d->flag.bits.lock?0:1,
							1);
                        if(FAILED(result)){
                            wprintf(L"烧写出错(%X)\n",result);
                        } else {
                            wprintf(L"*********************************************\n");
                            wprintf(L"*                 烧写成功                  *\n");
                            wprintf(L"*********************************************\n");
                        }
                        if(d->flag.bits.goRun){
                            result=SetTargetGo();
                            if(FAILED(result)){
                                wprintf(L"Failed to run target MCU(%x)\n",result);
                            }else {
                                wprintf(L"Target running...\n");
                            }
                            
                        }
                    }
                }
                disconnectC2();
            }
            wc=ascii2unicode(pChar);
            wprintf(L"%d:%s\n",(int)i,wc);
            free(wc);
        } else {
            wprintf(L"Get device serial number error!\n");
        }
    }
}

/*
static inline void parse(wchar_t **argv,struct data *pd)
{
    while((opt = getopt(argc,argv,"ledr:Rm:")) != -1){
        switch(opt){
            case 'l':d.lock=0;break;
            case 'e':d.erase=0;break;
            case 'd':d.disableDialog=1;break;
            case 'R':d.goRun=1;break;
            case 'm':
                switch(optarg[0]){
                    case 'x':readXram=1;break;
                    case 'i':readRam=1;break;
                    case 'd':intact=1;break;
                    default:
                        wprintf(L"-m should be 'x' or 'i'\n");
                        usage();return 0;
                }

            break;
            case 'r':

                readMCU=1;
                switch(atoi(optarg)){
                    case 1:readSize=8*1024;break;
                    case 2:readSize=16*1024;break;
                    case 3:readSize=32*1024;break;
                    default:
                        wprintf(L"-r should 1,2,3\n");
                        usage();return 0;
                }
                break;
            
            case '?':
            default:
                usage();return 0;
        }
    }
}*/
int console_main(struct data *pd){
    DWORD dwDevices;
    HRESULT result;

    struct stat buf;
    const char *dllUSBVersionStr;
    char *dllVersionStr;
    wchar_t *wbuf;
    

    
    
    result=GetUSBDLLVersion(&dllUSBVersionStr);

    if(FAILED(result)){
        wprintf(L"dll version error\n");
    } else {
        wbuf=ascii2unicode(dllUSBVersionStr);
        wprintf(L"USB DLL version:%s\n",wbuf);
        free(wbuf);
    }
    dllVersionStr=GetDLLVersion();
    if(dllVersionStr){
        wbuf=ascii2unicode(dllVersionStr);
        wprintf(L"DLL version:%s\n",wbuf);
        free(wbuf);
    }
    

    result=USBDebugDevices(&dwDevices);
    if(SUCCEEDED(result)){
        const char *ps;
        if(dwDevices <=0){
            wprintf(L"No USB device found!\n");
            return 0;
        }
        result=GetDeviceName(&ps);
        wprintf(L"Target: %x\n",result);
		switch(pd->action){
			case XRAM:
			{
				unsigned int size;
				if(pd->size)
					size=pd->size;
				else
					size=512;
				readXMemory(size);
				return 0;
			}
			case IRAM:
				readMemory();
				return 0;
			case FLASH:
			{
				unsigned int size;
				if(pd->size)
					size=pd->size;
				else
					size=8*1024;
				readCodeMemory(pd->str,size);
				return 0;
			}
			case BURNIT:
			{
				if(stat(pd->str,&buf)){

					wbuf=ascii2unicode(pd->str);
					wprintf(L"file %s open error\n",wbuf);
					free(wbuf);

					return 0;
				}
				if(!S_ISREG(buf.st_mode)){
					wbuf=ascii2unicode(pd->str);
					wprintf(L"%s is not a regular file\n",wbuf);
					free(wbuf);
					return 0;
				}
				break;
			}
			case INTACT:
				interAction();
				return 0;
			
		}
        wprintf(L"找到%d个烧写设备\n",(int)dwDevices);
        listUSB(dwDevices,pd);
    } else {
        wprintf(L"search device error\n");
    }
    
    return 0;
}
