#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

typedef struct {
    char* buffer;
    size_t length;
    size_t capacity;
} StringBuilder;
void init_sb(StringBuilder *sb);
void free_sb(StringBuilder *sb);
void debug_sb(StringBuilder *sb);
void concat_sb(StringBuilder *sb, const char *src);
void copy_sb(StringBuilder *sb1, StringBuilder *sb2);

#endif // STRINGBUILDER_H
