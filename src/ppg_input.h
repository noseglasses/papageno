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

#ifndef PPG_INPUT_H
#define PPG_INPUT_H

/** @file */

#include <stdbool.h>

/** @brief The type used as input identifier.
 * 
 * This type is used as an identifier for inputs.
 * Please supply a comparison callback via ppg_set_input_id_equal_function.
 * The default behavior is to compare input ids by value.
 */
typedef void * PPG_Input_Id;

/** @brief The type that represents input state.
 */
typedef void * PPG_Input_State;

/** @brief The input activation check callback function type
 */
typedef bool (*PPG_Input_Active_Check_Fun)(PPG_Input_Id input_id,
															PPG_Input_State state);

/** @brief Definition of an input
 */
typedef struct {
	PPG_Input_Id	input_id; ///< The Input identifier
	PPG_Input_Active_Check_Fun check_active; ///< A callback that provides checking if an input is activated
} PPG_Input;

/** @brief Function type of a callback function that compares user defined input ids
 * 
 * @param input_id1 The first input identifier
 * @param input_id2 The second input identifier
 */
typedef bool (*PPG_Input_Id_Equal_Fun)(PPG_Input_Id input_id1, PPG_Input_Id input_id2);

/** @brief Defines a custom input id comparison function.
 * 
 * The default behavior is to compare input ids by value.
 * 
 * @param fun The function callback to be registered
 */
void ppg_set_input_id_equal_function(PPG_Input_Id_Equal_Fun fun);

/** @brief Auxiliary macro to simplify passing input arrays to functions such as
 * ppg_cluster or ppg_chord
 * 
 * @param ... The array members
 */
#define PPG_INPUTS(...) \
	sizeof((PPG_Input[]){ __VA_ARGS__ })/sizeof(PPG_Input), \
	(PPG_Input[]){ __VA_ARGS__ }

#endif
