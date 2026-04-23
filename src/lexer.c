#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../include/lexer.h"
#include "../include/memory.h"

const char* tokentype_to_string(TokenType type) {
    switch (type) {
        // punctuators
        case PUNC_LBRACE: return "PUNC_LBRACE";
        case PUNC_RBRACE: return "PUNC_RBRACE";
        case PUNC_LBRACKET: return "PUNC_LBRACKET";
        case PUNC_RBRACKET: return "PUNC_RBRACKET";
        case PUNC_LPAREN: return "PUNC_LPAREN";
        case PUNC_RPAREN: return "PUNC_RPAREN";
        case PUNC_SEMICOLON: return "PUNC_SEMICOLON";
        case PUNC_COLON: return "PUNC_COLON";
        case PUNC_COMMA: return "PUNC_COMMA";
        // operators
        case OP_ADD: return "OP_ADD";
        case OP_SUBTRACT: return "OP_SUBTRACT";
        case OP_MULTIPLY: return "OP_MULTIPLY";
        case OP_DIVIDE: return "OP_DIVIDE";
        case OP_MODULO: return "OP_MODULO";
        case OP_AND: return "OP_AND";
        case OP_OR: return "OP_OR";
        case OP_XOR: return "OP_XOR";
        case OP_NOT: return "OP_NOT";
        case OP_ASSIGN: return "OP_ASSIGN";
        // keywords
        case KW_INT: return "KW_INT";
        case KW_FLOAT: return "KW_FLOAT";
        case KW_STRING: return "KW_STRING";
        case KW_VOID: return "KW_VOID";
        case KW_RETURN: return "KW_RETURN";
        // literals
        case LIT_INTEGER: return "LIT_INTEGER";
        case LIT_FLOAT: return "LIT_FLOAT";
        case LIT_STRING: return "LIT_STRING";
        // others
        case TOK_IDENTIFIER: return "TOK_IDENTIFIER";
        case TOK_EOF: return "TOK_EOF";
        case TOK_ERROR: return "TOK_ERROR";
        case TOK_NOP: return "TOK_NOP";
        default: return "unknown";
    }
}

// ===== PUBLIC API =====
char *token_to_string(Token token) {
    char *name = calloc_s(token.length, sizeof(char));

    sprintf(name, "%.*s", token.length, token.start);
    return name;
}
void print_token(Token token) {
   printf("'%.*s' (type %s) @ line %d\n", token.length, token.start, tokentype_to_string(token.type), token.line);
}
void init_lexer(Lexer* lexer, char* src, bool debug) {
    lexer->start = lexer->current = src;
    lexer->line = 1;
    lexer->debug = debug;
}

// ===== INTERNAL FUNCTIONS =====
// see character at specified offset -> peek character
static char _peek(Lexer* lexer, const int offset) {
    return lexer->current[offset];
}
// advance current pointer by 1 -> consume character
static char _consume(Lexer* lexer) {
    lexer->current++;
    return lexer->current[-1];
}
// skip all whitespace characters + comments + directives (to be implemented later)
static void _skip(Lexer* lexer) {
    char c;

    do {
        c = _peek(lexer, 0);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                _consume(lexer);
                break;
            case '\n':
                lexer->line++;
                _consume(lexer);
                break;
            case '/':
                if (_peek(lexer, 1) == '/') {
                    _consume(lexer); // consume first /
                    _consume(lexer); // consume second /
                    while (_peek(lexer, 0) != '\n') _consume(lexer);
                    break;
                } else return;
            default:
                return;
        }
    } while (true);
}
// match token type to identifier or keywords
static TokenType _word_type(Lexer* lexer) {
    switch ((int)(lexer->current - lexer->start)) {
        case 3:
            if (strncmp(lexer->start, "int", 3) == 0) return KW_INT;
            break;
        case 4:
            if (strncmp(lexer->start, "void", 4) == 0) return KW_VOID;
            break;
        case 5:
            if (strncmp(lexer->start, "float", 5) == 0) return KW_FLOAT;
        case 6:
            if (strncmp(lexer->start, "return", 6) == 0) return KW_RETURN;
            if (strncmp(lexer->start, "string", 6) == 0) return KW_STRING;
            break;
    }
    return TOK_IDENTIFIER;
}
// make token of specified type
static Token _make_token(Lexer* lexer, TokenType type) {
    Token token;
    token.type = type;
    token.start = lexer->start;
    token.length = (int)(lexer->current - lexer->start);
    token.line = lexer->line;
    return token;
}
// specific token implementations
static Token _identifier(Lexer* lexer) {
    while (isalpha(_peek(lexer, 0)) || isdigit(_peek(lexer, 0)) || _peek(lexer, 0) == '_')
        _consume(lexer);

    return _make_token(lexer, _word_type(lexer));
}
static Token _number(Lexer* lexer) {
    while (isdigit(_peek(lexer, 0)))
        _consume(lexer);

    return _make_token(lexer, LIT_INTEGER);
}
static Token _string(Lexer* lexer) {
    while (isascii(_peek(lexer, 0)) && _peek(lexer, 0) != '"')
        _consume(lexer);

    _consume(lexer);
    return _make_token(lexer, LIT_STRING);
}

// what the fuck is this
Token next_token(Lexer* lexer) {
    char c;

    _skip(lexer);

    lexer->start = lexer->current;
    c = _consume(lexer);

    if (isalpha(c) || c == '_') return _identifier(lexer);
    if (isdigit(c)) return _number(lexer);
    switch (c) {
        // punctuator
        case '(': return _make_token(lexer, PUNC_LPAREN);
        case ')': return _make_token(lexer, PUNC_RPAREN);
        case '[': return _make_token(lexer, PUNC_LBRACKET);
        case ']': return _make_token(lexer, PUNC_RBRACKET);
        case '{': return _make_token(lexer, PUNC_LBRACE);
        case '}': return _make_token(lexer, PUNC_RBRACE);
        case ';': return _make_token(lexer, PUNC_SEMICOLON);
        case ':': return _make_token(lexer, PUNC_COLON);
        case ',': return _make_token(lexer ,PUNC_COMMA);
        // operator
        case '+': return _make_token(lexer, OP_ADD);
        case '-': return _make_token(lexer, OP_SUBTRACT);
        case '*': return _make_token(lexer, OP_MULTIPLY);
        case '/': return _make_token(lexer, OP_DIVIDE);
        case '%': return _make_token(lexer, OP_MODULO);
        case '=': return _make_token(lexer, OP_ASSIGN);
        case '&': return _make_token(lexer, OP_AND);
        case '|': return _make_token(lexer, OP_OR);
        case '^': return _make_token(lexer, OP_XOR);
        case '!': return _make_token(lexer, OP_NOT);
        // ???
        case '"': return _string(lexer);
    }
    return _make_token(lexer, TOK_EOF);
}
