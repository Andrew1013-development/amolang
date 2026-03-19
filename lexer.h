#ifndef LEXER_H
#define LEXER_H

typedef enum {
    // punctuator
    PUNC_LBRACE, PUNC_RBRACE, // {}
    PUNC_LBRACKET, PUNC_RBRACKET, // []
    PUNC_LPAREN, PUNC_RPAREN, // ()
    PUNC_SEMICOLON, // ;

    // operator
    OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE, OP_MODULO, // math
    OP_AND, OP_OR, OP_XOR, OP_NOT, // bitwise
    OP_ASSIGN, // =

    // keyword
    KW_INT, KW_STRING, // data types
    KW_RETURN,

    // literals
    LIT_NUMBER, LIT_STRING,

    // other
    TOK_IDENTIFIER, TOK_EOF, TOK_ERROR,
    TOK_NOP // useless token type, used for debugging
} TokenType;
typedef struct {
    TokenType type;
    const char* start; // first character of token
    int length;
    int line;
} Token;
void print_token(Token token);
typedef struct {
    char* start; // first character of token
    char* current; // current character of token
    int line;
} Lexer;
void init_lexer(Lexer* lexer, char* src);
Token next_token(Lexer* lexer);

#endif
