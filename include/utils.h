#ifndef UTILS_H
#define UTILS_H

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

void exit_with_error(char* message, int code);
long get_filesize(FILE* fptr);
char *canonical_path(char *path);
char *resolve_include_path(char *base_path, char *include_path);

#endif // UTILS_H
