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

#ifndef PPG_ACTION_FLAGS_H
#define PPG_ACTION_FLAGS_H

/** @file */

#include <stdint.h>

/** @brief Action flags are defined as power of two to be compined using bitwise operations
 */
enum PPG_Action_Flags {
	PPG_Action_Default = 0,
	PPG_Action_Fall_Through = 1 << 0,
	PPG_Action_Fall_Back = 1 << 2,
	PPG_Action_Immediate = 1 << 3
};

#endif