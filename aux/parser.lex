%{
 
#include <stdio.h>
#include "parser.tab.h"
int c;

extern YylvalType yylval;
%}
%%
" "       ;
[_[:alnum:]]*   { strncpy(yylval.token, yytext, MAX_TOKEN_LENGTH); return ID; }
^layer: { return LAYER; }

.* { strncpy(yylval.token, yytext, MAX_TOKEN_LENGTH); return ALIAS; }
