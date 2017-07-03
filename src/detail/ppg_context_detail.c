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

#include "detail/ppg_context_detail.h"
#include "ppg_debug.h"

static PPG_Context *ppg_context = NULL;

void ppg_initialize_context(PPG_Context *context) {
	
	PPG_PRINTF("Initializing context\n");
	
	context->n_events = 0;
	context->pattern_root_initialized = false;
	context->process_actions_if_aborted = false;
	context->current_token = NULL;
	context->papageno_enabled = true;
	context->papageno_temporarily_disabled = false;
	ppg_init_input(&context->abort_input);
	context->inputpress_timeout = NULL;
	context->input_id_equal = (PPG_Input_Id_Equal_Fun)ppg_input_id_simple_equal;
	context->time = ppg_default_time;
	context->time_difference = ppg_default_time_difference;
	context->time_comparison = ppg_default_time_comparison;
};
