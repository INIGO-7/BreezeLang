#include "ast.h"
#include <stdbool.h>
#include <string.h>

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

Value create_string_value(const char* s) {
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

static float evaluate_expr(astnode_t *node);

void evaluate_ast(astnode_t *node) {
  if (!node) return;

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
      if (node->child[0]) {
        Value value = evaluate_expr(node->child[0]);
        if (value.type == TYPE_FLOAT) put_symbol_float(node->val.id, value->data);
        else if (value.type == TYPE_INT) put_symbol_int(node->val.id, value->data);
        else put_symbol_char(node->val.id, value->data);
      }
      break;

    case NODE_PRINT:
      // Evaluate and print the expression
      if (node->child[0]) {
        Value value = evaluate_expr(node->child[0]);
        if (value.type == (TYPE_FLOAT || TYPE_INT)) printf("%g\n", value->data);
        else printf("%s\n", value->data);
      }
      break;

    default:
      // For other nodes, evaluate as expression
      evaluate_expr(node);
      break;
  }
}

static Value evaluate_expr(astnode_t *node) {
  if (!node) return false;

  float result;
  Value left, right;
  SymbolNode *symbol;

  switch (node->type) {
    case NODE_NUM:
      return create_int_value(node->val.num)

    case NODE_DEC:
      return create_float_value(node->val.decimal);

    case NODE_STRING:
      return create_string_value(node->val.str);

    case NODE_ID:
      symbol = lookup_symbol(node->val.id);
      if (!symbol) {
        fprintf(stderr, "Error: Undefined variable '%s'\n", node->val.id);
        exit(EXIT_FAILURE);
      }
      return symbol->val;

    case NODE_ADD:
      left = evaluate_expr(node->child[0]);
      right = evaluate_expr(node->child[1]);
      result = left->data + right->data;
      
      // Check if result has any decimal part
      if (fmod(result, 1.0) == 0.0) {
          return create_int_value((int)result);
      } else {
         return create_float_value(result);
      }

    case NODE_SUB:
      left = evaluate_expr(node->child[0]);
      right = evaluate_expr(node->child[1]);
      result = left->data - right->data;
      
      // Check if result has any decimal part
      if (fmod(result, 1.0) == 0.0) {
          return create_int_value((int)result);
      } else {
         return create_float_value(result);
      }

    case NODE_MUL:
      left = evaluate_expr(node->child[0]);
      right = evaluate_expr(node->child[1]);
      result = left->data * right->data;
      
      // Check if result has any decimal part
      if (fmod(result, 1.0) == 0.0) {
          return create_int_value((int)result);
      } else {
         return create_float_value(result);
      }

    case NODE_DIV:
      left = evaluate_expr(node->child[0]);
      right = evaluate_expr(node->child[1]);
      if (right->data == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        exit(EXIT_FAILURE);
      }
      result = left->data / right->data;
      
      // Check if result has any decimal part
      if (fmod(result, 1.0) == 0.0) {
          return create_int_value((int)result);
      } else {
         return create_float_value(result);
      }

    case NODE_EXP:
      left = evaluate_expr(node->child[0]);
      right = evaluate_expr(node->child[1]);
      result = pow(left->data, right->data);
      
      // Check if result has any decimal part
      if (fmod(result, 1.0) == 0.0) {
          return create_int_value((int)result);
      } else {
         return create_float_value(result);
      }

    case NODE_BOOL:
      return create_int_value(
        node->val.boolean ? false : true
      );

    case NODE_BOOL_OP:
      left = evaluate_expr(node->child[0]);
      right = evaluate_expr(node->child[1]);

      if (strcmp(node->val.bool_op_type, "and") == 0)
        result = (left && right) ? true : false;
        return create_int_value((int)result);
      else if (strcmp(node->val.bool_op_type, "or") == 0)
        result = (left || right) ? true : false;
        return create_int_value((int)result);
      else if (strcmp(node->val.bool_op_type, "not") == 0)
        result = (!left) ? true : false;
        return create_int_value((int)result);
      else if (strcmp(node->val.bool_op_type, "eq") == 0)
        result = (left == right) ? true : false;
        return create_int_value((int)result);
      else if (strcmp(node->val.bool_op_type, "neq") == 0)
        result = (left != right) ? true : false;
        return create_int_value((int)result);
      else if (strcmp(node->val.bool_op_type, "lt") == 0)
        result = (left < right) ? true : false;
        return create_int_value((int)result);
      else if (strcmp(node->val.bool_op_type, "le") == 0)
        result = (left <= right) ? true : false;
        return create_int_value((int)result);
      else if (strcmp(node->val.bool_op_type, "gt") == 0)
        result = (left > right) ? true : false;
        return create_int_value((int)result);
      else if (strcmp(node->val.bool_op_type, "ge") == 0)
        result = (left >= right) ? true : false;
        return create_int_value((int)result);

    default:
      fprintf(stderr, "Error: Unknown node type in evaluation\n");
      exit(EXIT_FAILURE);
  }
}
