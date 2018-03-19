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

#include "Settings/GLS_Settings.hpp"
#include "Misc/GLS_StringHandling.hpp"
#include "CommandLine/GLS_CommandLine.hpp"

#include <iostream>

extern int yydebug;

namespace Glockenspiel {
   
Settings settings;

   Settings
      ::Settings()
   :  debug(false),
      join_duplicate_entities(false),
      join_duplicate_actions(false),
      join_duplicate_inputs(false),
      join_note_sequences(false),
      
      allow_auto_type_definition(false),
      allow_auto_input_type_definition(false),
      allow_auto_action_type_definition(false)
{
}

void 
   Settings
      ::set(const std::string &name, const std::string &value)
{
   auto it = setters_.find(name);
   
   if(it == setters_.end()) {
      THROW_ERROR("Unable to find setting \'" << name << "\'");
   }
   
   (it->second)(value);
}

void
   Settings
      ::init()
{
   #define ADD_SETTER(NAME, ...) \
      NAME = Glockenspiel::commandLineArgs.NAME##_flag; \
      setters_[#NAME] = [this](const std::string &value) -> void { \
         std::stringstream s(value); \
         s >> std::boolalpha >> Settings::NAME; \
         if(!s.good()) { \
            THROW_ERROR("Failed reading setting \'" << #NAME \
               << "\' from \'" << value << "\'"); \
         } \
         __VA_ARGS__ \
      };
         
   ADD_SETTER(debug, yydebug = debug;)
      
   ADD_SETTER(join_duplicate_entities)
   ADD_SETTER(join_duplicate_actions)
   ADD_SETTER(join_duplicate_inputs)
   ADD_SETTER(join_note_sequences)
   ADD_SETTER(join_note_sequences)
   
   ADD_SETTER(allow_auto_type_definition)
   ADD_SETTER(allow_auto_input_type_definition)
   ADD_SETTER(allow_auto_action_type_definition)
   
   #define ADD_STRING_SETTER(NAME) \
      if(Glockenspiel::commandLineArgs.NAME##_arg) { \
         NAME = Glockenspiel::commandLineArgs.NAME##_arg; \
      } \
      setters_[#NAME] = [this](const std::string &value) -> void { \
         Settings::NAME = value; \
      };
      
   ADD_STRING_SETTER(macros_prefix)
   ADD_STRING_SETTER(symbols_prefix)
}

} // namespace Glockenspiel
