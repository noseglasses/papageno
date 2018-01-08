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

#ifndef PPG_TAP_DANCE_H
#define PPG_TAP_DANCE_H

/** @file */

#include "ppg_action.h"
#include "ppg_token.h"
#include "ppg_input.h"
#include "ppg_layer.h"

/** @brief A tap specification
 * 
 * Defines an action for a given number of taps
 */
typedef struct {
   PPG_Action action; ///< The action associated with the given number of taps
   PPG_Count tap_count; ///< The number of taps necessary to trigger the action
} PPG_Tap_Definition;

/** @brief Auxiliary macro to simplify passing tap definitions
 * 
 * @param ... The array members
 */
#define PPG_TAP_DEFINITIONS(...) \
   sizeof((PPG_Tap_Definition[]){ __VA_ARGS__ })/sizeof(PPG_Tap_Definition), \
   (PPG_Tap_Definition[]){ __VA_ARGS__ }

/** @brief This macro simplifies the specification of taps as actions that are associated with specific a number of taps 
 * 
 * @param TAP_COUNT The number of taps the action is associated with
 * @param ACTION The action
 */
#define PPG_TAP(TAP_COUNT, ACTION) \
   (PPG_Tap_Definition) { .action = ACTION, .tap_count = TAP_COUNT }

/** @brief Defines a tap dance. 
 * 
 * Tap dances are a sequence of input presses of the same input. With each press 
 * an action can be associated. As an enhancement to the original tap dance idea.
 * It is also possible to fall back to the last action if e.g. an action
 * is defined for three input presses and five input presses. If default_action_flags
 * contains PPG_Action_Fallback the action associated with the next lower 
 * amount of input presses is triggered, in this example the action associated with
 * three presses.
 * 
 * @param layer The layer the pattern is associated with
 * @param input The input the tap dance is associated with
 * @param n_tap_definitions The number of tap definitions
 * @param tap_definitions A tap definitions array.
 * @returns The constructed token
 */
PPG_Token ppg_tap_dance(   
                     PPG_Layer layer,
                     PPG_Input_Id input,
                     PPG_Count n_tap_definitions,
                     PPG_Tap_Definition tap_definitions[]);

#endif
