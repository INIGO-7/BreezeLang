%{
#include "common_lib.h"
#include "symtab.h"

int yylex(void);
void yyerror(const char *s);
%}

%union {
    int number;
    float decimal; 
    char* string;
    bool boolean;
}

%token <number> NUMBER
%token <decimal> DECIMAL
%token <string> IDENTIFIER
%token TRUE FALSE
%token AND OR NOT
%token EQ NEQ LT GT LE GE
%token PRINT ASSIGN SEMICOLON
%token PLUS MINUS MUL DIV EXP
%token OPENPAR CLOSEPAR

/* Declare types for our new non-terminals */
%type <boolean> bool_expr compar_expr
%type <decimal> expr term factor

/* Operator precedence and associativity */
%left PLUS MINUS
%left MUL DIV EXP
%left OR
%left AND
%right NOT
%left EQ NEQ
%left LT LE GT GE

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
            | IDENTIFIER ASSIGN bool_expr SEMICOLON {
                put_symbol($1, $3);
                free($1);
            }
            | PRINT expr SEMICOLON { 
                printf("%g\n", $2);
            }
            | PRINT bool_expr SEMICOLON {
                printf("%s\n", $2 ? "true" : "false");
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

factor      : DECIMAL           { $$ = $1; }
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
            | OPENPAR expr CLOSEPAR   { $$ = $2; }
            | MINUS factor            { $$ = -$2; }  /* Handle unary minus */
            ;

bool_expr   : TRUE                            { $$ = true; }
            | FALSE                           { $$ = false; }
            | bool_expr AND bool_expr         { $$ = $1 && $3; }
            | bool_expr OR bool_expr          { $$ = $1 || $3; }
            | NOT bool_expr                   { $$ = !$2; }
            | compar_expr                     { $$ = $1; }
            | OPENPAR bool_expr CLOSEPAR      { $$ = $2; }
            ;

compar_expr : expr EQ expr    { $$ = $1 == $3; }
            | expr NEQ expr   { $$ = $1 != $3; }
            | expr LT expr    { $$ = $1 < $3; }
            | expr LE expr    { $$ = $1 <= $3; }
            | expr GT expr    { $$ = $1 > $3; }
            | expr GE expr    { $$ = $1 >= $3; }
            ;
%%

int main(void) {
    yyparse();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
