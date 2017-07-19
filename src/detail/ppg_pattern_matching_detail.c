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

void ppg_branch_cleanup(
                        PPG_Token__ *cur_token,
                        PPG_Token__ *end_token)
{
   // Unwind and cleanup back to the current furcation or to the
   // root node
   // 
   while(cur_token != end_token) {

      // Restore state flag to initialization state
      //
      ppg_token_reset(cur_token);
      
      // Reset members
      //
      PPG_CALL_VIRT_METHOD(cur_token, reset);
      
      cur_token = cur_token->parent;
   }
}

static PPG_Token__ *ppg_furcation_revert(PPG_Token__ *start_token)
{
   // If there is no current furcation, we at least clean up to 
   // the root node. Otherwise we cleanup to the current furcation.
   //
   PPG_Token__ *furcation_token = NULL;
      
   while(1) {
      
      furcation_token
         = (PPG_FB.cur_furcation == -1) ? NULL : PPG_CUR_FUR.token;
   
      // Recursively reset all tokens of the branch we are on
      // back to the first branch node or back to the first
      // node after the root node of the search tree.
      // 
      ppg_branch_cleanup(start_token, furcation_token);
      
      // If there is no current furcation, we can't do anything else
      //
      if(!furcation_token) { 
         
         // By returning NULL we signal that there is no
         // reversion to another furcation possible.
         //
         return NULL;
      }
      
      --PPG_CUR_FUR.n_branch_candidates;
      
      // If we already tried all possible branches of the current furcation
      //
      if(PPG_CUR_FUR.n_branch_candidates == 0) {
         
         // ... we mark all children as initialized. 
         
         // The actual reset of the nodes/tokens state variables
         // has already taken place during ppg_branch_cleanup above.
         //
         for(PPG_Count i = 0; i < furcation_token->n_children; ++i) {
            ppg_token_reset(furcation_token->children[i]);
         }
         
         // Set the current branch token as new start token for
         // further branch cleanup
         //
         start_token = PPG_CUR_FUR.token;
         
         #ifdef PPG_HAVE_STATISTICS
         ++ppg_context->statistics.n_reversions;
         #endif
         
         // Replace the current furcation with the previous one (if possible)
         //
         --PPG_FB.cur_furcation;
      }
      else {
      
         // Mark the current branch (i.e. the branch's root node) as invalid
         //
         PPG_CUR_FUR.branch->state = PPG_Token_Invalid;
         
         break;
      }
   }
   
   // Reset the current event that is used for further processing
   //
   PPG_EB.cur = PPG_CUR_FUR.event_id;
      
   #ifdef PPG_HAVE_ASSERTIONS
   ppg_check_event_buffer_validity();
   #endif
   
   return PPG_CUR_FUR.token;
}

static PPG_Token__ *ppg_token_get_most_appropriate_branch(
                        PPG_Token__ *parent_token,
                        PPG_Count *n_branch_candidates)
{
   // Determine the number of possible candidates
   //
   PPG_Layer highest_layer = -1;
   PPG_Token__ *branch_token = NULL;
   PPG_Count precedence = 0;
   
   *n_branch_candidates = 0;
   
//    PPG_LOG("Getting most appropriate child\n");
   
   /* Find the most suitable token with respect to the current ppg_context->layer.
   */
   for(PPG_Count i = 0; i < parent_token->n_children; ++i) {
         
      if(parent_token->children[i]->state == PPG_Token_Invalid) {
         continue;
      }
      
      /* Accept only paths through the search tree whose
      * nodes' ppg_context->layer tags are lower or equal the current ppg_context->layer
      */
      if(parent_token->children[i]->layer > ppg_context->layer) { 
         parent_token->children[i]->state = PPG_Token_Invalid;
         continue; 
      }

      ++(*n_branch_candidates);
   
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
         
         branch_token = parent_token->children[i];
      }
      else {
         
//          PPG_LOG("Equal precedence\n");
         
         if(parent_token->children[i]->layer > highest_layer) {
            highest_layer = parent_token->children[i]->layer;
            branch_token = parent_token->children[i];
         }
      }
//       PPG_LOG("match_id %d\n", match_id);
   }
   
   return branch_token;
}

static PPG_Token__ *ppg_token_get_next_possible_branch(
                                 PPG_Token__ *parent_token)
                        
