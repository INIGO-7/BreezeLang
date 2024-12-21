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

SymbolNode *put_symbol(const char *name, int value) {
    // Check if symbol exists
    SymbolNode *node = lookup_symbol(name);
    
    if (node != NULL) {
        // Update existing symbol
        node->value = value;
        return node;
    }
    
    // Create new symbol
    node = malloc(sizeof(SymbolNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    
    node->name = strdup(name);
    node->value = value;
    
    // Add to front of list
    node->next = symbol_table;
    symbol_table = node;
    
    return node;
}

void print_symbol(const char *name) {
    SymbolNode *node = lookup_symbol(name);
    if (node != NULL) {
        printf("%d", node->value);  // Just print the value
    } else {
        fprintf(stderr, "Error: Undefined variable '%s'\n", name);
    }
}
