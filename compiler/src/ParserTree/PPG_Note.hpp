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

#include "ParserTree/PPG_Token.hpp"

namespace Papageno {
namespace ParserTree {
     
class Note : public Token
{
   public:
      
      Note(int flags = PPG_Note_Flags_A_N_D)
         :  flags_(flags)
      {
         input_ = Input::popInput();
       
         if(Action::hasNextActions()) {
            this->setAction(Action::popNextAction());
         }
      }
            
      virtual std::string getPropertyDescription() const override {
         return TO_STRING(Node::getPropertyDescription() << ", input = " 
            << input_->getDescription() << ", flags = " << flags_);
      }
      
      virtual std::string getNodeType() const override { return "Note"; }
      
      virtual bool isEqual(const Token &other) const override {
         auto otherNote = dynamic_cast<const Note *>(&other);
         if(!otherNote) { return false; }
         return otherNote->input_->isEqual(*input_);
      }
      
      virtual std::shared_ptr<Token> clone() const override {
         return std::makeShared<Note>(*this);
      }
      
   protected:
      
      virtual void generateCCodeInternal(std::ostream &out) const override {
         
         out <<
"PPG_Note " << this->getId() << " (PPG_Note) {\n";
"   .super = \n"
"   {\n";
         this->Token::generateCCode(out);
         out <<
"   },
"   .input = PPG_INPUT_INITIALIZATION_" << input_->getType()
   << "(" << input->getParameters() << ")\n";
         out <<
"};\n\n";
      }
      
   protected:
      
      std::shared_ptr<Input> input_;
      
      int flags_;
};

} // namespace ParserTree
} // namespace Papageno
   
