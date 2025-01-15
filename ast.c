#include "ast.h"
#include "symtab.h"
#include <stdbool.h>
#include <string.h>

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
        case NODE_ID: printf("ID: %s\n", node->val.id); break;
        case NODE_ASSIGN: printf("ASSIGN\n"); break;
        case NODE_ADD: printf("ADD\n"); break;
        case NODE_SUB: printf("SUB\n"); break;
        case NODE_MUL: printf("MUL\n"); break;
        case NODE_DIV: printf("DIV\n"); break;
        case NODE_EXP: printf("EXP\n"); break;
        case NODE_PRINT: printf("PRINT\n"); break;
        case NODE_BOOL_OP: printf("BOOL_OP\n"); break;
        case NODE_BOOL: printf("BOOL: %s\n", node->val.boolean ? "true" : "false"); break;
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
                float value = evaluate_expr(node->child[0]);
                put_symbol(node->val.id, value);
            }
            break;

        case NODE_PRINT:
            // Evaluate and print the expression
            if (node->child[0]) {
                float value = evaluate_expr(node->child[0]);
                printf("%g\n", value);
            }
            break;

        default:
            // For other nodes, evaluate as expression
            evaluate_expr(node);
            break;
    }
}

static float evaluate_expr(astnode_t *node) {
    if (!node) return false;

    float left, right;
    SymbolNode *symbol;

    switch (node->type) {
        case NODE_NUM:
            return node->val.num;

        case NODE_ID:
            symbol = lookup_symbol(node->val.id);
            if (!symbol) {
                fprintf(stderr, "Error: Undefined variable '%s'\n", node->val.id);
                exit(EXIT_FAILURE);
            }
            return symbol->value;

        case NODE_ADD:
            left = evaluate_expr(node->child[0]);
            right = evaluate_expr(node->child[1]);
            return left + right;

        case NODE_SUB:
            left = evaluate_expr(node->child[0]);
            right = evaluate_expr(node->child[1]);
            return left - right;

        case NODE_MUL:
            left = evaluate_expr(node->child[0]);
            right = evaluate_expr(node->child[1]);
            return left * right;

        case NODE_DIV:
            left = evaluate_expr(node->child[0]);
            right = evaluate_expr(node->child[1]);
            if (right == 0) {
                fprintf(stderr, "Error: Division by zero\n");
                exit(EXIT_FAILURE);
            }
            return left / right;

        case NODE_EXP:
            left = evaluate_expr(node->child[0]);
            right = evaluate_expr(node->child[1]);
            return pow(left, right);

        case NODE_BOOL:
            return node->val.boolean ? false : true;

        case NODE_BOOL_OP:
            left = evaluate_expr(node->child[0]);
            right = evaluate_expr(node->child[1]);
            
            if (strcmp(node->val.bool_op_type, "and") == 0)
                return (left && right) ? true : false;
            else if (strcmp(node->val.bool_op_type, "or") == 0)
                return (left || right) ? true : false;
            else if (strcmp(node->val.bool_op_type, "not") == 0)
                return (!left) ? true : false;
            else if (strcmp(node->val.bool_op_type, "eq") == 0)
                return (left == right) ? true : false;
            else if (strcmp(node->val.bool_op_type, "neq") == 0)
                return (left != right) ? true : false;
            else if (strcmp(node->val.bool_op_type, "lt") == 0)
                return (left < right) ? true : false;
            else if (strcmp(node->val.bool_op_type, "le") == 0)
                return (left <= right) ? true : false;
            else if (strcmp(node->val.bool_op_type, "gt") == 0)
                return (left > right) ? true : false;
            else if (strcmp(node->val.bool_op_type, "ge") == 0)
                return (left >= right) ? true : false;

        default:
            fprintf(stderr, "Error: Unknown node type in evaluation\n");
            exit(EXIT_FAILURE);
    }
}
