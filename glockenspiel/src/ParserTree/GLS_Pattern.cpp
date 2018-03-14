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

#include "ParserTree/GLS_Pattern.hpp"

#include "ParserTree/GLS_Token.hpp"
#include "ParserTree/GLS_Phrase.hpp"
#include "ParserTree/GLS_Input.hpp"
#include "ParserTree/GLS_Note.hpp"
#include "ParserTree/GLS_Sequence.hpp"
#include "Misc/GLS_StringHandling.hpp"

#include <algorithm>

namespace Glockenspiel {
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
   auto count = Glockenspiel::Misc::atol(countString);
   
   for(int i = 0; i < (count - 1); ++i) {
      
      tokens_.push_back(tokens_.back()->clone());
   }
}

void
   Pattern
      ::addAlphaSequence(const std::string &alphaSeqString)
{
   std::string alphaSeqLower = alphaSeqString;
   std::transform(alphaSeqLower.begin(), alphaSeqLower.end(), alphaSeqLower.begin(), ::tolower);
   for(std::size_t i = 0; i < alphaSeqLower.size(); ++i) {
      std::string inputId(alphaSeqLower.substr(i, 1));
      
      auto newNote = std::make_shared<Note>("PPG_Note_Flags_A_N_D", inputId);
       
      tokens_.push_back(newNote);  
   }
}

void  
   Pattern
      ::applyActions()
{   
   std::vector<CountToAction> actions;
   Action::getNextEntities(actions);
   
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
      
      tokens_[targetTokenPos]->setAction(cta.second);
   }
   
   bool haveFallback = false;
   for(std::size_t i = sequenceStart_; i < tokens_.size(); ++i) {
      auto &token = tokens_[i];
      if(!token->getAction().getText().empty()) {
         haveFallback = true;
      }
      else if(haveFallback) {
         token->setActionFlagCode("PPG_Action_Fallback");
      }
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
   auto root = Pattern::getTreeRoot();
   
   std::shared_ptr<Token> curToken = root;
   int pos = 0;
   
   while(curToken->hasChildren()) {
      
      if(pos >= tokens_.size()) { break; }
   
      bool redundantTokenFound = false;
   
      // Check all children of the current token
      //
      const auto &children = curToken->getChildren();
   
      for(const auto &childToken: children) {
         
         // If a child is equal to the token in the new pattern
         // at the current position, we continue with
         // the next tokens children
         //
         if(   !childToken->getChildren().empty() // only share interior nodes
            && (pos + 1 < tokens_.size())
            && childToken->isEqual(*tokens_[pos])) {
            
            const auto &action1 = Alias::replace(childToken->getAction().getText());
            const auto &action2 = Alias::replace(tokens_[pos]->getAction().getText());
            
            if(!action1.empty() || !action2.empty()) {
               if(   (!action1.empty() && action2.empty())
                  || (action1.empty() && !action2.empty())
                  || (action1 != action2)
               ) {
                  if(childToken->getLayer().getText() == tokens_[pos]->getLayer().getText()) {
                     THROW_ERROR("Ambiguous pattern define :" << childToken->getLOD()
                        << " vs. " << tokens_[pos]->getLOD());

                  }
               }
            }
            
            const auto &layer1 = childToken->getLayer().getText();
            const auto &layer2 = tokens_[pos]->getLayer().getText();
            
            if(layer1 != layer2) {
               std::ostringstream s;
               s << "((" << layer1 << ") < (" << layer2
                  << ")) ? (" << layer1 << ") : (" 
                  << layer2 << ")";
               childToken->setLayer(s.str());
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
      tokens_[pos]->setParent(*subtreeRoot);
      tokens_[pos]->touchActionsAndInputs();
      subtreeRoot->addChild(tokens_[pos]);
      subtreeRoot = tokens_[pos];
   }
}

std::shared_ptr<Token>  
   Pattern
      ::getTreeRoot()
{ 
   if(!root_) {
      root_ = std::make_shared<Token>();
   }
   return root_; 
}

void   
   Pattern
      ::performSequenceReplacement()
{
   for(const auto &childToken: root_->getChildren()) {
      performSequenceReplacement(childToken, *root_);
   }
}

void   
   Pattern
      ::performSequenceReplacement(std::shared_ptr<Token> from,
                                   Token &parent)
{
   auto curToken = from;
   
   // Walk along a branch until the next token is found that has 
   // either more or less than one child or has an action or is not a Note.
   //
   while(   (std::dynamic_pointer_cast<Note>(curToken))
         && (curToken->getChildren().size() == 1)
         && (curToken->getAction().getText().empty())
   ) {
      if(!std::dynamic_pointer_cast<Note>(curToken->getChildren()[0])) {
         break;
      }
      curToken = curToken->getChildren()[0];
   }
   
   // If the sequence is longer than one token
   //
   if(from.get() != curToken.get()) {
   
      auto sequence = replaceNoteRangeBySequence(from, curToken);
   
      auto &parentChildren = parent.getChildren();
      
      // Replace the "from" token by the new sequence token
      //
      for(std::size_t i = 0; i < parentChildren.size(); ++i) {
         if(parentChildren[i].get() == from.get()) {
            parentChildren[i] = sequence;
            sequence->setParent(parent);
            break;
         }
      }
      
      from = sequence;
   }
   
   // Traverse all children
   //
   for(auto &child: from->getChildren()) {
      performSequenceReplacement(child, *from);
   }
}

std::shared_ptr<Token>
   Pattern
      ::replaceNoteRangeBySequence(
                         const std::shared_ptr<Token> &from,
                         const std::shared_ptr<Token> &to)
{
   auto newSequence = std::make_shared<Sequence>();
   
   auto curToken = from;
   
   while(curToken.get() != to.get()) {
      
      auto notePtr = std::dynamic_pointer_cast<Note>(curToken);
      assert(notePtr);
      newSequence->addInput(notePtr->getInput());
      assert(curToken->getChildren().size() == 1);
      curToken = curToken->getChildren()[0];
   }
   
   newSequence->setAction(to->getAction());
   newSequence->setChildren(to->getChildren());
   
   for(const auto &token: newSequence->getChildren()) {
      token->setParent(*newSequence);
   }
   
   return newSequence;
}
   
} // namespace ParserTree
} // namespace Glockenspiel

