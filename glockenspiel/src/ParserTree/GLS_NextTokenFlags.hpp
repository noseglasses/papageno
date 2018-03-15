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

#include <map>
#include <functional>

namespace Glockenspiel {
namespace ParserTree {
   
struct TokenFlags;
class Token;
   
class NextTokenFlags
{
   public:
      
      typedef std::function<void(ParserTree::TokenFlags &)> ApplyFunction;
      
      struct FlagFunctions {
         ApplyFunction apply_;
         bool enabled_;
      };
      
      typedef std::map<char, FlagFunctions> CharToFunctions;
      
      NextTokenFlags();
      
      void init();
      
      void setFlags(const std::string &);
      
      void apply(ParserTree::Token &token);
      void apply(ParserTree::TokenFlags &tf);
      
   private:
      
      void clearFlags();
      
   private:
      
      CharToFunctions functions_;
};

extern NextTokenFlags nextTokenFlags;

} // namespace ParserTree
} // namespace Glockenspiel
