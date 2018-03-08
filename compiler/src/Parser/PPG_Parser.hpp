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

namespace Papageno {
namespace Parser {
   
class LocationOfDefinition;

extern LocationOfDefinition currentLocation;
extern const char *currentFileParsed;

void generateTree(const char *inputFile);

} // namespace ParserTree
} // namespace Papageno