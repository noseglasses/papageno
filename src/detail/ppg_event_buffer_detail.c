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
#include "ppg_debug.h"
#include "ppg_settings.h"

#include <stdlib.h>

#define PPG_EB ppg_context->event_buffer

PPG_Count ppg_event_buffer_size(void)
{
   return PPG_EB.size;
}

// Reurns an in place version of the event
//
PPG_Event * ppg_event_buffer_store_event(PPG_Event *event)
{
   #ifdef PAPAGENO_HAVE_ASSERTIONS
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
   
   //PPG_LOG("Storing event at %u\n", PPG_EB.end);
   
   PPG_EB.events[new_pos] = *event;
   
   ++PPG_EB.size;
   
   return &PPG_EB.events[new_pos];
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

      // Events that were not considered and are 
      // not control tags such as those used for 
      // abort input events are flushed
      //
      ppg_context->event_processor(event, NULL);
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
   
static void ppg_event_buffer_check_and_tag_considered(PPG_Event *event, 
                                            void *user_data)
{
   bool on_success = (bool)user_data;
   
   PPG_LOG("I %u, a %d\n", event->input, event->flags & PPG_Event_Active);
   
   #if PAPAGENO_HAVE_ASSERTIONS
   bool already_activated 
         = ppg_bitfield_get_bit(&ppg_context->active_inputs,
                              event->input);
   #endif
         
   bool event_activates = event->flags & PPG_Event_Active;
   
   if(event_activates) {
      
      PPG_ASSERT(!already_activated);
   }
   else {
      
      if(!already_activated) {
         
         // Inore non matching deactivations, i.e. deactivations
         // of inputs that were not activated by a match before.
         //
         return;
      }
   }
         
   ppg_bitfield_set_bit(&ppg_context->active_inputs,
                        event->input,
                        event_activates
                     );

   if(on_success || already_activated) {
      event->flags |= PPG_Event_Considered;
   }
}

void ppg_even_buffer_flush_and_remove_first_event(bool on_success)
{
   ppg_event_buffer_check_and_tag_considered(
                           &PPG_EB.events[PPG_EB.start],
                           (void*)on_success);
   
   ppg_context->event_processor(&PPG_EB.events[PPG_EB.start], NULL);
   
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
   
void ppg_event_buffer_prepare_on_success(void)
{
   ppg_event_buffer_iterate(
         (PPG_Event_Processor_Fun)ppg_event_buffer_check_and_tag_considered,
         (void*)true
   );
}

void ppg_event_buffer_prepare_on_failure(void)
{
//    PPG_LOG("Prp. evt bffr on fail.\n");
//    
//    ppg_input_list_all_active();
//       
//    ppg_event_buffer_iterate(
//          (PPG_Event_Processor_Fun)ppg_event_buffer_check_and_tag_considered,
//          (void*)false
//    );
}

void ppg_event_buffer_on_match_success(void)
{
   ppg_recurse_and_cleanup_active_branch();
            
   PPG_LOG("Prp. evt bffr on suc.\n");
   
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
