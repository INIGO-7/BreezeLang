%{
#include "common_lib.h"

int yylex(void);
void yyerror(const char *s);

astnode_t *root_ast;
%}

%debug

%union {
    astnode_t *ast;
    int number;
    float dec; 
    char* string;
    int boolean;
}

%token <number> INT
%token <dec> FLOAT
%token <string> IDENTIFIER STRING
%token WHILE FOR FUNC IF ELSE IFELSE FUNCSTART FUNCEND FUNCRET
%token TRUE FALSE
%token AND OR NOT
%token EQ NEQ LT GT LE GE
%token PRINT ASSIGN SEMICOLON COLON
%token PLUS MINUS MUL DIV EXP
%token OPENPAR CLOSEPAR OPENBRKT CLOSEBRKT
%token READ

/* Declare types for our new non-terminals */
%type <ast> stmt stmts expr term factor 
%type <ast> for_init for_update
%type <ast> params args

/* Operator precedence and associativity */
%right UMINUS
%right NOT
%left OR
%left AND
%left EQ NEQ LT LE GT GE
%left PLUS MINUS
%left MUL DIV
%right EXP

%%

program
    : stmts 
      {
        init_symbol_table();
        root_ast = $1;
      }
    ;

stmts
    : stmt SEMICOLON
      {
        $$ = astnode_new(NODE_STMTS);
        astnode_add_child($$, $1, 0);
      }
    | stmts stmt SEMICOLON
      {
        $$ = astnode_new(NODE_STMTS);
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $2, 1);
      }
    ;

stmt
    : IDENTIFIER ASSIGN expr
      {
        $$ = astnode_new(NODE_ASSIGN);
        $$->data.id = $1;
        astnode_add_child($$, $3, 0);
      }
    | PRINT expr
      {
        $$ = astnode_new(NODE_PRINT);
        astnode_add_child($$, $2, 0);
      }
    | WHILE expr FUNCSTART stmts FUNCEND
      {
        $$ = astnode_new(NODE_WHILE);
        astnode_add_child($$, $2, 0);  // condition (now expr)
        astnode_add_child($$, $4, 1);  // body
      }
    | FOR for_init COLON expr COLON for_update FUNCSTART stmts FUNCEND
      {
        $$ = astnode_new(NODE_FOR);
        astnode_add_child($$, $2, 0);  // for init
        astnode_add_child($$, $4, 1);  // for condition (now expr)
        astnode_add_child($$, $6, 2);  // for update
        astnode_add_child($$, $8, 3);  // body
      }
    | IF expr FUNCSTART stmts FUNCEND
      {
        $$ = astnode_new(NODE_IF);
        astnode_add_child($$, $2, 0);
        astnode_add_child($$, $4, 1);
      }
    | IF expr FUNCSTART stmts ELSE FUNCSTART stmts FUNCEND
      {
        $$ = astnode_new(NODE_IFELSE);
        astnode_add_child($$, $2, 0);
        astnode_add_child($$, $4, 1);
        astnode_add_child($$, $7, 2);
      }
    | FUNC IDENTIFIER OPENPAR params CLOSEPAR FUNCSTART stmts FUNCEND
      {
        $$ = astnode_new(NODE_FUNC);
        $$->data.id = $2;
        astnode_add_child($$, $4, 0);
        astnode_add_child($$, $7, 1);
      }
    | READ IDENTIFIER
      {
        $$ = astnode_new(NODE_READ);
        $$->data.id = $2;
      }
    | expr    // Temporary fallback
      {
        $$ = $1;
      }
    | FUNCRET expr
      {
        $$ = astnode_new(NODE_FUNCRET);
        astnode_add_child($$, $2, 0);
      }
    ;


for_init
    : IDENTIFIER ASSIGN expr
      {
        $$ = astnode_new(NODE_ASSIGN);
        $$->data.id = $1;
        astnode_add_child($$, $3, 0);
      }
    ;

for_update
    : IDENTIFIER ASSIGN expr
      {
        $$ = astnode_new(NODE_ASSIGN);
        $$->data.id = $1;
        astnode_add_child($$, $3, 0);
      }
    ;

params
    : /* no parameters option */
      {
        /* Instead of returning null, we return a node with no children
        Just so we have a valid ast node returned.*/
        $$ = astnode_new(NODE_STMTS);
      }
    | IDENTIFIER
      {
        astnode_t* paramNode = astnode_new(NODE_ID);
        paramNode->data.id = $1;
        
        $$ = astnode_new(NODE_STMTS);
        astnode_add_child($$, paramNode, 0);
      }
    | params COLON IDENTIFIER
      {
        astnode_t* paramNode = astnode_new(NODE_ID);
        paramNode->data.id = $3;

        int i = 0;
        while ($1->child[i] != NULL && i < MAXCHILDREN) i++;
        if (i >= MAXCHILDREN) {
          fprintf(stderr, "Too many parameters!\n");
          exit(EXIT_FAILURE);
        }
        astnode_add_child($1, paramNode, i);

        $$ = $1;
      }
    ;

