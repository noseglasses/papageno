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

#ifndef PPG_ACTION_FLAGS_H
#define PPG_ACTION_FLAGS_H

/** @file */

#include <stdint.h>

/** @brief Action flags are defined as power of two to be compined using bitwise operations
 */
enum PPG_Action_Flags {
   PPG_Action_Default = 0,
   PPG_Action_Fallback = 1 << 0,
   PPG_Action_Deactivate_On_Token_Unmatch = PPG_Action_Fallback << 1
};

#endif
