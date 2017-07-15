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

#ifndef PPG_TYPES_H
#define PPG_TYPES_H

/** @file */

#include "stdint.h"

// The maximum number of events that can be stored
//
#ifndef PPG_MAX_EVENTS

/** @brief The maximum lenght of a sequence of events that can represent a pattern.
 */
#define PPG_MAX_EVENTS 100
#endif

#ifndef PPG_MAX_INPUTS

/** @brief The maximum number of inputs that can be used continuously.
 * 
 * Please be careful when increasing this number, as memory
 * is allocated for the number of inputs. Also larger interger 
 * data types must be used in some places when this value exceeds 256.
 */
#define PPG_MAX_INPUTS 256
#endif

#ifndef PPG_SMALL_UNSIGNED_INT_TYPE

/** @brief An unsigned integer type that is used to store 
 * small numbers
 */
#define PPG_SMALL_UNSIGNED_INT_TYPE uint8_t
#endif

#ifndef PPG_SMALL_SIGNED_INT_TYPE

/** @brief A signed integer type that is used to store 
 * small numbers
 */
#define PPG_SMALL_SIGNED_INT_TYPE int8_t
#endif

#ifndef PPG_MEDIUM_SIGNED_INT_TYPE

/** @brief A signed integer type that is used to store 
 * larger numbers
 */
#define PPG_MEDIUM_SIGNED_INT_TYPE int16_t
#endif

// Define PPG_PEDANTIC_TOKENS
// to ensure that any match requires a
// matching deactivation of an input for 
// every activation event

#ifndef PPG_COUNT_TYPE

/** @brief This macro enables to define the count type from outside the
 * compile process, e.g. from a build system
 */
#define PPG_COUNT_TYPE PPG_SMALL_UNSIGNED_INT_TYPE
#endif

/** @brief An integer type that is generally used to count entities and as loop counter
 */
typedef PPG_COUNT_TYPE PPG_Count;

#ifndef PPG_ID_TYPE

/** @brief This macro enables to define the id type from outside the
 * compile process, e.g. from a build system
 */
#define PPG_ID_TYPE PPG_MEDIUM_SIGNED_INT_TYPE
#endif

/** @brief An integer type that is used whenever a signed id is used to code non zero initial state as error
 */
typedef PPG_ID_TYPE PPG_Id;

#ifndef PPG_ACTION_FLAGS_TYPE

/** @brief This macro enables to define the action flags type from outside the
 * compile process, e.g. from a build system
 */
#define PPG_ACTION_FLAGS_TYPE PPG_SMALL_UNSIGNED_INT_TYPE
#endif

/** @brief The data type used to store action flags
 */
typedef PPG_ACTION_FLAGS_TYPE PPG_Action_Flags_Type;

#ifndef PPG_LAYER_TYPE 

/** @brief This macro enables to define the layer type from outside the
 * compile process, e.g. from a build system
 */
#define PPG_LAYER_TYPE PPG_SMALL_SIGNED_INT_TYPE
#endif

/** The data type that is used to reference layers
 */
typedef PPG_LAYER_TYPE PPG_Layer;

#ifndef PPG_SLOT_ID_TYPE 

/** @brief This macro enables to define the slot id type from outside the
 * compile process, e.g. from a build system
 */
#define PPG_SLOT_ID_TYPE PPG_SMALL_UNSIGNED_INT_TYPE
#endif

/** @brief The data type used to identify slots
 */
typedef PPG_SLOT_ID_TYPE PPG_Slot_Id;

#ifndef PPG_TIME_COMPARISON_RESULT_TYPE

/** @brief This macro enables to define the time comparison result type from outside the
 * compile process, e.g. from a build system
 */
#define PPG_TIME_COMPARISON_RESULT_TYPE PPG_SMALL_SIGNED_INT_TYPE
#endif

/** @brief The signed integer type used to code the result of the time comparison function
 */
typedef PPG_TIME_COMPARISON_RESULT_TYPE PPG_Time_Comparison_Result_Type;

#ifndef PPG_EVENT_BUFFER_INDEX_TYPE

/** @brief This macro enables to define the event buffer index type from outside the
 * compile process, e.g. from a build system
 */
#define PPG_EVENT_BUFFER_INDEX_TYPE PPG_SMALL_SIGNED_INT_TYPE
#endif

/** @brief The signed data type that is used to index events
 */
typedef PPG_SMALL_SIGNED_INT_TYPE PPG_Event_Buffer_Index_Type;

#ifndef PPG_PROCESSING_STATE_TYPE

/** @brief This macro enables to define the processing state flag type from outside the
 * compile process, e.g. from a build system
 */
#define PPG_PROCESSING_STATE_TYPE PPG_SMALL_UNSIGNED_INT_TYPE
#endif

/** @brief The data type used for processing states
 */
typedef PPG_PROCESSING_STATE_TYPE PPG_Processing_State;

#ifndef PPG_TIME_IDENTIFIER_TYPE

#ifdef __AVR_ATmega32U4__ 

/** @brief This macro enables to define the time identifier type from outside the
 * compile process, e.g. from a build system
 */
#define PPG_TIME_IDENTIFIER_TYPE uintptr_t
#else

/** @brief This macro enables to define the time identifier type from outside the
 * compile process, e.g. from a build system
 */
#define PPG_TIME_IDENTIFIER_TYPE long unsigned
#endif
#endif

/** @brief Time identifier type.
 */
typedef PPG_TIME_IDENTIFIER_TYPE PPG_Time;

#ifdef PAPAGENO_PRINT_SELF_ENABLED

#include <stdio.h>

inline
static void ppg_indent(int tabs)
{
   for(int i = 0; i < tabs; ++i) {
      printf("\t");
   }
}

#define PPG_I ppg_indent(indent);

#endif

#endif
