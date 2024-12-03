%{
#include "parser.tab.h"
%}

%%
[0-9]+      { yylval.iVal = atoi(yytext); return NUMBER; }
[0-9]+\.[0-9]+ { yylval.fVal = atof(yytext); return FLOAT; }
[+*/()-]    { return yytext[0]; }
if          { return IF; }
else        { return ELSE; }
while       { return WHILE; }
[ \t\n]     { /* ignore whitespace */ }
int         { return INT; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sVal = strdup(yytext); return IDENTIFIER; }
"="         { return ASSIGN; }
";"         { return SEMICOLON; }
.           { return yytext[0]; }
%%

int yywrap(void) {
    return 1;
}
