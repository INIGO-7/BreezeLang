%{  
#include "common_lib.h"
#include "parser.tab.h"
extern FILE *yyin;
%}

%option yylineno

%%
"print"                   { return PRINT; }
"true"                    { return TRUE; }
"false"                   { return FALSE; }

"&&"                      { return AND; }
"||"                      { return OR; }
"!"                       { return NOT; }
"=="                      { return EQ; }
"!="                      { return NEQ; }
"<"                       { return LT; }
"<="                      { return LE; }
">"                       { return GT; }
">="                      { return GE; }

"return"                  { return FUNCRET; }

[0-9]+\.[0-9]+            { yylval.dec = atof(yytext); return FLOAT; }
[0-9]+                    { yylval.number = atoi(yytext); return INT; }
[a-zA-Z_][a-zA-Z0-9_]*    { yylval.string = strdup(yytext); return IDENTIFIER; }
\"[^\"]*\"                { yylval.string = strdup(yytext); return STRING; }

"+"                       { return PLUS; }
"-"                       { return MINUS; }
"/"                       { return DIV; }
"*"                       { return MUL; }
"**"                      { return EXP; }

"("                       { return OPENPAR; }
")"                       { return CLOSEPAR; }
"["                       { return OPENBRKT; }
"]"                       { return CLOSEBRKT; }

"="                       { return ASSIGN; }
";"                       { return SEMICOLON; }
","                       { return COLON; }

"w{"                      { return WHILE; }
"f{"                      { return FOR; }
"d{"                      { return FUNC; }
"i{"                      { return IF; }
"e{"                      { return ELSE; }
"ie{"                     { return IFELSE; }
"->"                      { return FUNCSTART; }
"}"                       { return FUNCEND; }

"what? ->"                { return READ; }

\/\*[^*]*\*+([^/*][^*]*\*+)*\/    { /* ignore block comments */ }
"//".*                            { /* ignore single line comments */ }
[ \t\n]                   { /* ignore whitespace */ }

.                         { fprintf(stderr, "Error: Unexpected character '%s'\n", yytext); return yytext[0]; }
%%

int yywrap(void) {
    return 1;
}
