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
#include <set>
#include "Parser/PPG_ParserToken.hpp"

#include "ParserTree/PPG_Node.hpp"

namespace Papageno {
namespace ParserTree {
   
class Input : public Node
{
   public:
      
      Input(  const Parser::Token &id,
               const Parser::Token &type, 
               const Parser::Token &parameters);
      
      static void define(const std::shared_ptr<Input> &input);
      
      static const std::shared_ptr<Input> &lookupInput(const std::string &id);
      
      static void pushNextInput(const Parser::Token &id);
      
      static std::shared_ptr<Input> popNextInput();
      
      static void getInputs(std::vector<Parser::Token> &inputs);
      
      virtual std::string getPropertyDescription() const;
      
      virtual std::string getNodeType() const;
            
      typedef std::vector<std::shared_ptr<Input>> InputCollection;
      typedef std::map<std::string, InputCollection> InputsByType;
      
      static InputsByType getInputsByType();
      
      static const std::map<std::string, std::shared_ptr<Input>> &getInputs();
      
      const Parser::Token &getType() const;
      const Parser::Token &getParameters() const;
      
   protected:
      
      Parser::Token           type_;
      Parser::Token           parameters_;
      
      static std::set<Parser::Token, Parser::TokenCompare> nextInputs_;
      
      static std::map<std::string, std::shared_ptr<Input>> inputs_;
      static std::map<std::string, YYLTYPE> locationsOfDefinition_;
};

bool inputsEqual(std::vector<Parser::Token> i1,
                 std::vector<Parser::Token> i2);

inline
bool operator==(const Input &i1, const Input &i2) 
{
   return i1.getId().getText() == i2.getId().getText();
}

} // namespace ParserTree
} // namespace Papageno

