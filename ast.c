#include "ast.h"
#include <stdbool.h>
#include <string.h>
#include <math.h>

// Helper functions' implementation
Value create_float_value(float f) {
  Value v;
  v.type = TYPE_FLOAT;
  v.data.float_val = f;
  return v;
}

Value create_int_value(int i) {
  Value v;
  v.type = TYPE_INT;
  v.data.int_val = i;
  return v;
}

Value create_str_value(const char* s) {
  Value v;
  v.type = TYPE_STRING;
  v.data.str_val = strdup(s);
  return v;
}

// Create a new AST node
astnode_t *astnode_new(int type) {
  astnode_t *node = calloc(1, sizeof(astnode_t));
  if (!node) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }
  node->type = type;
  return node;
}

// Add a child node to the parent at a specific index
void astnode_add_child(astnode_t *parent, astnode_t *child, int index) {
  if (index < 0 || index >= MAXCHILDREN) {
    fprintf(stderr, "Error: Invalid child index\n");
    exit(EXIT_FAILURE);
  }
  parent->child[index] = child;
}

// Print the AST (for debugging)
void print_ast(astnode_t *node, int depth) {
  if (!node) return;

  // Indent based on depth
  for (int i = 0; i < depth; i++) printf("  ");

  // Print node type and value
  switch (node->type) { 
    case NODE_NUM: printf("NUM: %d\n", node->val.num); break;
    case NODE_DEC: printf("DEC: %f\n", node->val.decimal); break;
    case NODE_ASSIGN: printf("ASSIGN\n"); break;
    case NODE_ADD: printf("ADD\n"); break;
    case NODE_SUB: printf("SUB\n"); break;
    case NODE_MUL: printf("MUL\n"); break;
    case NODE_DIV: printf("DIV\n"); break;
    case NODE_EXP: printf("EXP\n"); break;
    case NODE_PRINT: printf("PRINT\n"); break;
    case NODE_BOOL_OP: printf("BOOL_OP\n"); break;
    case NODE_BOOL: printf("BOOL: %s\n", node->val.boolean ? "true" : "false"); break;
    case NODE_STRING: printf("STRING: %s\n", node->val.str); break; 
    default: printf("UNKNOWN NODE\n");
  }

  // Recursively print children
  for (int i = 0; i < MAXCHILDREN; i++) {
    print_ast(node->child[i], depth + 1);
  }
}

// Free the AST recursively
void free_ast(astnode_t *node) {
  if (!node) return;

  // Free children
  for (int i = 0; i < MAXCHILDREN; i++) {
    if (!node->child[i]) break;
    free_ast(node->child[i]);
  }

  // Free identifier and boolean operation type strings
  if (node->type == NODE_ID && node->val.id) {
    free(node->val.id);
  } else if (node->type == NODE_BOOL_OP && node->val.bool_op_type) {
    free(node->val.bool_op_type);
  }

  free(node);
}

// ----------- EVALUATION FUNCTION -----------

static Value evaluate_expr(astnode_t *node);

void evaluate_ast(astnode_t *node) {
  if (!node) {
    fprintf(stderr, "Error: NULL pointer in evaluate_ast.\n");
    exit(EXIT_FAILURE); 
  }

  switch (node->type) {
    case NODE_STMTS:
      // Evaluate all statements in sequence
      for (int i = 0; i < MAXCHILDREN; i++) {
        if (node->child[i]) {
          evaluate_ast(node->child[i]);
        }
      }
      break;

    case NODE_ASSIGN:
      // Evaluate right-hand side and store in symbol table
      Value value = evaluate_expr(node->child[0]);
      switch(value.type) {
        case TYPE_FLOAT:
          put_symbol_float(node->val.id, value.data.float_val);
          break;
        case TYPE_INT:
          put_symbol_int(node->val.id, value.data.int_val);
          break;
        case TYPE_STRING:
          put_symbol_string(node->val.id, value.data.str_val);
          break;
      }
      break;

    case NODE_PRINT:
      // Evaluate and print the expression
      if (node->child[0]) {
        Value value = evaluate_expr(node->child[0]);
        if (value.type == TYPE_FLOAT) printf("%f\n", value.data.float_val);
        else if (value.type == TYPE_INT) printf("%d\n", value.data.int_val);
        else if (value.type == TYPE_STRING) printf("%s\n", value.data.str_val);
      }
      break;

    default:
      // For other nodes, evaluate as expression
      evaluate_expr(node);
      break;
  }
}

