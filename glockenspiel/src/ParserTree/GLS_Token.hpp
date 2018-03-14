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
#include "ParserTree/GLS_Action.hpp"

#include <memory>
#include <vector>
#include <map>

namespace Glockenspiel {
namespace ParserTree {
   
class Input;
   
struct InputAssignment {
   std::string             pathString_;
   std::shared_ptr<Input>  entity_;
};

typedef std::map<std::string, std::vector<InputAssignment>> InputAssignmentsByTag;

struct ActionAssignment {
   std::string             pathString_;
   std::shared_ptr<Action> entity_;
};

typedef std::map<std::string, std::vector<ActionAssignment>> ActionAssignmentsByTag;
   
class Token : public Node
{
   public:  
      
      Token()
         :  layer_(curLayer_),
            parent_(nullptr)
      {}
      
      Token(const Parser::Token &id)
         :  Node(id),
            layer_(curLayer_),
            parent_(nullptr)
      {}
      
      void setAction(const Parser::Token &action) {
         action_ = action;
      }
      
      const Parser::Token &getAction() const {
         return action_;
      }
      
      void setFlagCode(const Parser::Token &flagCode) {
         flagCode_ = flagCode;
      }
      
      const Parser::Token &getFlagCode() const {
         return flagCode_;
      }
      
      const Parser::Token &getLayer() const {
         return layer_;
      }
      
      void addChild(const std::shared_ptr<Token> &token) {
         children_.push_back(token);
      }
      
      void setChildren(std::vector<std::shared_ptr<Token>> &children) {
         children_ = children;
      }
      
      void setParent(const Token &parent) {
         parent_ = &parent;
      }
      
      const Token *getParent() const {
         return parent_;
      }
      
      const std::vector<std::shared_ptr<Token>> &getChildren() const {
         return children_;
      }
      
      std::vector<std::shared_ptr<Token>> &getChildren() {
         return children_;
      }
      
      bool hasChildren() const { return !children_.empty(); }
      
      virtual int getNumInputs() const { return 0; }
      
      virtual bool isEqual(const Token &other) const { return false; }
      
      virtual std::shared_ptr<Token> clone() const { return std::shared_ptr<Token>(); }
      
      static void setCurrentLayer(const Parser::Token &layer) { 
         curLayer_ = layer;
      }
      
      void outputCTokenDeclaration(std::ostream &out) const {
         out << this->getTokenType() << " " << this->getId().getText();
      }
      
      void generateCCode(std::ostream &out) const {
         
         this->generateDependencyCodeInternal(out);

         // Output children
         //
         if(!children_.empty()) {
            out << 
"PPG_Token__ *" << this->getId().getText() << "_children[" << children_.size() << "] = {\n";

            for(int i = 0; i < children_.size(); ++i) {
               out <<
"   (PPG_Token__*)&" << children_[i]->getId().getText();
               if(i < children_.size() - 1) {
                  out << ",";
               }
               out << "\n";
            }
            out <<
"};\n\n";
         }
          
         this->outputCTokenDeclaration(out);
         
         out << " = {\n";

         this->generateCCodeInternal(out);
         
         out <<
"};\n\n";


         // TODO: Howto set parent ptrs?
      }
      
      virtual std::string getNodeType() const override { return "Token"; }
      
      virtual void collectInputAssignments(InputAssignmentsByTag &iabt) const {}
      
      void collectActionAssignments(ActionAssignmentsByTag &aabt) const {
         if(action_.getText().empty()) { return; }
         std::ostringstream path;
         path << this->getId().getText() << "." << this->getActionPath();
         const auto &actionPtr = Action::lookup(action_.getText());
         aabt[actionPtr->getType().getText()].push_back( 
            (ActionAssignment) { 
               path.str(),
               actionPtr
            }
         );
      }
           
      virtual void touchActionsAndInputs() {
         if(action_.getText().empty()) { return; }
         const auto &actionPtr = Action::lookup(action_.getText());
         actionPtr->setWasRequested(true);
      }
           
   protected:
      
      virtual void generateDependencyCodeInternal(std::ostream &out) const {}
      
      virtual void generateCCodeInternal(std::ostream &out) const {

         out <<
"      .vtable = " << this->getVTableId() << ",\n";

         if(parent_) {
            out <<
"      .parent = (PPG_Token__*)&" << parent_->getId().getText() << ",\n";
         }
         else {
            out <<
"      .parent = NULL,\n";
         }
         
         if(!children_.empty()) {
            out <<
"      .children = " << this->getId().getText() << "_children,\n" <<
"      .n_allocated_children = sizeof(" << this->getId().getText() << "_children)/sizeof(PPG_Token__*),\n" <<
"      .n_children = sizeof(" << this->getId().getText() << "_children)/sizeof(PPG_Token__*),\n";
         }
         else {
            out <<
"      .children = NULL,\n"
"      .n_allocated_children = 0,\n"
"      .n_children = 0,\n";
         }
         
         if(!action_.getText().empty()) {
            const auto &actionPtr = Action::lookup(action_.getText());
            out <<
"      .action = PPG_ACTION_INITIALIZE_GLOBAL___" << actionPtr->getType().getText() << "("
            << actionPtr->getId().getText();
            if(actionPtr->getParametersDefined()) {
               out << ", " << actionPtr->getParameters().getText();
            }
            out << "), // " 
               << actionPtr->getId().getText() << ": " << actionPtr->getLOD() << "\n";
         }
         else {
            
            out <<
"      .action = { \n"
"         .callback = (PPG_Action_Callback) {\n"
"            .func = NULL,\n"
"            .user_data = NULL\n"
"         }\n"
"      },\n";
         }         
         out <<
"      .misc = (PPG_Misc_Bits) {\n"
"         .state = PPG_Token_Initialized,\n"
"         .flags = ";
         if(!flagCode_.getText().empty()) {
            out << flagCode_.getText() << " | ";
         }
         out << this->getFlags() << ",\n"
"         .action_state = 0,\n"
"         .action_flags = PPG_Action_Default\n"
"      },\n";
         out <<
"      .layer = " << this->layer_.getText() << "\n";
      }
      
      virtual std::string getFlags() const { return "0"; }
      
      virtual std::string getTokenType() const { return "PPG_Token__"; }
      virtual std::string getVTableId() const { return "&ppg_token_vtable"; }
      
      virtual std::string getActionPath() const { return "super.action"; }
      
   protected:
      
      Parser::Token                         action_;
      std::vector<std::shared_ptr<Token>>   children_;
      const Token                           *parent_;
      Parser::Token                         layer_;
      Parser::Token                         flagCode_;
      
      static Parser::Token                  curLayer_;
};

}
}
