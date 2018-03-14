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

#include <functional>
#include <string>
#include <map>

namespace Glockenspiel {
   
class Settings 
{
   public:
         
      typedef std::function<void(const std::string &)> SetterFunction;
      typedef std::map<std::string, SetterFunction> NameToSetter;
      
      static void set(const std::string &name, const std::string &value);
      
      static void init();
      
      static bool join_duplicate_entities;
      static bool join_duplicate_actions;
      static bool join_duplicate_inputs;
      static bool join_note_sequences;
      
   protected:
      
      static NameToSetter setters_;
};

} // namespace Glockenspiel
