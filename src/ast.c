#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "stringbuilder.h"

static const char* data_type(TokenType type) {
    switch (type) {
        case KW_INT: return "int";
        case KW_FLOAT: return "float";
        case KW_STRING: return "string";
        case KW_VOID: return "void";
        default: return "unknown";
    }
}

// function arguments
FuncArg *func_arg(TokenType type, char *name) {
    FuncArg *arg = malloc_s(sizeof(FuncArg));

    arg->type = type;
    arg->name = name;

    return arg;
}
char *func_args_to_string(FuncArg *args, size_t count) {
    char temp[1024];
    StringBuilder sb;

    init_sb(&sb);
    concat_sb(&sb, "[");
    for (size_t i = 0; i < count; i++) {
        sprintf(temp, "FuncArg(%s, %s), ", data_type(args[i].type), args[i].name);
        concat_sb(&sb, temp);
    }
    concat_sb(&sb, "]");
    return sb.buffer;
}

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
char *expr_to_string(Expr *expr) {
    char *str = calloc_s(1024, sizeof(char));

    switch (expr->type) {
        case EXPR_LIT_INTEGER:
            sprintf(str, "IntLiteral(%d)", expr->int_literal.value);
            break;
        case EXPR_LIT_STRING:
            sprintf(str, "StrLiteral(%s)", expr->str_literal.value);
            break;
        case EXPR_IDENTIFIER:
            sprintf(str, "Identifier(%s)", expr->identifier.name);
            break;
        case EXPR_BINARY:
        case EXPR_UNARY:
        case EXPR_FUNC_CALL:
            break;
    }

    return str;
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
            printf("VarDecl(%s, %s, %s)\n", data_type(stmt->var_decl.type), stmt->var_decl.name, expr_to_string(stmt->var_decl.value));
            break;
        case STMT_VAR_ASSIGN:
            printf("VarAssign(%s, %s)\n", stmt->var_assign.name, expr_to_string(stmt->var_assign.value));
            break;
        case STMT_BLOCK:
            break;
        case STMT_FUNC_DECL:
            printf(
                   "FuncDecl(%s, %s, %s, %zu, %zu)\n",
                   data_type(stmt->func_decl.type),
                   stmt->func_decl.name,
                   func_args_to_string(stmt->func_decl.args, stmt->func_decl.count),
                   stmt->func_decl.count,
                   stmt->func_decl.capacity
            );
            break;
        case STMT_RETURN:
            printf("Return(%s)\n", expr_to_string(stmt->return_stmt.value));
            break;
        case STMT_EXPR:
            printf("Expr(%s)\n", expr_to_string(stmt->expr.value));
            break;
    }
}

// programs
void init_program(Program *prog) {
    prog->count = 0;
    prog->capacity = 8;
    prog->statements = calloc_s(prog->capacity, sizeof(Stmt*));
}
void print_program(Program *prog) {
    printf("===== PROGRAM DETAILS =====\n");
    printf("length: %zu, capacity: %zu\n", prog->count, prog->capacity);
    printf("===== START OF PROGRAM CONTENT =====\n");
    for (size_t i = 0; i < prog->count; i++)
        print_statement(prog->statements[i]);
    printf("===== END OF PROGRAM CONTENT =====\n");
}
