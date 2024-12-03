#include <stdio.h>
#include "parser.tab.h"

extern int yylex(void);
extern int yyparse(void);

int main(void) {
    printf("Enter code for your cool language:\n");
    yyparse();
    return 0;
}
