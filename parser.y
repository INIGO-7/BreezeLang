%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "symtab.h"

int yylex(void);
void yyerror(const char *s);
%}

%union {
    int number;
    float decimal; 
    char* string;
}

%token <number> NUMBER
%token <decimal> DECIMAL
%token <string> IDENTIFIER
%token PRINT ASSIGN SEMICOLON
%token PLUS MINUS MUL DIV EXP
%token OPENPAR CLOSEPAR

/* Declare types for our new non-terminals */
%type <decimal> expr term factor

/* Operator precedence and associativity */
%left PLUS MINUS
%left MUL DIV EXP

%%

program     : { init_symbol_table(); } statements
            ;

statements  : statement statements
            | /* empty */
            ;

statement   : IDENTIFIER ASSIGN expr SEMICOLON {
                put_symbol($1, $3);
                free($1);
            }
            | PRINT expr SEMICOLON { 
                printf("%g\n", $2); 
            }
            ;

expr        : expr PLUS term    { $$ = $1 + $3; }
            | expr MINUS term   { $$ = $1 - $3; }
            | term              { $$ = $1; }
            ;

term        : term MUL factor   { $$ = $1 * $3; }
            | term DIV factor   { 
                if ($3 == 0) {
                    fprintf(stderr, "Error: Division by zero\n");
                    exit(EXIT_FAILURE);
                }
                $$ = $1 / $3; 
            }
            | term EXP factor   { $$ = pow($1, $3); }
            | factor            { $$ = $1; }
            ;

factor      : NUMBER            { $$ = $1; }
            | DECIMAL           { $$ = $1; }
            | IDENTIFIER        {
                SymbolNode *node = lookup_symbol($1);
                if (node == NULL) {
                    yyerror("Undefined variable");
                    $$ = 0;
                } else {
                    $$ = node->value;
                }
                free($1);
            }
            | OPENPAR expr CLOSEPAR   { $$ = $2; }
            | MINUS factor            { $$ = -$2; }  /* Handle unary minus */
            ;

%%

int main(void) {
    yyparse();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
