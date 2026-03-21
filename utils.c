#include <stdio.h>
#include <stdlib.h>

void exit_with_error(char* message, int code) {
    printf("%s\n", message);
    exit(code);
}

long get_filesize(FILE* fptr) {
    long fsize = 0;

    fseek(fptr, 0, SEEK_END); // seek cursor to EOF
    fsize = ftell(fptr);
    rewind(fptr); // return cursor to 0
    return fsize;
}
