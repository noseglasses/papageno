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

namespace Glockenspiel {
   
Settings::NameToSetter Settings::setters_;
   
bool Settings::join_duplicate_entities;
bool Settings::join_duplicate_actions;
bool Settings::join_duplicate_inputs;
bool Settings::join_note_sequences;

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
   #define ADD_SETTER(NAME, INITIAL_VALUE) \
      NAME = INITIAL_VALUE; \
      setters_[#NAME] = [](const std::string &value) -> void { \
         std::stringstream s; \
         s.str(value); \
         s >> Settings::NAME; \
         if(!s) { \
            THROW_ERROR("Failed reading setting \'" << #NAME \
               << "\' from \'" << value << "\'"); \
         } \
      };
         
   ADD_SETTER(join_duplicate_entities, Glockenspiel::commandLineArgs.join_duplicate_entities_flag)
   ADD_SETTER(join_duplicate_actions, Glockenspiel::commandLineArgs.join_duplicate_actions_flag)
   ADD_SETTER(join_duplicate_inputs, Glockenspiel::commandLineArgs.join_duplicate_inputs_flag)
   ADD_SETTER(join_note_sequences, Glockenspiel::commandLineArgs.join_note_sequences_flag)
}

} // namespace Glockenspiel
