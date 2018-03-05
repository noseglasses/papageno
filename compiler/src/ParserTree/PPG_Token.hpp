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

#include <memory>
#include <vector>

namespace Papageno {
namespace ParserTree {
   
class Token : public Node
{
   public:
      
      Token(const std::string &id)
         :  Node(id),
            layer_(curLayer_)
      {}
      
      void setAction(const std::shared_ptr<Action> &action) {
         action_ = action;
      }
      
      void addChild(const std::shared_ptr<Token> &token) {
         children_->push_back(token);
      }
      
      const std::vector<std::shared_ptr<Token>> &getChildren() const {
         return children_;
      }
      
      bool hasChildren() const { return !children_.empty(); }
      
      virtual bool isEqual(const Token &other) = 0;
      
      virtual std::shared_ptr<Token> clone() const = 0;

      
      static void setCurrentLayer(const std::string &layer) { 
         curLayer_ = layer;
      }
      
      void generateCCode(std::ostream &out) {
         
         this->generateDependencyCodeInternal();

         // Output children
         //
         if(!children_.empty()) {
            out << 
"PPG_Token " << this->getId() << "_children[] = {\n";

            for(int i = 0; i < children_.size(); ++i) {
               out <<
"   &" << children_[i]->getId();
               if(i < children_size() - 1) {
                  out << ",";
               }
               out << "\n";
            }
            out <<
"};\n\n";
         }
                  
         out <<
"PPG_" << this->getNodeType() << " " << this->getId() << " = (PPG_" << this->getNodeType() << ") {\n";

         this->generateCCodeInternal(out);
         
         out <<
"};\n\n";


         // TODO: Howto set parent ptrs?
      }
           
   protected:
      
      virtual void generateDependencyCodeInternal(std::ostream &out) {}
      
      virtual void generateCCodeInternal(std::ostream &out) {
         
         out <<
"   .misc = (PPG_Misc_Bits) {\n"
"       .state = PPG_Token_Initialized,\n"
"       .flags = 0,\n"
"       .action_state = 0,\n"
"       .action_flags = PPG_Action_Default\n"
"    },\n";
         
         if(!children_.empty()) {
            out <<
"    .children = " << this->getId() << "_children,\n" <<
"    .n_allocated_children = sizeof(" << this->getId() << "_children),\n" <<
"    .n_children = sizeof(" << this->getId() << "_children),\n";
         }
         else {
            out <<
"    .children = nullptr,\n"
"    .n_allocated_children = 0,\n"
"    .n_children = 0\n";
         }
         
         if(action_) {
            out <<
"    .action = PPG_ACTION_INITIALIZATION_" << action_->getType() << "(" 
            << action_->getParameters() << ")\n"
         }
         else {
            
            out <<
"    .action =.callback.func = NULL,\n"
"    .action.callback.user_data = NULL,\n";
         }
         out <<
"    .layer = " << this->layer_ << "\n";
      }
      
   protected:
      
      std::shared_ptr<Action>               action_;
      std::vector<std::shared_ptr<Token>>   children_;
      
      std::string                           layer_;
      static std::string                    curLayer_ = 0;
};

}
}
