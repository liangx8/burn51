/*
  网上找到的这个据说是Ｓｉｌｉｂ的库，用命令查看与WINDOW方有一样的函数
  nm libslab8051.so
  但是这个程序尝试调用其中一个程序。结果显示这个库为32位ELF
 */

#include <stdio.h>
#include <dlfcn.h>

long (*getversion)(const char **);

int main(int argc,char **argv)
{
  const char *version;
  void *handle=dlopen("./libslab8051.so",RTLD_LAZY);
  if(handle == NULL){
    printf("error %s\n",dlerror());
    return 0;
  }
  getversion=dlsym(handle,"GetUSBDLLVersion");
  
  
  getversion(&version);
  printf("%s\n",version);
  return 0;
}
