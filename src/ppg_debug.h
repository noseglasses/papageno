/* Copyright 2017 Florian Fleissner
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

#ifndef PPG_DEBUG_H
#define PPG_DEBUG_H

/** @file */

#include "ppg_settings.h"

#include <stdlib.h>

#ifdef PPG_BUILDING_FOR_QMK 
#   ifndef USER_PRINT
#      define USER_PRINT
#   endif
#   include "debug.h"
#endif

#if PPG_HAVE_LOGGING

// Define the preprocessor macro PPG_PRINT_SELF_ENABLED
// to enable verbose class output of token classes

#ifdef PPG_BUILDING_FOR_QMK 
#   define PPG_LOG(...) uprintf(__VA_ARGS__);

#else
#   include <stdio.h>
#   define PPG_LOG(...) printf(__VA_ARGS__);
#endif

#endif //PPG_HAVE_LOGGING

#ifdef PPG_BUILDING_FOR_QMK 
   /** @brief A macro for printf style error output
 */
#   define PPG_ERROR(...) uprintf("*** Error: " __VA_ARGS__);

#else

#   include <stdio.h>
   /** @brief A macro for printf style error output
 */
#   define PPG_ERROR(...) printf("*** Error: " __VA_ARGS__);
#endif
      
#if PPG_HAVE_ASSERTIONS
 
/** @brief A macro for assertions of boolean expressions
 */  
#define PPG_ASSERT(...) \
   if(!(__VA_ARGS__)) { \
      PPG_ERROR("%s: %d: Assertion failed: %s\n", __FILE__, __LINE__, #__VA_ARGS__) \
      abort(); \
   }
#else
/** @brief A macro for assertions of boolean expressions
 */
#define PPG_ASSERT(...)
#endif // PPG_HAVE_ASSERTIONS

#ifndef PPG_LOG
/** @brief A macro for printf style output
 */
#define PPG_LOG(...)
#endif

#endif