static Value evaluate_expr(astnode_t *node) {

  if (!node) {
    fprintf(stderr, "Error: NULL pointer in evaluate_expr.\n");
    exit(EXIT_FAILURE); 
  }

  float result;
  Value left, right;
  SymbolNode *symbol;

  switch (node->type) {
    case NODE_NUM:
      return create_int_value(node->val.num);

    case NODE_DEC:
      return create_float_value(node->val.decimal);

    case NODE_STRING:
      return create_str_value(node->val.str);

    case NODE_ID:
      symbol = lookup_symbol(node->val.id);
      if (!symbol) {
        fprintf(stderr, "Error: Undefined variable '%s'\n", node->val.id);
        exit(EXIT_FAILURE);
      }
      switch (symbol->type) {

        case TYPE_STRING:
          return create_str_value(symbol->data.string_val);
        case TYPE_FLOAT:
          return create_float_value(symbol->data.float_val);
        case TYPE_INT:
          return create_int_value(symbol->data.int_val);

        default:
          fprintf(stderr, "Error, the type of the variable isn't recognized\n");
          exit(EXIT_FAILURE);
      }

    case NODE_ADD:
      left = evaluate_expr(node->child[0]);
      right = evaluate_expr(node->child[1]);

      // Ensure both values are numeric (int or float)
      if ((left.type == TYPE_STRING || right.type == TYPE_STRING)) {
        fprintf(stderr, "Error: Cannot add string values\n");
        exit(EXIT_FAILURE); // Exit or handle the error as appropriate
      }

      // Perform the addition and handle type promotion
      if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {
        float left_val = (left.type == TYPE_FLOAT) ? left.data.float_val : (float)left.data.int_val;
        float right_val = (right.type == TYPE_FLOAT) ? right.data.float_val : (float)right.data.int_val;
        result = left_val + right_val;
        return create_float_value(result);
      } else if (left.type == TYPE_INT && right.type == TYPE_INT) {
        int result = left.data.int_val + right.data.int_val;
        return create_int_value(result);
      } else {
        fprintf(stderr, "Error: Invalid types for addition\n");
        exit(EXIT_FAILURE);
      }

    case NODE_SUB:
      left = evaluate_expr(node->child[0]);
      right = evaluate_expr(node->child[1]);

      if ((left.type == TYPE_STRING || right.type == TYPE_STRING)) {
        fprintf(stderr, "Error: Cannot subtract string values\n");
        exit(EXIT_FAILURE);
      }

      if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {
        float left_val = (left.type == TYPE_FLOAT) ? left.data.float_val : (float)left.data.int_val;
        float right_val = (right.type == TYPE_FLOAT) ? right.data.float_val : (float)right.data.int_val;
        result = left_val - right_val;
        return create_float_value(result);
      } else {
        int result = left.data.int_val - right.data.int_val;
        return create_int_value(result);
      }

    case NODE_MUL:
      left = evaluate_expr(node->child[0]);
      right = evaluate_expr(node->child[1]);

      if ((left.type == TYPE_STRING || right.type == TYPE_STRING)) {
        fprintf(stderr, "Error: Cannot multiply string values\n");
        exit(EXIT_FAILURE);
      }

      if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {
        float left_val = (left.type == TYPE_FLOAT) ? left.data.float_val : (float)left.data.int_val;
        float right_val = (right.type == TYPE_FLOAT) ? right.data.float_val : (float)right.data.int_val;
        result = left_val * right_val;
        return create_float_value(result);
      } else {
        result = left.data.int_val * right.data.int_val;
        return create_int_value((int)result);
      }

    case NODE_DIV:
      left = evaluate_expr(node->child[0]);
      right = evaluate_expr(node->child[1]);

      if ((left.type == TYPE_STRING || right.type == TYPE_STRING)) {
        fprintf(stderr, "Error: Cannot divide string values\n");
        exit(EXIT_FAILURE);
      }

      if ((right.type == TYPE_INT && right.data.int_val == 0) ||
        (right.type == TYPE_FLOAT && right.data.float_val == 0.0)) {
        fprintf(stderr, "Error: Division by zero\n");
        exit(EXIT_FAILURE);
      }

      float left_val = (left.type == TYPE_FLOAT) ? left.data.float_val : (float)left.data.int_val;
      float right_val = (right.type == TYPE_FLOAT) ? right.data.float_val : (float)right.data.int_val;
      result = left_val / right_val;
      return create_float_value(result);

    case NODE_EXP:
      left = evaluate_expr(node->child[0]);
      right = evaluate_expr(node->child[1]);

      if ((left.type == TYPE_STRING || right.type == TYPE_STRING)) {
        fprintf(stderr, "Error: Cannot exponentiate string values\n");
        exit(EXIT_FAILURE);
      }

      float base = (left.type == TYPE_FLOAT) ? left.data.float_val : (float)left.data.int_val;
      float exponent = (right.type == TYPE_FLOAT) ? right.data.float_val : (float)right.data.int_val;
      result = pow(base, exponent);
      return create_float_value(result);

    case NODE_BOOL:
      return create_int_value(node->val.boolean ? 1 : 0);

    case NODE_BOOL_OP:
      left = evaluate_expr(node->child[0]);

      if (strcmp(node->val.bool_op_type, "not") == 0) {
        return create_int_value((!left.data.int_val) ? 1 : 0);
      }

      // Only evaluate right child for binary operations
      right = evaluate_expr(node->child[1]);

      if (strcmp(node->val.bool_op_type, "and") == 0) {
        return create_int_value((left.data.int_val && right.data.int_val) ? 1 : 0);
      } else if (strcmp(node->val.bool_op_type, "or") == 0) {
        return create_int_value((left.data.int_val || right.data.int_val) ? 1 : 0);
      } else if (strcmp(node->val.bool_op_type, "eq") == 0) {
        return create_int_value((left.data.int_val == right.data.int_val) ? 1 : 0);
      } else if (strcmp(node->val.bool_op_type, "neq") == 0) {
        return create_int_value((left.data.int_val != right.data.int_val) ? 1 : 0);
      } else if (strcmp(node->val.bool_op_type, "lt") == 0) {
        return create_int_value((left.data.int_val < right.data.int_val) ? 1 : 0);
      } else if (strcmp(node->val.bool_op_type, "le") == 0) {
        return create_int_value((left.data.int_val <= right.data.int_val) ? 1 : 0);
      } else if (strcmp(node->val.bool_op_type, "gt") == 0) {
        return create_int_value((left.data.int_val > right.data.int_val) ? 1 : 0);
      } else if (strcmp(node->val.bool_op_type, "ge") == 0) {
        return create_int_value((left.data.int_val >= right.data.int_val) ? 1 : 0);
      } else {
        fprintf(stderr, "Error: Unknown boolean operator\n");
        exit(EXIT_FAILURE);
      }

    default:
      fprintf(stderr, "Error: Unknown node type in evaluation\n");
      exit(EXIT_FAILURE);
  }
}
