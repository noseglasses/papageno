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

#include <map>
#include "Parser/PPG_ParserToken.hpp"
#include <vector>
#include <memory>

namespace Glockenspiel {
namespace ParserTree {
   
class Token;

class Phrase : public Node
{
   public:
      
      Phrase(const Parser::Token &id);
      
      virtual std::string getNodeType() const;
      
      const std::vector<std::shared_ptr<Token>> &getTokens() const;
      
      static std::shared_ptr<Phrase> lookupPhrase(const std::string &id);
      
      static void storePhrase(const Parser::Token &id);
      
      static void storePhrase(const std::shared_ptr<Phrase> &phrase);
      
   protected:
      
      std::vector<std::shared_ptr<Token>>                tokens_;
      
      static std::map<std::string, std::shared_ptr<Phrase>>  phrases_;
      static std::map<std::string, Parser::LocationOfDefinition>              locationsOfDefinition_;
};
      
} // namespace ParserTree
} // namespace Glockenspiel
