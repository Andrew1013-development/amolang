#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/memory.h"
#include "../include/utils.h"
#include "../include/ast.h"

// ===== FORWARD DECLARATIONS =====
static Expr *_parse_expression(Parser *parser, double precedence);
static Expr *_parse_prefix(Parser *parser);
static Expr *_parse_infix(Parser *parser, Expr *left, TokenType op);
static Expr *_parse_func_call(Parser *parser, char *name);
static Stmt *_parse_statement(Parser *parser);
static Stmt *_parse_block(Parser *parser);
static Stmt *_parse_if(Parser *parser);
static Stmt *_parse_declaration(Parser *parser, TokenType type);
static Stmt *_parse_func_decl(Parser *parser, TokenType type, char *name);
static FuncArg *_parse_func_args(Parser *parser, size_t *count, size_t *capacity);
static Stmt *_parse_var_decl(Parser *parser, TokenType type, char *name);

// ===== PUBLIC APIS =====
void init_parser(Parser *parser, Lexer *lexer, bool debug) {
    parser->lexer = lexer;
    parser->lookahead = next_token(parser->lexer);
    parser->debug = debug;
}

// ===== INTERNAL FUNCTIONS =====
// return lookahead token
static Token _peek(Parser *parser) {
    return parser->lookahead;
}
// set current to lookahead then move lookahead
static void _consume(Parser *parser) {
    parser->current = parser->lookahead;
    parser->lookahead = next_token(parser->lexer);
}

// consume current if lookahead is of expected type
static bool _match(Parser *parser, TokenType type) {
    if (parser->lookahead.type == type) {
        _consume(parser);
        return true;
    }
    return false;
}
// check if lookahead is of expected type, then consume if type matches
static void _expect(Parser *parser, TokenType type) {
    char msg[256];
    
    if (parser->lookahead.type != type) {
        sprintf(msg, "mismatched token types (expected %s, got %s) on line %d", tokentype_to_string(type), tokentype_to_string(parser->lookahead.type), parser->lookahead.line);
        exit_with_error(msg, 8);
    }
    _consume(parser);
}

