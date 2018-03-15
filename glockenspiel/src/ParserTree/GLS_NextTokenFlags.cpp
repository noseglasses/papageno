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

#include "ParserTree/GLS_NextTokenFlags.hpp"
#include "ParserTree/GLS_Token.hpp"

namespace Glockenspiel {
namespace ParserTree {     
   
NextTokenFlags nextTokenFlags;

   NextTokenFlags
      ::NextTokenFlags()
{
}

void 
   NextTokenFlags
      ::init()
{
   {
      FlagFunctions &f = functions_['f'];
         f.enabled_ = false;
         f.apply_ = [](TokenFlags &tf) -> void {
            tf.actionFlags_.set("PPG_Action_Fallback");
         };
   }
}

void  
   NextTokenFlags
      ::setFlags(const std::string &flags)
{
   for(const char &c: flags) {
      auto it = functions_.find(c);
      if(it == functions_.end()) {
         THROW_ERROR("Unable to set token flag \'" << c << "\'");
      }
      it->second.enabled_ = true;
   }
}

void 
   NextTokenFlags
      ::clearFlags()
{
   for(auto &f: functions_) {
      f.second.enabled_ = false;
   }
}

void   
   NextTokenFlags
      ::apply(ParserTree::Token &token)
{
   apply(token.getFlags());
}

void  
   NextTokenFlags
      ::apply(ParserTree::TokenFlags &tf)
{
   for(auto &f: functions_) {
      if(!f.second.enabled_) { continue; }
      f.second.apply_(tf);
   }
   clearFlags();
}
      
} // namespace Parser
} // namespace Glockenspiel
