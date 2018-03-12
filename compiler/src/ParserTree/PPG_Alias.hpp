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

#include "Parser/PPG_ParserToken.hpp"

#include <map>
#include <string>

namespace Papageno {
namespace ParserTree {
     
class Alias
{
   public:
      
      static void defineAlias(const std::string &name, const Parser::Token&target);
      
      static std::string replaceAlias(const std::string &name);
      
   private:
      
      static std::map<std::string, Parser::Token> aliasMapping_;
};

} // namespace ParserTree
} // namespace Papageno
