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

#include "Parser/GLS_Parser.hpp"

#include <sstream>
#include <stdexcept>

extern YYLTYPE yylloc;

#define TO_STRING_WITH_LOCATION(...) ([&]() -> std::string { \
      std::ostringstream tmp; \
      if(Glockenspiel::Parser::currentFileParsed) { \
         tmp << Glockenspiel::Parser::currentFileParsed << ":"; \
      } \
      if(Glockenspiel::Parser::currentLocation) { \
         tmp << Glockenspiel::Parser::currentLocation.location_.first_line << ":" \
            << Glockenspiel::Parser::currentLocation.location_.first_column; \
      } \
      tmp << ": error: " << __VA_ARGS__; \
      return tmp.str(); \
   }() \
)

#define THROW_ERROR(...) \
   throw std::runtime_error(TO_STRING_WITH_LOCATION(__VA_ARGS__))
   
