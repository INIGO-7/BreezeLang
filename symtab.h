// symtab.h
#ifndef SYMTAB_H
#define SYMTAB_H

// A simple symbol table entry
typedef struct SymbolNode {
    char *name;           // Variable name
    float value;           // Variable value
    struct SymbolNode *next;
} SymbolNode;

// Initialize the symbol table
void init_symbol_table(void);

// Look up a symbol by name
SymbolNode *lookup_symbol(const char *name);

// Insert or update a symbol
SymbolNode *put_symbol(const char *name, float value);

// Print a symbol's value
void print_symbol(const char *name);

#endif
