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

#include "Settings/GLS_Settings.hpp"

#include <string>

namespace Glockenspiel {
namespace Generator {
   
inline
std::string macroPrefix() {
   if(settings.macros_prefix.empty()) {
      return "";
   }
   return settings.macros_prefix + "_";
}

inline
std::string symbolsPrefix() {
   if(settings.symbols_prefix.empty()) {
      return "";
   }
   return settings.symbols_prefix + "_";
} 

} // namespace Generator
} // namespace Glockenspiel
