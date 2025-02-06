#include <stdio.h>
#include <string.h> // For strcmp
#include "common_lib.h"
#include "ast.h"
#include "parser.tab.h"

extern int yyparse(void);
extern astnode_t *root_ast;
extern FILE *yyin;
extern int yydebug;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-v] <input_file>\n", argv[0]);
        return 1;
    }

    int verbose = 0; // Flag to track if -v is present
    char *input_file = NULL;

    // Process command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else {
            input_file = argv[i];
        }
    }

    if (!input_file) {
        fprintf(stderr, "Error: No input file provided.\n");
        fprintf(stderr, "Usage: %s [-v] <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(input_file, "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    yydebug = 0;
    yyin = file; // Set the input file for Flex
    if (yyparse() == 0) {
        printf("Parsing completed successfully.\n");
    } else {
        fprintf(stderr, "Parsing failed.\n");
    }

    fclose(file);

    if (verbose) {
        printf("\nScript's Abstract Syntax Tree:\n");
        print_ast(root_ast, 0);
    }

    printf("\nBreezeLang script output: \n");
    evaluate_ast(root_ast);
    free_ast(root_ast);
    return 0;
}
