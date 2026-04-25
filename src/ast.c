#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../include/ast.h"
#include "../include/stringbuilder.h"

static const char* data_type(TokenType type) {
    switch (type) {
        case KW_INT: return "int";
        case KW_FLOAT: return "float";
        case KW_STRING: return "string";
        case KW_VOID: return "void";
        default: return "unknown data type";
    }
}
static const char* operation_type(TokenType type) {
    switch (type) {
        case OP_ADD: return "+";
        case OP_SUBTRACT: return "-";
        case OP_MULTIPLY: return "*";
        case OP_DIVIDE: return "/";
        case OP_MODULO: return "%";
        case OP_LOGICAL_AND: return "&&";
        case OP_LOGICAL_OR: return "||";
        case OP_LOGICAL_NOT: return "!";
        case OP_BITWISE_AND: return "&";
        case OP_BITWISE_OR: return "|";
        case OP_BITWISE_XOR: return "^";
        case OP_BITWISE_NOT: return "~";
        case OP_BITWISE_LEFT_SHIFT: return "<<";
        case OP_BITWISE_RIGHT_SHIFT: return ">>";
        case COMP_LESS: return "<";
        case COMP_GREATER: return ">";
        case COMP_LESS_EQUAL: return "<=";
        case COMP_GREATER_EQUAL: return ">=";
        case COMP_EQUAL: return "==";
        case COMP_NOT_EQUAL: return "!=";
        default: return "unknown operation";
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
// function parameters (basically just a bunch of expressions)
char *func_params_to_string(Expr **params, size_t count) {
    char temp[1024];
    StringBuilder sb;

    init_sb(&sb);
    concat_sb(&sb, "[");
    for (size_t i = 0; i < count; i++) {
        sprintf(temp, "%s, ", expr_to_string(params[i]));
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
Expr *func_call(char *name, Expr **params, size_t count, size_t capacity) {
    Expr *expr = malloc_s(sizeof(Expr));

    expr->type = EXPR_FUNC_CALL;
    expr->func_call.name = name;
    expr->func_call.params = params;
    expr->func_call.count = count;
    expr->func_call.capacity = capacity;
    return expr;
}
Expr *int_literal(long long value) {
    Expr *expr = malloc_s(sizeof(Expr));

    expr->type = EXPR_LIT_INTEGER;
    expr->int_literal.value = value;
    return expr;
}
Expr *float_literal(double value) {
    Expr *expr = malloc_s(sizeof(Expr));
    
    expr->type = EXPR_LIT_FLOAT;
    expr->float_literal.value = value;
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

    if (expr == NULL) {
        sprintf(str, "NULL");
        return str;
    }
    switch (expr->type) {
        case EXPR_LIT_INTEGER:
            sprintf(str, "IntLiteral(%lld)", expr->int_literal.value);
            break;
        case EXPR_LIT_FLOAT:
            sprintf(str, "FloatLiteral(%f)", expr->float_literal.value);
            break;
        case EXPR_LIT_STRING:
            sprintf(str, "StrLiteral(%s)", expr->str_literal.value);
            break;
        case EXPR_IDENTIFIER:
            sprintf(str, "Identifier(%s)", expr->identifier.name);
            break;
        case EXPR_BINARY:
            sprintf(str,
                    "BinaryExpr(%s, %s, %s)",
                    expr_to_string(expr->binary.left),
                    operation_type(expr->binary.op),
                    expr_to_string(expr->binary.right)
            );
            break;
        case EXPR_UNARY:
            sprintf(str,
                    "UnaryExpr(%s, %s)",
                    operation_type(expr->unary.op),
                    expr_to_string(expr->unary.right)
            );
            break;
        case EXPR_FUNC_CALL:
            sprintf(str,
                    "FuncCall(%s, %s, %zu, %zu)",
                    expr->func_call.name,
                    func_params_to_string(expr->func_call.params, expr->func_call.count),
                    expr->func_call.count,
                    expr->func_call.capacity
            );
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
Stmt *if_stmt(Expr *condition, Stmt *then_branch, Stmt *else_branch) {
    Stmt *stmt = malloc_s(sizeof(Stmt));

    stmt->type = STMT_IF;
    stmt->if_stmt.condition = condition;
    stmt->if_stmt.then_branch = then_branch;
    stmt->if_stmt.else_branch = else_branch;
    return stmt;
}
void print_statement(Stmt *stmt) {
    if (stmt == NULL) {
        printf("NULL\n");
        return;
    }
    
    switch (stmt->type) {
        case STMT_VAR_DECL:
            printf("VarDecl(%s, %s, %s)\n", data_type(stmt->var_decl.type), stmt->var_decl.name, expr_to_string(stmt->var_decl.value));
            break;
        case STMT_VAR_ASSIGN:
            printf("VarAssign(%s, %s)\n", stmt->var_assign.name, expr_to_string(stmt->var_assign.value));
            break;
        case STMT_BLOCK:
            printf("===== BlockStmt DETAILS =====\n");
            printf("count = %zu, capacity = %zu\n", stmt->block.count, stmt->block.capacity);
            printf("===== START OF BlockStmt =====\n");
            for (size_t i = 0; i < stmt->block.count; i++)
                print_statement(stmt->block.statements[i]);
            printf("===== END OF BlockStmt =====\n");
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
            print_statement(stmt->func_decl.body);
            break;
        case STMT_RETURN:
            printf("Return(%s)\n", expr_to_string(stmt->return_stmt.value));
            break;
        case STMT_EXPR:
            printf("Expr(%s)\n", expr_to_string(stmt->expr.value));
            break;
        case STMT_IF:
            printf("If(%s)\n", expr_to_string(stmt->if_stmt.condition));
            printf("===== START OF then_branch =====\n");
            print_statement(stmt->if_stmt.then_branch);
            printf("===== END OF then_branch =====\n");
            printf("===== START OF else_branch =====\n");
            print_statement(stmt->if_stmt.else_branch);
            printf("===== END OF else_branch =====\n");
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
void free_program(Program *prog) {
    free_s(prog->statements);
    prog->statements = NULL;
    prog->capacity = prog->count = 0;
}

// in accordance with C/C++ operator precedence
double op_precedence(TokenType op_type, bool rightside) {
    double val = 0.0;

    switch (op_type) {
        case OP_LOGICAL_NOT:
        case OP_BITWISE_NOT:
            val = 11.0;
            break;
        // multiplicative operators
        case OP_MULTIPLY:
        case OP_DIVIDE:
        case OP_MODULO:
            val = 10.0;
            break;
        // additive operators
        case OP_ADD:
        case OP_SUBTRACT:
            val = 9.0;
            break;
        case OP_BITWISE_LEFT_SHIFT:
        case OP_BITWISE_RIGHT_SHIFT:
            val = 8.0;
            break;
        case COMP_LESS:
        case COMP_GREATER:
        case COMP_LESS_EQUAL:
        case COMP_GREATER_EQUAL:
            val = 7.0;
            break;
        case COMP_EQUAL:
        case COMP_NOT_EQUAL:
            val = 6.0;
            break;
        // bitwise operations
        case OP_BITWISE_AND:
            val = 5.0;
            break;
        case OP_BITWISE_XOR:
            val = 4.0;
            break;
        case OP_BITWISE_OR:
            val = 3.0;
            break;
        case OP_LOGICAL_AND:
            val = 2.0;
            break;
        case OP_LOGICAL_OR:
            val = 1.0;
            break;
        default:
            break;
    }
    // use 0.5 to ensure proper comparison in floating point (0.5 = 2^-1)
    return val + 0.5 * (rightside);
}
