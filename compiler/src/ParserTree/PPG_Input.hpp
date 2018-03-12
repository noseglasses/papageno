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
#include "ParserTree/PPG_Entity.hpp"

namespace Papageno {
namespace ParserTree {
   
class Input : public Entity<Input, Parser::Token, Parser::TokenCompare>
{
   public:
      
      typedef Entity<Input, Parser::Token, Parser::TokenCompare> Parent;
      
      using Parent::Entity;
      
      virtual std::string getNodeType() const { return "Input"; }
};

bool inputsEqual(std::vector<Parser::Token> i1,
                 std::vector<Parser::Token> i2);

inline
bool operator==(const Input &i1, const Input &i2) 
{
   return i1.getId().getText() == i2.getId().getText();
}

} // namespace ParserTree
} // namespace Papageno

