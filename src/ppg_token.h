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

#ifndef PPG_TOKEN_H
#define PPG_TOKEN_H

/** @file */

#include "ppg_action.h"
#include "ppg_action_flags.h"
#include "ppg_settings.h"

/** @brief The token type
 */
typedef void * PPG_Token;

/** @brief Assigns an action to a token.
 * 
 * @param token The token to be modified
 * @param action The action definition to associate with the token
 * @returns Returns the value of parameter token to allow for setter-chaining
 */
PPG_Token ppg_token_set_action(
                           PPG_Token token,
                           PPG_Action action);

/** @brief Returns the action that is associated with a token
 * 
 * @param token The token whose action is supposed to be retreived
 * @returns The associated action
 */
PPG_Action ppg_token_get_action(PPG_Token token);

/** @brief Assigns action flags to a token. 
 * 
 * To modify the current state of 
 * the action flags of a token, use the function ppg_token_get_action_flags, 
 * modify its return value by means of applying bit wise operations and pass it 
 * to ppg_token_set_action_flags.
 * 
 * @param token The token to be modified
 * @param action_flags The new value of action flags
 * @returns Returns the value of parameter token to allow for setter-chaining
 */
PPG_Token ppg_token_set_action_flags(
                           PPG_Token token,
                           PPG_Action_Flags_Type action_flags);

/** @brief Returns the action flags associated with a token
 * 
 * @param token The token whose action flags are supposed to be retreived
 * @returns The current value of the action flags associated with the token.
 */
PPG_Action_Flags_Type ppg_token_get_action_flags(PPG_Token token);

/** @brief Assigns flags to a token. 
 * 
 * To modify the current state of 
 * the flags of a token, use the function ppg_token_get_flags, 
 * modify its return value by means of applying bit wise operations and pass it 
 * to ppg_token_set_flags.
 * 
 * @note Be careful when modifying flags manually as different token types
 *       support different types flags. Read the documentation of the
 *       respective token type carefully before manipulating its flags.
 * 
 * @param token The token to be modified
 * @param flags The new value of flags
 * @returns Returns the value of parameter token to allow for setter-chaining
 */
PPG_Token ppg_token_set_flags(
                           PPG_Token token,
                           PPG_Count flags);

/** @brief Returns the flags associated with a token
 * 
 * @param token The token whose flags are supposed to be retreived
 * @returns The current value of the flags associated with the token.
 */
PPG_Count ppg_token_get_flags(PPG_Token token);

/** @brief Auxiliary macro to simplify passing tokens to functions such as
 * ppg_pattern
 * 
 * @param ... The array members
 */
#define PPG_TOKENS(...) \
   sizeof((PPG_Token[]) { __VA_ARGS__ })/sizeof(PPG_Token), \
   (PPG_Token[]) { __VA_ARGS__ }

#endif
