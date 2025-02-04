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

char *process_escapes(const char *input) {
  if (!input) return NULL;

  char *output = malloc(strlen(input) + 1);
  char *dest = output;

  while (*input) {
    if (*input == '\\') {
      input++;
      switch (*input) {
        case 'n': *dest++ = '\n'; break;
        case 't': *dest++ = '\t'; break;
        case '\\': *dest++ = '\\'; break;
        case '"': *dest++ = '"'; break;
        default:  // Handle unknown escapes
          fprintf(stderr, "Warning: Unknown escape \\%c\n", *input);
          *dest++ = *input;
      }
    } else {
      *dest++ = *input;
    }
    input++;
  }
  *dest = '\0';
  return output;
}

Value create_str_value(const char *s) {
  Value v;
  v.type = TYPE_STRING;
  // Validate that input is enclosed in quotes
  if (s[0] != '"' || s[strlen(s)-1] != '"') {
    v.data.str_val = process_escapes(s);
    return v;

  } else {
    // Remove surrounding quotes and process escapes
    char *temp = strdup(s + 1);          // Skip opening "
    temp[strlen(temp)-1] = '\0';             // Remove trailing "

    v.data.str_val = process_escapes(temp);  // Handle \n, \t, etc.
    free(temp);

    return v;
  }
}

Value create_bool_value(int i) {
  Value v;
  v.type = TYPE_BOOL;
  v.data.int_val = i;
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
    case NODE_INT:     printf("NUM: %d\n", node->data.num); break;
    case NODE_FLOAT:     printf("DEC: %f\n", node->data.dec); break;
    case NODE_ASSIGN:  printf("ASSIGN\n"); break;
    case NODE_ADD:     printf("ADD\n"); break;
    case NODE_SUB:     printf("SUB\n"); break;
    case NODE_MUL:     printf("MUL\n"); break;
    case NODE_DIV:     printf("DIV\n"); break;
    case NODE_EXP:     printf("EXP\n"); break;
    case NODE_PRINT:   printf("PRINT\n"); break;
    case NODE_BOOL_OP: printf("BOOL_OP\n"); break;
    case NODE_BOOL:    printf("BOOL: %s\n", node->data.boolean ? "true" : "false"); break;
    case NODE_STRING:  printf("STRING: %s\n", node->data.str); break; 
    case NODE_WHILE:
      printf("WHILE loop\n"); 
      for (int i = 0; i < depth; i++) printf("  ");
      printf("Child node 1 (condition):\n");
      print_ast(node->child[0], depth+1);
      for (int i = 0; i < depth; i++) printf("  ");
      printf("Child node 2 (while's body):\n");
      print_ast(node->child[1], depth+1);
      break; 
    case NODE_FOR:
      printf("FOR loop\n"); 
      for (int i = 0; i < depth; i++) printf("  ");
      printf("Child node 1 (for init):\n");
      print_ast(node->child[0], depth+1);
      for (int i = 0; i < depth; i++) printf("  ");
      printf("Child node 2 (for condition):\n");
      print_ast(node->child[1], depth+1);
      for (int i = 0; i < depth; i++) printf("  ");
      printf("Child node 3 (for update):\n");
      print_ast(node->child[2], depth+1);
      for (int i = 0; i < depth; i++) printf("  ");
      printf("Child node 4 (for body):\n");
      print_ast(node->child[3], depth+1);
      break;
    case NODE_IF:
      printf("IF statement\n"); 
      for (int i = 0; i < depth; i++) printf("  ");
      printf("Child node 1 (condition):\n");
      print_ast(node->child[0], depth+1);
      for (int i = 0; i < depth; i++) printf("  ");
      printf("Child node 2 (if's body):\n");
      print_ast(node->child[1], depth+1);
      break;
    case NODE_IFELSE:
      printf("IF-ELSE statement\n"); 
      for (int i = 0; i < depth; i++) printf("  ");
      printf("Child node 1 (condition):\n");
      print_ast(node->child[0], depth+1);
      for (int i = 0; i < depth; i++) printf("  ");
      printf("Child node 2 (if-else's body):\n");
      print_ast(node->child[1], depth+1);
      break;

    //TODO: Add function nodes!
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

  // Free identifier 
  if (node->type == NODE_ID && node->data.id) {
    free(node->data.id);
  }

  free(node);
}

