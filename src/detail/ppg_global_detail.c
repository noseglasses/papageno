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

#include "detail/ppg_global_detail.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_pattern_matching_detail.h"
#include "ppg_debug.h"

/* Returns if an action has been triggered.
 */
bool ppg_recurse_and_process_actions(PPG_Token__ *cur_token)
{        
   if(!cur_token) { return false; }
   
//    PPG_LOG("Triggering action of most recent token\n");
   
   PPG_Token__ *action_tokens[ppg_context->tree_depth];
   PPG_Count n_actions = 0;
   
   // It may be possible that a token was just started but not
   // finished. Then we go back to the parent token, that
   // was the last registered match.
   //
   if(cur_token->misc.state != PPG_Token_Matches) {
      cur_token = cur_token->parent;
   }
   
   while(cur_token) {

      if(cur_token->action.callback.func) {
         
         // Store the tokens that carry actions in reversed order
         //
         action_tokens[n_actions] = cur_token;
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
   
   // Traverse and trigger token actions traveling
   // from the root of the token tree to its leaf
   //
   for(PPG_Id i = n_actions - 1; i >= 0; --i) {
      
      PPG_LOG("Trg act tk 0x%"
         PRIXPTR ", func: 0x%"
         PRIXPTR ", data: 0x%"
         PRIXPTR "\n",
         (uintptr_t)action_tokens[i],
         (uintptr_t)action_tokens[i]->action.callback.func,
         (uintptr_t)action_tokens[i]->action.callback.user_data
      );
         
      action_tokens[i]->action.callback.func(
         action_tokens[i]->action.callback.user_data);
   }
   
//    PPG_LOG("Done\n");
   
   return n_actions > 0;
}

void ppg_recurse_and_cleanup_active_branch(void)
{        
   if(!ppg_context->current_token) { return; }
   
   PPG_Token__ *cur_token = ppg_context->current_token;
   
   PPG_Token__ *furcation_token = NULL;
   
   do {
      
      furcation_token
         = (PPG_FB.cur_furcation == -1) ? NULL : PPG_CUR_FUR.token;
   
      // Recursively reset all tokens of the branch we are on
      // back to the first branch node or back to the first
      // node after the root node of the search tree.
      // 
      ppg_branch_cleanup(cur_token, furcation_token);
      
      if(furcation_token) {
         
         for(PPG_Count i = 0; i < furcation_token->n_children; ++i) {
            ppg_token_reset(furcation_token->children[i]);
         }
                  
         cur_token = PPG_CUR_FUR.token;

         --PPG_FB.cur_furcation;
      }
      
   } while(furcation_token);
}

void ppg_delete_stored_events(void)
{
   ppg_event_buffer_init(&ppg_context->event_buffer); 
}

void ppg_reset_pattern_matching_engine(void)
{
   ppg_context->current_token = NULL;
            
   PPG_FB.cur_furcation = -1;
   
   // The token root's state has been reset to PPG_Token_Initialized 
   // during cleanup.
   //
   ppg_context->pattern_root.misc.state
         = PPG_Token_Root;
   
   // Start with the first token in the queue
   //
   PPG_EB.cur = PPG_EB.start;
}
