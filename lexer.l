%{
#include "parser.tab.h"
#include <stdlib.h>
%}

%%
[1-9][0-9]*               { yylval.number = atof(yytext); return NUMBER; }
([1-9][0-9]|0)\.[0-9]+    { yylval.decimal = atof(yytext); return DECIMAL; }
"print"                   { return PRINT; }
[a-zA-Z_][a-zA-Z0-9_]*    { yylval.string = strdup(yytext); return IDENTIFIER; }

"+"                       { return PLUS; }
"-"                       { return MINUS; }
"/"                       { return DIV; }
"*"                       { return MUL; }
"**"                      { return EXP; }

"("                       { return OPENPAR; }
")"                       { return CLOSEPAR; }

"="                       { return ASSIGN; }
";"                       { return SEMICOLON; }

[ \t\n]                   { /* ignore whitespace */ }
.                         { fprintf(stderr, "Error: Unexpected character '%s'\n", yytext); return yytext[0]; }
%%

int yywrap(void) {
    return 1;
}
