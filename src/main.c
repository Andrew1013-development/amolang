#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "preprocessor.h"
#include "lexer.h"
#include "memory.h"
#include "parser.h"
#include "info.h"

int main(int argc, char** argv) {
    Lexer lexer;
    char* buf;
    Parser parser;
    Program *program;

    printf("amocc %s\n", VERSION);
    printf("%s\n", COPYRIGHT);
    if (argc != 2)
        exit_with_error("incorrect number of arguments", 1);

    // preprocessing
    buf = preprocess(argv[1]);

    // lexical analysis + parsing
    init_lexer(&lexer, buf);
    init_parser(&parser, &lexer);
    program = parse(&parser);
    print_program(program);

    // codegen

    // cleanup
    free_s(buf);
    //free_program(program);
    return 0;
}
