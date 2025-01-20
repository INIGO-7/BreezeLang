#include <stdio.h>
#include "common_lib.h"
#include "ast.h"
#include "parser.tab.h"

extern int yyparse(void);
extern astnode_t *root_ast;
extern FILE *yyin;
extern int yydebug;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
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

    /*
    printf("\nScript's Abstract Syntax Tree:\n");
    print_ast(root_ast, 0);
    */

    printf("\nBreezeLang script output: \n");
    evaluate_ast(root_ast);
    free_ast(root_ast);
    return 0;
}
