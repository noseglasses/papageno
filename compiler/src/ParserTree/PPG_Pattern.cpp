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

#include "ParserTree/PPG_Pattern.hpp"

#include "ParserTree/PPG_Token.hpp"
#include "ParserTree/PPG_Phrase.hpp"
#include "Misc/PPG_StringHandling.hpp"

namespace Papageno {
namespace ParserTree {
   
std::vector<std::shared_ptr<Token>> Pattern::tokens_;
      
std::shared_ptr<Token> Pattern::root_;
      
int Pattern::sequenceStart_ = 0;
      
const std::shared_ptr<Token> &
   Pattern
      ::getMostRecentToken()
{
   assert(!tokens_.empty());
   return tokens_.back();
}
      
void 
   Pattern
      ::pushToken(const std::shared_ptr<Token> &token)
{
   tokens_.push_back(token);
   sequenceStart_ = tokens_.size() - 1;
}

void  
   Pattern
      ::pushPhrase(const Parser::Token &id) 
{
   auto phrasePtr = Phrase::lookupPhrase(id.getText());
   
   const auto &tokens = phrasePtr->getTokens();
   
   std::copy(tokens.begin(), tokens.end(), std::back_inserter(tokens_));
}

void  
   Pattern
      ::repeatLastToken(const Parser::Token &countString)
{   
   auto count = Papageno::Misc::atol(countString);
   
   for(int i = 0; i < (count - 1); ++i) {
      
      tokens_.push_back(tokens_.back()->clone());
   }
}

void  
   Pattern
      ::applyActions()
{   
   std::vector<Action::CountToAction> actions = Action::getNextActions();
   
   for(const auto &cta: actions) {
      
      int targetTokenPos = 0;
      
      assert(cta.first != 0);
      
      if(cta.first > 0) {
         targetTokenPos = sequenceStart_ + cta.first - 1;
      }
      else {
         targetTokenPos = tokens_.size() + cta.first;
      }
         
      if(targetTokenPos >= tokens_.size()) {
         THROW_ERROR("Unable to apply action to token " << cta.first
            << " of sequence");
      }
      
      tokens_[targetTokenPos]->setAction(Action::lookupAction(cta.second.getText()));
   }
}
      
void  
   Pattern
      ::getTokens(std::vector<std::shared_ptr<Token>> &tokens)
{
   tokens.clear();
   tokens.swap(tokens_);
}

void  
   Pattern
      ::finishPattern()
{   
   if(!root_) {
      root_ = std::make_shared<Token>();
   }
   
   std::shared_ptr<Token> curToken = root_;
   int pos = 0;
   bool redundantTokenFound = false;
   
   while(curToken->hasChildren()) {
      
      if(pos >= tokens_.size()) { break; }
   
      // Check all children of the current token
      //
      const auto &children = curToken->getChildren();
   
      for(const auto &childToken: children) {
         
         // If a child is equal to the token in the new pattern
         // at the current position, we continue with
         // the next tokens children
         //
         if(childToken->isEqual(*tokens_[pos])) {
            if(*childToken->getAction() != *tokens_[pos]->getAction()) {
               THROW_ERROR("Unable to insert pattern. Conflict between tokens " 
                  << childToken->getLOD() << " and " << tokens_[pos]->getLOD() << ".");
            }
            redundantTokenFound = true;
            curToken = childToken;
            ++pos;
            break;
         }
      }
      
      if(!redundantTokenFound) { 
         break;
      }
   }
      
   if(pos >= tokens_.size()) { 
      // The current pattern is a complete and already defined subpattern
      // of the pattern tree
      return;
   }
   
   insertChildPatterns(curToken, pos);
   
   tokens_.clear();
}

void  
   Pattern
      ::insertChildPatterns(std::shared_ptr<Token> subtreeRoot, int startPos)
{
   for(int pos = startPos; pos < tokens_.size(); ++pos) {
      subtreeRoot->addChild(tokens_[pos]);
      subtreeRoot = tokens_[pos];
   }
}

std::shared_ptr<Token>  
   Pattern
      ::getTreeRoot()
{ 
   return root_; 
}
   
} // namespace ParserTree
} // namespace Papageno