{
   // First check if there is already a furcation for the 
   // parent node.
   //
   bool furcation_already_present = 
            (PPG_CUR_FUR.token 
                  == parent_token);
            
   bool revert_to_previous_furcation = false;
   
   if(parent_token->state == PPG_Token_Invalid) {
      revert_to_previous_furcation = true;
   }
   else if(parent_token->n_children == 1) {
      
      if(parent_token->children[0]->state == PPG_Token_Invalid) {
         revert_to_previous_furcation = true;
      }
      else {
         return parent_token->children[0];
      }
   }
   
   if(revert_to_previous_furcation) {
      
      PPG_LOG("Reverting to previous furcation\n");
      
      parent_token = ppg_furcation_revert(parent_token);
      
      if(!parent_token) {
         
         // By returning NULL, we signal that there is no next
         // possible branch
         //
         return NULL;
      }
      
      furcation_already_present = true;
   }
      
   PPG_Count n_branch_candidates = 0;
   
   PPG_Token__ *branch 
      = ppg_token_get_most_appropriate_branch(
                                             parent_token,
                                             &n_branch_candidates
      );

   if(furcation_already_present) {
      
      PPG_LOG("Updating furcation for token 0x%" PRIXPTR "\n", 
            (uintptr_t)ppg_context->current_token);
      
      PPG_CUR_FUR.branch = branch;
   }
   else {
      
      #ifdef PPG_HAVE_STATISTICS
      ++ppg_context->statistics.n_furcations;
      #endif
      
      // Create a new furcation data set
      //
      PPG_LOG("Creating furcation for token 0x%" PRIXPTR "\n", 
            (uintptr_t)parent_token);

      PPG_ASSERT(PPG_FB.cur_furcation < PPG_FB.n_furcations - 1);
      
      ++PPG_FB.cur_furcation;
                  
      PPG_CUR_FUR.event_id = PPG_EB.cur;
      PPG_CUR_FUR.token = parent_token;
      PPG_CUR_FUR.branch = branch;
   }
   
   PPG_CUR_FUR.n_branch_candidates 
                  = n_branch_candidates;
   
   PPG_LOG("Next branch 0x%" PRIXPTR ": candidates %u\n", 
             (uintptr_t)branch, n_branch_candidates);

   return branch;
}

static PPG_Count ppg_process_next_event(void)
{  
   PPG_LOG("Processing next event\n");

   PPG_LOG("Current token 0x%" PRIXPTR ", state %u\n", 
             (uintptr_t)ppg_context->current_token,
             ppg_context->current_token->state
          );
 
   if(!  ((ppg_context->current_token->state == PPG_Token_In_Progress)
      || (ppg_context->current_token->state == PPG_Token_Initialized))) {
      
      PPG_Token__ *branch_token = NULL;
   
      switch (ppg_context->current_token->state) {
         
         case PPG_Token_Root:
         case PPG_Token_Matches:
         case PPG_Token_Invalid:
            {
               PPG_LOG("Determining branch\n");
               
               // Here, we can rely on the fact that after a match
               // during processing the previous event, we already checked
               // for the pattern being finished. So the current token
               // must have children if we ended here.

               // Find the most appropriate branch to continue with
               //
               branch_token 
                     = ppg_token_get_next_possible_branch(
                                             ppg_context->current_token);

            }
            break;
//          case PPG_Token_Invalid:
//             {
//                branch_token
//                      = ppg_furcation_revert(ppg_context->current_token);
//             }
//             break;
      }
                     
      // There are no possible branches left, e.g.
      // if we already tried all options on this level ...
      //
      if(!branch_token) {
            
         // If no furcation can be found this means that
         // we already traversed all candidate branches of
         // the search tree. That no matching branch can 
         // be found means that no match for the overall pattern
         // is possible.
         //
         return PPG_Pattern_Invalid;
      }
      
      #ifdef PPG_HAVE_STATISTICS
      ++ppg_context->statistics.n_nodes_visited;
      #endif
      
      // We are lucky. There are further branches left.
      
      // Continue with the branch
      //
      ppg_context->current_token = branch_token;
   }

   PPG_Event *event = &PPG_EB.events[PPG_EB.cur];
   
   PPG_LOG("Branch token  0x%" PRIXPTR "\n", 
             (uintptr_t)ppg_context->current_token);
   
   PPG_LOG("Event queue: start: %u, cur: %u, end: %u, size: %u\n", 
              PPG_EB.start, PPG_EB.cur, PPG_EB.end, PPG_EB.size);
   
   // As the token to process the event.
   //
   ppg_context->current_token
            ->vtable->match_event(  
                     ppg_context->current_token, 
                     event
               );
            
   #ifdef PPG_HAVE_STATISTICS
   ++ppg_context->statistics.n_token_checks;
   #endif
            
   PPG_LOG("Token state after match_event: %u\n", ppg_context->current_token->state);

   switch(ppg_context->current_token->state) {
      
      case PPG_Token_Matches:
         if(ppg_context->current_token->n_children == 0){
            
            PPG_LOG("p match\n");
         
            // We found a matching pattern
            //
            return PPG_Pattern_Matches;
         }
         break;
      case PPG_Token_Invalid:
  
         return PPG_Pattern_Branch_Reversion;
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

void ppg_pattern_matching_run(void)
{
   //PPG_LOG("ppg_pattern_matching_run\n");
   
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
            
            // Repeat with the same event. The current 
            // event might be changed during branch reversion.
            
            continue;
      }
      
      // Prepare for restart of pattern matching
      
      ppg_reset_pattern_matching_engine();
   }
}