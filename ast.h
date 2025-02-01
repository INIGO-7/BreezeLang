#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>
// Include ValueType and Value structures 
#include "symtab.h"

#define MAXCHILDREN 50

// AST Node Types
enum NodeType {
    NODE_STMTS,
    NODE_ASSIGN,
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_DIV,
    NODE_EXP,
    NODE_INT,
    NODE_FLOAT,
    NODE_ID,
    NODE_PRINT,
    NODE_BOOL_OP,
    NODE_BOOL,
    NODE_STRING,
    NODE_WHILE,
    NODE_FOR,
    NODE_ERROR
};

enum BoolOpType {
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_LE,
    OP_GT,
    OP_GE
};

// AST Node Structure
typedef struct astnode {
    enum NodeType type;                             // Node type
    union {
        int num;                          // Integer values
        float dec;                   // Decimal values
        char *id;                         // Identifier (variable name)
        char *str;                        // Strings
        int boolean;                      // Boolean values (1 for true, 0 for false)
        enum BoolOpType bool_op;         // Type of boolean operation
    } data;
    struct astnode *child[MAXCHILDREN];   // Pointers to child nodes
} astnode_t;

// Helper functions to create values
Value create_float_value(float f);
Value create_int_value(int i);
Value create_string_value(const char* s);
Value create_bool_value(int i);

// AST Functions
astnode_t *astnode_new(int type);
void astnode_add_child(astnode_t *parent, astnode_t *child, int index);
void print_ast(astnode_t *node, int depth);
void free_ast(astnode_t *node);
void evaluate_ast(astnode_t *node);

#endif // AST_H
