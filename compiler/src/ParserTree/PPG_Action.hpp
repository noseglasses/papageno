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

#include "ParserTree/PPG_Node.hpp"
#include "Misc/PPG_StringHandling.hpp"

#include <vector>
#include "Parser/PPG_ParserToken.hpp"

namespace Papageno {
namespace ParserTree {
      
class Action : public Node
{
   public:
      
      typedef std::pair<int, Parser::Token> CountToAction;
      
      Action(  const Parser::Token &id,
               const Parser::Token &type, 
               const Parser::Token &parameters);
      
      static void define(const std::shared_ptr<Action> &action);
      
      static void pushNextAction(const Parser::Token &countString,
                             const Parser::Token &id);
      
      static void pushNextAction(const Parser::Token &id);
      
      static void pushNextAction(const CountToAction &cta);
      
      static std::shared_ptr<Action> popNextAction();
      
      static std::vector<CountToAction> getNextActions();
         
      static bool hasNextActions();
      
      const Parser::Token &getType() const;
      const Parser::Token &getParameters() const;
      
      virtual std::string getPropertyDescription() const;
      
      virtual std::string getNodeType() const;
      
      typedef std::vector<std::shared_ptr<Action>> ActionCollection;
      typedef std::map<std::string, ActionCollection> ActionsByType;
      
      static ActionsByType getActionsByType();
      
      static const std::map<std::string, std::shared_ptr<Action>> getActions();
      
      static std::shared_ptr<Action> lookupAction(const std::string &id);
      
   protected:
      
      Parser::Token           type_;
      Parser::Token           parameters_;
      
      static std::vector<CountToAction>    nextActions_;
      
      static std::map<std::string, std::shared_ptr<Action>> actions_;
      static std::map<std::string, YYLTYPE> locationsOfDefinition_;
      
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
} // namespace Papageno
