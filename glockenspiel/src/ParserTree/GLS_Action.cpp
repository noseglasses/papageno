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

#include "ParserTree/GLS_Action.hpp"
#include "ParserTree/GLS_Alias.hpp"

namespace Glockenspiel {
namespace ParserTree {

void 
   Action
      ::pushNext(const Parser::Token &countString,
                        const Parser::Token &id)
{
   auto count = Glockenspiel::Misc::atol(countString);
   
   Parent::pushNext(CountToAction(count, id));
}

void 
   Action
      ::pushNext(const Parser::Token &id) 
{   
   Parent::pushNext(CountToAction(-1, id));
}

} // namespace ParserTree
} // namespace Glockenspiel
