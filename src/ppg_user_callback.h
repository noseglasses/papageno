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

#ifndef PPG_USER_CALLBACK_H
#define PPG_USER_CALLBACK_H

/** @brief Function type of user callback functions
 * 
 *	 @param slot_id
 *  @param user_data Optional user data.
 */
typedef void (*PPG_User_Callback_Fun)(uint8_t slot_id, void *user_data);

/** @brief The PPG_User_Callback struct groups use callback information
 *  in an object oriented fashion (functor).
 */
typedef struct {
	PPG_User_Callback_Fun func; ///< The callback function
	void *	user_data; ///< Optional user data that is passed to the callback when called
} PPG_User_Callback;

#endif
