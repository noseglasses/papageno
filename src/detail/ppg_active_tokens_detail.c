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

#include "detail/ppg_active_tokens_detail.h"
#include "detail/ppg_context_detail.h"
#include "ppg_debug.h"

void ppg_active_tokens_init(PPG_Active_Tokens *active_tokens)
{
   for(PPG_Count i = 0; i < PPG_ACTIVE_TOKENS_SPACE; ++i) {
      active_tokens->tokens[i] = NULL;
      active_tokens->free_ids[i] = i;
   }
   active_tokens->n_tokens = 0;
}

static void ppg_active_tokens_add(PPG_Token__ *token)
{
   PPG_Count next_free_id = PPG_GAT.free_ids[PPG_GAT.n_tokens];
   ++PPG_GAT.n_tokens;
   PPG_GAT.tokens[next_free_id] = token;
}

static void ppg_active_tokens_remove(PPG_Count id)
{
   --PPG_GAT.n_tokens;
   PPG_GAT.free_ids[PPG_GAT.n_tokens] = id;
   PPG_GAT.tokens[id] = NULL;
}

static void ppg_active_tokens_search_remove(PPG_Token__ *token)
{
   for(PPG_Count i = 0; i < PPG_GAT.n_tokens; ++i) {
      if(token == PPG_GAT.tokens[i]) {
         ppg_active_tokens_remove(i);
         break;
      }
   }
}

static void ppg_active_tokens_update_aux(
                                    PPG_Event_Queue_Entry *eqe,
                                    void *user_data)
{
   if(eqe->event.flags & PPG_Event_Active) {
      
      // The event activates an input
      
      if(   (eqe->token_state.state == PPG_Token_Matches)
         && eqe->token_state.changed
      ) {
         // The last event led to a match
            
         // Add the token to the active set
         //
         PPG_ASSERT(eqe->consumer);
         ppg_active_tokens_add(eqe->consumer);
         
         // As the token just matched. Lets check if
         // we are allowed to trigger the respective action.
         //
         if(eqe->consumer->misc.action_state == PPG_Action_Enabled) {
            eqe->consumer->action.callback.func(true /* signal activation */,
               eqe->consumer->action.callback.user_data);
         }
      }
   }
   else {
      
      // The event deactivates an input
      
      if(eqe->consumer) {
         
         // It there is a consumer listed, this means that the event was
         // consumed by one of the tokens of the matching branch
         
         if(eqe->token_state.changed) {
            
            // Token state just changed...
            
            if(eqe->token_state.state 
                        == PPG_Token_Deactivation_In_Progress) {
               
               // It turned from state "matching" to state
               // "deactivation in progress"
               
               if(eqe->consumer->misc.action_flags & PPG_Action_Deactivate_On_Token_Unmatch) {

                  if(eqe->consumer->misc.action_state == PPG_Action_Enabled) {
                     eqe->consumer->action.callback.func(false /* signal deactivation */,
                        eqe->consumer->action.callback.user_data);
                  }
               }
            }
            else if(eqe->token_state.state 
                              == PPG_Token_Initialized) {
               
               // The token just became initialized, i.e. all related inputs were deactivated
               // again.
              
               if((eqe->consumer->misc.action_flags & PPG_Action_Deactivate_On_Token_Unmatch) == 0) {
               
                  if(eqe->consumer->misc.action_state == PPG_Action_Enabled) {
                     eqe->consumer->action.callback.func(false /* signal deactivation */,
                        eqe->consumer->action.callback.user_data);
                  }
               }
               
               // The token is now free to be used for the next pattern processing round.
               // Reset it.
               
               // Restore state flag to initialization state
               //
               ppg_token_reset_control_state(eqe->consumer);
               
               // Reset members
               //
               PPG_CALL_VIRT_METHOD(eqe->consumer, reset);
               
               // Remove it from the active set as no further events will affect it
               //
               ppg_active_tokens_search_remove(eqe->consumer);
            }
         }
      }
      else {
         
         // No consumer (token) is listed for the deactivation event. 
         // This means that the input deactivation that
         // is represented by the token is related to an input from the
         // active set that has been registered in one of the previous pattern matching rounds
         // and whose inputs have not all been deactivated yet. 
         
         // Thus, we first have to find it in the active token set.
         //
         for(PPG_Count i = 0; i < PPG_GAT.n_tokens; ++i) {
            
            bool event_consumed = PPG_GAT.tokens[i]
                                    ->vtable->match_event(  
                                             PPG_GAT.tokens[i], 
                                             &eqe->event
                                       );
            if(!event_consumed) {
               continue;
            }
            
            // One of the tokens from the active set consumed our deactivation event
            
            // Mark it, so the user can recognize it as consumed during event 
            // buffer itearation.
            //
            eqe->event.flags |= PPG_Event_Considered;
            
            // If the token just became initialized, we
            // can reset it and remove it from the active token set.
            //
            if(PPG_GAT.tokens[i]->misc.state == PPG_Token_Initialized) { 
               
               // Restore state flag to initialization state
               //
               ppg_token_reset_control_state(PPG_GAT.tokens[i]);
               
               // Reset members
               //
               PPG_CALL_VIRT_METHOD(PPG_GAT.tokens[i], reset);
              
               ppg_active_tokens_remove(i);
            }
            
            break;
         }
      }
   }
}

void ppg_active_tokens_update(void)
{
   ppg_event_buffer_iterate2(
      (PPG_Event_Processor_Visitor)ppg_active_tokens_update_aux, 
      NULL);
}