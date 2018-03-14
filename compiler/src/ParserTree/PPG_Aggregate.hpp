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

#include "ParserTree/PPG_Token.hpp"

#include <vector>
#include <memory>

namespace Glockenspiel {
namespace ParserTree {
   
class Input;
class Token;
   
class Aggregate : public Token
{
   public:
   
      virtual std::string getPropertyDescription() const override;
      
      virtual bool isEqual(const Token &other) const override;
      
      virtual int getNumInputs() const override { return inputs_.size(); }
      
      virtual void collectInputAssignments(InputAssignmentsByTag &iabt) const override;
            
      virtual void touchActionsAndInputs() override;
      
   protected:
      
      virtual void generateDependencyCodeInternal(std::ostream &out) const override;
      
      virtual void generateCCodeInternal(std::ostream &out) const override;
      
      virtual std::string getEntitiesPath() const { return "inputs"; }
      
      Aggregate();
      
   protected:
      
      std::vector<Parser::Token>   inputs_;
};
   
} // namespace ParserTree
} // namespace Glockenspiel
