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

#include "Parser/PPG_ParserToken.hpp"
#include "Misc/PPG_ErrorHandling.hpp"

#include <cstdlib>
#include <climits>
#include <sstream>

#define TO_STRING(...) ([&]() -> std::string { \
      std::ostringstream tmp; \
      tmp << __VA_ARGS__; \
      return tmp.str(); \
   }() \
)

namespace Glockenspiel {
namespace Misc {
   
inline long atol(const Parser::Token &token)
{
   errno = 0;    /* To distinguish success/failure after call */
   char *dummy = NULL;
   long val = strtol(token.getText().c_str(), &dummy, 10);

   /* Check for various possible errors */

   if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            || (errno != 0 && val == 0)) {
      THROW_TOKEN_ERROR(token, "Failed to convert tap count to integer\n");
   }
   
   return val;
}

} // namespace Misc
} // namespace Glockenspiel
