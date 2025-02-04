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
  NODE_SLICE,
  NODE_STRLEN,
  NODE_BREAK,
  NODE_CONTINUE,
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
  enum NodeType type;                      
  union {
    int num;              // For NODE_INT
    float dec;            // For NODE_FLOAT
    char *id;             // For NODE_ID, function names, etc.
    char *str;            // For NODE_STRING
    int boolean;          // For NODE_BOOL
    enum BoolOpType bool_op; // For NODE_BOOL_OP
  } data;
  struct astnode *child[MAXCHILDREN];
} astnode_t;

// Symbol data for each variable or function
typedef union SymbolData {
  float float_val;
  int int_val;
  int bool_val;
  char* string_val;
  astnode_t *func_ast;  // For storing function definitions
} SymbolData;

typedef struct SymbolNode {
  char *name;           // Variable or function name
  ValueType type;       // e.g., TYPE_INT, TYPE_FLOAT, etc.
  SymbolData data;
  struct SymbolNode *next;
} SymbolNode;

#endif
