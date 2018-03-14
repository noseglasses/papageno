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
 
#pragma once

#include <istream>
#include <vector>

#define MAX_ID_LENGTH 256

#define YYLTYPE_IS_DECLARED
#define YYLTYPE_IS_TRIVIAL 1

typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;

#define YYSTYPE std::string

/* Same as previous macro, but useful when we know that the buffer stack is not
 * NULL or when we need an lvalue. For internal use only.
 */
#define YY_CURRENT_BUFFER_LVALUE yyg->yy_buffer_stack[yyg->yy_buffer_stack_top]

extern int yylex(YYSTYPE * yylval_param ,YYLTYPE *yylloc, void* yyscanner);

#define YY_DECL int yylex \
               (YYSTYPE * yylval_param , YYLTYPE *yylloc, yyscan_t yyscanner)

namespace Glockenspiel {
namespace Parser {
   
class LocationOfDefinition;

extern std::vector<std::string> filesParsed;
extern std::vector<std::string> codeParsed;

extern LocationOfDefinition currentLocation;
extern const char *currentFileParsed;

void generateTree(const char *inputFile);

} // namespace ParserTree
} // namespace Glockenspiel
