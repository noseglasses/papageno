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
//    for(PPG_Count i = 0; i < PPG_ACTIVE_TOKENS_SPACE; ++i) {
//       active_tokens->free_ids[i] = i;
//    }
   active_tokens->n_tokens = 0;
}

static void ppg_active_tokens_add(PPG_Token__ *token)
{
   PPG_GAT.tokens[PPG_GAT.n_tokens] = token;
   
//    PPG_Count next_free_id = PPG_GAT.free_ids[PPG_GAT.n_tokens];
//    
//    // Store the token
//    //
//    PPG_GAT.tokens[next_free_id] = token;
//    
//    // Store the back pointer
//    //
//    PPG_GAT.pos[next_free_id] = PPG_GAT.n_tokens;
   
   ++PPG_GAT.n_tokens;
}

static void ppg_active_tokens_remove(PPG_Count id)
{
   for(PPG_Count i = id; i < PPG_GAT.n_tokens - 1; ++i) {
      PPG_GAT.tokens[i] = PPG_GAT.tokens[i + 1];
   }
   
   --PPG_GAT.n_tokens;
   
//    // pos is the index int the free_ids array where the
//    // index that points into the tokens-array is stored
//    //
//    PPG_Count pos = PPG_GAT.pos[id];
//    
//    // Switch with the entry on top of the stack
//    //
//    PPG_GAT.free_ids[pos] = PPG_GAT.free_ids[PPG_GAT.n_tokens];
//    
//    // Return the id
//    //
//    PPG_GAT.free_ids[PPG_GAT.n_tokens] = id;
//    
//    // Update the back pointer
//    //
//    PPG_GAT.pos[PPG_GAT.free_ids[pos]] = pos;
//    
//    PPG_GAT.tokens[id] = NULL;
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
      
   #if PPG_PEDANTIC_TOKENS
         
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
   #endif
      
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
            
            // The token is now free to be used for the next pattern processing round.
            // Reset it.
            
            // Reset members
            //
   //                PPG_CALL_VIRT_METHOD(consumer, reset);
            
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
      PPG_CALL_VIRT_METHOD(consumer, print_self, 0, false /* do not recurse */);
      
      PPG_Count old_state = consumer->misc.state;
      
      event_consumed = consumer
                              ->vtable->match_event(  
                                       consumer, 
                                       event
                                 );
                              
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
      PPG_CALL_VIRT_METHOD(eqe->consumer, print_self, 0, false /* do not recurse */);
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
         #if !PPG_PEDANTIC_TOKENS
         // As the token just matched. Lets check if
         // we are allowed to trigger the respective action.
         //
         if(eqe->consumer->misc.action_state == PPG_Action_Enabled) {
            
            PPG_LOG("      Triggering activation action\n");
            
            eqe->consumer->action.callback.func(true /* signal activation */,
               eqe->consumer->action.callback.user_data);
            
            eqe->consumer->misc.action_state = PPG_Action_Activation_Triggered;
         }
         #endif
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
         
         /*bool event_consumed = */ppg_active_tokens_check_consumption(&eqe->event);
               
//          PPG_ASSERT(event_consumed);
         
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
