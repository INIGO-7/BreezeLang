#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>

#define MAXCHILDREN 5

// AST Node Types
enum NodeType {
    NODE_STMTS,
    NODE_ASSIGN,
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_DIV,
    NODE_EXP,
    NODE_NUM,
    NODE_ID,
    NODE_PRINT,
    NODE_BOOL_OP,
    NODE_BOOL,
};

// AST Node Structure
typedef struct astnode {
    int type;                             // Node type
    union {
        int num;                          // Integer values
        double decimal;                   // Decimal values
        char *id;                         // Identifier (variable name)
        int boolean;                      // Boolean values (1 for true, 0 for false)
        char *bool_op_type;               // Type of boolean operation
    } val;
    struct astnode *child[MAXCHILDREN];   // Pointers to child nodes
} astnode_t;

// Function Prototypes
astnode_t *astnode_new(int type);
void astnode_add_child(astnode_t *parent, astnode_t *child, int index);
void print_ast(astnode_t *node, int depth);
void free_ast(astnode_t *node);
void evaluate_ast(astnode_t *node);

#endif // AST_H
