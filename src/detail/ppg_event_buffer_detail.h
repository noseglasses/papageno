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

#ifndef PPG_EVENT_BUFFER_DETAIL_H
#define PPG_EVENT_BUFFER_DETAIL_H

#include "ppg_event.h"
#include "ppg_settings.h"

// The event buffer is a ring buffer
//
typedef struct {
	
	PPG_Event events[PPG_MAX_EVENTS];
	
	PPG_Event_Buffer_Index_Type start;
	PPG_Event_Buffer_Index_Type end;
	PPG_Event_Buffer_Index_Type cur;
	
	PPG_Count size;
	
} PPG_Event_Buffer;

PPG_Count ppg_event_buffer_size(void);

void ppg_event_buffer_store_event(PPG_Event *event);

void ppg_event_buffer_init(PPG_Event_Buffer *eb);

void ppg_event_buffer_iterate_events(PPG_Slot_Id slot_id, PPG_Event_Processor_Fun fun, void *user_data);

bool ppg_event_buffer_events_left(void);

void ppg_event_buffer_advance(void);

void ppg_event_buffer_truncate_at_front(void);

void ppg_even_buffer_flush_and_remove_first_event(
							PPG_Slot_Id slot_id);

#define PPG_EB ppg_context->event_buffer

#endif
