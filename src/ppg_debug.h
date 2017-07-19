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

#ifdef PPG_HAVE_DEBUG

// Define the preprocessor macro PPG_PRINT_SELF_ENABLED
// to enable verbose class output of token classes

#ifdef __AVR_ATmega32U4__ 
#   include "debug.h"
#   define PPG_LOG(...) \
      uprintf(__VA_ARGS__);
      
#ifdef PPG_HAVE_ASSERTIONS
   
#define PPG_ASSERT(...) \
   if(!(__VA_ARGS__)) { \
      PPG_ERROR("Assertion failed: " #__VA_ARGS__ "\n"); \
   }
#endif // PPG_HAVE_ASSERTIONS

#else

#   include <stdio.h>

#   define PPG_LOG(...) \
      printf(__VA_ARGS__);
      
#ifdef PPG_HAVE_ASSERTIONS
#   include <assert.h>
#   define PPG_ASSERT(...) assert(__VA_ARGS__)
#endif // PPG_HAVE_ASSERTIONS
#endif

#   define PPG_ERROR(...) PPG_LOG("*** Error: " __VA_ARGS__)

#endif //PPG_HAVE_DEBUG

#ifndef PPG_LOG
/** @brief A macro for printf style output
 */
#define PPG_LOG(...)
#endif

#ifndef PPG_ERROR
/** @brief A macro for printf style error output
 */
#define PPG_ERROR(...)
#endif

#ifndef PPG_ASSERT
/** @brief A macro for assertions of boolean expressions
 */
#define PPG_ASSERT(...)
#endif

#endif
