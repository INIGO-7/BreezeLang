// symtab.h
#ifndef SYMTAB_H
#define SYMTAB_H

// Implement different Types
typedef enum {
  TYPE_FLOAT,
  TYPE_INT,
  TYPE_STRING
} ValueType;

typedef struct {
  ValueType type;
  union {
    float float_val;
    int int_val;
    char* str_val;
  } data;
} Value

typedef struct SymbolNode {
    char *name;           // Variable name
    ValueType type;       // Type of the value
    Value val;         // The value itself
    struct SymbolNode *next;
} SymbolNode;

// Initialize the symbol table
void init_symbol_table(void);

// Look up a symbol by name
SymbolNode *lookup_symbol(const char *name);

// Insert or update different symbols
SymbolNode *put_symbol_num(const char *name, const float value);
SymbolNode *put_symbol_string(const char *name, const char *value);

// Print a symbol's value
void print_symbol(const char *name);

// Free the table
void free_symbol_table(void);

#endif
