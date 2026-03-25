#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

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
        exit_with_error("mismatched token", 8);
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

// function declaration: [type] [identifier] ([type] [identifier],...) { [statement], ... }
static FuncArg *_parse_func_args(Parser *parser, FuncArg *args, size_t *count, size_t *capacity) {
    TokenType type;
    char *name;

    // initial capacity
    *count = 0;
    *capacity = 8;
    args = calloc_s(*capacity, sizeof(FuncArg));

    while (_peek(parser).type != PUNC_RPAREN && _peek(parser).type != TOK_EOF) {
        do {
            switch (_peek(parser).type) {
                case KW_INT:
                case KW_FLOAT:
                case KW_STRING:
                    // parse arg type
                    type = _peek(parser).type;
                    _consume(parser);
                    // parse arg name
                    name = token_to_string(_peek(parser));
                    _consume(parser);

                    if (*count >= *capacity) {
                        *capacity *= 2;
                        args = realloc_s(args, *capacity * sizeof(FuncArg));
                    }

                    args[*count].type = type;
                    args[*count].name = name;
                    (*count)++;
                    break;
                default:
                    exit_with_error("function argument has invalid or no type", 9);
            }
        } while (_match(parser, PUNC_COMMA));
    }
    return args;
}
static Stmt *_parse_func_decl(Parser *parser, TokenType type, char *name) {
    FuncArg *args;
    size_t count, capacity;

    // parse function arguments
    _consume(parser); // consume PUNC_LPAREN
    args = _parse_func_args(parser, args, &count, &capacity);
    _consume(parser); // consume PUNC_RPAREN

    // TODO: parse function body
    _consume(parser); // consume PUNC_LBRACE
    while (_peek(parser).type != PUNC_RBRACE && _peek(parser).type != TOK_EOF) _consume(parser);
    _consume(parser); // consume PUNC_RBRACE

    return func_decl(type, name, args, count, capacity, NULL);
}
// variable declaration: [type] [identifier] = [expression]
static Stmt *_parse_var_decl(Parser *parser, TokenType type, char *name) {
    Expr *expr;

    _consume(parser); // consume identifier
    _consume(parser); // consume OP_ASSIGN

    // TODO: parse expression
    expr = int_literal(16);
    while (_peek(parser).type != PUNC_SEMICOLON) _consume(parser);

    _check(parser, PUNC_SEMICOLON);
    _consume(parser);

    return var_decl(type, name, expr);
}
static Stmt *_parse_decl(Parser *parser, TokenType type) {
    char *name;

    _consume(parser); // consume type token

    // parse identifier value
    _check(parser, TOK_IDENTIFIER);
    _consume(parser);
    name = token_to_string(parser->current);

    switch (_peek(parser).type) {
        case PUNC_LPAREN:
            // function declaration
            return _parse_func_decl(parser, type, name);
        case OP_ASSIGN:
            // variable declaration with initial value
            return _parse_var_decl(parser, type, name);
        case PUNC_SEMICOLON:
            // variable declaration without initial value
            return var_decl(type, name, NULL);
        default:
            print_token(_peek(parser));
            exit_with_error("declaration error", 7);
    }
}

// add statement to program
static void _add_statement(Program *prog, Stmt *stmt) {
    if (prog->count >= prog->capacity) {
        prog->statements = realloc_s(prog->statements, prog->capacity * 2 * sizeof(Stmt*));
        prog->capacity *= 2;
    }
    prog->statements[prog->count++] = stmt;
}

// what the fuck is this
Program *parse(Parser* parser) {
    Program *prog = calloc_s(1, sizeof(Program));
    Stmt *stmt;

    init_program(prog);

    while (_peek(parser).type != TOK_EOF && _peek(parser).type != TOK_ERROR) {
        stmt = NULL;
        switch (_peek(parser).type) {
            case KW_INT:
            case KW_FLOAT:
            case KW_STRING:
            case KW_VOID:
                stmt = _parse_decl(parser, _peek(parser).type);
                break;
            default:
                print_token(_peek(parser));
                exit_with_error("unexpected token", 6);
        }
        _add_statement(prog, stmt);
    }

    return prog;
}
