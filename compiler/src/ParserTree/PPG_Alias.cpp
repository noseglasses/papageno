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

#include "ParserTree/PPG_Alias.hpp"
#include "Misc/PPG_ErrorHandling.hpp"

namespace Papageno {
namespace ParserTree {

std::map<std::string, Parser::Token> Alias::aliasMapping_;
      
void
   Alias
      ::define(const std::string &name, const Parser::Token &target)
{
   auto it = aliasMapping_.find(name);
   
   if(it != aliasMapping_.end()) {
      THROW_ERROR("Redundant definition of alias \'" << name << "\' encountered. "
         "This is the first definition encountered: " << it->second);
   }
   
   aliasMapping_[name] = target;
}
      
std::string 
   Alias
      ::replace(const std::string &name)
{
   auto it = aliasMapping_.find(name);
   
   constexpr int maxRepl = 100;
   int repl = 0;
   
   std::string result = name;
   while(it != aliasMapping_.end()) {
      result = it->second.getText();
      it = aliasMapping_.find(result);
      ++repl;
      if(repl > maxRepl) {
         THROW_ERROR("Unable to replace alias \'" << name << "\'. Alias definition loop encountered.");
      }
   }
   
   return result;
}

} // namespace ParserTree
} // namespace Papageno
