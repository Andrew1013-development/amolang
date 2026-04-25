#ifndef LEXER_H
#define LEXER_H

typedef enum {
    // punctuator
    PUNC_LBRACE, PUNC_RBRACE, // {}
    PUNC_LBRACKET, PUNC_RBRACKET, // []
    PUNC_LPAREN, PUNC_RPAREN, // ()
    PUNC_SEMICOLON, PUNC_COLON, PUNC_COMMA, PUNC_DOT, // ;:,.

    // operator
    OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE, OP_MODULO, // math
    OP_LOGICAL_AND, OP_LOGICAL_OR, OP_LOGICAL_NOT, // logical
    OP_BITWISE_AND, OP_BITWISE_OR, OP_BITWISE_XOR, OP_BITWISE_NOT, // bitwise operations
    OP_BITWISE_LEFT_SHIFT, OP_BITWISE_RIGHT_SHIFT, // << >>
    OP_ASSIGN, // =

    // comparator
    COMP_LESS, COMP_GREATER, // < >
    COMP_LESS_EQUAL, COMP_GREATER_EQUAL, // <= >=
    COMP_EQUAL, COMP_NOT_EQUAL, // == !=

    // keyword
    KW_INT, KW_FLOAT, KW_STRING, KW_VOID, // data types
    KW_IF, KW_ELSE, // conditional
    KW_FOR, KW_WHILE, KW_BREAK, KW_CONTINUE, // iterations
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
