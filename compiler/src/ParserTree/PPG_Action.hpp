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
#include "Parser/PPG_ParserToken.hpp"
#include "ParserTree/PPG_Entity.hpp"

namespace Papageno {
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
        return x.first < x.first;
    }
};

class Action : public Entity<Action, CountToAction, CountToActionCompare>
{
   public:
      
      typedef Entity<Action, CountToAction, CountToActionCompare> Parent;
      
      using Parent::Entity;
      
//       typedef std::pair<int, Parser::Token> CountToAction;
//       
//       Action(  const Parser::Token &id,
//                const Parser::Token &type, 
//                const Parser::Token &parameters,
//                bool parametersDefined
//             );
//       
//       static void define(const std::shared_ptr<Action> &action);
//       
      static void pushNext(const Parser::Token &countString,
                             const Parser::Token &id);
//       
      static void pushNext(const Parser::Token &id);
//       
//       static void pushNextAction(const CountToAction &cta);
//       
//       static std::shared_ptr<Action> popNext();
//       
//       static std::vector<CountToAction> getNextActions();
//          
//       static bool hasNextEntities();
//       
//       const Parser::Token &getType() const;
//       const Parser::Token &getParameters() const;
//       bool getParametersDefined() const;
//       
//       void setWasRequested(bool state) { wasRequested_ = state; }
//       bool getWasRequested() const { return wasRequested_; }
//       
//       virtual std::string getPropertyDescription() const;
      
      virtual std::string getNodeType() const { return "Action"; }
      
//       typedef std::vector<std::shared_ptr<Action>> ActionCollection;
//       typedef std::map<std::string, ActionCollection> ActionsByType;
//       
//       static ActionsByType getEntitiesByType(bool onlyRequested = false);
//       
//       static const std::map<std::string, std::shared_ptr<Action>> getActions();
//       
//       static std::shared_ptr<Action> lookup(const std::string &id);
      
//    protected:
//       
//       Parser::Token           type_;
//       Parser::Token           parameters_;
//       bool                    parametersDefined_;
//       bool                    wasRequested_;
//       
//       static std::vector<CountToAction>    nextActions_;
//       
//       static std::map<std::string, std::shared_ptr<Action>> actions_;
//       static std::map<std::string, Parser::LocationOfDefinition> locationsOfDefinition_;
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
