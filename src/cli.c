#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "cli.h"
#include "info.h"
#include "utils.h"
#include "args.h"

void init_config(Configuration *config) {
    config->verbose = config->debug_preprocessor = config->debug_lexer = config->debug_parser = false;
    config->input_file = config->output_file = NULL;
}
void print_config(Configuration *config) {
    printf("===== START OF CONFIGURATION =====\n");
    printf("DEBUG_PREPROCESSOR = %d\n", config->debug_preprocessor);
    printf("DEBUG_LEXER = %d\n", config->debug_lexer);
    printf("DEBUG_PARSER = %d\n", config->debug_parser);
    printf("INPUT_FILE = %s\n", config->input_file);
    printf("OUTPUT_FILE = %s\n", config->output_file);
    printf("===== END OF CONFIGURATION =====\n");
}
static void _print_help() {
    printf("Usage: amocc [options] file...\n");
    printf("Options:\n");
    for (int i = 0; i < 7; i++) {
        if (args[i].key != NO_KEY) printf("\t-%c, --%s\t%s\n", args[i].key, args[i].name, args[i].doc);
        else printf("\t--%s\t%s\n", args[i].name, args[i].doc);
    }
    exit(0);
}
static void _print_version() {
    printf("amocc %s\n", VERSION);
    printf("%s\n", COPYRIGHT);
    exit(0);
}
void parse_args(Configuration *config, int argc, char **argv) {
    init_config(config);

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // parse argument flags
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
                _print_help();
            else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
                config->verbose = config->debug_preprocessor = config->debug_lexer = config->debug_parser = true;
            else if (strcmp(argv[i], "--version") == 0)
                _print_version();
            else if (strcmp(argv[i], "--debug-preprocessor") == 0)
                config->debug_preprocessor = true;
            else if (strcmp(argv[i], "--debug-lexer") == 0)
                config->debug_lexer = true;
            else if (strcmp(argv[i], "--debug-parser") == 0)
                config->debug_parser = true;
            else
                exit_with_error("invalid argument flag", 12);
        } else {
            if (config->input_file)
                exit_with_error("compilation with multiple files not supported", 13);
            config->input_file = argv[i];
        }
    }
    if (config->input_file == NULL)
        exit_with_error("no input file specified", 14);
}

