#include <stdio.h>
#include <stdlib.h>

#include "../include/cli.h"
#include "../include/utils.h"
#include "../include/preprocessor.h"
#include "../include/lexer.h"
#include "../include/memory.h"
#include "../include/parser.h"

int main(int argc, char** argv) {
    Configuration config;
    Lexer lexer;
    char* buf;
    Parser parser;
    Program *program;

    // command-line arguments
    if (argc < 2)
        exit_with_error("no arguments specified", 1);
    parse_args(&config, argc, argv);
    #ifdef IN_DEVELOPMENT
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        printf("@     NOTE: YOU ARE RUNNING A DEVELOPMENT BUILD!     @\n");
        printf("@ Stability is not guaranteed! Proceed with caution! @\n");
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        print_config(&config);
    #else
        if (config.verbose)
            print_config(&config);
    #endif

    // preprocessing
    buf = preprocess(config.input_file, config.debug_preprocessor);

    // lexical analysis + parsing
    init_lexer(&lexer, buf, config.debug_lexer);
    init_parser(&parser, &lexer, config.debug_parser);
    program = parse(&parser);
    if (config.debug_parser)
        print_program(program);

    // codegen
    
    // cleanup
    free_s(buf);
    free_program(program);
    return 0;
}
