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
   
struct BinaryFlags {
   std::string string_;
   void set(const std::string &flags) {
      string_ += " | " + flags;
   }
   void unset(const std::string &flags) {
      string_ = "(" + string_ + ") &= ~(" + flags + ")";
   }
};
      
struct TokenFlags {
   BinaryFlags tokenFlags_;
   BinaryFlags actionFlags_;
};

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

      const TokenFlags &getFlags() const {
         return flags_;
      }
      TokenFlags &getFlags() {
         return flags_;
      }
      
      void setLayer(const std::string &layer) {
         layer_ = Parser::Token(layer);
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
      
      void outputCTokenDeclaration(std::ostream &out) const;
      
      void generateCCode(std::ostream &out) const;
      
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
           
      virtual std::string getInputs() const { return std::string(); }
           
      void setFlagsString(const std::string &flagChars) {
         for(const auto &fc: flagChars) {
            switch(fc) {
               case ' ':
               case '\t':
               case '\n':
                  break;
               default:
                  this->setFlagChar(fc);
            }
         }
      }
      
   protected:
      
      virtual void generateDependencyCodeInternal(std::ostream &out) const {}
      
      virtual void generateCCodeInternal(std::ostream &out) const;
      
      virtual void setFlagChar(char flagChar) {
         switch(flagChar) {
            case 'f':
               flags_.actionFlags_.set("PPG_Action_Fallback");
               break;
            default:
            {
               THROW_ERROR("Unable to consider flag character \'" << flagChar
                  << "\'");
            }
         }
      }
      
      virtual std::string getTokenType() const { return "PPG_Token__"; }
      virtual std::string getVTableId() const { return "&ppg_token_vtable"; }
      
      virtual std::string getActionPath() const { return "super.action"; }
      
   protected:
      
      Parser::Token                         action_;
      std::vector<std::shared_ptr<Token>>   children_;
      const Token                           *parent_;
      Parser::Token                         layer_;
      ParserTree::TokenFlags                flags_;
      
      static Parser::Token                  curLayer_;
};

}
}
