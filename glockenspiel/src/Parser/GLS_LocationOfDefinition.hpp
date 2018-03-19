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

typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;

namespace Glockenspiel {
namespace Parser {
   
struct LocationOfDefinition
{
   LocationOfDefinition(YYLTYPE location = (YYLTYPE){ .first_line = -1, .first_column = -1, .last_line = -1, .last_column = -1 },
                        const char *file = nullptr)
      :  file_(file),
         location_(location)
   {}
   
   operator bool() const {
      return location_.first_line != -1;
   }
   
   const char* file_;
   YYLTYPE location_;
};

} // namespace Parser
} // namespace Glockenspiel
