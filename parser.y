%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);

%}

%union {
    int iVal;     // For integer values
    float fVal;   // For float values
    char* sVal;   // For string values (identifiers)
    int type; ;   // for representing types (INT, FLOAT...)
}

%token <iVal> NUMBER
%token <fVal> FLOAT
%token <sVal> IDENTIFIER
%token IF ELSE WHILE
%token PLUS MINUS MUL DIV ASSIGN
%token INT FLOAT_TYPE SEMICOLON

%type <iVal> expr term factor
%type <type> type /* Declaring 'type' to use 'type' field from %union */
%%

program     : statements
            ;

statements  : statement statements
            | /* empty */
            ;

statement   : expr SEMICOLON
            | IF expr '{' statements '}' ELSE '{' statements '}'
            | WHILE expr '{' statements '}'
            | type IDENTIFIER ASSIGN expr SEMICOLON  /* Match the 'int' keyword */
            | IDENTIFIER ASSIGN expr SEMICOLON
            ;

type        : INT         { $$ = INT; }  /* Return the type */
            | FLOAT_TYPE  { $$ = FLOAT_TYPE; }  /* Return the type for floats */
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
    yyparse();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
