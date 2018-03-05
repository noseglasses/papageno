%{
 /* Copyright 2018 noseglasses <shinynoseglasses@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "Parser/PPG_Parser.hpp"
#include <stdio.h>
#include "Parser/PPG_Parser.yacc.hpp"

offset_t offset;
extern YYLTYPE yylloc;

#define YY_USER_ACTION         \
  offset += yyleng;            \
  yylloc.last_line = yylineno; \
  yylloc.last_column = offset;
  
int c;

// [^\n]* { strncpy(yylval.id, yytext, MAX_ID_LENGTH); return ALIAS; }
%}
%%
" "       |
"\t"      |
"\\\n"    ; // Allow continuation lines by ignoring them
layer     return LAYER_KEYWORD;
input     return INPUT_KEYWORD;
action    return ACTION_KEYWORD;
phrase    return PHRASE_KEYWORD;
"->"      return ARROW;
[\|\:;,\*=\(\){}\[\]<>#]       { c = yytext[0]; return c; }
[^\\]\n   return LINE_END;
[_[:alnum:]]*   { printf("Reading ID=%s\n", yytext); strncpy(yylval.id, yytext, MAX_ID_LENGTH); return ID; }
===[^\n]* { printf("Reading ID=%s\n", yytext); strncpy(yylval.id, yytext, MAX_ID_LENGTH); return RAW_CODE; }
