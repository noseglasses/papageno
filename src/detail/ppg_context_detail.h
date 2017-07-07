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

#ifndef PPG_CONTEXT_DETAIL_H
#define PPG_CONTEXT_DETAIL_H

#include "ppg_event.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_event_buffer_detail.h"
#include "detail/ppg_event_buffer_detail.h"
#include "detail/ppg_furcation_detail.h"

#include <stddef.h>

typedef struct PPG_Context_Struct
{
	PPG_Event_Buffer event_buffer;
	PPG_Furcation_Buffer furcation_buffer;
	
	PPG_Token__ pattern_root;
	bool pattern_root_initialized;
	bool process_actions_if_aborted;

	PPG_Token__ *current_token;

	bool timeout_enabled;
	bool papageno_enabled;
	bool papageno_temporarily_disabled;
	
	PPG_Layer layer;

	PPG_Input abort_input;

	PPG_Time time_last_event;

	PPG_Time event_timeout;
	
	PPG_Input_Id_Equal_Fun input_id_equal;
	
	PPG_Event_Processor_Fun input_processor;
	
	PPG_Time_Fun time;
	PPG_Time_Difference_Fun time_difference;
	PPG_Time_Comparison_Fun time_comparison;
	
	PPG_User_Callback abort_callback;
	PPG_User_Callback timeout_callback;
  
} PPG_Context;

PPG_Context *ppg_context;

void ppg_global_initialize_context(PPG_Context *context);

/* The following macros influence the build

PAPAGENO_DISABLE_CONTEXT_SWITCHING 
		Disables the capability to switch contexts 

*/

#endif
