/* Copyright 2017 noseglasses <shinynoseglasses@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PPG_EVENT_BUFFER_DETAIL_H
#define PPG_EVENT_BUFFER_DETAIL_H

#include "ppg_event.h"
#include "ppg_settings.h"
#include "ppg_bitfield.h"
#include "detail/ppg_token_detail.h"

typedef struct {
   unsigned char state         : PPG_Token_N_State_Bits;
   unsigned int changed       : 1;
} PPG_Token_State;

typedef struct {
   PPG_Event      event;
   PPG_Token__    *consumer;
   PPG_Token_State      token_state;
} PPG_Event_Queue_Entry;

// The event buffer is a ring buffer
//
typedef struct {
   
//    PPG_Event_Queue_Entry events[PPG_MAX_EVENTS];
   PPG_Event_Queue_Entry *events;
   
   PPG_Event_Buffer_Index_Type start;
   PPG_Event_Buffer_Index_Type end;
   PPG_Event_Buffer_Index_Type cur;
   
   PPG_Count size;
   
   PPG_Count max_size;
   
} PPG_Event_Buffer;

void ppg_event_buffer_resize(PPG_Event_Buffer *event_buffer,
                             PPG_Count new_size);

PPG_Count ppg_event_buffer_size(void);

void ppg_event_buffer_restore(PPG_Event_Buffer *eb);

PPG_Event * ppg_event_buffer_store_event(PPG_Event *event);

void ppg_event_buffer_init(PPG_Event_Buffer *eb);

void ppg_event_buffer_reset(PPG_Event_Buffer *eb);

void ppg_event_buffer_free(PPG_Event_Buffer *event_buffer);

bool ppg_event_buffer_events_left(void);

void ppg_event_buffer_advance(void);

void ppg_event_buffer_truncate_at_front(void);

void ppg_event_buffer_remove_first_event(void);

void ppg_even_buffer_flush_and_remove_first_event(bool on_success);

void ppg_event_buffer_flush_and_remove_non_processable_deactivation_events(void);

typedef void (*PPG_Event_Processor_Visitor)(PPG_Event_Queue_Entry *eqe,
                        void *user_data);

uint8_t ppg_event_buffer_iterate2(
                        PPG_Event_Processor_Visitor event_processor,
                        void *user_data);

// The two following functions prepare the
// event buffer for iteration from signal callbacks.
//
void ppg_event_buffer_prepare_on_failure(void);

void ppg_event_buffer_on_match_success(void);

#if PPG_HAVE_ASSERTIONS
void ppg_check_event_buffer_validity(void);
#endif

#endif
