#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../include/stringbuilder.h"
#include "../include/utils.h"

#define MAX_MACROS 100

char* _read(const char* fname) {
    FILE *fptr;
    long fsize;
    char *buf, *msg;

    fptr = fopen(fname, "rb"); // read in binary mode -> ensure file size correctness
    if (fptr == NULL) {
        sprintf(msg, "unable to read file %s", fname);
        exit_with_error(msg, 2);
    }        

    fsize = get_filesize(fptr);
    buf = calloc(fsize + 1, sizeof(char));
    if (fread(buf, sizeof(char), fsize, fptr) == 0 && ferror(fptr))
        exit_with_error("something went wrong while reading file", 3);

    fclose(fptr);
    return buf;
}

typedef struct {
    char name[64];
    char value[256];
} Macro;
Macro macros[MAX_MACROS];
int macros_cnt = 0;
void _add_macro(const char *name, const char *value) {
    if (macros_cnt < MAX_MACROS) {
        strcpy(macros[macros_cnt].name, name);
        strcpy(macros[macros_cnt].value, value);
        macros_cnt++;
    } else
        exit_with_error("max macros limit reached", 4);
}

// process directives -> final source code
char* preprocess(const char* fname, bool debug) {
    FILE* fptr;
    StringBuilder sb;
    char line[SIZE_BUFFER], buf[SIZE_BUFFER], *ptr, *inc, *msg;
    size_t buflen = 0;
    bool macro_match;

    fptr = fopen(fname, "rb"); // read in binary mode -> ensure file size correctness
    if (fptr == NULL) {
        sprintf(msg, "unable to read file %s", fname);
        exit_with_error(msg, 2);
    }

    init_sb(&sb);

    // read in lines or 2KiB chunks
    while (fgets(line, SIZE_BUFFER, fptr)) {
        ptr = line; // set pointer to start of line

        // handle #include macros
        if (strncmp(line, "#include", 8) == 0) {
            char inc_file[_MAX_PATH];

            // read inclusion file -> remove macro and append to buffer
            if (sscanf(line, "#include \"%259[^\"]\"", inc_file) == 1) {
                inc = preprocess(resolve_include_path(fname, inc_file), false);
                concat_sb(&sb, inc);
                free(inc);
            }
            continue;
        }
        // handle #define macros
        if (strncmp(line, "#define", 7) == 0) {
            char name[64], value[256];

            // record macro into symbol table for search-and-replace
            if (sscanf(line, "#define %63s %255[^\r\n]", name, value) == 2)
                _add_macro(name, value);
            continue;
        }
        // search and replace on symbol table
        while (*ptr != '\0') {
            if (isalpha(*ptr) || *ptr == '_') {
                // flush old buffer
                buf[buflen] = '\0';
                concat_sb(&sb, buf);
                buflen = 0;

                // isolate word
                while (isalnum(*ptr) || *ptr == '_') {
                    buf[buflen++] = *ptr;
                    ptr++;
                }
                buf[buflen] = '\0';

                macro_match = false;
                for (int i = 0; i < macros_cnt; i++) {
                    if (strcmp(buf, macros[i].name) == 0) {
                        concat_sb(&sb, macros[i].value);
                        macro_match = true;
                        break;
                    }
                }
                if (!macro_match) concat_sb(&sb, buf);
                buflen = 0;
            } else {
                buf[buflen++] = *ptr;
                ptr++;
            }
        }
        // flush remaining buffer
        buf[buflen] = '\0';
        concat_sb(&sb, buf);
        buflen = 0;
    }

    if (debug) {
        printf("final source code\n");
        debug_sb(&sb);
    }
    fclose(fptr);

    return sb.buffer;
}
