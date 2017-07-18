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
#include "detail/ppg_furcation_detail.h"
#include "detail/ppg_event_buffer_detail.h"
#include "detail/ppg_signal_detail.h"
#include "ppg_debug.h"
#include "ppg_bitfield.h"

enum {
   PPG_Pattern_State_Undefined = 0,
   PPG_Pattern_Matches,
   PPG_Pattern_Invalid,
   PPG_Pattern_Orphaned_Deactivation,
   PPG_Pattern_In_Progress,
   PPG_Pattern_Branch_Reversion
};

static PPG_Token__ * ppg_token_get_most_appropriate_branch_token(
                        PPG_Token__ *parent_token,
                        PPG_Count *n_candidates)
                        
{
   PPG_Layer highest_layer = -1;
   PPG_Token__ *child_token = NULL;
   PPG_Count precedence = 0;
   
   PPG_Count n_candidates__ = 0;
   
//    PPG_LOG("Getting most appropriate child\n");
   
   /* Find the most suitable token with respect to the current ppg_context->layer.
      */
   for(PPG_Count i = 0; i < parent_token->n_children; ++i) {
   
      /* Accept only paths through the search tree whose
      * nodes' ppg_context->layer tags are lower or equal the current ppg_context->layer
      */
      if(parent_token->children[i]->layer > ppg_context->layer) { continue; }
      
      if(parent_token->children[i]->state == PPG_Token_Invalid) {
         continue;
      }
      
      ++n_candidates__;
   
//       PPG_LOG("Child %d\n", i);
      
//       PPG_CALL_VIRT_METHOD(parent_token->children[i], print_self, 0, false);
      
      PPG_Count cur_precedence 
            = parent_token->children[i]
                  ->vtable->token_precedence(parent_token->children[i]);
                  
//       PPG_LOG("Cur precedence %d\n", cur_precedence);
//       PPG_LOG("precedence %d\n", precedence);
            
      if(cur_precedence > precedence) {
         precedence = cur_precedence;
         highest_layer = parent_token->children[i]->layer;
         
         child_token = parent_token->children[i];
      }
      else {
         
//          PPG_LOG("Equal precedence\n");
         
         if(parent_token->children[i]->layer > highest_layer) {
            highest_layer = parent_token->children[i]->layer;
            child_token = parent_token->children[i];
         }
      }
//       PPG_LOG("match_id %d\n", match_id);
   }
   
   *n_candidates = n_candidates__;
   
   return child_token;
}

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
   
static PPG_Count ppg_process_next_event(void)
{  
   //PPG_LOG("ppg_process_next_event\n");

   PPG_Event *event = &PPG_EB.events[PPG_EB.cur];

   PPG_LOG("Parent tk 0x%" PRIXPTR "\n", 
             (uintptr_t)ppg_context->current_token);
   
   // Check if the current node was a match after the last
   // step. If so, set the next possible branch
   // as current token.
   //
   if(   (ppg_context->current_token->state == PPG_Token_Matches)
      || (ppg_context->current_token->state == PPG_Token_Root)
   ) {
      // Here, we can rely on the fact that after a match
      // during processing the previous event, we already checked
      // for the pattern being finished. So the current token
      // must have children if we ended here.
 
      PPG_Count n_branch_candidates = 0;
      
      PPG_Token__ *branch_token = NULL;
      
      while(!branch_token) {
      
         // Find the most appropriate branch to continue with
         //
         branch_token 
               = ppg_token_get_most_appropriate_branch_token(
                                       ppg_context->current_token,
                                       &n_branch_candidates);
         
         // There are no possible branches left, e.g.
         // if we already tried all options on this level ...
         //
         if(!branch_token) {
            
            // ... revert to the next previous furcation
            //
            bool furcation_found 
               = ppg_furcation_revert(ppg_context->current_token);
               
            if(!furcation_found) {
               
               // If no furcation can be found this means that
               // we already traversed all candidate branches of
               // the search tree. That no matching branch can 
               // be found means that no match for the overall pattern
               // is possible.
               //
               return PPG_Pattern_Invalid;
            }
            
            // We are lucky. There are further branches left.
            
            // Revert the current token to the next possible furcation
            //
            ppg_context->current_token = PPG_CUR_FUR.token;
         }
      }

      // Only branch if there are more than one child node.
      //
      if(ppg_context->current_token->n_children > 1) {
         
         // We create a new furcation data set, the first
         // time we traverse the node. 
         // The next time we only update the existing furcation.
         //
         ppg_furcation_create_or_update(
                           n_branch_candidates,
                           branch_token);
      }
         
      ppg_context->current_token = branch_token;
   }
   
   PPG_LOG("Chk tk 0x%" PRIXPTR "\n", 
             (uintptr_t)ppg_context->current_token);
   
   PPG_LOG("start: %u, cur: %u, end: %u, size: %u\n", 
              PPG_EB.start, PPG_EB.cur, PPG_EB.end, PPG_EB.size);
   
   // As the token to process the event.
   //
   ppg_context->current_token
            ->vtable->match_event(  
                     ppg_context->current_token, 
                     event
               );

   switch(ppg_context->current_token->state) {
      
      case PPG_Token_Matches:
   
         if(ppg_context->current_token->n_children == 0) {
            
            PPG_LOG("p match\n");
         
            // We found a matching pattern
            //
            return PPG_Pattern_Matches;
         }
         break;
         
      case PPG_Token_Invalid:
         {
            PPG_LOG("t invalid\n");
            
            // Revert to the next previous furcation
            //
            bool furcation_found 
               = ppg_furcation_revert(
                                 ppg_context->current_token);
            
            if(!furcation_found) {
               PPG_LOG("p invalid\n");
               return PPG_Pattern_Invalid;
            }
            
            return PPG_Pattern_Branch_Reversion;
         }
         break;
   }
   
   // If the event is a deactivation event that was obviously
   // not matched and it is the first in the queue, 
   // we flush.
   //
   if(   !(event->flags & PPG_Event_Active)
      && (ppg_event_buffer_size() == 1)) {
      
      PPG_LOG("orpth deact\n");
   
      return PPG_Pattern_Orphaned_Deactivation;
   }
      
   PPG_LOG("p in prog\n");
   
   return PPG_Pattern_In_Progress;
}

