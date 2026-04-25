#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../include/utils.h"
#include "../include/memory.h"

void exit_with_error(char* message, int code) {
    fprintf(stderr, "Error %d: %s\n", code, message);
    exit(code);
}

long get_filesize(FILE* fptr) {
    long fsize = 0;

    fseek(fptr, 0, SEEK_END); // seek cursor to EOF
    fsize = ftell(fptr);
    rewind(fptr); // return cursor to 0
    return fsize;
}

char *canonical_path(char *path) {
    #ifdef _WIN32
        return _fullpath(NULL, path, _MAX_PATH);
    #else
        return realpath(path, NULL);
    #endif
}

char *resolve_include_path(const char *base_path, char *include_path) {
    char *last_forward, *last_backward, *messy_path, *final_path;
    size_t delimiter_pos;

    last_forward = strrchr(base_path, '/'); // Linux path starts with "/" -> at least 1
    last_backward = strrchr(base_path, '\\'); // Windows path starts with "<root>:\" -> at least 1    
    delimiter_pos = max((size_t)last_backward, (size_t)last_forward) - (size_t)base_path;
    
    messy_path = calloc_s(_MAX_PATH, sizeof(char));
    strncpy(messy_path, base_path, delimiter_pos + 1); // copy base directory
    strcat(messy_path, include_path); // copy include path
    final_path = canonical_path(messy_path); // force OS to resolve
    free_s(messy_path);
    return final_path;
}