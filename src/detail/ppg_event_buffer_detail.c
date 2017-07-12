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

#include "detail/ppg_event_buffer_detail.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_global_detail.h"
#include "ppg_debug.h"
#include "ppg_settings.h"

#include <stdlib.h>

#define PPG_EB ppg_context->event_buffer

PPG_Count ppg_event_buffer_size(void)
{
	return PPG_EB.size;
}

void ppg_event_buffer_store_event(PPG_Event *event)
{
	#ifdef DEBUG_PAPAGENO
	if(PPG_EB.start > PPG_EB.end) {
		PPG_Count n_events = PPG_MAX_EVENTS + PPG_EB.end - PPG_EB.start;
		PPG_ASSERT(n_events < PPG_MAX_EVENTS - 1); // At least one left!
	}
	else {
		PPG_Count n_events = PPG_EB.end - PPG_EB.start;
		PPG_ASSERT(n_events < PPG_MAX_EVENTS - 1); // At least one left!
	}
	#endif
	
	PPG_Event_Buffer_Index_Type new_pos = PPG_EB.end;
	
	if(PPG_EB.end == PPG_MAX_EVENTS - 1) {
		PPG_EB.end = 0;
	}
	else {
		++PPG_EB.end;
	}
	
	//PPG_PRINTF("Storing event at %u\n", PPG_EB.end);
	
	PPG_EB.events[new_pos] = *event;
	
	++PPG_EB.size;
}

void ppg_event_buffer_init(PPG_Event_Buffer *eb)
{
	eb->start = 0;
	eb->end = 0;
	eb->cur = 0;
	
	eb->size = 0;
}

bool ppg_event_buffer_events_left(void)
{
	return 	(PPG_EB.cur != PPG_EB.end);
}

void ppg_event_buffer_advance(void)
{
	if(PPG_EB.size == 0) { return; }
	
	if(PPG_EB.cur == PPG_MAX_EVENTS - 1) {
		PPG_EB.cur = 0;
	}
	else {
		++PPG_EB.cur;
	}
} 

static void ppg_even_buffer_recompute_size(void)
{
	if(PPG_EB.end > PPG_EB.start) {
		
		PPG_EB.size = PPG_EB.end - PPG_EB.start;
	}
	else if(PPG_EB.end == PPG_EB.start) {
		PPG_EB.size = 0;
	}
	else {
		
		PPG_EB.size = PPG_MAX_EVENTS + PPG_EB.end - PPG_EB.start;
	}
}

static void ppg_flush_non_considered_events(PPG_Event *event, 
														  void* user_data)
{
	if(!(event->flags & PPG_Event_Considered)) {

		// Events that were not considered are flushed
		//
		ppg_context->input_processor(event, NULL);
	}
}

void ppg_event_buffer_truncate_at_front(void)
{
	if(PPG_EB.cur == PPG_EB.end) {
		ppg_event_buffer_init(&PPG_EB);
	}
	else {
		
		ppg_event_buffer_advance();
		
		// Any events that were not
		// considered by the last match, e.g. intermixed deactivations 
		// of currently unrelated inputs are flushed.
		
		// Temporarily reset the end of the event buffer to
		// simplify flushing
		//
		PPG_Count old_end = PPG_EB.end;
		PPG_EB.end = PPG_EB.cur;
		
		ppg_event_buffer_iterate(
			(PPG_Event_Processor_Fun)ppg_flush_non_considered_events, 
			NULL);
		
		PPG_EB.end = old_end; // Revert the original end
		
		PPG_EB.start = PPG_EB.cur; // Truncate the front of the queue
		
		ppg_even_buffer_recompute_size();
	}
}

void ppg_even_buffer_flush_and_remove_first_event(
							PPG_Slot_Id slot_id)
{
	ppg_context->input_processor(&PPG_EB.events[PPG_EB.start], NULL);
	
	if(PPG_EB.size > 1) {
		if(PPG_EB.start < PPG_MAX_EVENTS - 1) {
			++PPG_EB.start;
		}
		else {
			PPG_EB.start = 0;
		}
		
		--PPG_EB.size;
	}
	else {
		ppg_event_buffer_init(&PPG_EB);
	}
}
	
static void ppg_event_buffer_check_and_tag_considered(PPG_Event *event, 
														  void *user_data)
{
	bool on_success = (bool)user_data;
	
	// Mark all those events that are activated and consumed. This
	// must only be called on success.
	//
	if(ppg_bitfield_get_bit(&ppg_context->active_inputs,
									event->input_id)) {
		
		if(event->flags & PPG_Event_Active) {
			
			// Something goes wrong here, as
			// an event must not be activated twice in a row
			//
			PPG_ERROR("Input %d was activated twice in a row without deactivating\n", event->input_id);
			PPG_ASSERT(0);
			exit(1);
		}
		else {
			event->flags |= PPG_Event_Considered;
			
			ppg_bitfield_set_bit(&ppg_context->active_inputs,
									event->input_id,
									false /* inactivate */
  								);
		}
	}
	else {
		
		if(event->flags & PPG_Event_Active) {
			
			if(on_success) {
				ppg_bitfield_set_bit(&ppg_context->active_inputs,
										event->input_id,
										true /* active */
									);
			}
		}
// 		else {
// 						
// 			// Something goes wrong here, as
// 			// an event must not be deactivated twice in a row
// 			//
// 			PPG_ERROR("Input %d was deactivated twice in a row without deactivating\n", event->input_id);
// 			assert(0);
// 		}
	}
}
	
void ppg_event_buffer_prepare_on_success(void)
{
	PPG_PRINTF("Preparing event buffer on success\n");
	
	ppg_event_buffer_iterate(
			(PPG_Event_Processor_Fun)ppg_event_buffer_check_and_tag_considered,
			(void*)true
	);
}

static void ppg_clear_considered_flag_aux(PPG_Event *event, 
														  void *user_data)
{
	event->flags &= ~PPG_Event_Considered;
}

void ppg_event_buffer_prepare_on_failure(void)
{
	PPG_PRINTF("Preparing event buffer on failure\n");
	
	ppg_event_buffer_iterate(
			(PPG_Event_Processor_Fun)ppg_clear_considered_flag_aux,
			NULL
	);
		
	ppg_event_buffer_iterate(
			(PPG_Event_Processor_Fun)ppg_event_buffer_check_and_tag_considered,
			(void*)false
	);
}

void ppg_event_buffer_on_match_success(void)
{
	ppg_recurse_and_cleanup_active_branch();
				
	ppg_event_buffer_prepare_on_success();
	
	// Even though the pattern matches, it is possible that not
	// all events were considered as there might have been a
	// a tree furcation traverse involved. This might leave events
	// after the current event that might be part of
	// a future match.
	//
	// Thus, we remove all events up to the current one and leave the
	// rest.
	//
	ppg_event_buffer_truncate_at_front();
}
