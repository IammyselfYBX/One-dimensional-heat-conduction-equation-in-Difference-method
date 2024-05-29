#include "xmalloc.h"
#include <stdio.h>

void *malloc_or_exit(size_t size, const char *file, int line) {
    void *ptr = malloc(size); // 这一步分配内存

    // 判断是否分配成功
    if (!ptr) {
        fprintf(stderr, "%s:%d: malloc(%zu) failed\n", file, line, size);
        exit(EXIT_FAILURE);
    }else{
        return ptr;
    }
}