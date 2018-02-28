%{
 
#include "parser.h"
#include <stdio.h>
#include "y.tab.h"

int c;

// [^\n]* { strncpy(yylval.id, yytext, MAX_ID_LENGTH); return ALIAS; }
%}
%%
" "       |
"\t"      ;
layer     return LAYER;
symbol    return SYMBOL;
alias     return ALIAS;
"->"      return ARROW;
[\:;,\*\n=\(\){}\[\]]       { c = yytext[0]; return c; }
[_[:alnum:]]*   { printf("Reading ID=%s\n", yytext); strncpy(yylval.id, yytext, MAX_ID_LENGTH); return ID; }
\"[^\"]*\" return QUOTED_STRING;
