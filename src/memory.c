#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "../include/utils.h"

void *malloc_s(size_t size) {
    void *ptr;

    if (size == 0) {
        printf("warning: malloc called with size = 0\n");
        return NULL;
    }
    ptr = malloc(size);
    if (ptr == NULL)
        exit_with_error("unable to allocate memory", 5);
    return ptr;
}

void *calloc_s(size_t n, size_t size) {
    void *ptr;

    if (n == 0) {
        printf("warning: calloc called with n = 0\n");
        return NULL;
    }
    if (size == 0) {
        printf("warning: calloc called with size = 0\n");
        return NULL;
    }
    ptr = calloc(n, size);
    if (ptr == NULL)
        exit_with_error("unable to allocate memory", 5);
    return ptr;
}

void *realloc_s(void *ptr, size_t new_size) {
    void *new_ptr;

    if (new_size == 0) {
        printf("warning: realloc called with new_size = 0\n");
        return NULL;
    }
    new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL)
        exit_with_error("unable to allocate memory", 5);
    return new_ptr;
}

void free_s(void *ptr) {
    if (ptr == NULL) return;
    free(ptr);
}
