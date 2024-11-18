#include <wchar.h>

const wchar_t *usage_str=L"转换hex格式的文件为无格式的二进制bin文件\n\
Usage:\n\
\t %s <options> input\n\
input:\n\
\t输入hex文件\n\
options:\n\
\t-o 输出的bin文件名\n\
\t-c 设定文件名\n\
设定文件格式:\n\
  (第一行)源hex文件\n\
  (第二行)目标bin文件\n\
  (。。。)文件偏移:填充０的数量\n\
从第三行起，每行就是一个填０的描述\n";
void usage(const char *name)
{
		wprintf(usage_str,name);
}