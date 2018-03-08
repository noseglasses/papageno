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

#include "ParserTree/PPG_Action.hpp"

namespace Papageno {
namespace ParserTree {
      
std::vector<Action::CountToAction>    Action::nextActions_;
std::map<std::string, std::shared_ptr<Action>> Action::actions_;
std::map<std::string, Parser::LocationOfDefinition> Action::locationsOfDefinition_;
      
   Action
      ::Action(const Parser::Token &id,
               const Parser::Token &type, 
               const Parser::Token &parameters,
               bool parametersDefined
              )
   :  Node(id),
      type_(type),
      parameters_(parameters),
      parametersDefined_(parametersDefined)
{}

void
   Action
      ::define(const std::shared_ptr<Action> &action)
{
   const auto &id = action->getId().getText();
   
   auto it = actions_.find(id);
   if(it != actions_.end()) {
      auto lodIt = locationsOfDefinition_.find(id);
      THROW_TOKEN_ERROR(action->getId(), 
         "Action multiply defined. First definition here: " 
         << lodIt->second);
   }
   
   actions_[id] = action;
   locationsOfDefinition_[id] = action->getId().getLOD();
}

void 
   Action
      ::pushNextAction(const Parser::Token &countString,
                        const Parser::Token &id)
{
   auto count = Papageno::Misc::atol(countString);
   
   pushNextAction(CountToAction(count, id));
}

void 
   Action
      ::pushNextAction(const Parser::Token &id) 
{   
   pushNextAction(CountToAction(-1, id));
}

void 
   Action
      ::pushNextAction(const CountToAction &cta)
{
   nextActions_.push_back(cta);
}

std::shared_ptr<Action> 
   Action
      ::popNextAction()
{
   if(nextActions_.empty()) {
      THROW_ERROR("No actions available");
   }
   
   auto tmp = nextActions_.back();
   nextActions_.pop_back();
   return lookupAction(tmp.second.getText());
}

std::vector<Action::CountToAction> 
   Action
      ::getNextActions()
{
   std::vector<CountToAction> tmp;
   tmp.swap(nextActions_);
   return tmp;
}
   
bool 
   Action
      ::hasNextActions()
{
   return !nextActions_.empty();      
}

const Parser::Token &
   Action
      ::getType() const 
{
   return type_; 
}

const Parser::Token &
   Action
      ::getParameters() const 
{ 
   return parameters_; 
}

bool
   Action
      ::getParametersDefined() const
{
   return parametersDefined_;
}

std::string
   Action
      ::getPropertyDescription() const
{
   return TO_STRING(Node::getPropertyDescription() << ", type = " << type_
      << ", parameters = \'" << parameters_ << "\'");
}

std::string
   Action
      ::getNodeType() const 
{ 
   return "Action"; 
}

Action::ActionsByType 
   Action
      ::getActionsByType()
{   
   ActionsByType result;
   
   for(const auto &actionsEntry: actions_) {
      result[actionsEntry.second->getType().getText()].push_back(actionsEntry.second);
   }
   
   return result;
}

const std::map<std::string, std::shared_ptr<Action>> 
   Action
      ::getActions()
{
   return actions_;
}

std::shared_ptr<Action> 
   Action
      ::lookupAction(const std::string &id) 
{
   auto it = actions_.find(id);
   
   if(it == actions_.end()) {
      THROW_ERROR("Unable to find action \'" << id << "\'");
   }
   
   return it->second;
}

} // namespace ParserTree
} // namespace Papageno
