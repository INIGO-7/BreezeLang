#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>
#include "symtab.h"

#define MAXCHILDREN 5

// Implement different Types
typedef enum {
  VAL_FLOAT,
  VAL_INT,
  VAL_STRING
} ValueType;

typedef struct {
  ValueType type;
  union {
    float float_val;
    int int_val;
    char* str_val;
  } data;
} Value;

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
    NODE_DEC,
    NODE_ID,
    NODE_PRINT,
    NODE_BOOL_OP,
    NODE_BOOL,
    NODE_STRING,
};

// AST Node Structure
typedef struct astnode {
    int type;                             // Node type
    union {
        int num;                          // Integer values
        double decimal;                   // Decimal values
        char *id;                         // Identifier (variable name)
        char *str;                        // Strings
        int boolean;                      // Boolean values (1 for true, 0 for false)
        char *bool_op_type;               // Type of boolean operation
    } val;
    struct astnode *child[MAXCHILDREN];   // Pointers to child nodes
} astnode_t;

// Helper functions to create values
Value create_float_value(float f);
Value create_int_value(int i);
Value create_string_value(const char* s);

// AST Functions
astnode_t *astnode_new(int type);
void astnode_add_child(astnode_t *parent, astnode_t *child, int index);
void print_ast(astnode_t *node, int depth);
void free_ast(astnode_t *node);
void evaluate_ast(astnode_t *node);

#endif // AST_H
