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

#include "ppg_event.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_global_detail.h"
#include "detail/ppg_pattern_matching_detail.h"
#include "detail/ppg_signal_detail.h"
#include "ppg_debug.h"
// 
#include <stdbool.h>

/** @brief Checks if an arriving event can possibly be ignored
 */
static bool ppg_check_ignore_event(PPG_Event *event)
{  
   /* Check if the pattern is being aborted
    */
   if(   (ppg_context->abort_input == event->input)
      && (event->flags & PPG_Event_Active)) {
      
      // Mark the event as a control event
      // to ensure that corresponding deactivation of the
      // respective input is swallowed (i.e. not flushed)
      //
      event->flags |= PPG_Event_Considered;
   
      /* If a pattern is in progress, we abort it and consume the abort input.
       */
      if(ppg_context->current_token) {
         
         PPG_LOG("Abrt trggr\n");
         
         ppg_global_abort_pattern_matching();
         
         return true;
      }
   
      return true;
   }
   
   return false;
}

void ppg_event_process(PPG_Event *event)
{
   if(!ppg_context->papageno_enabled) {
      return;
   }
   
   PPG_LOG("Input 0x%d, active %d\n", event->input, 
              event->flags & PPG_Event_Active);
   
   ppg_timeout_check();
   
   // Register the time of arrival to check for timeout
   //
   ppg_context->time_manager.time(&ppg_context->time_last_event);
   
   
//    PPG_LOG("time: %ld\n", ppg_context->time_last_event);
   
   event = ppg_event_buffer_store_event(event);
   
   // If there are active tokens on the stack,
   // we allow them to consume the event without
   // storing it.
   //
   if(ppg_active_tokens_check_consumption(event)) {
      
      ppg_signal(PPG_On_Orphaned_Deactivation);       

      ppg_delete_stored_events();
      
      return;
   }

   if(ppg_check_ignore_event(event)) {
      return;
   }

   ppg_pattern_matching_run();
   
   return;
}
