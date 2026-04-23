#ifndef LEXER_H
#define LEXER_H

typedef enum {
    // punctuator
    PUNC_LBRACE, PUNC_RBRACE, // {}
    PUNC_LBRACKET, PUNC_RBRACKET, // []
    PUNC_LPAREN, PUNC_RPAREN, // ()
    PUNC_SEMICOLON, PUNC_COLON, PUNC_COMMA, // ;:,

    // operator
    OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE, OP_MODULO, // math
    OP_AND, OP_OR, OP_XOR, OP_NOT, // bitwise
    OP_ASSIGN, // =

    // keyword
    KW_INT, KW_FLOAT, KW_STRING, KW_VOID, // data types
    KW_RETURN,

    // literals
    LIT_INTEGER, LIT_FLOAT, LIT_STRING, // data types

    // other
    TOK_IDENTIFIER, TOK_EOF, TOK_ERROR,
    TOK_NOP // useless token type, used for debugging
} TokenType;
typedef struct {
    TokenType type;
    const char* start; // first character of token
    int length;
    int line;
    bool debug;
} Token;
char *token_to_string(Token token);
const char *tokentype_to_string(TokenType type);
void print_token(Token token);
typedef struct {
    char* start; // first character of token
    char* current; // current character of token
    int line;
    bool debug;
} Lexer;
void init_lexer(Lexer* lexer, char* src, bool debug);
Token next_token(Lexer* lexer);

#endif // LEXER_H
