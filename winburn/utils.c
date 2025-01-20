#include <windows.h>
#include "memmgr.h"
struct error_stack{
    const char *msg;
    struct error_stack *next;
};
struct error_stack *top=NULL;

/// @brief ascii to unicode
/// @ref win32api: MultiByteToWideChar()
/// @param wc 
/// @param sn 
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
wchar_t buf[512];
void error_stack_last_code(const char *srcfile,int line){
    DWORD errCode=GetLastError();
    LPWSTR fmt=L"%ls(%d):%ls(%d)";
    DWORD_PTR args[]={(DWORD_PTR)srcfile,(DWORD_PTR)line,0,0};
    void *mem=auto_alloc(256);                                          // 返回错误的处理
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,errCode,0,buf,512,NULL); // 返回错误的处理
    
    args[2]=(DWORD_PTR)buf;
    args[3]=(DWORD_PTR)errCode;
    FormatMessage(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,fmt,0,0,mem,256,(va_list *)args);// 返回错误的处理
    
    
    struct error_stack *head=auto_alloc(sizeof(struct error_stack));    // 返回错误的处理
    head->msg=mem;
    if (top==NULL){
        top=head;
        return;
    }
    struct error_stack *ptr;
    ptr=top;
    
    while(ptr->next){
        ptr=ptr->next;
    }
    ptr->next=head;
}
void error_stack_msg(const char *srcfile,int line,const char *msg){

}