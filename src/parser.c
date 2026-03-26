#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "lexer.h"
#include "parser.h"
#include "memory.h"
#include "utils.h"
#include "ast.h"

// FORWARD DECLARATIONS
static Expr *_parse_expression(Parser *parser, double precedence);
static Expr *_parse_prefix(Parser *parser, Token token);
static Expr *_parse_infix(Parser *parser, Expr *left, TokenType op);

// ===== PUBLIC APIS =====
void init_parser(Parser *parser, Lexer *lexer, bool debug) {
    parser->lexer = lexer;
    parser->lookahead = next_token(parser->lexer);
    parser->debug = debug;
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
        print_token(parser->lookahead);
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
/// get ready: a lot of very messy parsing code will follow this comment.
/// you have been warned.
/// [please just end my misery already]
// parse expression with right-side priority -> parse right side then merge to left side
static Expr *_parse_expression(Parser *parser, double precedence) {
    Expr *left;
    TokenType op;

    left = _parse_prefix(parser, _peek(parser));
    _consume(parser);
    //while (_peek(parser).type != PUNC_SEMICOLON) _consume(parser);
    while (precedence < op_precedence(_peek(parser).type, false)) {
        op = _peek(parser).type;
        _consume(parser);
        left = _parse_infix(parser, left, op);
    }
    return left;
}
// parse prefix (any element before operation)
static Expr *_parse_prefix(Parser *parser, Token token) {
    switch (token.type) {
        case LIT_INTEGER: return int_literal(atoi(token_to_string(token)));
        case LIT_STRING: return string_literal(token_to_string(token));
        case TOK_IDENTIFIER: return identifier(token_to_string(token));
        default:
            exit_with_error("expected expression", 11);
            return NULL;
    }
}
// parse infix (operation + any element after operation)
static Expr *_parse_infix(Parser *parser, Expr *left, TokenType op) {
    Expr *right;

    switch (op) {
        case OP_ADD:
        case OP_SUBTRACT:
        case OP_MULTIPLY:
        case OP_DIVIDE:
        case OP_MODULO:
            right = _parse_expression(parser, op_precedence(op, true));
            return binary_expr(left, op, right);
        default:
            return left;
    }
}
static Stmt *_parse_statement();
// function argument: [type] [identifier],...
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
                    // parse type
                    type = _peek(parser).type;
                    _consume(parser);

                    // parse name
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
// function declaration: [type] [identifier] ([function arg],...) { [statement], ... }
static Stmt *_parse_func_decl(Parser *parser, TokenType type, char *name) {
    FuncArg *args;
    size_t count, capacity;

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

    // TODO: parse expression
    expr = _parse_expression(parser, 0.0);

    _check(parser, PUNC_SEMICOLON);
    _consume(parser);

    return var_decl(type, name, expr);
}
static Stmt *_parse_declaration(Parser *parser, TokenType type) {
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
            return NULL;
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
                stmt = _parse_declaration(parser, _peek(parser).type);
                break;
            default:
                print_token(_peek(parser));
                exit_with_error("unexpected token", 6);
        }
        _add_statement(prog, stmt);
    }

    return prog;
}
