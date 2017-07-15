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

#include "detail/ppg_context_detail.h"
#include "detail/ppg_input_detail.h"
#include "ppg_debug.h"

PPG_Context *ppg_context = NULL;

static void ppg_default_time(PPG_Time *time)
{
	*time = 0;
}

static void ppg_default_time_difference(PPG_Time time1, PPG_Time time2, PPG_Time *delta)
{
	*delta = 0;
}

PPG_Time_Comparison_Result_Type ppg_default_time_comparison(
								PPG_Time time1,
								PPG_Time time2)
{
	return 0;
}

/** @brief This function initializes a signal callback
 *
 * @param cb A pointer to the callback struct
 */
inline
static void ppg_signal_callback_init(PPG_Signal_Callback *cb)
{
	cb->func = NULL;
	cb->user_data = NULL;
}

void ppg_global_initialize_context(PPG_Context *context) {
	
// 	PPG_PRINTF("Initializing context\n");
	
	ppg_event_buffer_init(&context->event_buffer);
	ppg_furcation_buffer_init(&context->furcation_buffer);

	context->active_inputs.n_bits = 0;
	
	ppg_bitfield_clear(&context->active_inputs);
	
	context->current_token = NULL;
	context->timeout_enabled = true;
	context->papageno_enabled = true;
	context->tree_depth = 0;
	context->layer = 0;
	ppg_global_init_input(&context->abort_input);
	context->event_timeout = 0;
	context->time_manager.time = ppg_default_time;
	context->time_manager.time_difference = ppg_default_time_difference;
	context->time_manager.compare_times = ppg_default_time_comparison;
	
	ppg_signal_callback_init(&context->signal_callback);

	/* Initialize the pattern root
	*/
	ppg_token_new(&context->pattern_root);
};
