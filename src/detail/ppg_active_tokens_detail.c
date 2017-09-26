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
#include "detail/ppg_token_detail.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_malloc_detail.h"

#include "ppg_debug.h"

#include <assert.h>
#include <string.h>

void ppg_active_tokens_resize(PPG_Active_Tokens *active_tokens,
                              PPG_Count new_size)
{
   PPG_ASSERT(active_tokens);
   
   if(new_size <= active_tokens->max_tokens) { return; }
   
   PPG_Token__ **new_tokens
      = (PPG_Token__**)PPG_MALLOC(sizeof(PPG_Token__*)*new_size);
      
   active_tokens->max_tokens = new_size;
   
   if(active_tokens->tokens && (active_tokens->n_tokens > 0)) {
      memcpy(new_tokens, active_tokens->tokens, 
             sizeof(PPG_Token__ *)*active_tokens->n_tokens);
   }
   
   active_tokens->tokens = new_tokens;
}

void ppg_active_tokens_init(PPG_Active_Tokens *active_tokens)
{
   active_tokens->tokens = NULL;
   active_tokens->n_tokens = 0;
   active_tokens->max_tokens = 0;
   
   ppg_active_tokens_resize(active_tokens, PPG_MAX_ACTIVE_TOKENS);
   
   for(size_t i = 0; i < PPG_MAX_ACTIVE_TOKENS; ++i) {
      active_tokens->tokens[i] = NULL;
   }
}

void ppg_active_tokens_restore(PPG_Active_Tokens *active_tokens)
{
   PPG_Count saved_size = active_tokens->max_tokens;
   
   active_tokens->tokens = NULL;
   active_tokens->max_tokens = 0; // This forces resize 
   
   ppg_active_tokens_resize(active_tokens, saved_size);
}

void ppg_active_tokens_free(PPG_Active_Tokens *active_tokens)
{
   if(!active_tokens->tokens) { return; }
   
   free(active_tokens->tokens);
   
   active_tokens->tokens = NULL;
}

static void ppg_active_tokens_add(PPG_Token__ *token)
{
   PPG_GAT.tokens[PPG_GAT.n_tokens] = token;
   
   PPG_ASSERT(PPG_GAT.n_tokens < PPG_MAX_ACTIVE_TOKENS);

   ++PPG_GAT.n_tokens;
}

