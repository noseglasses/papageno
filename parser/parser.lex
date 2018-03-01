%{
 
#include "parser.h"
#include <stdio.h>
#include "parser.yacc.h"

int c;

// [^\n]* { strncpy(yylval.id, yytext, MAX_ID_LENGTH); return ALIAS; }
%}
%%
" "       |
"\t"      ;
layer     return LAYER_KEYWORD;
symbol    return SYMBOL_KEYWORD;
input     return INPUT_KEYWORD;
action    return ACTION_KEYWORD;
phrase    return PHRASE_KEYWORD;
"->"      return ARROW;
[\|\:;,\*\n=\(\){}\[\]<>#]       { c = yytext[0]; return c; }
[_[:alnum:]]*   { printf("Reading ID=%s\n", yytext); strncpy(yylval.id, yytext, MAX_ID_LENGTH); return ID; }
===[^\n]* { printf("Reading ID=%s\n", yytext); strncpy(yylval.id, yytext, MAX_ID_LENGTH); return RAW_CODE; }
