#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <stdbool.h>

#include "lexer.h"
#include "memory.h"

// forward declarations -> struct have pointer to themselves
typedef struct Expr Expr;
typedef struct Stmt Stmt;

// extra classes
typedef struct {
    TokenType type;
    char *name;
} FuncArg;
FuncArg *func_arg(TokenType type, char *name);
char *func_args_to_string(FuncArg *arg, size_t count);

// expressions
typedef enum {
    EXPR_LIT_INTEGER,
    EXPR_LIT_STRING,
    EXPR_IDENTIFIER,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_FUNC_CALL
} ExprType;

typedef struct {
    Expr *left;
    TokenType op;
    Expr *right;
} BinaryExpr;
typedef struct {
    TokenType op;
    Expr *right;
} UnaryExpr;
typedef struct {
    char *name;
} IdentifierExpr;
typedef struct {
    char *name;
    Expr **args;
    size_t count;
    size_t capacity;
} FuncCallExpr;
typedef struct {
    int value;
} IntLiteral;
typedef struct {
    char* value;
} StringLiteral;

struct Expr {
    ExprType type;
    union {
        BinaryExpr binary;
        UnaryExpr unary;
        IdentifierExpr identifier;
        FuncCallExpr func_call;
        IntLiteral int_literal;
        StringLiteral str_literal;
    };
};
Expr *binary_expr(Expr *left, TokenType op, Expr *right);
Expr *unary_expr(TokenType op, Expr *right);
Expr *identifier(char *name);
Expr *func_call(char *name, Expr **args, size_t count, size_t capacity);
Expr *int_literal(int value);
Expr *string_literal(char *name);
char *expr_to_string(Expr* expr);

// statements
typedef enum {
    STMT_VAR_DECL,
    STMT_VAR_ASSIGN,
    STMT_BLOCK,
    STMT_FUNC_DECL,
    STMT_RETURN,
    STMT_EXPR
} StmtType;

typedef struct {
    TokenType type;
    char *name;
    Expr *value;
} VarDeclStmt;
typedef struct {
    char *name;
    Expr *value;
} VarAssignStmt;
typedef struct {
    Stmt **statements;
    size_t count;
    size_t capacity;
} BlockStmt;
typedef struct {
    TokenType type;
    char *name;
    FuncArg *args; // TODO: upgrade to default arguments (C++) later
    size_t count;
    size_t capacity;
    Stmt *body;
} FuncDeclStmt;
typedef struct {
    Expr *value;
} ReturnStmt;
typedef struct {
    Expr *value;
} ExprStmt;
struct Stmt {
    StmtType type;
    union {
        VarDeclStmt var_decl;
        VarAssignStmt var_assign;
        BlockStmt block;
        FuncDeclStmt func_decl;
        ReturnStmt return_stmt;
        ExprStmt expr;
    };
};
Stmt *var_decl(TokenType type, char *name, Expr *value);
Stmt *var_assign(char *name, Expr *value);
Stmt *block(Stmt **statements, size_t count, size_t capacity);
Stmt *func_decl(TokenType type, char *name, FuncArg *args, size_t count, size_t capacity, Stmt *body);
Stmt *return_stmt(Expr *value);
Stmt *expr_stmt(Expr *value);
void print_statement(Stmt *stmt);

// program
typedef struct {
    Stmt **statements;
    size_t count;
    size_t capacity;
} Program;
void init_program(Program *prog);
void print_program(Program *prog);
//void free_program(Program *prog);

double op_precedence(TokenType op_type, bool rightside);

#endif // AST_H
