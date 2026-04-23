#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

typedef struct {
    bool verbose;
    bool debug_preprocessor;
    bool debug_lexer;
    bool debug_parser;
    char *input_file;
    char *output_file;
} Configuration;
void init_config(Configuration *config);
void print_config(const Configuration *config);
void parse_args(Configuration *config, int argc, char **argv);

#endif // CLI_H
