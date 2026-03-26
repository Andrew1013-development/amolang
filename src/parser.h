#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Lexer* lexer;
    Token current;
    Token lookahead;
    bool debug;
} Parser;
void init_parser(Parser *parser, Lexer *lexer, bool debug);
Program *parse(Parser *parser);

#endif // PARSER_H
