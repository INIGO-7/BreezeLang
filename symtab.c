// symtab.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

static SymbolNode *symbol_table = NULL;

void init_symbol_table(void) {
  symbol_table = NULL;
}

SymbolNode *lookup_symbol(const char *name) {
  SymbolNode *current = symbol_table;

  while (current != NULL) {

    if (strcmp(current->name, name) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

SymbolNode *put_symbol_float(const char *name, float value) {
  SymbolNode *node = lookup_symbol(name);

  if (node != NULL) {
    // If it was a string previously, free memory
    if (node->type == TYPE_STRING) {
      free(node->data.string_val);
    }
    // Update existing symbol
    node->type = TYPE_FLOAT;
    node->data.float_val = value;
    return node;
  }

  // Create new symbol
  node = malloc(sizeof(SymbolNode));
  if (node == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(1);
  }

  node->name = strdup(name);
  node->type = TYPE_FLOAT;
  node->data.float_val = value;

  // Add to front of list
  node->next = symbol_table;
  symbol_table = node;

  return node;
}

SymbolNode *put_symbol_int(const char *name, int value) {
  SymbolNode *node = lookup_symbol(name);

  if (node != NULL) {
    // If it was a string previously, free memory
    if (node->type == TYPE_STRING) {
      free(node->data.string_val);
    }
    // Update existing symbol
    node->type = TYPE_INT;
    node->data.int_val = value;
    return node;
  }

  // Create new symbol
  node = malloc(sizeof(SymbolNode));
  if (node == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(1);
  }

  node->name = strdup(name);
  node->type = TYPE_INT;
  node->data.int_val = value;

  // Add to front of list
  node->next = symbol_table;
  symbol_table = node;

  return node;
}

SymbolNode *put_symbol_string(const char *name, const char *value) {
  SymbolNode *node = lookup_symbol(name);

  if (node != NULL) {
    // If it was previously a string, free the old string
    if (node->type == TYPE_STRING) {
      free(node->data.string_val);
    }
    node->type = TYPE_STRING;
    node->data.string_val = strdup(value);
    return node;
  }

  node = malloc(sizeof(SymbolNode));
  if (node == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(1);
  }

  node->name = strdup(name);
  node->type = TYPE_STRING;
  node->data.string_val = strdup(value);

  // Add to front of list
  node->next = symbol_table;
  symbol_table = node;

  return node;
}

// In symtab.c, modify put_symbol_bool:
SymbolNode *put_symbol_bool(const char *name, int value) {

  // First try to find existing symbol
  SymbolNode *node = lookup_symbol(name);

  if (node != NULL) {
    // Update existing symbol
    node->type = TYPE_BOOL;
    node->data.int_val = value ? 1 : 0;
    return node;
  }

  // Create new symbol
  node = malloc(sizeof(SymbolNode));
  if (node == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(1);
  }

  node->name = strdup(name);
  node->type = TYPE_BOOL;
  node->data.int_val = value ? 1 : 0;

  // Add to front of list
  node->next = symbol_table;
  symbol_table = node;

  return node;
}

void free_symbol_table(void) {
  SymbolNode *current = symbol_table;
  while (current != NULL) {
    SymbolNode *next = current->next;
    if (current->type == TYPE_STRING) {
      free(current->data.string_val);
    }
    free(current->name);
    free(current);
    current = next;
  }
  symbol_table = NULL;
}
