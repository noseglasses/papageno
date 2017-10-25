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

#ifndef PPG_INPUT_H
#define PPG_INPUT_H

/** @file */

#include <stdbool.h>
#include <stdint.h>

/** @brief The type used as input identifier.
 * 
 * This type is used as an identifier for inputs. Inputs must be numbered contiguously.
 */
typedef uint8_t PPG_Input_Id;

/** @brief Auxiliary macro to simplify passing input arrays to functions such as
 * ppg_cluster or ppg_chord
 * 
 * @param ... The array members
 */
#define PPG_INPUTS(...) \
   sizeof((PPG_Input_Id[]){ __VA_ARGS__ })/sizeof(PPG_Input_Id), \
   \
   /* The following akward construnct serves to silence the compiler warning \
    * error: taking address of temporary array \
    */ \
   (PPG_Input_Id*)(const PPG_Input_Id[]){ __VA_ARGS__ }

#endif
