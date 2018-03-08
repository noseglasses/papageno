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

#include "Parser/PPG_Parser.hpp"

#include <sstream>
#include <stdexcept>

extern YYLTYPE yylloc;

#define TO_STRING_WITH_LOCATION(...) ([&]() -> std::string { \
      std::ostringstream tmp; \
      if(Papageno::Parser::currentFileParsed) { \
         tmp << Papageno::Parser::currentFileParsed << ":"; \
      } \
      if(Papageno::Parser::currentLocation) { \
         tmp << Papageno::Parser::currentLocation.location_.first_line << ":" \
            << Papageno::Parser::currentLocation.location_.first_column; \
      } \
      else { \
         tmp << yylloc.first_line << ":" << yylloc.first_column; \
      } \
      tmp << ": error: " << __VA_ARGS__; \
      return tmp.str(); \
   }() \
)

#define THROW_ERROR(...) \
   throw std::runtime_error(TO_STRING_WITH_LOCATION(__VA_ARGS__))
   