static void ppg_active_tokens_remove(PPG_Count id)
{
   for(PPG_Count i = id; i < PPG_GAT.n_tokens - 1; ++i) {
      PPG_GAT.tokens[i] = PPG_GAT.tokens[i + 1];
   }
   
   --PPG_GAT.n_tokens;
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

static void ppg_active_tokens_on_deactivation(PPG_Token__ *consumer,
                                              PPG_Count state,
                                              bool changed
                                             )
{
   // The event deactivates an input

   if(consumer) {
      
      if(consumer->misc.flags & PPG_Token_Flags_Pedantic) {
         
         if(   (state 
                        == PPG_Token_Matches) 
            && (changed)) {
               
            PPG_LOG("   Causes activation and deactivation\n");
         
            if(consumer->misc.action_state == PPG_Action_Enabled) {
                     
               PPG_LOG("      Triggering activation action\n");
                     
               consumer->action.callback.func(true /* signal deactivation */,
                        consumer->action.callback.user_data);
               
               consumer->misc.action_state = PPG_Action_Activation_Triggered;
                     
               consumer->action.callback.func(false /* signal deactivation */,
                        consumer->action.callback.user_data);
                     
               consumer->misc.action_state = PPG_Action_Deactivation_Triggered;
            }
            
            ppg_active_tokens_search_remove(consumer);
         }
         
         return;
      }
      
      // It there is a consumer listed, this means that the event was
      // consumed by one of the tokens of the matching branch
      
      if(changed) {
         
         // Token state just changed...
         
         if(state 
                     == PPG_Token_Deactivation_In_Progress) {
            
            PPG_LOG("   Causes deactivation\n");
         
            // It turned from state "matching" to state
            // "deactivation in progress"
            
            if(consumer->misc.action_flags & PPG_Action_Deactivate_On_Token_Unmatch) {

               if(consumer->misc.action_state == PPG_Action_Enabled) {
                  
                  PPG_LOG("      Triggering deactivation action\n");
                  consumer->action.callback.func(false /* signal deactivation */,
                     consumer->action.callback.user_data);
                  
                  consumer->misc.action_state = PPG_Action_Deactivation_Triggered;
               }
            }
         }
         else if(state 
                           == PPG_Token_Finalized) {
            
            PPG_LOG("   Causes finalization of 0x%" PRIXPTR "\n", (uintptr_t)consumer);
         
            // The token just became initialized, i.e. all related inputs were deactivated
            // again.
            
            if(   ((consumer->misc.action_flags & PPG_Action_Deactivate_On_Token_Unmatch) == 0)
               || (consumer->misc.action_state != PPG_Action_Deactivation_Triggered)) {
               
               if(consumer->misc.action_state == PPG_Action_Enabled) {
                  PPG_LOG("      Triggering activation action\n");
                  consumer->action.callback.func(true /* signal deactivation */,
                     consumer->action.callback.user_data);
                  
                  consumer->misc.action_state = PPG_Action_Activation_Triggered;
               }
               
               if(consumer->misc.action_state == PPG_Action_Activation_Triggered) {
                  PPG_LOG("      Triggering deactivation action\n");
                  consumer->action.callback.func(false /* signal deactivation */,
                     consumer->action.callback.user_data);
                  
                  consumer->misc.action_state = PPG_Action_Deactivation_Triggered;
               }
            }
            
            // Remove it from the active set as no further events will affect it
            //
            ppg_active_tokens_search_remove(consumer);
         }
      }
   }
}

bool ppg_active_tokens_check_consumption(
                                    PPG_Event *event)
{
   if(PPG_GAT.n_tokens == 0) {
      return false;
   }
   
   // No consumer (token) is listed for the deactivation event. 
   // This means that the input deactivation that
   // is represented by the token is related to an input from the
   // active set that has been registered in one of the previous pattern matching rounds
   // and whose inputs have not all been deactivated yet. 
   
   PPG_Token__ *consumer = NULL;
   bool event_consumed = false;
   PPG_Count i;
   bool state_changed = false;
   PPG_Count new_state;
   
   // Thus, we first have to find it in the active token set.
   //
   for(i = 0; i < PPG_GAT.n_tokens; ++i) {
      
      consumer = PPG_GAT.tokens[i];
      
      PPG_LOG("   consumer: 0x%" PRIXPTR "\n", (uintptr_t)consumer);
      PPG_LOG("   consumer action state: %d\n", consumer->misc.action_state);
      
      PPG_Count old_state = consumer->misc.state;
      
      event_consumed = consumer
                              ->vtable->match_event(  
                                       consumer, 
                                       event,
                                       true /*modify only if consuming*/
                                 );
                         
      PPG_PRINT_TOKEN(consumer)
      
      if(!event_consumed) {
         continue;
      }
      
      new_state = consumer->misc.state;
      
      if(old_state != new_state) {
         state_changed = true;
      }
      
      break;
   }
   
   if(!event_consumed) {
      return false;
   }
   
   PPG_LOG("   consumed\n");
   PPG_LOG("   new_state: %d\n", new_state);
   PPG_LOG("   state_changed: %d\n", state_changed);

   // The token just became initialized, i.e. all related inputs were deactivated
   // again.
   
   if((event->flags & PPG_Event_Active) == 0) {
      
      ppg_active_tokens_on_deactivation(consumer,
                                        new_state,
                                        state_changed);

   }
      
   // One of the tokens from the active set consumed our deactivation event
   
   // Mark it, so the user can recognize it as consumed during event 
   // buffer itearation.
   //
   event->flags |= PPG_Event_Considered;
   
   return true;
}

static void ppg_active_tokens_update_aux(
                                    PPG_Event_Queue_Entry *eqe,
                                    void *user_data)
{
   PPG_LOG("Event: Input 0x%d, active: %d\n",
           eqe->event.input,
           eqe->event.flags & PPG_Event_Active);
   
   // Check if the event has already been considered 
   // This is the case for deactivation events that
   // have been consumed by unfinished tokens of a previous pattern match.
   //
   if(eqe->event.flags & PPG_Event_Considered) {
      return;
   }
      
   if(eqe->consumer) {
      PPG_LOG("   consumer: 0x%" PRIXPTR "\n", (uintptr_t)eqe->consumer);
      PPG_LOG("   consumer action state: %d\n", eqe->consumer->misc.action_state);
      PPG_PRINT_TOKEN(eqe->consumer)
   }
  
   if(eqe->event.flags & PPG_Event_Active) {
      
      // The event activates an input
      
      if(   (eqe->token_state.state == PPG_Token_Matches)
         && eqe->token_state.changed
      ) {
         PPG_LOG("   Causes token match\n");
         
         // The last event led to a match
            
         // Add the token to the active set
         //
         PPG_ASSERT(eqe->consumer);
         ppg_active_tokens_add(eqe->consumer);
          
         // In pedantic tokens mode, tokens can only trigger actions 
         // when all their related inputs became inactive
         //
         if((eqe->consumer->misc.flags & PPG_Token_Flags_Pedantic) == 0) {
            
            // As the token just matched. Lets check if
            // we are allowed to trigger the respective action.
            //
            if(eqe->consumer->misc.action_state == PPG_Action_Enabled) {
               
               PPG_LOG("      Triggering activation action\n");
               
               eqe->consumer->action.callback.func(true /* signal activation */,
                  eqe->consumer->action.callback.user_data);
               
               eqe->consumer->misc.action_state = PPG_Action_Activation_Triggered;
            }
         }
      }
      
      eqe->event.flags |= PPG_Event_Considered;
      
      if(eqe->consumer->misc.flags & PPG_Token_Flags_Done) {
         ppg_active_tokens_search_remove(eqe->consumer);
      }
   }
   else {
        
      // The event deactivates an input
      
      if(eqe->consumer) {
         ppg_active_tokens_on_deactivation(eqe->consumer,
                                        eqe->token_state.state,
                                        eqe->token_state.changed);

         eqe->event.flags |= PPG_Event_Considered;
      }
      else {
         
         PPG_LOG("   Not part of current match branch\n");
         
         ppg_active_tokens_check_consumption(&eqe->event);
      }
   }
}

void ppg_active_tokens_update(void)
{
   PPG_LOG("************************\n")
   PPG_LOG("Activating active tokens\n")
   ppg_event_buffer_iterate2(
      (PPG_Event_Processor_Visitor)ppg_active_tokens_update_aux, 
      NULL);
}
