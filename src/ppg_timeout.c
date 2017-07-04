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
