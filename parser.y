%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);

%}

%union {
    int iVal;
    float fVal;
    char* sVal;
}

%token <iVal> NUMBER
%token <fVal> FLOAT
%token <sVal> IDENTIFIER
%token IF ELSE WHILE
%token PLUS MINUS MUL DIV ASSIGN

%type <iVal> expr term factor

%%

program     : statements
            ;

statements  : statement statements
            | /* empty */
            ;

statement   : expr ';'
            | IF expr '{' statements '}' ELSE '{' statements '}'
            | WHILE expr '{' statements '}'
            | "int" IDENTIFIER ASSIGN expr ';'
            | "float" IDENTIFIER ASSIGN expr ';'
            | IDENTIFIER ASSIGN expr ';'
            ;

expr        : expr PLUS term     { $$ = $1 + $3; }
            | expr MINUS term    { $$ = $1 - $3; }
            | term               { $$ = $1; }
            ;

term        : term MUL factor    { $$ = $1 * $3; }
            | term DIV factor    { $$ = $1 / $3; }
            | factor             { $$ = $1; }
            ;

factor      : IDENTIFIER         { $$ = 0; } /* you can define symbol table here */
            | NUMBER             { $$ = $1; }
            | FLOAT              { $$ = $1; }
            | '(' expr ')'       { $$ = $2; }
            ;

%%

int main(void) {
    printf("Enter your code:\n");
    yyparse();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
