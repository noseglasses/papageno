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

#ifndef PPG_EVENT_H
#define PPG_EVENT_H

#include "ppg_input.h"
#include "ppg_slots.h"
#include "ppg_time.h"

/** @brief Specification of an input event
 */
typedef struct {
	PPG_Input_Id input_id; ///< The input identifier associated with the input that is pressed or released
	PPG_Time time; ///< The time at which the input event occured
	PPG_Input_State state; ///< The state of the input when the event occured
} PPG_Event;

/** @brief A callback function that is used during processing of stored input events
 * 
 * @param event A pointer to a PPG_Event struct that provides information 
 * 						about the input event
 * @param slot_id The slot identifier informs about the slot that triggered input event processing
 * @param user_data Optional user data
 * @returns A boolean value that decides about whether input event processing is continued (true) or aborted (false).
 */
typedef bool (*PPG_Event_Processor_Fun)(PPG_Event *event,
														 PPG_Slot_Id slot_id, 
														 void *user_data);

#endif
