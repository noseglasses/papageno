/* Copyright 2017 Florian Fleissner
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

#include "detail/ppg_event_buffer_detail.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_global_detail.h"
#include "detail/ppg_malloc_detail.h"
#include "ppg_debug.h"
#include "ppg_settings.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PPG_EB ppg_context->event_buffer

PPG_Count ppg_event_buffer_size(void)
{
   return PPG_EB.size;
}

void ppg_event_buffer_resize(PPG_Event_Buffer *event_buffer,
                             PPG_Count new_size)
{
   PPG_ASSERT(event_buffer);
   
   if(new_size <= event_buffer->max_size) { return; }
   
   PPG_Event_Queue_Entry *new_events
      = (PPG_Event_Queue_Entry*)PPG_MALLOC(sizeof(PPG_Event_Queue_Entry)*new_size);
      
   event_buffer->max_size = new_size;
   
   if(event_buffer->events && (event_buffer->size > 0)) {
      memcpy(new_events, event_buffer->events, 
             sizeof(PPG_Event_Queue_Entry)*event_buffer->size);
   }
   
   event_buffer->events = new_events;
}

// Reurns an in place version of the event
//
PPG_Event * ppg_event_buffer_store_event(PPG_Event *event)
{
   #if PPG_HAVE_ASSERTIONS
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
   
   PPG_EB.events[new_pos] = (PPG_Event_Queue_Entry) {
      .event = *event,
      .consumer = NULL,
      .token_state = (PPG_Token_State) {
         .state = 0,
         .changed = false
      }
   };
   
   ++PPG_EB.size;
   
   PPG_LOG("Storing event at %u\n", PPG_EB.end);
   PPG_LOG("   start: %u, cur: %u, end: %u, size: %u\n", 
              PPG_EB.start, PPG_EB.cur, PPG_EB.end, PPG_EB.size);
   
   return &PPG_EB.events[new_pos].event;
}

void ppg_event_buffer_reset(PPG_Event_Buffer *eb)
{
   eb->start = 0;
   eb->end = 0;
   eb->cur = 0;
   
   eb->size = 0;
   
   PPG_LOG("Event queue reset\n");
   PPG_LOG("   start: %u, cur: %u, end: %u, size: %u\n", 
              eb->start, eb->cur, eb->end, eb->size);
}

void ppg_event_buffer_init(PPG_Event_Buffer *eb)
{
   ppg_event_buffer_reset(eb);
   
   eb->max_size = 0;
   eb->events = NULL;
   
   ppg_event_buffer_resize(eb, PPG_MAX_EVENTS);
}


void ppg_event_buffer_restore(PPG_Event_Buffer *eb)
{
   PPG_Count safed_size = eb->max_size;
   
   eb->events = NULL; 
   eb->max_size = 0; // This forces resize 
   
   ppg_event_buffer_resize(eb, safed_size);
}

void ppg_event_buffer_free(PPG_Event_Buffer *event_buffer)
{
   if(!event_buffer->events) { return; }
   
   free(event_buffer->events);
   
   event_buffer->events = NULL;
}

bool ppg_event_buffer_events_left(void)
{
//    PPG_LOG("ppg_event_buffer_events_left: %u, cur: %u, end: %u, size: %u\n", 
//               PPG_EB.start, PPG_EB.cur, PPG_EB.end, PPG_EB.size);
   return   (PPG_EB.cur != PPG_EB.end);
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
   
   #if PPG_HAVE_ASSERTIONS
   ppg_check_event_buffer_validity();
   #endif
   
   PPG_LOG("Event queue advanced\n");
   PPG_LOG("   start: %u, cur: %u, end: %u, size: %u\n", 
              PPG_EB.start, PPG_EB.cur, PPG_EB.end, PPG_EB.size);
} 

#if PPG_HAVE_ASSERTIONS
void ppg_check_event_buffer_validity(void)
{
   if(PPG_EB.end > PPG_EB.start) {
      PPG_ASSERT(PPG_EB.cur >= PPG_EB.start);
      PPG_ASSERT(PPG_EB.cur <= PPG_EB.end);
   }
   else if(PPG_EB.end == PPG_EB.start) {
      PPG_ASSERT(PPG_EB.size == 0);
      PPG_ASSERT(PPG_EB.end == PPG_EB.cur);
   }
   else {
      PPG_ASSERT( !(    (PPG_EB.cur > PPG_EB.end)
                     && (PPG_EB.cur < PPG_EB.start)));
   }
}
#endif

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

static void ppg_flush_non_considered_events(PPG_Event_Queue_Entry *eqe, 
                                            void* user_data)
{
   if(!(eqe->event.flags & PPG_Event_Considered)) {

      // Events that were not considered and are 
      // not control tags such as those used for 
      // abort input events are flushed
      //
      ppg_context->event_processor(&eqe->event, NULL);
   }
}

void ppg_event_buffer_truncate_at_front(void)
{
   if(PPG_EB.cur == PPG_EB.end) {
      ppg_event_buffer_reset(&PPG_EB);
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
      
      ppg_event_buffer_iterate2(
         (PPG_Event_Processor_Visitor)ppg_flush_non_considered_events, 
         NULL);
      
      PPG_EB.end = old_end; // Revert the original end
      
      PPG_EB.start = PPG_EB.cur; // Truncate the front of the queue
      
      ppg_even_buffer_recompute_size();
      
   }
//    PPG_LOG("Event queue truncated at front\n"); 
//    PPG_LOG("   start: %u, cur: %u, end: %u, size: %u\n", 
//               PPG_EB.start, PPG_EB.cur, PPG_EB.end, PPG_EB.size);
   
}

// This method does not erase events but just changes
// the input position
//
void ppg_event_buffer_remove_first_event(void)
{
   if(PPG_EB.size == 0) { return; }
   
   if(PPG_EB.start < PPG_MAX_EVENTS - 1) {
      ++PPG_EB.start;
   }
   else {
      PPG_EB.start = 0;
   }
   
   --PPG_EB.size;
   
//    PPG_LOG("Event queue removed first event\n");
//    PPG_LOG("   start: %u, cur: %u, end: %u, size: %u\n", 
//               PPG_EB.start, PPG_EB.cur, PPG_EB.end, PPG_EB.size);
   
}

void ppg_even_buffer_flush_and_remove_first_event(bool on_success)
{
//    PPG_LOG("Flushing and removing first event\n");
   
   ppg_context->event_processor(&PPG_EB.events[PPG_EB.start].event, NULL);
   
   if(PPG_EB.size > 1) {
      ppg_event_buffer_remove_first_event();
   }
   else {
      ppg_event_buffer_reset(&PPG_EB);
   }
}

void ppg_event_buffer_on_match_success(void)
{
   PPG_LOG("Prp. evt bffr on suc.\n");
            
   ppg_active_tokens_update();
   
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

void ppg_event_buffer_iterate2(
                        PPG_Event_Processor_Visitor visitor,
                        void *user_data)
{
   if(ppg_event_buffer_size() == 0) { return; }
   
   if(PPG_EB.size == 0) { return; }
   
   if(PPG_EB.start > PPG_EB.end) {
      
      for(PPG_Count i = PPG_EB.start; i < PPG_MAX_EVENTS; ++i) {
      
         visitor(&PPG_EB.events[i], user_data);
      }
      for(PPG_Count i = 0; i < PPG_EB.end; ++i) {
         
         visitor(&PPG_EB.events[i], user_data);
      }
   }
   else {
      for(PPG_Count i = PPG_EB.start; i < PPG_EB.end; ++i) {
      
         visitor(&PPG_EB.events[i], user_data);
      }
   }
}
