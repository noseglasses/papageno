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

#include "ParserTree/PPG_Input.hpp"
#include "ParserTree/PPG_Node.hpp"
#include "ParserTree/PPG_Alias.hpp"
#include "Parser/PPG_ParserToken.hpp"

#include <memory>
#include <vector>
#include <set>

namespace Glockenspiel {
namespace ParserTree {
   
// template<> Input::Parent::NextEntities Input::Parent::nextEntities_;
// template<> Input::Parent::Entities Input::Parent::entities_;
// template<> Input::Parent::LocationsOfDefinition Input::Parent::locationsOfDefinition_;

bool inputsEqual( std::vector<Parser::Token> i1,
                  std::vector<Parser::Token> i2)
{
   if(i1.size() != i2.size()) { return false; }
   
   std::set<std::string> ids;
   
   for(const auto &input: i1) {
      ids.insert(input.getText());
   }
   
   for(const auto &input: i2) {
      if(ids.find(input.getText()) == ids.end()) { return false; }
   }
   
   return true;
}

} // namespace ParserTree
} // namespace Glockenspiel
