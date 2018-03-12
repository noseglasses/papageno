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

#include <memory>
#include <vector>
#include <set>
#include "Parser/PPG_ParserToken.hpp"

namespace Papageno {
namespace ParserTree {

std::set<Parser::Token, Parser::TokenCompare> Input::nextInputs_;

std::map<std::string, std::shared_ptr<Input>> Input::inputs_;
std::map<std::string, Parser::LocationOfDefinition> Input::locationsOfDefinition_;
      
   Input
      ::Input(  const Parser::Token &id,
               const Parser::Token &type, 
               const Parser::Token &parameters,
               bool parametersDefined
             )
   :  Node(id),
      type_(type),
      parameters_(parameters),
      parametersDefined_(parametersDefined),
      wasRequested_(false)
{}

void 
   Input
      ::define(const std::shared_ptr<Input> &input)
{
   const auto &id = input->getId().getText();
   
   auto it = inputs_.find(id);
   if(it != inputs_.end()) {
      auto lodIt = locationsOfDefinition_.find(id);
      THROW_TOKEN_ERROR(input->getId(), 
         "Action multiply defined. First definition here: " 
         << lodIt->second);
   }
   
   inputs_[id] = input;
   locationsOfDefinition_[id] = input->getId().getLOD();
}

const std::shared_ptr<Input> & 
   Input
      ::lookupInput(const std::string &id)
{
   auto it = inputs_.find(id);
   
   if(it == inputs_.end()) {
      THROW_ERROR("Unable to retreive undefined input \'" << id << "\'");
   }
   
   return it->second;
}

void  
   Input
      ::pushNextInput(const Parser::Token &id) 
{
   auto it = nextInputs_.find(id);
   if(it != nextInputs_.end()) {
      THROW_TOKEN_ERROR(id, "Redundant input added. Previous definition: " << it->getLOD());
   }
   nextInputs_.insert(id);
}

std::shared_ptr<Input>  
   Input
      ::popNextInput() 
{
   if(nextInputs_.empty()) {
      THROW_ERROR("No inputs available");
   }
   if(nextInputs_.size() > 1) {
      THROW_ERROR("Ambiguous inputs specified");
   }
   auto tmp = *nextInputs_.begin();
   nextInputs_.clear();
   return lookupInput(tmp.getText());
}

void  
   Input
      ::getInputs(std::vector<Parser::Token> &inputs) 
{
   inputs.clear();
   std::copy(nextInputs_.begin(), nextInputs_.end(), std::back_inserter(inputs));
   nextInputs_.clear();
}

std::string
   Input
      ::getPropertyDescription() const 
{
   return TO_STRING(Node::getPropertyDescription() << ", type = " << type_
      << ", parameters = \'" << parameters_ << "\'");
}

std::string
   Input
      ::getNodeType() const 
{ 
   return "Input"; 
}

Input::InputsByType  
   Input
      ::getInputsByType(bool onlyRequested) 
{   
   InputsByType result;
   
   for(const auto &inputsEntry: inputs_) {
      if(onlyRequested && !inputsEntry.second->getWasRequested()) { continue; }
      result[inputsEntry.second->getType().getText()].push_back(inputsEntry.second);
   }
   
   return result;
}

const std::map<std::string, std::shared_ptr<Input>> & 
   Input
      ::getInputs() 
{
   return inputs_;
}

const Parser::Token & 
   Input
      ::getType() const 
{
   return type_; 
}

const Parser::Token & 
   Input
      ::getParameters() const
{
   return parameters_; 
}

bool
   Input
      ::getParametersDefined() const
{
   return parametersDefined_;
}

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
} // namespace Papageno
