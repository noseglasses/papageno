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

#include "Settings/GLS_Defaults.hpp"
#include "Misc/GLS_ErrorHandling.hpp"
#include "Generator/GLS_Prefix.hpp"

#define SP Glockenspiel::Generator::symbolsPrefix()
#define MP Glockenspiel::Generator::macroPrefix()

#include <iostream>
#include <algorithm>

namespace Glockenspiel {
   
Defaults defaults;

   Defaults
      ::Defaults()
{
}

void 
   Defaults
      ::set(const std::string &name, const std::string &value)
{
   auto it = defaults_.find(name);
   
   if(it == defaults_.end()) {
      THROW_ERROR("Unable to set unknown default \'" << name << "\'");
   }
   
   it->second = value;
}

void
   Defaults
      ::init()
{
   #define DEFAULT(NAME, VALUE) \
      defaults_[NAME] = VALUE;
      
   DEFAULT("layer", "0");
   DEFAULT("time_function", "ppg_default_time")
   DEFAULT("time_difference_function", "ppg_default_time_difference")
   DEFAULT("time_comparison_function", "ppg_default_time_comparison")
   DEFAULT("timeout_enabled", "true")
   DEFAULT("event_timeout", "1")
   DEFAULT("papageno_enabled", "true")
   DEFAULT("logging_enabled", "true")
   DEFAULT("abort_trigger_input", "(PPG_Input_Id)((uintptr_t)-1)")
   DEFAULT("event_processor", "NULL")
   DEFAULT("signal_callback_func", "NULL")
   DEFAULT("signal_callback_user_data", "NULL")
   DEFAULT("logging", "false");
}

namespace {
std::string 
   myToupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
                // static_cast<int(*)(int)>(std::toupper)         // wrong
                // [](int c){ return std::toupper(c); }           // wrong
                // [](char c){ return std::toupper(c); }          // wrong
                   [](unsigned char c){ return std::toupper(c); } // correct
                  );
    return s;
}
}

void 
   Defaults 
      ::outputC(std::ostream &out)
{
   for(const auto &entry: defaults_) {
      out <<
"#ifndef " << MP << "GLS_INITIAL_" << myToupper(entry.first) << "\n"
"#define " << MP << "GLS_INITIAL_" << myToupper(entry.first) << " " << entry.second << "\n" <<
"#endif\n"
"\n";
   }
}

} // namespace Glockenspiel
