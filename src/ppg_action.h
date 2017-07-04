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

#ifndef PPG_ACTION_H
#define PPG_ACTION_H

#include "ppg_user_callback.h"

/** @brief Action information
 */
typedef struct {
	PPG_User_Callback user_callback; ///< The user callback that represents that action
	uint8_t flags; ///< A bitfield that codes specific attributes associated with the action
} PPG_Action;

/** @brief Use this macro to simplify specification of action callbacks
 * @param FUNC The callback function pointer
 * @param USER_DATA A pointer to user data or NULL if none is required
 */
#define PPG_ACTION_USER_CALLBACK(FUNC, USER_DATA) \
	(PPG_Action) { \
		.flags = PPG_Action_Default, \
		.user_callback = (PPG_User_Callback) { \
			.func = (PPG_User_Callback_Fun)FUNC, \
			.user_data = USER_DATA \
		} \
	}
	
/** @brief Use this macro to specify an action that does nothing
 */
#define PPG_ACTION_NOOP \
	(PPG_Action) { \
		.flags = PPG_Action_None, \
		.user_callback = (PPG_User_Callback) { \
			.func = NULL, \
			.user_data = NULL \
		} \
	}

#endif
