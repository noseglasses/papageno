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

#include "ParserTree/GLS_Node.hpp"
#include "Parser/GLS_ParserToken.hpp"
#include "ParserTree/GLS_Entity.hpp"
#include "Settings/GLS_Settings.hpp"

namespace Glockenspiel {
namespace ParserTree {
   
struct CountToAction {
   CountToAction(int count, const Parser::Token &action) 
      :  first(count),
         second(action)
   {}
   
   int first;
   Parser::Token second;
   const Parser::Token &getLOD() const { return second; }
   const std::string &getText() const { return second.getText(); }
};

struct CountToActionCompare 
   : public std::binary_function<const CountToAction &, const CountToAction &, bool>
{
    bool operator()(const CountToAction &x, const CountToAction &y) const
    {   
        if(x.first == y.first) {
           return x.second.getText() < y.second.getText();
        }
        return x.first < y.first;
    }
};

class Action : public Entity<Action, CountToAction, CountToActionCompare>
{
   public:
      
      typedef Entity<Action, CountToAction, CountToActionCompare> Parent;
      
      using Parent::Entity;

      static void pushNext(const Parser::Token &countString,
                             const Parser::Token &id);
  
      static void pushNext(const Parser::Token &id);

      virtual std::string getNodeType() const { return "Action"; }
      
      static std::string entityName() { return "action"; }
      
      static bool allowEntityDefinition() {
         return settings.allow_auto_type_definition
            || settings.allow_auto_action_type_definition;
      }
};

inline
bool operator==(const Action &a1, const Action &a2) {
   return a1.getId().getText() == a2.getId().getText();
}

inline
bool operator!=(const Action &a1, const Action &a2) {
   return !operator==(a1, a2);
}

} // namespace ParserTree
} // namespace Glockenspiel
