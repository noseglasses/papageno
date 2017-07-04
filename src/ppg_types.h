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

#ifndef PPG_TYPES_H
#define PPG_TYPES_H

/** @file */

#include "stdint.h"

/** An integer type that is generally used to count entities and as loop counter
 */
typedef uint8_t PPG_Count;

/** An integer type that is used whenever a signed id is used to code non zero initial state as error
 */
typedef int16_t PPG_Id;

#endif
