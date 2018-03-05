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

#include <memory>
#include <vector>
#include <string>

#include "ParserTree/PPG_Node.hpp"

namespace Papageno {
namespace ParserTree {
   
class Input : public Node
{
   public:
      
      Input(  const std::string &id,
               const std::string &type, 
               const std::string &parameters)
         :  Node(id),
            type_(type),
            parameters_(source)
      {}
      
      static void define(const std::shared_ptr<Input> &input) {
         inputs_[input->getId()] = input;
      }
      
      static const std::shared_ptr &lookupInput(const std::string &id) {
         auto it = inputs_.find(id);
         
         if(it == inputs_.end()) {
            THROW_ERROR("Unable to retreive undefined input \'" << id << "\'");
         }
         
         return it->second;
      }
      
      static void pushNextInput(const std::string &id) {
         if(nextInputs_.find(id) != nextInputs_.end()) {
            THROW_ERROR("Redundant input \'" << id << "\' added");
         }
         nextInputs_.insert(id);
      }
      
      static std::shared_ptr<Input> popNextInput() {
         if(nextInputs_.empty()) {
            THROW_ERROR("No inputs available");
         }
         if(nextInputs_.size() > 1) {
            THROW_ERROR("Ambiguous inputs specified");
         }
         auto tmp = *nextInputs_.begin();
         nextInputs_.clear();
         return tmp;
      }
      
      static void getInputs(std::vector<std::string> &inputs) {
         inputs.clear();
         std::copy(nextInputs_.begin(), nextInputs_.end(), std::back_inserter(inputs));
         nextInputs_.clear();
      }
      
      virtual std::string getPropertyDescription() const {
         return TO_STRING(Node::getPropertyDescription() << ", type = " << type_
            << ", parameters = \'" << parameters_ << "\'");
      }
      
      virtual std::string getNodeType() const { return "Input"; }
            
      typedef std::vector<std::shared_ptr<Input>> InputCollection;
      typedef std::map<std::string, InputCollection> InputsByType;
      
      static InputsByType getInputsByType() {
         
         InputsByType result;
         
         for(const auto &actionsEntry: inputs_) {
            result[actionsEntry->second->type_].push_back(actionsEntry->second);
         }
         
         return result;
      }
   protected:
      
      std::string           type_;
      std::string           parameters_;
      
      static std::map<std::string, std::shared_ptr<Input>> inputs_;
      
      static std::set<std::string> nextInputs_;
};

inline bool inputsEqual(std::vector<std::shared_ptr<Input>> i1,
                        std::vector<std::shared_ptr<Input>> i1
) {
   if(i1.size() != i2.size()) { return false; }
   
   std::set<std::string> ids;
   
   for(const auto &input: i1) {
      ids.insert(input->getId());
   }
   
   for(const auto &input: i2) {
      if(ids.find(input->getId()) == ids.end()) { return false; }
   }
   
   return true;
}

} // namespace ParserTree
} // namespace Papageno