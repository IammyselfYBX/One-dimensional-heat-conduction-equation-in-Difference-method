#ifndef XMALLOC_H
#define XMALLOC_H

#include <stdlib.h>
/*
 * 函数名: malloc_or_exit
 * 功能: 申请内存空间/申请失败时退出程序并报告是那个文件的第几行调用了 xmalloc 函数出错
 * 参数: size: 申请空间的大小
 *       file: 调用 xmalloc 的文件名
 *       line: 调用 xmalloc 的行号
 * 返回值: 申请到的内存空间的首地址(void *类型)
 * 注意事项：
 *          1)依赖： <stdlib.h> 中malloc 函数
 *          2)用完需要 free 掉
*/
void *malloc_or_exit(size_t size, const char *file, int line);

/**
 * 宏定义: xmalloc
 * 功能: 申请内存空间/申请失败时退出程序并报告是那个文件的第几行调用了 xmalloc 函数出错
*/
#define xmalloc(size) malloc_or_exit((size), __FILE__, __LINE__)

#endif