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

#include "papageno.h"

#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdarg.h>

#include "detail/ppg_token.h"
#include ppg_debug.h"is

#define PPG_CALL_VIRT_METHOD(THIS, METHOD, ...) \
	THIS->vtable->METHOD(THIS, ##__VA_ARGS__);

static bool ppg_input_id_simple_equal(PPG_Input_Id kp1, PPG_Input_Id kp2)
{
	return 	kp1 == kp2;
}

static bool ppg_recurse_and_process_actions(uint8_t slot_id);

static void ppg_default_time(PPG_Time *time)
{
	*time = 0;
}

static void ppg_default_time_difference(PPG_Time time1, PPG_Time time2, PPG_Time *delta)
{
	*delta = 0;
}

int8_t ppg_default_time_comparison(
								PPG_Time time1,
								PPG_Time time2)
{
	return 0;
}

static void ppg_init_input(PPG_Input *input)
{
	input->input_id = (PPG_Input_Id)((uint16_t)-1);
	input->check_active = NULL;
}

static void ppg_store_event(PPG_Event *event)
{
	PPG_ASSERT(ppg_context->n_events < PPG_MAX_KEYCHANGES);
	
	ppg_context->events[ppg_context->n_events] = *event;
	
	++ppg_context->n_events;
}

void ppg_flush_stored_events(
								uint8_t slot_id, 
								PPG_Event_Processor_Fun input_processor,
								void *user_data)
{
	if(ppg_context->n_events == 0) { return; }
	
	PPG_Event_Processor_Fun kp	=
		(input_processor) ? input_processor : ppg_context->input_processor;
	
	if(!kp) { return; }
	
	/* Make sure that no recursion is possible if the input processor 
	 * invokes ppg_process_event 
	 */
	ppg_context->papageno_temporarily_disabled = true;
	
	for(uint16_t i = 0; i < ppg_context->n_events; ++i) {
		
		if(!kp(&ppg_context->events[i], slot_id, user_data)) { 
			break;
		}
	}
	
	ppg_context->papageno_temporarily_disabled = false;
}

static void ppg_delete_stored_events(void)
{
	ppg_context->n_events = 0;
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

/* Returns if an action has been triggered.
 */
static bool ppg_recurse_and_process_actions(uint8_t slot_id)
{			
	if(!ppg_context->current_token) { return false; }
	
// 	PPG_PRINTF("Triggering action of most recent token\n");
	
	PPG_Token__ *cur_token = ppg_context->current_token;
	
	while(cur_token) {
		
		bool action_present = ppg_token_trigger_action(cur_token, slot_id);
		
		if(action_present) {
			if(cur_token->action.flags &= PPG_Action_Fall_Through) {
				cur_token = cur_token->parent;
			}
			else {
				return true;
			}
		}
		else {
			if(cur_token->action.flags &= PPG_Action_Fall_Back) {
				cur_token = cur_token->parent;
			}
			else {
				return false;
			}
		}
	}
	
	PPG_PRINTF("Done\n");
	
	return false;
}

static void ppg_on_timeout(void)
{
	if(!ppg_context->current_token) { return; }
	
	/* The frase could not be parsed. Reset any previous tokens.
	*/
	ppg_token_reset_successors(ppg_context->current_token);
	
	/* It timeout was hit, we either trigger the most recent action
	 * (e.g. necessary for tap dances) or flush the inputevents
	 * that happend until this point
	 */
	
	bool action_triggered 
		= ppg_recurse_and_process_actions(PPG_On_Timeout);
	
	/* Cleanup and issue all inputpresses as if they happened without parsing a pattern
	*/
	if(!action_triggered) {
		ppg_flush_stored_events(	PPG_On_Timeout, 
											NULL /* input_processor */, 
											NULL /* user data */);
	}
	
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

bool ppg_check_timeout(void)
{
// 	PPG_PRINTF("Checking timeout\n");
	
	#ifdef DEBUG_PAPAGENO
	if(!ppg_context->time) {
		PPG_ERROR("Time function undefined\n");
	}
	if(!ppg_context->time_difference) {
		PPG_ERROR("Time difference function undefined\n");
	}
	if(!ppg_context->time_comparison) {
		PPG_ERROR("Time comparison function undefined\n");
	}
	#endif
	
	PPG_Time cur_time;
	
	ppg_context->time(&cur_time);
	
	PPG_Time delta;
	ppg_context->time_difference(
					ppg_context->time_last_inputpress, 
					cur_time, 
					&delta);
	
	if(ppg_context->current_token
		&& (ppg_context->time_comparison(
					delta,
					ppg_context->inputpress_timeout
			) > 0)
	  ) {
		
		PPG_PRINTF("Pattern detection timeout hit\n");
	
		/* Too late...
			*/
		ppg_on_timeout();
	
		return true;
	}
	
	return false;
}

bool ppg_process_event(PPG_Event *event,
								  uint8_t cur_layer)
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
		ppg_context->n_events = 0;
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
	
	uint8_t result 
		= ppg_token_consider_event(	
										&ppg_context->current_token,
										event,
										cur_layer
								);
		
	switch(result) {
		
		case PPG_Token_In_Progress:
			
			return false;
			
		case PPG_Pattern_Completed:
			
			ppg_recurse_and_process_actions(PPG_On_Pattern_Completed);
			
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

PPG_Time_Fun ppg_set_time_function(PPG_Time_Fun fun)
{
	PPG_Time_Fun old_fun = fun;
	
	ppg_context->time = fun;
	
	return old_fun;
}

PPG_Time_Difference_Fun ppg_set_time_difference_function(PPG_Time_Difference_Fun fun)
{
	PPG_Time_Difference_Fun old_fun = ppg_context->time_difference;
	
	ppg_context->time_difference = fun;
	
	return old_fun;
}

PPG_Time_Comparison_Fun ppg_set_time_comparison_function(PPG_Time_Comparison_Fun fun)
{
	PPG_Time_Comparison_Fun old_fun = ppg_context->time_comparison;
	
	ppg_context->time_comparison = fun;
	
	return old_fun;
}
