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

#include "ppg_global.h"
#include "ppg_token.h"
#include "ppg_debug.h"
#include "ppg_action_flags.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_global_detail.h"
#include "detail/ppg_event_buffer_detail.h"
#include "ppg_context.h"

#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdarg.h>

void ppg_flush_stored_events(
								PPG_Slot_Id slot_id, 
								PPG_Event_Processor_Fun input_processor,
								void *user_data)
{
	if(ppg_event_buffer_size() == 0) { return; }
	
	PPG_Event_Processor_Fun kp	=
		(input_processor) ? input_processor : ppg_context->input_processor;
	
	if(!kp) { return; }
	
	/* Make sure that no recursion is possible if the input processor 
	 * invokes ppg_process_event 
	 */
	ppg_context->papageno_temporarily_disabled = true;
	
	ppg_event_buffer_iterate_events(slot_id, kp, user_data);
	
	ppg_context->papageno_temporarily_disabled = false;
}
	
void ppg_abort_pattern(void)
{		
	if(!ppg_context->current_token) { return; }
	
// 	PPG_PRINTF("Aborting pattern\n");
	
	/* The frase could not be parsed. Reset any successor tokens.
	*/
	ppg_token_reset_successors(ppg_context->current_token);
	
	if(ppg_context->process_actions_if_aborted) {
		ppg_recurse_and_process_actions(PPG_On_Abort);
	}
	
	/* Cleanup and issue all inputpresses as if they happened without parsing a pattern
	*/
	ppg_flush_stored_events(	PPG_On_Abort, 
										NULL /* input_processor */, 
										NULL /* user data */);
	
	ppg_delete_stored_events();
	
	ppg_context->current_token = NULL;
}

void ppg_init(void) {

	if(!ppg_context) {
		
		ppg_context = (PPG_Context *)ppg_create_context();
	}
	
	if(!ppg_context->pattern_root_initialized) {
		
		/* Initialize the pattern root
		 */
		ppg_token_new(&ppg_context->pattern_root);
		
		ppg_context->pattern_root_initialized = true;
	}
}

void ppg_finalize(void) {
	
	if(!ppg_context) { return; }
	
	ppg_token_free_successors(&ppg_context->pattern_root);
	
	free(ppg_context);
	
	ppg_context = NULL;
}

void ppg_reset(void)
{
	ppg_finalize();
	
	ppg_context = (PPG_Context *)ppg_create_context();
}

bool ppg_process_event(PPG_Event *event,
								  PPG_Layer cur_layer)
{ 
	if(!ppg_context->papageno_enabled) {
		return true;
	}
	
	/* When a pattern could not be finished, all inputstrokes are
	 * processed through the process_record_quantum method.
	 * To prevent infinite recursion, we have to temporarily disable 
	 * processing patterns.
	 */
	if(ppg_context->papageno_temporarily_disabled) { return true; }
	
	/* Early exit if no pattern was registered 
	 */
	if(!ppg_context->pattern_root_initialized) { return true; }
	
	/* Check if the pattern is being aborted
	 */
	if(ppg_context->input_id_equal(ppg_context->abort_input.input_id, event->input_id)) {
		
		/* If a pattern is in progress, we abort it and consume the abort input.
		 */
		if(ppg_context->current_token) {
			PPG_PRINTF("Processing melodies interrupted by user\n");
			ppg_abort_pattern();
			return false;
		}
	
		return false;
	}
	
// 	PPG_PRINTF("Starting inputprocessing\n");
	
	if(!ppg_context->current_token) {
		
// 		PPG_PRINTF("New pattern \n");
		
		/* Start of pattern processing
		 */
		ppg_event_buffer_init(&ppg_context->event_buffer);
		
		ppg_context->current_token = &ppg_context->pattern_root;
		
		ppg_context->time(&ppg_context->time_last_inputpress);
	}
	else {
		
		if(ppg_check_timeout()) {
			
			/* Timeout hit. Cleanup already done.
			 */
			return false;
		}
		else {
			ppg_context->time(&ppg_context->time_last_inputpress);
		}
	}
	
	PPG_Processing_State result 
		= ppg_token_match_event(	
										&ppg_context->current_token,
										event,
										cur_layer
								);
		
	switch(result) {
		
		case PPG_Token_In_Progress:
			
			return false;
			
		case PPG_Pattern_Matches:
			
			ppg_recurse_and_process_actions(PPG_On_Pattern_Matches);
			
			ppg_context->current_token = NULL;
			
			ppg_delete_stored_events();
											  
			return false;
			
		case PPG_Token_Invalid:
			
			PPG_PRINTF("-\n");
		
			ppg_abort_pattern();
			
			return false; /* The input(s) have been already processed */
			//return true; // Why does this require true to work and 
			// why is t not written?
	}
	
	return true;
}

/* Use this function to define a input_id that always aborts a pattern
 */
PPG_Input ppg_set_abort_input(PPG_Input input)
{
	PPG_Input old_input = ppg_context->abort_input;
	
	ppg_context->abort_input = input;
	
	return old_input;
}

PPG_Input ppg_get_abort_input(void)
{
	return ppg_context->abort_input;
}

bool ppg_set_process_actions_if_aborted(bool state)
{
	bool old_value = ppg_context->process_actions_if_aborted;
	
	ppg_context->process_actions_if_aborted = state;
	
	return old_value;
}

bool ppg_get_process_actions_if_aborted(void)
{
	return ppg_context->process_actions_if_aborted;
}

PPG_Time ppg_set_timeout(PPG_Time timeout)
{
	PPG_Time old_value = ppg_context->inputpress_timeout;
	
	ppg_context->inputpress_timeout = timeout;
	
	return old_value;
}

PPG_Time ppg_get_timeout(void)
{
	return ppg_context->inputpress_timeout;
}

void ppg_set_input_id_equal_function(PPG_Input_Id_Equal_Fun fun)
{
	ppg_context->input_id_equal = fun;
}

PPG_Event_Processor_Fun ppg_set_default_input_processor(PPG_Event_Processor_Fun input_processor)
{
	PPG_Event_Processor_Fun old_input_processor = ppg_context->input_processor;
	
	ppg_context->input_processor = input_processor;
	
	return old_input_processor;
}

bool ppg_set_enabled(bool state)
{
	bool old_state = ppg_context->papageno_enabled;
	
	ppg_context->papageno_enabled = state;

	return old_state;
}

bool ppg_set_enable_timeout(bool state)
{
	bool previous_state = ppg_context->timeout_enabled;
	
	ppg_context->timeout_enabled = state;
	
	return previous_state;
}

bool ppg_get_enable_timeout(void)
{
	return ppg_context->timeout_enabled;
}
