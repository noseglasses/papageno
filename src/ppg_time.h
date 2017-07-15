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

#ifndef PPG_TIME_H
#define PPG_TIME_H

/** @file */

#include "ppg_settings.h"

#include <stdint.h>

/** @brief Function type of a callback that computes the current time
 * 
 * @param The pointer to the time value to receive current time
 */
typedef void (*PPG_Time_Fun)(PPG_Time *time);

/** @brief Function type of a callback that computes differences between time values.
 * 
 * The result is expected as time2 - time1.
 * 
 * @param time1 The first time value
 * @param time2 The second time value
 * @param delta Pointer to the result of the difference computation
 */
typedef void (*PPG_Time_Difference_Fun)(PPG_Time time1, PPG_Time time2, PPG_Time *delta);

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

/** @brief A collection of functions for time handling
 */
typedef struct {
	PPG_Time_Fun time; ///< Retreives time
	PPG_Time_Difference_Fun time_difference; ///< Computes time differences
	PPG_Time_Comparison_Fun compare_times; ///< Compares time values
} PPG_Time_Manager;

/** @brief Sets a new global time handler
 * 
 * @param time_manager The new time handler
 * @returns The previously used time handler
 */
PPG_Time_Manager ppg_global_set_time_manager(PPG_Time_Manager time_manager);

/** @brief Retreives the currently active global time handler
 * 
 * @returns The currently active global time handler
 */
PPG_Time_Manager ppg_global_get_time_manager(void);

#endif
