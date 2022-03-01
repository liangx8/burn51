#include <stdio.h>
#include <malloc.h>

#include <windows.h>
#include "dll.h"
HRESULT connectC2(const char *sn,int powertarget,int disableDialog);
HRESULT disconnectC2(void);
HRESULT run_target(void){
	HRESULT result=SetTargetGo();
	if(SUCCEEDED(result)){
		Sleep(1000);		// sleep 2 seconds
	} else {
		wprintf(L"error (%x)\n",result);
	}
	return result;
}

void a2u(wchar_t *wc,const char *sn){
    int c=strlen(sn);
    int i;
    for(i=0;i<c;i++){
		char c=sn[i];
		if(c & 0x80){
			wc[i]=L'?';
		} else {
			wc[i]=sn[i];
		}
    }
    wc[c]=0;
	
}
HRESULT conn_and_print(wchar_t *wc,const char **sn){
	HRESULT result;
	result=GetUSBDeviceSN(0,sn);
	if(FAILED(result)){
		wprintf(L"Get device serial number error(%x)!\n",result);
	} else {
		a2u(wc,*sn);
		wprintf(L"EC6 设备:[%s]\n",wc);
	}
	return result;
}
void writebin(const char *file,BYTE *b,int size){
	FILE *fp=fopen(file,"wb");
		wchar_t *p=malloc(256);
		a2u(p,file);
	if(fp==NULL){
		wprintf(L"%s can't be created\n",p);
		free(p);
		return;
	}
	fwrite(b,1,size,fp);
	fclose(fp);
	wprintf(L"保存到 %s ...\n",p);
	free(p);
}
void readCodeMemory(const char *file,DWORD size){
	BYTE *buf;
	
	HRESULT result;
	const char *pChar;
	int num_ok=0;
	if(file==NULL){
		wprintf(L"提供文件名\n");
		return ;
	}
	for(int ii=1;ii<80;ii++){
		if(size == ii * 256){
			num_ok=1;
			break;
		}
	}
	if (num_ok){
		buf=malloc(size);
	} else {
		wprintf(L"错误:大小必须是256的倍%d\n",size);
		return;
	}
	buf=malloc(size);
	if(buf==NULL){
		wprintf(L"insufficient memory!\n");
		return;
	}
	result=conn_and_print((wchar_t *)buf,&pChar);
	if(FAILED(result)){
		wprintf(L"Get device serial number error(%x)!\n",result);
		free(buf);
		return;
	}
	
	result=connectC2(pChar,0,1);
	if(FAILED(result)){
		wprintf(L"连接出错了%d\n",result);
		free(buf);
		return;
	}

	result=GetCodeMemory(buf,0,size);
	if(FAILED(result)){
		wprintf(L"read code memory error,%d\n",result);
	} else{
		writebin(file,buf,size);
	}
	disconnectC2();
	free(buf);
	return ;
}
void show(BYTE *buf,int size){
	int i,j;
	wprintf(L"    ");
	for(i=0;i<16;i++){
		wprintf(L" %02x",i);
	}
	wprintf(L"\n");
	int cols=size / 16;
	for (i=0;i<cols;i++){
		wprintf(L"%04x",i*16);
		for(j=0;j<16;j++){
			if(i * 16+j < size)
				wprintf(L" %02x",buf[i*16+j]);
			else
				wprintf(L" ..");
		}
		wprintf(L"\t");
		for(j=0;j<16;j++){
			if(i * 16+j < size){
				BYTE c=buf[i*16+j];
				if ((c>='0' && c <='9') || (c >='a' && c <='z') || (c >= 'A' && c <= 'Z'))
					wprintf(L"%c",c);
				else
					wprintf(L".");
			} else {
				wprintf(L" ");
			}
		}
		wprintf(L"\n");
	}

}
void readMemory(void){
	BYTE buf[256];
	HRESULT result;
	const char *pChar;
	result=conn_and_print((wchar_t *)buf,&pChar);
	if(FAILED(result)){
		wprintf(L"Get device serial number error(%x)!\n",result);
		return;
	}
	result=connectC2(pChar,0,1);
	if(FAILED(result)){
		return;
	}

	if(FAILED(run_target())){
		goto error_end;
	}

	result=SetTargetHalt();
	if(FAILED(result)){
		wprintf(L"Set target halt error(%x)\n",result);
		goto error_end;
		
	}
	result=GetRAMMemory(buf, 0, 256);
	if(FAILED(result)){
		wprintf(L"Read RAM error(%x)\n",result);
		goto error_end;
	}
	result=SetTargetGo();
	if(FAILED(result)){
		wprintf(L"Failed to run target MCU(%x)\n",result);
	}else {
		wprintf(L"Target running...\n");
	}

	show(buf,256);
error_end:
	disconnectC2();
}
// buffer size should be 1,2,3,4
void readXMemory(size_t sz){
	BYTE *buf;
	HRESULT result;
	const char *pChar;
	int num_ok=0;
	for(int ii=1;ii<17;ii++){
		if(sz == ii * 256){
			num_ok=1;
			break;
		}
	}
	if (num_ok){
		buf=malloc(sz);
	} else {
		wprintf(L"错误:大小必须是256的倍%d\n",sz);
		return;
	}
	result=conn_and_print((wchar_t *)buf,&pChar);
	if(FAILED(result)){
		wprintf(L"Get device serial number error(%x)!\n",result);
		free(buf);
		return;
	}
	result=connectC2(pChar,0,1);
	if(FAILED(result)){
		free(buf);
		return;
	}
	wprintf(L"Target running...\n");
	if(FAILED(run_target())){
		goto error_end;
	}
	result=SetTargetHalt();
	if(FAILED(result)){
		wprintf(L"Set target halt error(%x)\n",result);
		goto error_end;
		
	}
	result=GetXRAMMemory(buf, 0, sz);
	if(FAILED(result)){
		wprintf(L"Read XRAM error(%x)\n",result);
		goto error_end;
	}
	show(buf,sz);

	Sleep(1000);

	result=SetTargetGo();
	if(FAILED(result)){
		wprintf(L"Failed to run target MCU(%x)\n",result);
	}

error_end:
	free(buf);
	disconnectC2();
	
}

