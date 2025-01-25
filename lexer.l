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

[0-9]+\.[0-9]+            { yylval.decimal = atof(yytext); return DECIMAL; }
[0-9]+                    { yylval.number = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*    { yylval.string = strdup(yytext); return IDENTIFIER; }
\"[^\"]*\"                { yylval.string = strdup(yytext); return STRING; }

"+"                       { return PLUS; }
"-"                       { return MINUS; }
"/"                       { return DIV; }
"*"                       { return MUL; }
"**"                      { return EXP; }

"("                       { return OPENPAR; }
")"                       { return CLOSEPAR; }

"="                       { return ASSIGN; }
";"                       { return SEMICOLON; }
","                       { return COLON; }

"w["                      { return WHILE; }
"f["                      { return FOR; }
"d["                      { return FUNC; }
"->"                      { return FUNCSTART; }
"]"                       { return FUNCEND; }

\/\*[^*]*\*+([^/*][^*]*\*+)*\/    { /* ignore block comments */ }
"//".*                            { /* ignore single line comments */ }
[ \t\n]                   { /* ignore whitespace */ }

.                         { fprintf(stderr, "Error: Unexpected character '%s'\n", yytext); return yytext[0]; }
%%

int yywrap(void) {
    return 1;
}
