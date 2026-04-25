#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#define SIZE_MACRO_NAME 256
#define SIZE_MACRO_VALUE 1024
#define MAX_MACROS 100

typedef struct {
    char name[SIZE_MACRO_NAME];
    char value[SIZE_MACRO_VALUE];
} Macro;
char *preprocess(const char* filename, bool debug);

#endif // PREPROCESSOR_H