#define RUNNING 1
#define STOP 2
#define WAIT_XRAM_PAGE 3
void interAction(void){
	BYTE buf[4096];
	HRESULT result;
	const char *pChar;
	int	mcu_sta;
	result=conn_and_print((wchar_t *)buf,&pChar);
	if(FAILED(result)){
		wprintf(L"Get device serial number error(%x)!\n",result);
		return;
	}
	result=connectC2(pChar,0,1);
	if(FAILED(result)){
		return;
	}
	wprintf(L"Target running...\n");
	if(FAILED(run_target())){
		goto error_end;
	}
	mcu_sta=RUNNING;
	wprintf(
L"***************************************\n\
*              debug mode             *\n\
***************************************\n\
\tq - quit\n\
\tx - read XRAM\n\
\ti - read RAM\n\
\th - halt target\n\
\tg - run target\n"
);

	while(1){
		char c;
		int num_page;

		c=getchar();
		if(mcu_sta==WAIT_XRAM_PAGE){
		  num_page = c - '0';
		  if(num_page<2 || num_page >9){
			wprintf(L"\ninput n( n * 256) 2 ~ 9:");
		  } else {
			result=GetXRAMMemory(buf, 0, 256*num_page);
			if(FAILED(result)){
			  wprintf(L"Read XRAM error(%x)\n",result);
			  goto error_end;
			}
			show(buf,256*num_page);
			mcu_sta=STOP;
		  }
		}
		switch(c){
			case 'q':goto error_end;
			case '\n':
				wprintf(L"[r/q/x?/i/h/g]\n");
				switch(mcu_sta){
				case RUNNING:
				  wprintf(L"R:");break;
				case STOP:
				  wprintf(L"H:");break;

		
				}

				break;
			case 'h':
				if(mcu_sta==STOP) break;
				result=SetTargetHalt();
				if(FAILED(result)){
					wprintf(L"Set target halt error(%x)\n",result);
					goto error_end;

				}
				wprintf(L"Target halt\n");
				mcu_sta=STOP;
				break;
			case 'g':
				if(mcu_sta==RUNNING) break;
				if(FAILED(run_target())){
					goto error_end;
				}
				wprintf(L"target running\n");
				mcu_sta=RUNNING;
				break;
			case 'x':
				if(mcu_sta==RUNNING){
					wprintf(L"halt target first\n");
				}else {
				  mcu_sta=WAIT_XRAM_PAGE;
				}
				break;
			case 'i':
				if(mcu_sta==RUNNING){
					wprintf(L"halt target first\n");
				}else {
					result=GetRAMMemory(buf, 0, 256);
					if(FAILED(result)){
						wprintf(L"Read RAM error(%x)\n",result);
						goto error_end;
					}
					show(buf,256);
				}
				break;

			default:
				break;
		}
		
	}

error_end:
	disconnectC2();
}
