#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_MACROS 100

long get_filesize(FILE* fptr) {
    long fsize = 0;

    fseek(fptr, 0, SEEK_END); // seek cursor to EOF
    fsize = ftell(fptr);
    rewind(fptr); // return cursor to 0
    return fsize;
}
char* _read(const char* fname) {
    FILE *fptr;
    long fsize;
    char* buf;

    fptr = fopen(fname, "rb"); // read in binary mode -> ensure file size correctness
    if (fptr == NULL)
        //exit_with_error("unable to read file", 2);

    fsize = get_filesize(fptr);
    buf = calloc(fsize + 1, sizeof(char));

    fread(buf, sizeof(char), fsize, fptr);
        //exit_with_error("something went wrong while reading file", 3);

    fclose(fptr);

    printf("%s\n", buf);
    return buf;
}
typedef struct {
    char* name[64];
    char* value[256];
} Macro;
Macro macros[MAX_MACROS];
int macros_cnt = 0;
void add_macro(const char *name, const char *value) {
    if (macros_cnt++ < MAX_MACROS) {
        //strcpy(macros[macros_cnt].name, name);
        //strcpy(macros[macros_cnt].value, value);
    } else {
        printf("max macros\n");
    }
}
// process directives -> final source code
char* preprocess(const char* fname) {
    FILE* fptr;
    char line[1024];
    char *buf, *ptr;

    fptr = fopen(fname, "rb"); // read in binary mode -> ensure file size correctness
    if (fptr == NULL)
        //exit_with_error("unable to read file", 2);

    buf = calloc(1024, sizeof(char));

    // read in lines or 1kb chunks
    while (fgets(line, 1024, fptr)) {
        ptr = line; // set pointer to start of line
        if (strncmp(line, "#include", 8) == 0) {
            char inc_file[256];

            // read inclusion file -> remove macro and append to buffer
            if (sscanf(line, "#include \"%255[^\"]\"", inc_file) == 1) {
                printf("include file: %s\n", inc_file);
                _read(inc_file);
            }
            continue;
        }
        if (strncmp(line, "#define", 7) == 0) {
            char name[64], value[256];

            // record macro into symbol table for search-and-replace
            if (sscanf(line, "#define %63s %255[^\n]", name, value) == 2) {
                printf("define %s = %s\n", name, value);
                //add_macro(name, value);
            }
            continue;
        }
        // search and replace on symbol table
        while (*ptr != '\0') {
            ptr++;
        }
    }
    fclose(fptr);

    return buf;
}
