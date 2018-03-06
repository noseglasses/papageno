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

#include "Parser/PPG_Parser.hpp"
#include "Misc/PPG_ErrorHandling.hpp"
#include "Misc/PPG_StringHandling.hpp"

#include <memory>
#include <map>
#include "Parser/PPG_ParserToken.hpp"
#include <cassert>

namespace Papageno {
namespace ParserTree {

class Node
{   
   public:
      
      Node() 
//          :  id_(Node::generateId()) // generate just in time
      {}
      
      
      Node(    const Parser::Token &id)
         :  id_(id)
      {
         this->retreiveSourcePosition();
         
         Node::storeNode(id, this);
      }
      
      virtual ~Node() {
         Node::deleteNode(id_);
      }
      
      const Parser::Token &getId() const { 
         if(id_.getText().empty()) {
            id_.setText(generateId());
         }
         
         return id_; 
      }
      
      int getSourceLine() const { return sourcePos_[0]; }
      int getSourceColumn() const { return sourcePos_[1]; }
   
      virtual std::string getPropertyDescription() const {
         return TO_STRING("id = " << id_);
      }
      
      virtual std::string getNodeType() const { return "Node"; }
      
      virtual std::string getDescription() const {
         return TO_STRING(this->getNodeType() << "{ " 
            << this->getPropertyDescription() << " }");
      }
   
   protected:
      
      void retreiveSourcePosition() {
         if(!Papageno::Parser::currentLocation) {
            sourcePos_[0] = -1;
            sourcePos_[1] = -1;
            return;
         }
         
         sourcePos_[0] = Papageno::Parser::currentLocation->first_line;
         sourcePos_[0] = Papageno::Parser::currentLocation->first_column;
      }
      
      static void storeNode(const Parser::Token &id, const Node *node) {
         
         auto it = ids_.find(id.getText());
         if(it != ids_.end()) {
            THROW_TOKEN_ERROR(id, "An entity with id \'" << id 
               << "\' has already been defined: " 
               << it->second->getPropertyDescription());
         }
         
         ids_[id.getText()] = node;
      }
      
      static void deleteNode(const Parser::Token &id) {
         auto it = ids_.find(id.getText());
         
         assert(it != ids_.end());
         
         ids_.erase(it);
      }
      
      std::string generateId() const {
         ++nextId_;
         return TO_STRING(this->getNodeType() << "_" << nextId_);
      }
         
   protected:
      
      mutable Parser::Token id_;
      int                   sourcePos_[2];
      
      static std::map<std::string, const Node*> ids_;
      
      static int           nextId_;
};
      
} // namespace ParserTree
} // namespace Papageno

