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

#ifndef PPG_GLOBAL_H
#define PPG_GLOBAL_H

/** @file */

#include "ppg_input.h"
#include "ppg_time.h"
#include "ppg_event.h"
#include "ppg_layer.h"

#include <stdbool.h>
#include <inttypes.h>

/** @brief Defines an input that aborts current pattern processing. 
 * 
 * If pattern processing is
 * aborted the actions of tokens that already matched are triggered.
 * 
 * @note By default there is no abort input defined
 * 
 * @param input The abort input
 * @returns The abort input id that was previously set
 */
PPG_Input ppg_global_set_abort_trigger(PPG_Input input);

/** @brief Retreives the current abort input. 
 * 
 * See ppg_global_set_abort_trigger for further information.
 */
PPG_Input ppg_global_get_abort_trigger(void);

/** @brief Aborts processing of the current pattern
 */
void ppg_global_abort_pattern_matching(void);

/** @brief Defines whether actions are supposed to be processed along the token
 * chain of the current pattern, based on the last token that matched. 
 * 
 * The default setting
 * is to process no action in case of an abortion of pattern processing.
 * 
 * @param state The new value to be set
 * 
 * @returns The previous setting
 */
bool ppg_global_set_process_actions_if_aborted(bool state);

/** @brief Retreives the current settings of the process action if aborted.
 * 
 * See the documentation of ppg_global_set_process_actions_if_aborted
 * 
 * @returns The current boolean value
 */
bool ppg_global_get_process_actions_if_aborted(void);

/** @brief Set the current timeout for pattern processing. 
 * 
 * If no input events are encountered
 * within the timeout interval after the last input event, pattern processing is 
 * aborted and actions are processed recursively starting with the last token that matched.
 * 
 * @param timeout The timeout time value
 * @returns The previous setting
 */
PPG_Time ppg_global_set_timeout(PPG_Time timeout);

/** @brief Returns the current timeout value
 * 
 * @returns The current timeout value
 */
PPG_Time ppg_global_get_timeout(void);


/** @brief Defines the default input processor callback. 
 * 
 * Some functions, e.g. ppg_event_buffer_flush
 * can be supplied with a custom input processor. If non is specified the default one is used
 * if defined.
 * 
 * @param fun The default input processor callback
 * @returns The callback that was active before resetting
 */
PPG_Event_Processor_Fun ppg_global_set_default_event_processor(PPG_Event_Processor_Fun fun);

/** @brief Call this function to actively flush any input events that occured since the last flush.
 * 
 * This function is called internally when melodies complete, on abort and on timeout.
 * 
 * @param slot_id The slot where this method was called. Pass PPG_On_User if you call this 
 * 					method from user code.
 * @param input_processor A custom input processor callback. If NULL the default processor registered by
 *                ppg_global_set_default_event_processor is used.
 * @param user_data Optional user data is passed on to the input processor callback
 */
void ppg_event_buffer_flush(
								PPG_Slot_Id slot_id, 
								PPG_Event_Processor_Fun input_processor,
								void *user_data);

/** @brief Use this function to temporarily disable/enable pattern processing. 
 * 
 * Processing is enabled by default.
 * 
 * @param state The new state. Enables if true, disables if false.
 * @returns The previous state
 */
bool ppg_global_set_enabled(bool state);

/** @brief Initialize Papageno
 */
void ppg_global_init(void);

/** @brief Compiles the Papageno pattern tree
 */
void ppg_global_compile(void);

/** @brief Finalizes Papageno, i.e. clears all patterns and frees all allocated memory.
 * 
 * Please not that this operation only operates on the current context. It you have created
 * several contexts, set and finalize them in a loop.
 */
void ppg_global_finalize(void);

/** @brief Resets papageno to initial state
 * 
 * This creates and initializes a new context. Other contexts are not affected. 
 * The currently active context is finalized before the reset operation takes place.
 */
void ppg_global_reset(void);

/** @brief The timeout check function
 * 
 * This timeout check function should be called frequently in a loop, e.g. when polling input presses
 * 
 * @returns True if timeout happend, i.e. the time elapsed since the last inputpress event 
 * 			exceeded the user defined timeout threshold.
 */
bool ppg_timeout_check(void);

/** @brief Toggles global timeout
 * 
 * @param state The current timeout state
 * 
 * @returns The previous state of global timeout enabled
 */
bool ppg_global_set_timeout_enabled(bool state);

/** @brief Determines if global timeout is enabled
 * 
 * @returns The current state of global timeout enabled
 */
bool ppg_global_get_timeout_enabled(void);

/** @brief Set the current layer
 * 
 * @returns The previously active layer
 */
PPG_Layer ppg_global_set_layer(PPG_Layer layer);

/** @brief Retreives the currently active layer
 * 
 * @returns The currently active layer
 */
PPG_Layer ppg_global_get_layer(void);

#endif