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

// Handle float type
SymbolNode *put_symbol_float(const char *name, float val) {
    // Check if symbol exists
    SymbolNode *node = lookup_symbol(name);
    
    if (node != NULL) {
        // If it was a string previously, free Memory
        if (node->type == TYPE_STRING) {
            free(node->val.string_val);
        }
        // Update existing symbol
        node->type = TYPE_FLOAT;
        node->val.num_val = val;
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
    node->val.num_val = val;
    
    // Add to front of list
    node->next = symbol_table;
    symbol_table = node;
    
    return node;
}

// Handle integer type
SymbolNode *put_symbol_int(const char *name, int val) {
    // Check if symbol exists
    SymbolNode *node = lookup_symbol(name);
    
    if (node != NULL) {
        // If it was a string previously, free Memory
        if (node->type == TYPE_STRING) {
            free(node->val.string_val);
        }
        // Update existing symbol
        node->type = TYPE_INT;
        node->val.num_val = val;
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
    node->val.num_val = val;
    
    // Add to front of list
    node->next = symbol_table;
    symbol_table = node;
    
    return node;
}

// Handle string type
SymbolNode *put_symbol_string(const char *name, const char *val) {
    SymbolNode *node = lookup_symbol(name);
    
    if (node != NULL) {
        // If it was previously a string, free the old string
        if (node->type == TYPE_STRING) {
            free(node->val.string_val);
        }
        node->type = TYPE_STRING;
        node->val.string_val = strdup(val);
        return node;
    }
    
    node = malloc(sizeof(SymbolNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    
    node->name = strdup(name);
    node->type = TYPE_STRING;
    node->val.string_val = strdup(val);
    
   // Add to front of list
    node->next = symbol_table;
    symbol_table = node;
    
    return node;
}

// Add this function to properly free memory
void free_symbol_table(void) {
    SymbolNode *current = symbol_table;
    while (current != NULL) {
        SymbolNode *next = current->next;
        if (current->type == TYPE_STRING) {
            free(current->val.string_val);
        }
        free(current->name);
        free(current);
        current = next;
    }
    symbol_table = NULL;
}
