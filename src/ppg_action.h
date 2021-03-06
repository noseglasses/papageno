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

#ifndef PPG_ACTION_H
#define PPG_ACTION_H

/** @file */

#include "ppg_signals.h"

#include <stddef.h>
#include <stdbool.h>

enum PPG_Action_Activation_Flags 
{
   PPG_Action_Activation_Flags_Empty = 0,
   PPG_Action_Activation_Flags_Active,
   PPG_Action_Activation_Flags_Repeated = (PPG_Action_Activation_Flags_Active << 1)
};
   
/** @brief Function type of user callback functions
 * 
 *  @param activation If true, the callback signals the activation (match) of a token, and deactivation otherwise
 *  @param user_data Optional user data.
 */
typedef void (*PPG_Action_Callback_Fun)(PPG_Count activation_flags, void *user_data);

/** @brief The PPG_Action_Callback struct groups use callback information
 *  in an object oriented fashion (functor).
 */
typedef struct {
   PPG_Action_Callback_Fun func; ///< The callback function
   void *   user_data; ///< Optional user data that is passed to the callback when called
} PPG_Action_Callback;

/** @brief This function initializes an action callback
 *
 * @param cb A pointer to the callback struct
 */
inline
void ppg_action_callback_init(PPG_Action_Callback *cb)
{
   cb->func = NULL;
   cb->user_data = NULL;
}

/** @brief Action information
 */
typedef struct {
   PPG_Action_Callback callback; ///< The user callback that represents that action
} PPG_Action;

/** @brief Use this macro to simplify specification of action callbacks
 * @param FUNC The callback function pointer
 * @param USER_DATA A pointer to user data or NULL if none is required
 */
#define PPG_ACTION_USER_CALLBACK(FUNC, USER_DATA) \
   (PPG_Action) { \
      .callback = (PPG_Action_Callback) { \
         .func = (PPG_Action_Callback_Fun)FUNC, \
         .user_data = USER_DATA \
      } \
   }
   
/** @brief Use this macro to specify an action that does nothing
 */
#define PPG_ACTION_NOOP \
   (PPG_Action) { \
      .callback = (PPG_Action_Callback) { \
         .func = NULL, \
         .user_data = NULL \
      } \
   }

#endif
