#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

// ===== PUBLIC API =====
void print_token(Token token) {
   printf("'%.*s' (type %d) @ line %d\n", token.length, token.start, token.type, token.line);
}
void init_lexer(Lexer* lexer, char* src) {
    lexer->start = lexer->current = src;
    lexer->line = 1;
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
                if (_peek(lexer, 0) == '/') {
                    _consume(lexer);
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
            if (strncmp(lexer->start, "int", 3)) return KW_INT;
            break;
        case 4:
            if (strncmp(lexer->start, "void", 4)) return KW_VOID;
        case 6:
            if (strncmp(lexer->start, "return", 6)) return KW_RETURN;
            if (strncmp(lexer->start, "string", 6)) return KW_STRING;
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

    return _make_token(lexer, LIT_NUMBER);
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
        case ':': return _make_token(lexer ,PUNC_COLON);
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
    return _make_token(lexer, TOK_ERROR);
}
