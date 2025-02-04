#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scope.h"

// Global pointer to the top of our scope stack
static Scope *current_scope = NULL;

// Create a new SymbolNode
static SymbolNode* create_symbol_node(const char *name, ValueType type) {
    SymbolNode* node = (SymbolNode*)malloc(sizeof(SymbolNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation for SymbolNode failed.\n");
        exit(EXIT_FAILURE);
    }
    node->name = strdup(name);
    node->type = type;
    node->next = NULL;
    return node;
}

/**
 * init_scopes
 * Called once at the start of runtime.
 * We push one "global" scope so there's always at least
 * one scope on the stack.
 */
void init_scopes(void) {
    current_scope = NULL;
    push_scope(); // push one global scope by default
}

// push_scope
void push_scope(void) {
    Scope *newScope = (Scope*)malloc(sizeof(Scope));
    if (!newScope) {
        fprintf(stderr, "Error: Memory allocation for Scope failed.\n");
        exit(EXIT_FAILURE);
    }
    newScope->symbols = NULL;
    newScope->parent = current_scope;
    current_scope = newScope;
}

/**
 * pop_scope
 * frees all symbols in the top scope, then discards it
 */
void pop_scope(void) {
    if (!current_scope) {
        fprintf(stderr, "Warning: pop_scope() called with no current scope.\n");
        return;
    }
    // free all symbol nodes in this scope
    SymbolNode *sym = current_scope->symbols;
    while (sym) {
        SymbolNode *tmp = sym->next;
        // If it's a string, free the string
        if (sym->type == TYPE_STRING && sym->data.string_val) {
            free(sym->data.string_val);
        }
        // If it's a function name, we do not free the AST
        free(sym->name);
        free(sym);
        sym = tmp;
    }
    // lastly, pop old
    Scope *oldScope = current_scope;
    current_scope = current_scope->parent;
    free(oldScope);
}

/**
 * lookup_symbol
 * Search from top scope outward
 */
SymbolNode* lookup_symbol(const char *name) {
    Scope *scopeIter = current_scope;
    while (scopeIter) {
        SymbolNode *sym = scopeIter->symbols;
        while (sym) {
            if (strcmp(sym->name, name) == 0) {
                return sym;
            }
            sym = sym->next;
        }
        scopeIter = scopeIter->parent;  // go outward
    }
    return NULL; // not found
}

SymbolNode* put_symbol_int(const char *name, int value) {
    if (!current_scope) {
        fprintf(stderr, "Error: No scope to put symbol '%s' in.\n", name);
        exit(EXIT_FAILURE);
    }
    // see if it already exists in the current scope
    SymbolNode* sym = current_scope->symbols;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            // update
            if (sym->type == TYPE_STRING && sym->data.string_val) {
                free(sym->data.string_val);
            }
            sym->type = TYPE_INT;
            sym->data.int_val = value;
            return sym;
        }
        sym = sym->next;
    }
    // create new
    SymbolNode* newNode = create_symbol_node(name, TYPE_INT);
    newNode->data.int_val = value;
    newNode->next = current_scope->symbols;
    current_scope->symbols = newNode;
    return newNode;
}

SymbolNode* put_symbol_float(const char *name, float value) {
    if (!current_scope) {
        fprintf(stderr, "Error: No scope to put symbol '%s' in.\n", name);
        exit(EXIT_FAILURE);
    }
    SymbolNode* sym = current_scope->symbols;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            // update
            if (sym->type == TYPE_STRING && sym->data.string_val) {
                free(sym->data.string_val);
            }
            sym->type = TYPE_FLOAT;
            sym->data.float_val = value;
            return sym;
        }
        sym = sym->next;
    }
    // create new
    SymbolNode* newNode = create_symbol_node(name, TYPE_FLOAT);
    newNode->data.float_val = value;
    newNode->next = current_scope->symbols;
    current_scope->symbols = newNode;
    return newNode;
}

SymbolNode* put_symbol_string(const char *name, const char *value) {
    if (!current_scope) {
        fprintf(stderr, "Error: No scope to put symbol '%s' in.\n", name);
        exit(EXIT_FAILURE);
    }
    SymbolNode* sym = current_scope->symbols;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            // update
            if (sym->type == TYPE_STRING && sym->data.string_val) {
                free(sym->data.string_val);
            }
            sym->type = TYPE_STRING;
            sym->data.string_val = strdup(value);
            return sym;
        }
        sym = sym->next;
    }
    // create new
    SymbolNode* newNode = create_symbol_node(name, TYPE_STRING);
    newNode->data.string_val = strdup(value);
    newNode->next = current_scope->symbols;
    current_scope->symbols = newNode;
    return newNode;
}

SymbolNode* put_symbol_bool(const char *name, int value) {
    if (!current_scope) {
        fprintf(stderr, "Error: No scope to put symbol '%s' in.\n", name);
        exit(EXIT_FAILURE);
    }
    SymbolNode* sym = current_scope->symbols;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            // update
            if (sym->type == TYPE_STRING && sym->data.string_val) {
                free(sym->data.string_val);
            }
            sym->type = TYPE_BOOL;
            sym->data.bool_val = value ? 1 : 0;
            return sym;
        }
        sym = sym->next;
    }
    // create new
    SymbolNode* newNode = create_symbol_node(name, TYPE_BOOL);
    newNode->data.bool_val = value ? 1 : 0;
    newNode->next = current_scope->symbols;
    current_scope->symbols = newNode;
    return newNode;
}

SymbolNode* put_symbol_function(const char *name, astnode_t *func_ast) {
    if (!current_scope) {
        fprintf(stderr, "Error: No scope to put function '%s' in.\n", name);
        exit(EXIT_FAILURE);
    }
    SymbolNode* sym = current_scope->symbols;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            // If previously something else, overwrite
            if (sym->type == TYPE_STRING && sym->data.string_val) {
                free(sym->data.string_val);
            }
            sym->type = TYPE_FUNCTION;
            sym->data.func_ast = func_ast;
            return sym;
        }
        sym = sym->next;
    }
    // create new
    SymbolNode* newNode = create_symbol_node(name, TYPE_FUNCTION);
    newNode->data.func_ast = func_ast;
    newNode->next = current_scope->symbols;
    current_scope->symbols = newNode;
    return newNode;
}
