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

#include <map>
#include <string>
#include <memory>

namespace Papageno {
namespace ParserTree {

class Phrase : public Node
{
   public:
      
      Phrase(const std::string &id) 
         :  Node(id)
      {
         Pattern::getTokens(tokens_);
      }
      
      virtual std::string getNodeType() const { return "Phrase"; }
      
      const std::vector<std::shared_ptr<Token>> &getTokens() { return tokens_; }
      
      static std::shared_ptr<Phrase> lookupPhrase(const std::string &id)
      {
         auto it = phrases_.find(id);
         
         if(it == phrases_.end()) {
            THROW_ERROR("Unable to find phrase " << id)
         }
         
         return it->second;
      }
      
      static void storePhrase(const std::string &id) {
         
         auto newPhrase = std::makeShared<Phrase>(id);
         Pattern::getTokens(newPhrase->phraseRoot_);
         phrases_[newPhrase->getId()] = newPhrase;
      }
      
      static void storePhrase(const std::shared_ptr<Phrase> &phrase) {
         phrases_[phrase->getId()] = phrase;
      }
      
   protected:
      
      std::vector<std::shared_ptr<Token>>                tokens_;
      
      static std::map<std::string, std::shared_ptr<Phrase>>  phrases_;
};
      
} // namespace ParserTree
} // namespace Papageno
