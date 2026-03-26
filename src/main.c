#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "utils.h"
#include "preprocessor.h"
#include "lexer.h"
#include "memory.h"
#include "parser.h"

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
    if (config.verbose)
        print_config(&config);

    // preprocessing
    buf = preprocess(config.input_file, config.debug_preprocessor);

    // lexical analysis + parsing
    init_lexer(&lexer, buf, config.debug_lexer);
    init_parser(&parser, &lexer, config.debug_parser);
    program = parse(&parser);
    if (config.verbose || IN_DEVELOPMENT)
        print_program(program);

    // codegen

    // cleanup
    free_s(buf);
    //free_program(program);
    return 0;
}
