#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void *malloc_s(size_t size);
void *calloc_s(size_t n, size_t size);
void *realloc_s(void *ptr, size_t new_size);
void free_s(void *ptr);

#endif // MEMORY_H
