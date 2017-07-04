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

#ifndef PPG_TIME_H
#define PPG_TIME_H

#include <stdint.h>

/** @brief Time identifier type.
 */
typedef void * PPG_Time;

/** @brief Function type of a callback that computes the current time
 * 
 * @param The pointer to the time value to receive current time
 */
typedef void (*PPG_Time_Fun)(PPG_Time *time);

/** @brief Defines a custom time retreival function
 * 
 * @param fun The function callback to be registered
 * @returns The callback previously active
 */
PPG_Time_Fun ppg_set_time_function(PPG_Time_Fun fun);

/** @brief Function type of a callback that computes differences between time values.
 * 
 * The result is expected as time2 - time1.
 * 
 * @param time1 The first time value
 * @param time2 The second time value
 * @param delta Pointer to the result of the difference computation
 */
typedef void (*PPG_Time_Difference_Fun)(PPG_Time time1, PPG_Time time2, PPG_Time *delta);

/** @brief Defines a custom function to compute time differences
 * 
 * @param fun The function callback to be registered
 * @returns The callback previously active
 */
PPG_Time_Difference_Fun ppg_set_time_difference_function(PPG_Time_Difference_Fun fun);

typedef int8_t PPG_Time_Comparison_Result_Type;

/** @brief Function type for time comparisons.
 * 
 * The function is expected to return a positive value when time1 is greater time2, 
 * a negative value it time 1 is less time 2 and zero if both are equal.
 * 
 * @param time1 The first time value
 * @param time2 The second time value
 */
typedef PPG_Time_Comparison_Result_Type (*PPG_Time_Comparison_Fun)(
									PPG_Time time1, PPG_Time time2);

/** @brief Defines a custom function to compare time values
 * 
 * @param fun The function callback to be registered
 * @returns The callback previously active
 */
PPG_Time_Comparison_Fun ppg_set_time_comparison_function(PPG_Time_Comparison_Fun fun);

#endif