// ===== EXPRESSION PARSING =====
// entry point to parse expressions 
// parse with right-side priority -> parse right side (infix) then merge to left side (prefix)
static Expr *_parse_expression(Parser *parser, double precedence) {
    Expr *left;
    TokenType op;

    left = _parse_prefix(parser);
    //_consume(parser);
    while (precedence < op_precedence(_peek(parser).type, false)) {
        op = _peek(parser).type;
        _consume(parser); // consume the operator
        left = _parse_infix(parser, left, op);
    }
    return left;
}
// parse prefix (any element before operation)
static Expr *_parse_prefix(Parser *parser) {
    Token token;
    Expr *expr;

    token = _peek(parser);
    _consume(parser);
    switch (token.type) {
        // literals
        case LIT_INTEGER: return int_literal(atoi(token_to_string(token)));
        case LIT_FLOAT: return float_literal(atof(token_to_string(token)));
        case LIT_STRING: return string_literal(token_to_string(token));
        // variables or function calls
        case TOK_IDENTIFIER:
            if (_peek(parser).type == PUNC_LPAREN)
                return _parse_func_call(parser, token_to_string(token));
            return identifier(token_to_string(token));
        // unary expressions
        case OP_SUBTRACT:
        case OP_BITWISE_NOT:
            expr = _parse_expression(parser, 100.0); // force 100 to dominate operator precedence
            return unary_expr(token.type, expr);
        case PUNC_LPAREN:
            expr = _parse_expression(parser, 0);
            _expect(parser, PUNC_RPAREN);
            return expr;
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
        case OP_LOGICAL_AND:
        case OP_LOGICAL_OR:
        case OP_LOGICAL_NOT:
        case OP_BITWISE_AND:
        case OP_BITWISE_OR:
        case OP_BITWISE_XOR:
        case OP_BITWISE_LEFT_SHIFT:
        case OP_BITWISE_RIGHT_SHIFT:
        case COMP_LESS:
        case COMP_GREATER:
        case COMP_LESS_EQUAL:
        case COMP_GREATER_EQUAL:
        case COMP_EQUAL:
        case COMP_NOT_EQUAL:
            right = _parse_expression(parser, op_precedence(op, true));
            return binary_expr(left, op, right);
        default:
            return left;
    }
}
// function call: [identifier]([params],...)
static Expr *_parse_func_call(Parser *parser, char *name) {
    Expr **params;
    size_t count, capacity;

    count = 0;
    capacity = 8;
    params = calloc_s(capacity, sizeof(Expr));

    _expect(parser, PUNC_LPAREN);
    while (_peek(parser).type != PUNC_RPAREN && _peek(parser).type != TOK_EOF) {
        if (count >= capacity) {
            capacity *= 2;
            params = realloc_s(params, capacity * sizeof(Expr));
        }
        params[count++] = _parse_expression(parser, 0.0);
        if (!(_match(parser, PUNC_COMMA))) 
            break;
    }
    _expect(parser, PUNC_RPAREN);
    return func_call(name, params, count, capacity);
}
// ===== STATEMENT PARSING =====
// entry point to parse statements
static Stmt *_parse_statement(Parser *parser) {
    Expr *value;
    char *identifier;

    switch (_peek(parser).type) {
        case KW_INT:
        case KW_FLOAT:
        case KW_STRING:
            return _parse_declaration(parser, _peek(parser).type);
        case KW_RETURN:
            _consume(parser); // consume KW_RETURN
            value = _parse_expression(parser, 0.0);
            _expect(parser, PUNC_SEMICOLON);
            return return_stmt(value);
        case KW_IF:
            _consume(parser); // consume KW_IF
            return _parse_if(parser);
        case TOK_IDENTIFIER:
            identifier = token_to_string(_peek(parser));
            _consume(parser); // consume TOK_IDENTIFIER
            switch (_peek(parser).type) {
                case OP_ASSIGN:
                    _consume(parser); // consume OP_ASSIGN
                    value = _parse_expression(parser, 0.0);
                    _expect(parser, PUNC_SEMICOLON);
                    return var_assign(identifier, value);
                case PUNC_LPAREN:
                    value = _parse_func_call(parser, identifier);
                    _expect(parser, PUNC_SEMICOLON);
                    return expr_stmt(value);
                default:
                    exit_with_error("expected assignment or function call", 10);
                    return NULL;
            }
        default:
            exit_with_error("expected statement", 15);
            return NULL;
    }
}
// block statement: { [statement],... }
static Stmt *_parse_block(Parser *parser) {
    size_t count = 0, capacity = 8;
    Stmt **statements = calloc_s(capacity, sizeof(Stmt*));

    _expect(parser, PUNC_LBRACE);
    while (_peek(parser).type != PUNC_RBRACE && _peek(parser).type != TOK_EOF) {
        if (count >= capacity) {
            capacity *= 2;
            statements = realloc_s(statements, capacity * sizeof(Stmt*));
        }
        statements[count++] = _parse_statement(parser);
    }
    _expect(parser, PUNC_RBRACE);
    return block(statements, count, capacity);
}
// if statement: 
static Stmt *_parse_if(Parser *parser) {
    Expr *condition;
    Stmt *then_branch, *else_branch;

    _expect(parser, PUNC_LPAREN);
    condition = _parse_expression(parser, 0.0);
    _expect(parser, PUNC_RPAREN);
    then_branch = _parse_block(parser);
    else_branch = NULL;
    if (_peek(parser).type == KW_ELSE) {
        _consume(parser); // consume KW_ELSE
        else_branch  = _parse_block(parser);
    }
    return if_stmt(condition, then_branch, else_branch);
}
// ===== DECLARATION PARSING =====
// entry point to parse declarations (function declarations and variable declarations)
static Stmt *_parse_declaration(Parser *parser, TokenType type) {
    char *name;

    _consume(parser); // consume type token

    // parse identifier value
    _expect(parser, TOK_IDENTIFIER);
    name = token_to_string(parser->current);

    switch (_peek(parser).type) {
        case PUNC_LPAREN:
            return _parse_func_decl(parser, type, name);
        case OP_ASSIGN:
            return _parse_var_decl(parser, type, name);
        case PUNC_SEMICOLON:
            _consume(parser); // consume PUNC_SEMICOLON
            return var_decl(type, name, NULL);
        default:
            exit_with_error("malformed declaration", 7);
            return NULL;
    }
}
// function declaration: [type] [identifier] ([function arg],...) { [block statement] }
static Stmt *_parse_func_decl(Parser *parser, TokenType type, char *name) {
    FuncArg *args;
    size_t count, capacity;
    Stmt *body;

    args = _parse_func_args(parser, &count, &capacity);
    body = _parse_block(parser);
    return func_decl(type, name, args, count, capacity, body);
}
// function argument: ([type] [identifier],...)
static FuncArg *_parse_func_args(Parser *parser, size_t *count, size_t *capacity) {
    FuncArg *args;
    TokenType type;
    char *name;

    // initial capacity
    *count = 0;
    *capacity = 8;
    args = calloc_s(*capacity, sizeof(FuncArg));

    _expect(parser, PUNC_LPAREN);
    while (_peek(parser).type != PUNC_RPAREN && _peek(parser).type != TOK_EOF) {
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
        if (!(_match(parser, PUNC_COMMA))) 
            break;
    }
    _expect(parser, PUNC_RPAREN);
    return args;
}
// variable declaration: [type] [identifier] = [expression]
static Stmt *_parse_var_decl(Parser *parser, TokenType type, char *name) {
    Expr *expr;

    _expect(parser, OP_ASSIGN);
    expr = _parse_expression(parser, 0.0);
    _expect(parser, PUNC_SEMICOLON);
    return var_decl(type, name, expr);
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
                exit_with_error("invalid top-level declaration", 6);
        }
        _add_statement(prog, stmt);
    }
    return prog;
}
