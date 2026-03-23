#include <stdio.h>

#include "ast.h"

// expressions
Expr *binary_expr(Expr *left, TokenType op, Expr *right) {
    Expr *expr = malloc_s(sizeof(Expr));

    expr->type = EXPR_BINARY;
    expr->binary.left = left;
    expr->binary.op = op;
    expr->binary.right = right;

    return expr;
}
Expr *unary_expr(TokenType op, Expr *right) {
    Expr *expr = malloc_s(sizeof(Expr));

    expr->type = EXPR_UNARY;
    expr->unary.op = op;
    expr->unary.right = right;

    return expr;
}
Expr *identifier(char *name) {
    Expr *expr = malloc_s(sizeof(Expr));

    expr->type = EXPR_IDENTIFIER;
    expr->identifier.name = name;

    return expr;
}
Expr *func_call(char *name, Expr **args, size_t count, size_t capacity) {
    Expr *expr = malloc_s(sizeof(Expr));

    expr->type = EXPR_FUNC_CALL;
    expr->func_call.name = name;
    expr->func_call.args = args;
    expr->func_call.count = count;
    expr->func_call.capacity = capacity;

    return expr;
}
Expr *int_literal(int value) {
    Expr *expr = malloc_s(sizeof(Expr));

    expr->type = EXPR_LIT_INTEGER;
    expr->int_literal.value = value;

    return expr;
}
Expr *string_literal(char *value) {
    Expr *expr = malloc_s(sizeof(Expr));

    expr->type = EXPR_LIT_STRING;
    expr->str_literal.value = value;

    return expr;
}
void print_expression(Expr *expr) {

}

// statements
Stmt *var_decl(TokenType type, char *name, Expr *value) {
    Stmt *stmt = malloc_s(sizeof(Stmt));

    stmt->type = STMT_VAR_DECL;
    stmt->var_decl.type = type;
    stmt->var_decl.name = name;
    stmt->var_decl.value = value;

    return stmt;
}
Stmt *var_assign(char *name, Expr *value) {
    Stmt *stmt = malloc_s(sizeof(Stmt));

    stmt->type = STMT_VAR_ASSIGN;
    stmt->var_assign.name = name;
    stmt->var_assign.value = value;

    return stmt;
}
Stmt *block(Stmt **statements, size_t count, size_t capacity) {
    Stmt *stmt = malloc_s(sizeof(Stmt));

    stmt->type = STMT_BLOCK;
    stmt->block.statements = statements;
    stmt->block.count = count;
    stmt->block.capacity = capacity;

    return stmt;
}
Stmt *func_decl(TokenType type, char *name, FuncArg *args, size_t count, size_t capacity, Stmt *body) {
    Stmt *stmt = malloc_s(sizeof(Stmt));

    stmt->type = STMT_FUNC_DECL;
    stmt->func_decl.type = type;
    stmt->func_decl.name = name;
    stmt->func_decl.args = args;
    stmt->func_decl.count = count;
    stmt->func_decl.capacity = capacity;
    stmt->func_decl.body = body;

    return stmt;
}
Stmt *return_stmt(Expr *value) {
    Stmt *stmt = malloc_s(sizeof(Stmt));

    stmt->type = STMT_RETURN;
    stmt->return_stmt.value = value;

    return stmt;
}
Stmt *expr_stmt(Expr *value) {
    Stmt *stmt = malloc_s(sizeof(Stmt));

    stmt->type = STMT_EXPR;
    stmt->expr.value = value;

    return stmt;
}
void print_statement(Stmt *stmt) {
    switch (stmt->type) {
        case STMT_VAR_DECL:
        case STMT_VAR_ASSIGN:
        case STMT_BLOCK:
        case STMT_FUNC_DECL:
        case STMT_RETURN:
        case STMT_EXPR:
            break;
    }
}

// programs
void print_program(Program *prog) {
    printf("length: %zu, capacity: %zu\n", prog->count, prog->capacity);
    printf("===== START OF PROGRAM =====\n");
    for (size_t i = 0; i < prog->count; i++)
        print_statement(prog->statements[i]);
    printf("===== END OF PROGRAM =====\n");
}
