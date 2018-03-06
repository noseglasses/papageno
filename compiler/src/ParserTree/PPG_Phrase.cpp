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

#include "ParserTree/PPG_Phrase.hpp"

#include "ParserTree/PPG_Token.hpp"
#include "ParserTree/PPG_Pattern.hpp"

#include <map>
#include "Parser/PPG_ParserToken.hpp"
#include <memory>

namespace Papageno {
namespace ParserTree {
  
   Phrase
      ::Phrase(const Parser::Token &id) 
   :  Node(id)
{
   Pattern::getTokens(tokens_);
}

std::string
   Phrase
      ::getNodeType() const 
{
   return "Phrase"; 
}

const std::vector<std::shared_ptr<Token>> & 
   Phrase
      ::getTokens() const
{
   return tokens_;
}

std::shared_ptr<Phrase>  
   Phrase
      ::lookupPhrase(const std::string &id)
{
   auto it = phrases_.find(id);
   
   if(it == phrases_.end()) {
      THROW_ERROR("Unable to find phrase " << id);
   }
   
   return it->second;
}

void  
   Phrase
      ::storePhrase(const Parser::Token &idToken) 
{   
   const auto &id = idToken.getText();
   
   auto it = phrases_.find(id);
   if(it != phrases_.end()) {
      auto lodIt = locationsOfDefinition_.find(id);
      THROW_TOKEN_ERROR(idToken, 
         "Phrase multiply defined. First definition here: " 
         << REPORT_LOCATION(lodIt->second));
   }
   
   auto newPhrase = std::make_shared<Phrase>(idToken);
   Pattern::getTokens(newPhrase->tokens_);
   
   phrases_[id] = newPhrase;
   locationsOfDefinition_[id] = idToken.getLocation();
}

void  
   Phrase
      ::storePhrase(const std::shared_ptr<Phrase> &phrase)
{
   phrases_[phrase->getId().getText()] = phrase;
}
      
} // namespace ParserTree
} // namespace Papageno
