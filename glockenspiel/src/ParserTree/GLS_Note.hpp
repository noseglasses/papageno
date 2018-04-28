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

#include "ParserTree/GLS_Token.hpp"
#include "Parser/GLS_ParserToken.hpp"
#include "ParserTree/GLS_Input.hpp"

namespace Glockenspiel {
namespace ParserTree {
     
class Note : public Token
{
   public:
      
      Note(const Parser::Token &input = Parser::Token())
      {
         input_ = (!input.getText().empty()) ? input : Input::popNext();
       
         if(Action::hasNextEntities()) {
            this->setAction(Action::popNext().second);
         }
      }
      
      const Parser::Token &getInput() const {
         return input_;
      }
      
      const std::shared_ptr<Input> &getInputPtr() const {
         return Input::lookup(input_.getText());
      }
            
      virtual std::string getPropertyDescription() const override {
         return TO_STRING(Node::getPropertyDescription() << ", input = " 
            << this->getInputPtr()->getDescription());
      }
      
      virtual std::string getNodeType() const override { return "Note"; }
      
      virtual bool isEqual(const Token &other) const override {
         auto otherNote = dynamic_cast<const Note *>(&other);
         if(!otherNote) { return false; }
         return *otherNote->getInputPtr() == *this->getInputPtr();
      }
      
      virtual std::shared_ptr<Token> clone() const override {
         return std::make_shared<Note>(*this);
      }
      
      virtual int getNumInputs() const override { return 1; }
      
      virtual void collectInputAssignments(InputAssignmentsByTag &iabt) const override {
         std::ostringstream path;
         path << this->getId().getText() << ".input";
         const auto &input = this->getInputPtr();
         iabt[input->getType().getText()].push_back( 
            (InputAssignment) { 
               path.str(),
               input
            }
         );
      }
      
      virtual void touchActionsAndInputs() override {
         this->Token::touchActionsAndInputs();
         this->getInputPtr()->setWasRequested(true);
      }
      
      virtual std::string getInputs() const override { return input_.getText(); }
      
   protected:
      
      virtual void generateCCodeInternal(std::ostream &out) const override;
      
      virtual std::string getVTableId() const override { return "&ppg_note_vtable"; }
      
      virtual std::string getTokenType() const override { return "PPG_Note"; }
      
   protected:
      
      Parser::Token input_;
};

} // namespace ParserTree
} // namespace Glockenspiel
   
