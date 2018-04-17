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

#include "detail/ppg_global_detail.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_pattern_matching_detail.h"
#include "detail/ppg_signal_detail.h"
#include "ppg_debug.h"

/* Returns if an action has been triggered.
 */
bool ppg_recurse_and_process_actions(PPG_Token__ *cur_token)
{        
   if(!cur_token) { return false; }
   
//    PPG_LOG("Triggering action of most recent token\n");
   
   PPG_Count n_actions = 0;
   
   // It may be possible that a token was just started but not
   // finished. Then we go back to the parent token, that
   // was the last registered match.
   //
   if(   (cur_token->misc.state != PPG_Token_Matches)
      && (cur_token->misc.state != PPG_Token_Finalized) 
   ) {
      cur_token = cur_token->parent;
   }
   
   while(cur_token) {

      if(cur_token->action.callback.func) {
         
         // Store the tokens that carry actions in reversed order
         //
         cur_token->misc.action_state = PPG_Action_Enabled;
         ++n_actions;
         
         if(cur_token->misc.action_flags & PPG_Action_Fallback) {
            cur_token = cur_token->parent;
         }
         else {
            break;
         }
      }
      else {
         if(cur_token->misc.action_flags & PPG_Action_Fallback) {
            cur_token = cur_token->parent;
         }
         else {
            break;
         }
      }
   }
   
   return n_actions > 0;
}

void ppg_delete_stored_events(void)
{
   ppg_event_buffer_reset(&ppg_context->event_buffer); 
}

void ppg_reset_pattern_matching_engine(void)
{
   ppg_context->current_token = NULL;
            
   PPG_FB.cur_furcation = -1;
   
   // The token root's state has been reset to PPG_Token_Initialized 
   // during cleanup.
   //
   ppg_context->pattern_root->misc.state
         = PPG_Token_Initialized;
   
   // Start with the first token in the queue
   //
   PPG_EB.cur = PPG_EB.start;
   
   ppg_signal(PPG_On_Initialization);
}
