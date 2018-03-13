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

#include "Parser/PPG_ParserToken.hpp"

#include <vector>
#include <memory>

namespace Papageno {
namespace ParserTree {
   
class Token;
     
class Pattern
{
   public:
      
      static const std::shared_ptr<Token> &getMostRecentToken();
      
      static void pushToken(const std::shared_ptr<Token> &token);
      
      static void pushPhrase(const Parser::Token &id);
      
      static void repeatLastToken(const Parser::Token &countString);
      
      static void addAlphaSequence(const std::string &alphaSeqString);
      
      static void applyActions();
            
      static void getTokens(std::vector<std::shared_ptr<Token>> &tokens);
      
      static void finishPattern();
      
      static void insertChildPatterns(std::shared_ptr<Token> subtreeRoot, int startPos);
      
      static std::shared_ptr<Token> getTreeRoot();
      
      static void performSequenceReplacement();

   protected:
      
      static std::vector<std::shared_ptr<Token>> tokens_;
      
      static std::shared_ptr<Token> root_;
      
      static int sequenceStart_;
      
};
} // namespace ParserTree
} // namespace Papageno