args
  : /* no arguments */
    {
      $$ = astnode_new(NODE_STMTS); /* empty container node */
    }
  | expr
    {
      astnode_t* listNode = astnode_new(NODE_STMTS);
      astnode_add_child(listNode, $1, 0);
      $$ = listNode;
    }
  | args COLON expr
    {
      int i = 0;
      while ($1->child[i] != NULL && i < MAXCHILDREN) i++;
      if (i >= MAXCHILDREN) {
        fprintf(stderr, "Too many arguments!\n");
        exit(EXIT_FAILURE);
      }
      astnode_add_child($1, $3, i);
      $$ = $1;
    }
  ;

expr
    : expr OR expr
      {
        $$ = astnode_new(NODE_BOOL_OP);
        $$->data.bool_op = OP_OR;
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | expr AND expr
      {
        $$ = astnode_new(NODE_BOOL_OP);
        $$->data.bool_op = OP_AND;
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | NOT expr
      {
        $$ = astnode_new(NODE_BOOL_OP);
        $$->data.bool_op = OP_NOT;
        astnode_add_child($$, $2, 0);
      }
    | expr EQ expr
      {
        $$ = astnode_new(NODE_BOOL_OP);
        $$->data.bool_op = OP_EQ;
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | expr NEQ expr
      {
        $$ = astnode_new(NODE_BOOL_OP);
        $$->data.bool_op = OP_NEQ;
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | expr LT expr
      {
        $$ = astnode_new(NODE_BOOL_OP);
        $$->data.bool_op = OP_LT;
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | expr LE expr
      {
        $$ = astnode_new(NODE_BOOL_OP);
        $$->data.bool_op = OP_LE;
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | expr GT expr
      {
        $$ = astnode_new(NODE_BOOL_OP);
        $$->data.bool_op = OP_GT;
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | expr GE expr
      {
        $$ = astnode_new(NODE_BOOL_OP);
        $$->data.bool_op = OP_GE;
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | expr PLUS term
      {
        $$ = astnode_new(NODE_ADD);
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | expr MINUS term
      {
        $$ = astnode_new(NODE_SUB);
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | term
      {
        $$ = $1;
      }
    | TRUE
      {
        $$ = astnode_new(NODE_BOOL);
        $$->data.boolean = 1;
      }
    | FALSE
      {
        $$ = astnode_new(NODE_BOOL);
        $$->data.boolean = 0;
      }
    ;

term
    : term MUL factor
      {
        $$ = astnode_new(NODE_MUL);
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | term DIV factor
      {
        if ($3 == 0) {
            fprintf(stderr, "Error: Division by zero\n");
            exit(EXIT_FAILURE);
        }
        $$ = astnode_new(NODE_DIV);
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | term EXP factor
      {
        $$ = astnode_new(NODE_EXP);
        astnode_add_child($$, $1, 0);
        astnode_add_child($$, $3, 1);
      }
    | factor
      {
        $$ = $1;
      }
    ;

factor
    : INT
      {
        $$ = astnode_new(NODE_INT);
        $$->data.num = $1;
      }
    | FLOAT
      {
        $$ = astnode_new(NODE_FLOAT);
        $$->data.dec = $1;
      }
    | STRING
      {
        $$ = astnode_new(NODE_STRING);
        $$->data.str = strdup($1);
      }
    | IDENTIFIER
      {
        $$ = astnode_new(NODE_ID);
        $$->data.id = $1;
      }
    | OPENPAR expr CLOSEPAR
      {
        $$ = $2;
      }
    | MINUS factor %prec UMINUS
      {
        /* If $2 is a numeric constant, do immediate negation. Otherwise,
           build a unary-minus AST node (depending on your language). */

      if ($2->type == NODE_INT)
        {
          $$ = astnode_new(NODE_INT);
          $$->data.num = -($2->data.num);
        }
        else if ($2->type == NODE_FLOAT)
        {
          $$ = astnode_new(NODE_FLOAT);
          $$->data.dec = -($2->data.dec);
        }
        else
        {
          fprintf(stderr, "Error: invalid type in unary minus\n");
          exit(EXIT_FAILURE);
        }
      }
    | IDENTIFIER OPENPAR args CLOSEPAR 
      {
        $$ = astnode_new(NODE_FUNCCALL);
        $$->data.id = $1;
        astnode_add_child($$, $3, 0);
      }
    | IDENTIFIER OPENBRKT expr CLOSEBRKT
      {
        $$ = astnode_new(NODE_INDEX);
        $$->data.id = $1;
        astnode_add_child($$, $3, 0);
      }
    ;

%%

void yyerror(const char *s) {
    extern char *yytext;
    extern int yylineno;
    fprintf(stderr, "Error: %s at line %d, near token '%s'\n", s, yylineno, yytext);
}
