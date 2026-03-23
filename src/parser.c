#include <stdio.h>
#include <stdbool.h>

#include "lexer.h"
#include "parser.h"
#include "memory.h"
#include "utils.h"
#include "ast.h"

// ===== PUBLIC APIS =====
void init_parser(Parser *parser, Lexer *lexer) {
    parser->lexer = lexer;
    parser->lookahead = next_token(parser->lexer);
}

// ===== INTERNAL FUNCTIONS =====
// see lookahead token
static Token _peek(Parser *parser) {
    return parser->lookahead;
}
// set current to lookahead then move lookahead
static void _consume(Parser *parser) {
    parser->current = parser->lookahead;
    parser->lookahead = next_token(parser->lexer);
}
// check if lookahead is of expected type
static bool _check(Parser *parser, TokenType type) {
    if (parser->lookahead.type != type) {
        exit_with_error("unexpected token", 6);
        return false;
    }
    return true;
}
// consume current if lookahead is of expected type
static bool _match(Parser *parser, TokenType type) {
    if (parser->lookahead.type == type) {
        _consume(parser);
        return true;
    }
    return false;
}
// TODO: implement parsing function declarations later
static Stmt *_parse_decl(Parser *parser, TokenType type) {
    char *name;

    _consume(parser);

    // next token after type must be identifier
    _check(parser, TOK_IDENTIFIER);
    _consume(parser);
    name = token_to_string(parser->current);

    switch (_peek(parser).type) {
        case PUNC_LPAREN:
            // function declaration
            _consume(parser);

            while (_peek(parser).type != PUNC_RPAREN) // TODO: replace this with parse function arguments
                _consume(parser);

            // check for proper opening of function body
            _check(parser, PUNC_LBRACE);
            _consume(parser);

            // TODO: parse statements within function

            return func_decl(type, name, NULL, 0, 0, NULL);
        case OP_ASSIGN:
            // variable declaration with value
            _consume(parser); // eat =

            _consume(parser); // TODO: replace this with _parse_expr()

            // check for proper semicolon ending
            _check(parser, PUNC_SEMICOLON);
            _consume(parser);

            return var_decl(type, name, int_literal(16));
        case PUNC_SEMICOLON:
            // variable declaration without value
            _consume(parser); // eat ;

            return var_decl(type, name, NULL);
        default:
            exit_with_error("declaration syntax error", 7);
    }
}
static void _add_statement(Program *prog, Stmt *stmt) {
    if (prog->count >= prog->capacity) {
        prog->statements = realloc_s(prog->statements, prog->capacity * 2 * sizeof(Stmt*));
        prog->capacity *= 2;
    }
    prog->statements[prog->count++] = stmt;
}

// what the fuck is this
Program *parse(Parser* parser) {
    Program* prog = calloc_s(1, sizeof(Program));
    prog->count = 0;
    prog->capacity = 8;
    prog->statements = calloc_s(prog->capacity, sizeof(Stmt*));

    while (_peek(parser).type != TOK_EOF && _peek(parser).type != TOK_ERROR) {
        Stmt *stmt = NULL;
        switch (parser->lookahead.type) {
            case KW_INT:
            case KW_FLOAT:
            case KW_STRING:
            case KW_VOID:
                stmt = _parse_decl(parser, _peek(parser).type);
                break;
            default:
                exit_with_error("unexpected token", 6);
                break;
        }
        _add_statement(prog, stmt);
    }

    return prog;
}
