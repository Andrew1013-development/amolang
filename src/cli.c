#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/cli.h"
#include "../include/info.h"
#include "../include/utils.h"
#include "../include/args.h"

void init_config(Configuration *config) {
    config->verbose = config->debug_preprocessor = config->debug_lexer = config->debug_parser = false;
    config->input_file = config->output_file = NULL;
}
void print_config(const Configuration *config) {
    printf("===== START OF CONFIGURATION =====\n");
    printf("VERBOSE = %d\n", config->verbose);
    printf("DEBUG_PREPROCESSOR = %d\n", config->debug_preprocessor);
    printf("DEBUG_LEXER = %d\n", config->debug_lexer);
    printf("DEBUG_PARSER = %d\n", config->debug_parser);
    printf("INPUT_FILE = %s\n", config->input_file);
    printf("OUTPUT_FILE = %s\n", config->output_file);
    printf("===== END OF CONFIGURATION =====\n");
}
static void _print_help() {
    char flag_msg[64];

    printf("Usage: amocc [options] file...\n");
    printf("Options:\n");
    for (int i = 0; i < NUM_ARGS; i++) {
        if (args[i].key != NO_KEY)
            sprintf(flag_msg, "\t-%c, --%s", args[i].key, args[i].name);
        else
            sprintf(flag_msg, "\t--%s", args[i].name);
        if (args[i].type == ARG_OPTION)
            strcat(flag_msg, " <option>");
        printf("%-25s %s\n", flag_msg, args[i].doc);
    }
    exit(0);
}
static void _print_version() {
    printf("amocc %s\n", VERSION);
    printf("%s\n", COPYRIGHT);
    exit(0);
}
void parse_args(Configuration *config, int argc, char **argv) {
    bool matched, match_short, match_long;
    char* val;

    init_config(config);
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            if (config->input_file != NULL)
                exit_with_error("compilation with multiple files not supported", 13);
            config->input_file = canonical_path(argv[i]);
            continue;
        }

        matched = false;
        for (int j = 0; j < NUM_ARGS; j++) {    
            match_short = (args[j].key != NO_KEY) 
                && (argv[i][1] == args[j].key) 
                && (argv[i][2] == '\0');
            match_long = (strncmp(argv[i], "--", 2) == 0)
                && (strcmp(argv[i] + 2, args[j].name) == 0);
            
            if (!match_long && !match_short)
                continue;

            matched = true;
            if (args[j].type == ARG_OPTION) val = argv[++i];

            switch (args[j].action) {
                case ACT_SET_STRING:
                    // what the fuck is this math
                    *(char**)((char*)config + args[j].configuration_field) = val;    
                    break;
                case ACT_SET_TRUE:
                    // what the fuck is this math
                    *(bool*)((char*)config + args[j].configuration_field) = true;
                    break;
                case ACT_HELP:
                    _print_help();
                    break;
                case ACT_VERSION:
                    _print_version();
                    break;
            }
            break;
        }
        if (!matched) 
            exit_with_error("invalid argument flag", 12);
    }
    if (config->input_file == NULL)
        exit_with_error("no input file specified", 14);
    if (config->output_file == NULL)
        config->output_file = "a.exe";
    if (config->verbose)
        config->debug_preprocessor = config->debug_lexer = config->debug_parser = true;
}
