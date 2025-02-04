// symtab.h
#ifndef SYMTAB_H
#define SYMTAB_H

#define MAXCHILDREN 50

// Implement different Types
typedef enum {
  TYPE_FLOAT,
  TYPE_INT,
  TYPE_STRING,
  TYPE_BOOL,
  TYPE_FUNCTION
} ValueType;

typedef struct {
  ValueType type;
  union {
    float float_val;
    int int_val;
    char* str_val;
    int bool_val;
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
  NODE_INT,
  NODE_FLOAT,
  NODE_ID,
  NODE_PRINT,
  NODE_BOOL_OP,
  NODE_BOOL,
  NODE_STRING,
  NODE_WHILE,
  NODE_FOR,
  NODE_IF,
  NODE_IFELSE,
  NODE_FUNC,
  NODE_FUNCCALL,
  NODE_FUNCRET,
  NODE_READ,
  NODE_INDEX,
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

typedef union SymbolData {
  float float_val;
  int int_val;
  int bool_val;
  char* string_val;
  astnode_t *func_ast;
} SymbolData;

typedef struct SymbolNode {
  char *name;           // Variable name
  ValueType type;       // Type of the value
  SymbolData data;
  struct SymbolNode *next;
} SymbolNode;

// Initialize the symbol table
void init_symbol_table(void);

// Look up a symbol by name
SymbolNode *lookup_symbol(const char *name);

// Insert or update different symbols
SymbolNode *put_symbol_int(const char *name, int value);
SymbolNode *put_symbol_float(const char *name, float value);
SymbolNode *put_symbol_string(const char *name, const char *value);
SymbolNode *put_symbol_bool(const char *name, int value);
SymbolNode *put_symbol_function(const char *name, astnode_t *func_ast);

// Print a symbol's value
void print_symbol(const char *name);

// Free the table
void free_symbol_table(void);

#endif
