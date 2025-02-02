#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>

// Include ValueType and Value structures 
#include "symtab.h"


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
