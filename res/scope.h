#ifndef SCOPE_H
#define SCOPE_H

#include "symtab.h"

/**
 * Each scope has its own linked list of SymbolNodes, plus
 * a pointer to the parent scope. This allows lexical scoping.
 */
typedef struct Scope {
    SymbolNode *symbols;      // Linked list of symbols in this scope
    struct Scope *parent;     // Next scope on the stack (the "previous" scope)
} Scope;

// Initialize the scope stack (called once at the beginning).
void init_scopes(void);

// Push a new scope onto the scope stack.
void push_scope(void);

// Pop the top scope (freeing its symbols) and restore the previous scope.
void pop_scope(void);

/**
 * Look up a symbol (variable or function) by name, starting
 * in the top scope and going outward until found or we run out of scopes.
 * Returns NULL if not found.
 */
SymbolNode* lookup_symbol(const char *name);

/**
 * The put_symbol_* functions always insert (or update) the symbol
 * in the current top scope.
 */
SymbolNode* put_symbol_int(const char *name, int value);
SymbolNode* put_symbol_float(const char *name, float value);
SymbolNode* put_symbol_bool(const char *name, int value);
SymbolNode* put_symbol_string(const char *name, const char *value);
SymbolNode* put_symbol_function(const char *name, astnode_t *func_ast);

#endif
