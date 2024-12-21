%{
#include <stdio.h>
#include <stdlib.h>
#include "symtab.h"

int yylex(void);
void yyerror(const char *s);
%}

%union {
    int number;
    char* string;
}

%token <number> NUMBER
%token <string> IDENTIFIER
%token PRINT ASSIGN SEMICOLON
%token PLUS MINUS MUL DIV
%token OPENPAR CLOSEPAR
%type <number> expr

%left PLUS MINUS
%left MUL DIV

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
            | PRINT expr SEMICOLON { printf("%d\n", $2); }
            ;

expr        : expr PLUS expr    { $$ = $1 + $3; }
            | expr MINUS expr   { $$ = $1 - $3; }
            | expr MUL expr     { $$ = $1 * $3; }
            | expr DIV expr     { 
                if ( $3 == 0 )  {
                    fprintf( stderr, "Error: Dvision by zero\n" );
                    exit( EXIT_FAILURE );
                }   
                $$ = $1 / $3; 
            }
            | NUMBER            { $$ = $1; }
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
            ;

%%

int main(void) {
    yyparse();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
