/* Copyright 2017 noseglasses <shinynoseglasses@gmail.com>
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
#include "ppg_context.h"

#include <stdlib.h>
#include <stdbool.h>

#ifdef PPG_BUILDING_FOR_QMK 
#   ifndef USER_PRINT
#      define USER_PRINT
#   endif
#   include "debug.h"
#endif

// Allow to force logging for arduino builds where 
// no additional command line macro definitions are possible.
//
// Be careful. The strings used for output can cause a quite large 
// binary that might exceed the resources available.
//
// #ifdef ARDUINO
// #   ifdef PPG_HAVE_LOGGING
// #      undef PPG_HAVE_LOGGING
// #   endif
// #   define PPG_HAVE_LOGGING 1
// #endif

// #define PPG_FORCE_LOGGING

#ifdef PPG_FORCE_LOGGING
#   ifdef PPG_HAVE_LOGGING
#      undef PPG_HAVE_LOGGING
#   endif
#   define PPG_HAVE_LOGGING 1
#endif

#if 0
#if defined(ARDUINO)

void serial_print(const char *c);
#   define PPG_SERIAL_PRINT(...) \
   { \
      char buffer[128]; \
      snprintf(buffer, 128, __VA_ARGS__); \
      serial_print(buffer); \
   }
#endif
#endif

#if PPG_HAVE_LOGGING

#   include <stdio.h>

// Define the preprocessor macro PPG_PRINT_SELF_ENABLED
// to enable verbose class output of token classes

#ifdef PPG_BUILDING_FOR_QMK 
#   define PPG_LOG(...) \
   if(ppg_logging_get_enabled()) { \
      uprintf("PPG: " __VA_ARGS__); \
   }


//    
#else
// #   include <stdio.h>

#   define PPG_LOG(...) \
   if(ppg_logging_get_enabled()) { \
      printf(__VA_ARGS__); \
   }
   
#endif

// #define PPG_LOG_TOKEN_LOOKUP_ENABLED
#ifdef PPG_LOG_TOKEN_LOOKUP_ENABLED
#   define PPG_LOG_TOKEN_LOOKUP(...) \
      PPG_LOG("TL: " __VA_ARGS__)
#endif

/** @brief Dynamically toggle logging output
 */
bool ppg_logging_set_enabled(bool state);

/** @brief Get state of dynamic logging settings
 */
bool ppg_logging_get_enabled(void);
   
/** @brief A macro to dynamically toggle logging. 
 * 
 * This macro helps to prevent
 * undefined symbol errors when logging is suppressed in optimized builds.
 */
#define PPG_LOGGING_SET_ENABLED(STATE) \
   ppg_logging_set_enabled(STATE);
#else
   
/** @brief A macro to dynamically toggle logging. 
 * 
 * This macro helps to prevent
 * undefined symbol errors when logging is suppressed in optimized builds.
 */
#define PPG_LOGGING_SET_ENABLED(STATE)

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
#define PPG_LOG_NOOP
#endif

#ifndef PPG_LOG_TOKEN_LOOKUP
#   define PPG_LOG_TOKEN_LOOKUP(...)
#endif

#define PPG_UNUSED(X) (void)(X)

#endif
