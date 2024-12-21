%{
#include "parser.tab.h"
#include <stdlib.h>
%}

%%
[0-9]+          { yylval.number = atoi(yytext); return NUMBER; }
"print"         { return PRINT; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.string = strdup(yytext); return IDENTIFIER; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"/"             { return DIV; }
"*"             { return MUL; }

"("             { return OPENPAR; }
")"             { return CLOSEPAR; }

"="             { return ASSIGN; }
";"             { return SEMICOLON; }

[ \t\n]         { /* ignore whitespace */ }
.               { fprintf(stderr, "Error: Unexpected character '%s'\n", yytext); return yytext[0]; }
%%

int yywrap(void) {
    return 1;
}
