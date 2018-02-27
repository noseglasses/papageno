%{
 
#include "parser.h"
#include <stdio.h>
#include "y.tab.h"

%}
%%
" "       ;
[_[:alnum:]]*   { strncpy(yylval.id, yytext, MAX_ID_LENGTH); return ID; }

.* { strncpy(yylval.id, yytext, MAX_ID_LENGTH); return ALIAS; }