// ----------- EVALUATION FUNCTION -----------

static Value evaluate_expr(astnode_t *node);
void evaluate_while(astnode_t *node);
void evaluate_for(astnode_t *node);
void evaluate_if(astnode_t *node);
void evaluate_ifelse(astnode_t *node);
void evaluate_func(astnode_t *node);
Value evaluate_funccall(astnode_t *node);

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
      Value value = evaluate_expr(node->child[0]);

      switch(value.type) {
        case TYPE_FLOAT:
          put_symbol_float(node->data.id, value.data.float_val);
          break;
        case TYPE_INT:
          put_symbol_int(node->data.id, value.data.int_val);
          break;
        case TYPE_STRING:
          put_symbol_string(node->data.id, value.data.str_val);
          break;
        case TYPE_BOOL:
          put_symbol_bool(node->data.id, value.data.int_val);
          break;        

        default:
          fprintf(stderr, "Error: assignment's type cannot be recognized. Type is: '%d'.\n", value.type);
          exit(EXIT_FAILURE);
      }
      break;

    case NODE_PRINT:
      if (!node->child[0]) {
        fprintf(stderr, "Error: print node has no arguments\n");
        exit(EXIT_FAILURE);
      }
      
      astnode_t *args = node->child[0];
      for (int i = 0; i < MAXCHILDREN; i++) {
        if (args->child[i]) {
          Value value = evaluate_expr(args->child[i]);
          
          // Handle different types
          if (value.type == TYPE_STRING) {
            // Print string WITHOUT quotes
            printf("%s", value.data.str_val);
          } else if (value.type == TYPE_FLOAT) {
            printf("%f", value.data.float_val);
          } else if (value.type == TYPE_INT) {
            printf("%d", value.data.int_val);
          } else if (value.type == TYPE_BOOL) {
            printf("%s", value.data.int_val ? "true" : "false");
          }
        }
      }
      break;

    case NODE_READ:
      const char* varName = node->data.id;
      printf("What do you want this time? ...\n");
      fflush(stdout);

      char buffer[256];
      if (!fgets(buffer, sizeof(buffer), stdin)) {
          fprintf(stderr, "Error reading input.\n");
          exit(EXIT_FAILURE);
      }
      // Remove trailing newline if present
      char *newline = strchr(buffer, '\n');
      if (newline) *newline = '\0';

      // Always store it as string
      put_symbol_string(varName, buffer);
      break;

    // TODO: Create a input() function-like expr. to use in runtime

    case NODE_WHILE:
      evaluate_while(node);
      break;

    case NODE_FOR:
      evaluate_for(node);
      break;

    case NODE_IF:
      evaluate_if(node);
      break;

    case NODE_IFELSE:
      evaluate_ifelse(node);
      break;

    case NODE_FUNC:
      evaluate_func(node);
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
    case NODE_INT:
      return create_int_value(node->data.num);

    case NODE_FLOAT:
      return create_float_value(node->data.dec);

    case NODE_STRING:
      return create_str_value(node->data.str);

    case NODE_ID:
      symbol = lookup_symbol(node->data.id);
      if (!symbol) {
        fprintf(stderr, "Error: Undefined variable '%s'\n", node->data.id);
        exit(EXIT_FAILURE);
      }

      switch (symbol->type) {

        case TYPE_STRING:
          return create_str_value(symbol->data.string_val);
        case TYPE_FLOAT:
          return create_float_value(symbol->data.float_val);
        case TYPE_INT:
          return create_int_value(symbol->data.int_val);
        case TYPE_BOOL:
          return create_bool_value(symbol->data.int_val);

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
      return create_bool_value(node->data.boolean ? 1 : 0);

    case NODE_BOOL_OP:
      left = evaluate_expr(node->child[0]);

      if (node->data.bool_op == OP_NOT) {
        return create_bool_value((!left.data.bool_val) ? 1 : 0);
      }

      // Only evaluate right child for binary operations
      right = evaluate_expr(node->child[1]);

      if (node->data.bool_op == OP_AND) {
        return create_bool_value((left.data.int_val && right.data.int_val) ? 1 : 0);

      } else if (node->data.bool_op == OP_OR) {
        return create_bool_value((left.data.int_val || right.data.int_val) ? 1 : 0);

      } else if (node->data.bool_op == OP_EQ) {

        if(left.type == TYPE_STRING && right.type == TYPE_STRING) {
          return create_bool_value(
            strcmp(left.data.str_val, right.data.str_val) == 0 ? 
            1 : 0
          );

        } else {
          return create_bool_value((left.data.int_val == right.data.int_val) ? 1 : 0);
        }
      } else if (node->data.bool_op == OP_NEQ) {

        if(left.type == TYPE_STRING && right.type == TYPE_STRING) {
          return create_bool_value(
            strcmp(left.data.str_val, right.data.str_val) == 1 ? 
            1 : 0
          );

        } else {
          return create_bool_value((left.data.int_val == right.data.int_val) ? 0 : 1);
        }
      } else if (node->data.bool_op == OP_LT) { 
        return create_bool_value((left.data.int_val < right.data.int_val) ? 1 : 0);
      } else if (node->data.bool_op == OP_LE) {
        return create_bool_value((left.data.int_val <= right.data.int_val) ? 1 : 0);
      } else if (node->data.bool_op == OP_GT) {
        return create_bool_value((left.data.int_val > right.data.int_val) ? 1 : 0);
      } else if (node->data.bool_op == OP_GE) {
        return create_bool_value((left.data.int_val >= right.data.int_val) ? 1 : 0);
      } else {
        fprintf(stderr, "Error: Unknown boolean operator\n");
        exit(EXIT_FAILURE);
      }

    case NODE_FUNCCALL:
      return evaluate_funccall(node);
      break;

    case NODE_FUNCRET:
      if(node->child[0]) {
        return evaluate_expr(node->child[0]);
      } else {
        fprintf(stderr, "Error: There isn't an expression associated to this return statement.\n");
        exit(EXIT_FAILURE);
      }

    case NODE_INDEX:
      SymbolNode *symbol = lookup_symbol(node->data.id);

      if (!symbol) {
        fprintf(stderr, "Error: Undefined variable '%s'\n", node->data.id);
        exit(EXIT_FAILURE);
      } else if(symbol->type != TYPE_STRING) {
        fprintf(stderr, "Error: indexing is only supported on strings for now.\n");
        exit(EXIT_FAILURE);
      }
      
      const char *str = symbol->data.string_val;
      int length = strlen(str);

      astnode_t *slice = node->child[0];

      if (!slice || slice->type != NODE_SLICE){
        fprintf(stderr, "Error: element inside braces has to be a slice!\n");
        exit(EXIT_FAILURE);
      } else if (!slice->child[0]) {
        fprintf(stderr, "Error: we need at least one element inside the slice!\n");
        exit(EXIT_FAILURE);
      }

      // TODO: Check that slice1 is an int
      int slice1 = evaluate_expr(slice->child[0]).data.int_val;

      if (slice1 < 0 || slice1 >= length) {
          fprintf(stderr, "Error: string index %d out of range (length %d).\n", slice1, length);
          exit(EXIT_FAILURE);
      }

      if (slice->child[1]) {
        // TODO: Check that slice2 is an int
        int slice2 = evaluate_expr(slice->child[1]).data.int_val;
        if (slice2 < 0 || slice2 >= length) {
          fprintf(stderr, "Error: string index %d out of range (length %d).\n", slice2, length);
          exit(EXIT_FAILURE);
        } else if (slice1 > slice2){
          fprintf(stderr, "Error: slice val 1 '%d' shouldn't be greater than slice val 2 '%d'\n", slice1, slice2);
          exit(EXIT_FAILURE);
        }

        int slicelen = slice2 - slice1 + 1;
        char finalStr[slicelen + 1];  // +1 for the terminating null
        
        strncpy(finalStr, str + slice1, slicelen);
        
        // Place terminating null at index slicelen
        finalStr[slicelen] = '\0';
        
        return create_str_value(finalStr);

      } else {
        // Build a new single‐character string
        char singleChar[2];
        singleChar[0] = str[slice1];
        singleChar[1] = '\0';

        return create_str_value(singleChar);
      }

    case NODE_STRLEN:
      symbol = lookup_symbol(node->data.id);
      if (!symbol) {
        fprintf(stderr, "Error: Undefined variable '%s'\n", node->data.id);
        exit(EXIT_FAILURE);
      } else if (symbol->type != TYPE_STRING) {
        fprintf(stderr, "Error: Variable '%s' must be of type string!\n", node->data.id);
        exit(EXIT_FAILURE);
      } else if (symbol->data.string_val == NULL) {
        fprintf(stderr, "Error: Variable '%s' is uninitialized (NULL)\n", node->data.id);
        exit(EXIT_FAILURE);
      }
      return create_int_value(strlen(symbol->data.string_val));

    default:
      fprintf(stderr, "Error: Unknown node type in evaluation. Maybe you should use evaluate_ast() instead of evaluate_expr()? Node type: %d\n", node->type);
      exit(EXIT_FAILURE);
  }
}

