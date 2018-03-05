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

#include <string>
#include <cstdlib>
#include <climits>

namespace Papageno {
namespace Misc {
   
   static long atol(const std::string &countString)
   {
      errno = 0;    /* To distinguish success/failure after call */
      char *dummy = NULL;
      long val = strtol(countString.c_str(), &dummy, 10);

      /* Check for various possible errors */

      if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
               || (errno != 0 && val == 0)) {
         THROW_ERROR("Failed to convert tap count " << countString << " to integer\n");
      }
      
      return val;
   }
} // namespace Misc
} // namespace Papageno
