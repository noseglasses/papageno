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

#include "ppg_timeout.h"
#include "ppg_debug.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_global_detail.h"
#include "detail/ppg_signal_detail.h"

static void ppg_on_timeout(void)
{
	if(!ppg_context->current_token) { return; }
	
	/* The frase could not be parsed. Reset any previous tokens.
	*/
	ppg_token_reset_children(ppg_context->current_token);
			
	/* It timeout was hit, we either trigger the most recent action
	 * (e.g. necessary for tap dances).
	 */
	bool action_processed = ppg_recurse_and_process_actions();
	
	if(action_processed) { 
		
		// If an action was processed, we consider the processing as a match
		//
		ppg_event_buffer_on_match_success();
		
		// Prevent the timeout signal handler from processig events
		//
		ppg_delete_stored_events();
	}
	else {
		
		ppg_event_buffer_prepare_on_failure();
		
	}
	
	ppg_signal(PPG_On_Timeout);

	ppg_recurse_and_cleanup_active_branch();
	
	if(!action_processed) { 
		
		// if action_processed is true, the events have already been deleted above

		ppg_delete_stored_events();
	}
	
	ppg_context->current_token = NULL;
}

bool ppg_timeout_check(void)
{	
	PPG_PRINTF("Checking timeout\n");
	
	if(!ppg_context->timeout_enabled) { 
		
		PPG_PRINTF("Timeout disabled\n");
		return false;
	}
	
	if(!ppg_context->current_token) {
		PPG_PRINTF("No current token\n");
		return false; 
	}
	
	PPG_PRINTF("Checking timeout\n");
	
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
					ppg_context->time_last_event, 
					cur_time, 
					&delta);
	
	bool timeout_hit = false;
	
// 	PPG_PRINTF("   delta: %ld\n", delta);
	
	if(ppg_context->current_token
		&& (ppg_context->time_comparison(
					delta,
					ppg_context->event_timeout
			) > 0)
	  ) {
		
		PPG_PRINTF("Pattern detection timeout hit\n");
	
		/* Too late...
			*/
		ppg_on_timeout();
	
		timeout_hit = true;
	}
	
	return timeout_hit;
}
