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
#include <string>

namespace Papageno {
namespace ParserTree {
      
class Action : public Node
{
   public:
      
      typedef std::pair<int, std::string> CountToAction;
      
      Action(  const std::string &id,
               const std::string &type, 
               const std::string &parameters)
         :  Node(id),
            type_(type),
            parameters_(parameters)
      {}
      
      static void define(const std::shared_ptr<Action> &action) {
         actions_[action->getId()] = action;
      }
      static void pushNextAction(const std::string &countString,
                             const std::string &id) {
         auto count = Papageno::Misc::atol(countString);
         
         pushNextAction(CountToAction(count, id));
      }
      
      static void pushNextAction(const std::string &id) {
         
         pushNextAction(CountToAction(-1, id));
      }
      
      static void pushNextAction(const CountToAction &cta) {
         nextActions_.push_back(cta);
      }
      
      static std::shared_ptr<Action> popNextAction() {
         if(nextActions_.empty()) {
            THROW_ERROR("No actions available");
         }
         
         auto tmp = nextActions_.back();
         nextActions_.pop_back();
         return lookupAction(tmp.second);
      }
      
      static std::vector<CountToAction> getNextActions() {
         std::vector<CountToAction> tmp;
         tmp.swap(nextActions_);
         return tmp;
      }
         
      static bool hasNextActions() {
         return !nextActions_.empty();      
      }
      
      const std::string &getType() const { return type_; }
      const std::string &getParameters() const { return parameters_; }
      
      virtual std::string getPropertyDescription() const {
         return TO_STRING(Node::getPropertyDescription() << ", type = " << type_
            << ", parameters = \'" << parameters_ << "\'");
      }
      
      virtual std::string getNodeType() const { return "Action"; }
      
      typedef std::vector<std::shared_ptr<Action>> ActionCollection;
      typedef std::map<std::string, ActionCollection> ActionsByType;
      
      static ActionsByType getActionsByType() {
         
         ActionsByType result;
         
         for(const auto &actionsEntry: actions_) {
            result[actionsEntry.second->type_].push_back(actionsEntry.second);
         }
         
         return result;
      }
      
      static const std::map<std::string, std::shared_ptr<Action>> getActions() {
         return actions_;
      }
      
      static std::shared_ptr<Action> lookupAction(const std::string &id) {
         auto it = actions_.find(id);
         
         if(it == actions_.end()) {
            THROW_ERROR("Unable to find action \'" << id << "\'");
         }
         
         return it->second;
      }
      
   protected:
      
      std::string           type_;
      std::string           parameters_;
      
      static std::vector<CountToAction>    nextActions_;
      
      static std::map<std::string, std::shared_ptr<Action>> actions_;
      
};

} // namespace ParserTree
} // namespace Papageno
