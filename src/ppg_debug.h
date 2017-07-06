/* Copyright 2017 Florian Fleissner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PPG_DEBUG_H
#define PPG_DEBUG_H

/** @file */

#ifdef DEBUG_PAPAGENO

#if PAPAGENO_PRINT_SELF_ENABLED

/** @brief This macro toggles class output
 */
#define PAPAGENO_PRINT_SELF_ENABLED 1
#endif

#ifdef __AVR_ATmega32U4__ 
#include "debug.h"
#define PPG_PRINTF(...) \
	uprintf(__VA_ARGS__);
#define PPG_ERROR(...) PPG_PRINTF("*** Error: " __VA_ARGS__)
#else
#define PPG_PRINTF(...)
#define PPG_ERROR(...)
#endif
	
#define PPG_ASSERT(...) \
	if(!(__VA_ARGS__)) { \
		PPG_ERROR("Assertion failed: " #__VA_ARGS__ "\n"); \
	}

#else //DEBUG_PAPAGENO

#ifndef PAPAGENO_PRINT_SELF_ENABLED

/** @brief This macro toggles class output
 */
#define PAPAGENO_PRINT_SELF_ENABLED 0
#endif

/** @brief A macro for printf style output
 */
#define PPG_PRINTF(...)

/** @brief A macro for printf style error output
 */
#define PPG_ERROR(...)

/** @brief A macro for assertions of boolean expressions
 */
#define PPG_ASSERT(...)

#endif //DEBUG_PAPAGENO

#endif
