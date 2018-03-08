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
#include "Parser/PPG_Parser.yacc.hpp"

#include <iostream>

long int offset = 1;
extern YYLTYPE yylloc;

#define YY_USER_ACTION         \
  yylloc.first_column = offset; \
  offset += yyleng;            \
  yylloc.first_line = yylineno; \
  yylloc.last_line = yylineno; \
  yylloc.last_column = offset;
  
#define CORRECT_LINE \
  --yylloc.first_line; \
  --yylloc.last_line; \
  
// #define YY_USER_ACTION \
//    yylloc.first_line = yylloc.last_line = yylineno;
  
int c;

// [^\n]* { strncpy(yylval.id, yytext, MAX_ID_LENGTH); return ALIAS; }
%}

%option noyywrap
%option yylineno

%%
" "      ;
\t       ;
\\\n     { 
/*             std::cout << "Continuation line detected\n"; */
            // yylineno was already updated before YY_USER_ACTION was called
            CORRECT_LINE 
            offset = 1; 
         } // Allow continuation lines by ignoring them
layer    return LAYER_KEYWORD;
input    return INPUT_KEYWORD;
action   return ACTION_KEYWORD;
phrase   return PHRASE_KEYWORD;
"->"     return ARROW;
[\|\:;,\*=\(\){}\[\]<>#@] { 
            return yytext[0]; 
         }
\n       { 
/*             std::cout << "Standard line end detected\n";  */
            // yylineno was already updated before YY_USER_ACTION was called
            CORRECT_LINE
            offset = 1; 
            return LINE_END;
         }
[_[:alnum:]]* { 
/*             std::cout << "Reading ID=" << yytext << std::endl;  */
            yylval = yytext; 
            return ID; 
         }
:=[^\n]* { 
/*             std::cout << "Reading RAW_CODE=" << yytext << std::endl;  */
            yylval = yytext;
            return RAW_CODE; 
         }
