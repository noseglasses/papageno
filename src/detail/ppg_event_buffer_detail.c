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
#include "ppg_debug.h"
#include "ppg_settings.h"

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

static void ppg_mark_active_inputs(PPG_Event *event, void* user_data)
{
	if(event->flags & PPG_Event_Considered) {
#ifndef PPG_PEDANTIC_ACTIONS
		ppg_bitfield_set_bit(&ppg_context->active_inputs,
									event->input_id,
									(event->flags & PPG_Event_Active)
		);
#endif
	}
	else {
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
		
		// Loop over all stored events and mark those that were 
		// considered as currently active. Any events that were not
		// considered, e.g. intermixed deactivations of currently
		// unrelated inputs are flushed.
		
		// Temporarily reset the end of the event buffer to
		// simplify flushing
		//
		PPG_Count old_end = PPG_EB.end;
		PPG_EB.end = PPG_EB.cur;
		
		ppg_event_buffer_iterate(ppg_mark_active_inputs, NULL);
		
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

void ppg_event_buffer_iterate(
								PPG_Event_Processor_Fun kp,
								void *user_data)
{
	if(ppg_event_buffer_size() == 0) { return; }
	
	if(PPG_EB.size == 0) { return; }
	
	if(PPG_EB.start > PPG_EB.end) {
		
		for(PPG_Count i = PPG_EB.start; i < PPG_MAX_EVENTS; ++i) {
		
			kp(&PPG_EB.events[i], user_data);
		}
		for(PPG_Count i = 0; i < PPG_EB.end; ++i) {
			
			kp(&PPG_EB.events[i], user_data);
		}
	}
	else {
		for(PPG_Count i = PPG_EB.start; i < PPG_EB.end; ++i) {
		
			kp(&PPG_EB.events[i], user_data);
		}
	}
}
