#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stddef.h>

#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)

void * mymalloc(size_t size, const char *file, int line);
void myfree(void* ptr, const char *file, int line);

#endif
