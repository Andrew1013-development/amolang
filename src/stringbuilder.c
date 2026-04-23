#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../include/stringbuilder.h"
#include "../include/memory.h"

// ===== PUBLIC API =====
void init_sb(StringBuilder *sb) {
    // allocate memory
    sb->buffer = calloc_s(SIZE_BUFFER, sizeof(char));
    sb->buffer[0] = '\0';

    // setup aux data
    sb->length = 0; // NOTE: DOES NOT INCLUDE NULL TERMINATOR
    sb->capacity = SIZE_BUFFER; // NOTE: DOES INCLUDE NULL TERMINATOR
}
void free_sb(StringBuilder *sb) {
    free_s(sb->buffer);
    sb->buffer = NULL;
    sb->length = sb->capacity = 0;
}
void debug_sb(StringBuilder *sb) {
    printf("===== STRING DETAILS =====\n");
    printf("length: %zu, capacity: %zu\n", sb->length, sb->capacity);
    printf("===== START OF STRING CONTENT =====\n");
    printf("%s\n", sb->buffer);
    printf("===== END OF STRING CONTENT =====\n");
}

// ===== INTERNAL FUNCTIONS =====
// check if current capacity > desired length, allocate more if needed
static void _check_allocate(StringBuilder *sb, size_t length) {
    if (length < sb->capacity) return;

    while (length > sb->capacity)
        sb->capacity *= 2; // geometric growth for amortized O(1) time
    sb->buffer = realloc_s(sb->buffer, sb->capacity);
}

// ===== STRING FUNCTIONS =====
// concatenate string src into sb->buffer
void concat_sb(StringBuilder *sb, const char *src) {
    _check_allocate(sb, sb->length + strlen(src) + 1); // +1 for null terminator

    // copy string to memory
    memcpy(sb->buffer + sb->length, src, strlen(src));
    sb->length += strlen(src);
    sb->buffer[sb->length] = '\0';
}
// copy string and metadata from sb1 to sb2
void copy_sb(StringBuilder *sb1, StringBuilder *sb2) {
    _check_allocate(sb2, sb1->length + 1);

    memcpy(sb2->buffer, sb1->buffer, sb1->length);
    sb2->length = sb1->length;
    sb2->buffer[sb2->length] = '\0';
}