static void ppg_event_process_all_possible(void)
{
   //PPG_LOG("ppg_event_process_all_possible\n");
   
   while(ppg_event_buffer_events_left()) {
      
      PPG_Count process_event_result = ppg_process_next_event();
      
      switch(process_event_result) {
         
         case PPG_Pattern_Matches:
            
            ppg_recurse_and_process_actions();
            
            ppg_event_buffer_on_match_success();
            
            break;
            
         case PPG_Pattern_Invalid:
            
            // Prepare the event buffer for 
            // user processing
            //
            ppg_event_buffer_prepare_on_failure();
            
            // If no furcation was found, there is no chance
            // for a match. Thus we remove the first stored event
            // and rerun the overall pattern matching based on a
            // new first event.
            //
            ppg_signal(PPG_On_Match_Failed);       
            
            PPG_LOG("Mtch fld\n");
            ppg_even_buffer_flush_and_remove_first_event(false /* no success */);
            
            break;
            
         case PPG_Pattern_Orphaned_Deactivation:
            
            PPG_LOG("Orph deact\n");
            
            // Prepare the event buffer for 
            // user processing
            //
            ppg_event_buffer_on_match_success();
            
            // If no furcation was found, there is no chance
            // for a match. Thus we remove the first stored event
            // and rerun the overall pattern matching based on a
            // new first event.
            //
            ppg_signal(PPG_On_Orphaned_Deactivation);       

            ppg_delete_stored_events();
            
            break;
            
         case PPG_Pattern_In_Progress:
            
            ppg_event_buffer_advance();
            
            continue;
            
         case PPG_Pattern_Branch_Reversion:
            
            // Repeat with same event
            //
            continue;
            
      }
      
      // Prepare for restart of pattern matching
      
      ppg_reset_pattern_matching_engine();
   }
}

void ppg_event_process(PPG_Event *event)
{
   if(!ppg_context->papageno_enabled) {
      return;
   }
   
   PPG_LOG("I %d: %d\n", event->input, 
              event->flags & PPG_Event_Active);
   
   ppg_timeout_check();
   
   // Register the time of arrival to check for timeout
   //
   ppg_context->time_manager.time(&ppg_context->time_last_event);
   
//    PPG_LOG("time: %ld\n", ppg_context->time_last_event);
   
   event = ppg_event_buffer_store_event(event);

   if(ppg_check_ignore_event(event)) {
      return;
   }

   ppg_event_process_all_possible();
   
   return;
}

void ppg_event_buffer_iterate(
                        PPG_Event_Processor_Fun event_processor,
                        void *user_data)
{
   if(ppg_event_buffer_size() == 0) { return; }
   
   if(PPG_EB.size == 0) { return; }
   
   if(PPG_EB.start > PPG_EB.end) {
      
      for(PPG_Count i = PPG_EB.start; i < PPG_MAX_EVENTS; ++i) {
      
         event_processor(&PPG_EB.events[i], user_data);
      }
      for(PPG_Count i = 0; i < PPG_EB.end; ++i) {
         
         event_processor(&PPG_EB.events[i], user_data);
      }
   }
   else {
      for(PPG_Count i = PPG_EB.start; i < PPG_EB.end; ++i) {
      
         event_processor(&PPG_EB.events[i], user_data);
      }
   }
}