Value evaluate_funcbody(astnode_t* node) {
  if (node->type == NODE_STMTS && node) {
    // Evaluate all statements in sequence
    for (int i = 0; i < MAXCHILDREN; i++) {
      if (!node->child[i]) continue;
      if (node->child[i]->type == NODE_FUNCRET) {
        // When a return is reached, stop funcbody evaluation and 
        // yield return's associated expression
          return evaluate_expr(node->child[i]);
      } else {
        // Otherwise, evaluate this existing child and move on.
        evaluate_ast(node->child[i]);
      }
    }
  } else {
    fprintf(stderr, "Error: Invalid function body.\n");
    exit(EXIT_FAILURE); 
  }
  // return 0 if no return was found
  return create_int_value(0);
}

void evaluate_loop(astnode_t* node){
  if (node->type == NODE_STMTS && node) {
    // Evaluate all statements in sequence
    for (int i = 0; i < MAXCHILDREN; i++) {
      if (!node->child[i]) continue;
      if (node->child[i]->type == NODE_BREAK) {
        // When break is reached, terminate loop execution
        break;
      } else if(node->child[i]->type == NODE_CONTINUE) {
        // When continue is reached, go to next loop
        continue;
      } else {
        // Otherwise, evaluate this existing child and move on.
        evaluate_ast(node->child[i]);
      }
    }
  } else {
    fprintf(stderr, "Error: Invalid function body.\n");
    exit(EXIT_FAILURE); 
  }
}

