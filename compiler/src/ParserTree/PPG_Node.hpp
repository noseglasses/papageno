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
#include <map>
#include <string>

#define TO_STRING(...) (std::ostringstream() << __VA_ARGS__).str()

#define THROW_ERROR(...) \
   throw TO_STRING(__VA_ARGS__)

namespace Papageno {
namespace ParserTree {

class Node
{   
   public:
      
      Node() 
//          :  id_(Node::generateId()) // generate just in time
      {}
      
      
      Node(    const std::string &id)
         :  id_(id)
      {
         this->retreiveSourcePosition();
         
         Node::storeNode(id);
      }
      
      virtual ~Node() {
         Node::deleteNode(id);
      }
      
      const std::string &getId() const { 
         if(id_.empty()) {
            id_ = generateId();
         }
         
         return id_; 
      }
      
      void getSourceLine() const { return sourcePos_[0]; }
      void getSourceColumn() const { return sourcePos_[1]; }
   
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
         todo
      }
      
      static void storeNode(const std::string &id, const Node *node) {
         
         auto it = ids_.find();
         if(it != ids_.end()) {
            THROW_ERROR("An entity with id \'" << id 
               << "\' has already been defined: " 
               << it->second->getPropertyDescription());
         }
         
         ids_[id] = node;
      }
      
      static void deleteNode(const std::string &id) {
         auto it = ids_.find();
         
         assert(it != ids_.end());
         
         ids_.erase(it);
      }
      
      static std::string generateId() {
         ++nextId_;
         return TO_STRING(this->getNodeType() << "_" << nextId_);
      }
         
   protected:
      
      std::string           id_;
      int                   sourcePos_[2];
      
      static std::map<std::string, *Node> ids_;
      
      static int           nextId_;
};
      
} // namespace ParserTree
} // namespace Papageno

