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
		PPG_Count n_events = PPG_MAX_EVENTS + PPG_EB.end - PPG_EB.start - 1;
		PPG_ASSERT(n_events < PPG_MAX_EVENTS - 1);
	}
	else {
		PPG_Count n_events = PPG_EB.end - PPG_EB.start + 1;
		PPG_ASSERT(n_events < PPG_MAX_EVENTS - 1);
	}
	#endif
	
	if(PPG_EB.end == PPG_MAX_EVENTS - 1) {
		PPG_EB.end = 0;
	}
	else {
		++PPG_EB.end;
	}
	
	PPG_EB.events[PPG_EB.end] = *event;
	
	++PPG_EB.size;
}

void ppg_event_buffer_init(PPG_Event_Buffer *eb)
{
	eb->start = 0;
	eb->end = -1;
	eb->cur = 0;
	
	eb->size = 0;
}

void ppg_event_buffer_iterate_events(PPG_Slot_Id slot_id, PPG_Event_Processor_Fun fun, void *user_data)
{
	if(PPG_EB.size == 0) { return; }
	
	if(PPG_EB.start > PPG_EB.end) {
		
		for(PPG_Count i = PPG_EB.start; i < PPG_MAX_EVENTS; ++i) {
		
			if(!fun(&PPG_EB.events[i], slot_id, user_data)) { 
				return;
			}
		}
		for(PPG_Count i = 0; i <= PPG_EB.end; ++i) {
			if(!fun(&PPG_EB.events[i], slot_id, user_data)) { 
				return;
			}
		}
	}
	else {
		for(PPG_Count i = PPG_EB.start; i <= PPG_EB.end; ++i) {
		
			if(!fun(&PPG_EB.events[i], slot_id, user_data)) { 
				return;
			}
		}
	}
}

bool ppg_event_buffer_events_left(void)
{
	return 	(PPG_EB.cur != PPG_EB.end)
			&& (PPG_EB.end != -1);
}

void ppg_event_buffer_advance(void)
{
	if(PPG_EB.end == -1) { return; }
	
	if(PPG_EB.cur == PPG_MAX_EVENTS - 1) {
		PPG_EB.cur = 0;
	}
	else {
		++PPG_EB.cur;
	}
} 

static void ppg_even_buffer_recompute_size(void)
{
	if(PPG_EB.end >= PPG_EB.start) {
		
		PPG_EB.size = PPG_EB.end + 1 - PPG_EB.start;
	}
	else {
		
		PPG_EB.size = PPG_MAX_EVENTS + PPG_EB.end - PPG_EB.start;
	}
}

void ppg_event_buffer_truncate_at_front(void)
{
	if(PPG_EB.cur == PPG_EB.end) {
		ppg_event_buffer_init(&PPG_EB);
	}
	else {
		ppg_event_buffer_advance();
		
		PPG_EB.start = PPG_EB.cur;
		
		ppg_even_buffer_recompute_size();
	}
}

void ppg_even_buffer_flush_and_remove_first_event(
							PPG_Slot_Id slot_id)
{
	ppg_context->input_processor(&PPG_EB.events[PPG_EB.start], slot_id, NULL);
	
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