void evaluate_while(astnode_t *node) {
  if (!node || node->type != NODE_WHILE) {
    fprintf(stderr, "Error: Invalid while loop node\n");
    exit(EXIT_FAILURE);
  }

  astnode_t *condition = node->child[0];
  astnode_t *body = node->child[1];

  if (!condition || !body) {
    fprintf(stderr, "Error: While loop missing condition or body\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    Value cond_value = evaluate_expr(condition);

    if (cond_value.type != TYPE_BOOL) {
      fprintf(stderr, "Error: While loop condition must evaluate to a boolean\n");
      exit(EXIT_FAILURE);
    }
    /* When the condition is not valid anymore, we exit the loop */
    if (!cond_value.data.int_val) {
      break;
    }
    evaluate_loop(body);
  }
}

void evaluate_for(astnode_t *node) {
  if (!node || node->type != NODE_FOR) {
    fprintf(stderr, "Error: Invalid for loop node\n");
    exit(EXIT_FAILURE);
  }

  astnode_t *init =       node->child[0];
  astnode_t *condition =  node->child[1];
  astnode_t *update =     node->child[2];
  astnode_t *body =       node->child[3];

  if (!init || !condition || !update || !body) {
    fprintf(stderr, "Error: For loop missing a fundamental building block (init | conditiion | update | body)\n");
    exit(EXIT_FAILURE);
  }

  evaluate_ast(init);

  while(1) {
    Value cond_value = evaluate_expr(condition);

    if (cond_value.type != TYPE_BOOL) {
      fprintf(stderr, "Error: For loop condition must evaluate to a boolean\n");
      exit(EXIT_FAILURE);
    }
    /* When the condition is not valid anymore, we exit the loop */
    if (!cond_value.data.int_val) {
      break;
    }
    evaluate_loop(body);
    evaluate_ast(update);
  }
}

void evaluate_if(astnode_t *node) {
  if (!node || node->type != NODE_IF) {
    fprintf(stderr, "Error: Invalid if statement node\n");
    exit(EXIT_FAILURE);
  }

  astnode_t *condition = node->child[0];
  astnode_t *body = node->child[1];

  if (!condition || !body) {
    fprintf(stderr, "Error: If statement missing condition or body\n");
    exit(EXIT_FAILURE);
  }

  Value cond_value = evaluate_expr(condition);

  if (cond_value.type != TYPE_BOOL) {
    fprintf(stderr, "Error: If statement condition must evaluate to a boolean\n");
    exit(EXIT_FAILURE);
  }

  if (cond_value.data.int_val) {
    evaluate_ast(body);
  }
}

void evaluate_ifelse(astnode_t *node) {
  if (!node || node->type != NODE_IFELSE) {
    fprintf(stderr, "Error: Invalid if-else statement node\n");
    exit(EXIT_FAILURE);
  }

  astnode_t *condition = node->child[0];

  if (!condition) {
    fprintf(stderr, "Error: If-else statement missing condition\n");
    exit(EXIT_FAILURE);
  }

  Value cond_value = evaluate_expr(condition);

  if (cond_value.type != TYPE_BOOL) {
    fprintf(stderr, "Error: If statement condition must evaluate to a boolean\n");
    exit(EXIT_FAILURE);
  }
  astnode_t *body;

  if (cond_value.data.int_val) {
    body = node->child[1];
  } else {
    body = node->child[2];
  }

  if (!body) {
    fprintf(stderr, "Error: If-else statement missing body\n");
    exit(EXIT_FAILURE);
  }
  evaluate_ast(body);
}

void evaluate_func(astnode_t * node) {
  const char *funcName = node->data.id;

  SymbolNode *symbol = lookup_symbol(funcName);
  if (symbol) {
    fprintf(stderr, "Error: this function has already been defined in the script!\n");
    exit(EXIT_FAILURE);
  } else {
    put_symbol_function(funcName, node);
  }
}

Value evaluate_funccall(astnode_t * node) {
  // 1. Look up the function by name
  SymbolNode *fnSymbol = lookup_symbol(node->data.id);
  if (!fnSymbol || fnSymbol->type != TYPE_FUNCTION) {
    fprintf(stderr, "Error: '%s' is not defined as a function.\n", node->data.id);
    exit(EXIT_FAILURE);
  }

  // 2. Retrieve the function AST
  astnode_t *funcDefNode = fnSymbol->data.func_ast; // This is the NODE_FUNCDEF

  astnode_t *paramList = funcDefNode->child[0];
  astnode_t *funcBody  = funcDefNode->child[1];

  // 3. Evaluate arguments (the child[0] of the call node)
  astnode_t *argListNode = node->child[0]; // This is the arg_list (NODE_STMTS)
  // We'll gather them in an array or a small vector
  Value argValues[MAXCHILDREN] = {0};
  int argCount = 0;
  for (int i = 0; i < MAXCHILDREN; i++) {
    if (!argListNode->child[i]) break;
    argValues[i] = evaluate_expr(argListNode->child[i]);
    argCount++;
  }

  // 4. Bind arguments to parameters
  for (int i = 0; i < argCount; i++) {
    astnode_t *paramNode = paramList->child[i];
    if (!paramNode) {
      fprintf(stderr, "Error: Too many arguments for function '%s'.\n", node->data.id);
      exit(EXIT_FAILURE);
    }
    const char *paramName = paramNode->data.id;
    Value v = argValues[i];
    // put_symbol_xxx paramName with v
    switch (v.type) {
      case TYPE_INT:
        put_symbol_int(paramName, v.data.int_val);
        break;
      case TYPE_FLOAT:
        put_symbol_float(paramName, v.data.float_val);
        break;
      case TYPE_STRING:
        put_symbol_string(paramName, v.data.str_val);
        break;
      case TYPE_BOOL:
        put_symbol_bool(paramName, v.data.int_val);
        break;

      default:
        fprintf(stderr, "Error: argument's type cannot be recognized. Type is: '%d'.\n", v.type);
        exit(EXIT_FAILURE);
    }
  }

  // TODO: Check if there are leftover parameters that didn't get an argument
  // or leftover arguments with no matching param

  // 5. Evaluate the function body, storing the optional return expr.
  Value ret = evaluate_funcbody(funcBody);

  // 6. Handle return statement (if available)
  if(ret.data.int_val != 0) {
    return ret;
  } else {
    // This will be returned when no return was found,
    // and when the actual return value is 0.
    return create_int_value(0);
  }
}
