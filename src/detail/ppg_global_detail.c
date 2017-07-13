/* Copyright 2017 Florian Fleissner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "detail/ppg_global_detail.h"
#include "detail/ppg_context_detail.h"
#include "ppg_debug.h"

/* Returns if an action has been triggered.
 */
bool ppg_recurse_and_process_actions(void)
{			
	if(!ppg_context->current_token) { return false; }
	
// 	PPG_PRINTF("Triggering action of most recent token\n");
	
	PPG_Token__ *cur_token = ppg_context->current_token;
	
	PPG_Token__ *action_tokens[ppg_context->tree_depth];
	PPG_Count n_actions = 0;
	
	while(cur_token) {

		if(cur_token->action.callback.func) {
			
			// Store the tokens that carry actions in reversed order
			//
			action_tokens[n_actions] = cur_token;
			++n_actions;
			
			if(cur_token->action.flags &= PPG_Action_Fallback) {
				cur_token = cur_token->parent;
			}
			else {
				break;
			}
		}
		else {
			if(cur_token->action.flags &= PPG_Action_Fallback) {
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
		
		PPG_PRINTF("Triggering actions of token 0x%"
			PRIXPTR "\n", (uintptr_t)action_tokens[i]);
			
		action_tokens[i]->action.callback.func(
			action_tokens[i]->action.callback.user_data);
	}
	
	PPG_PRINTF("Done\n");
	
	return n_actions > 0;
}

void ppg_recurse_and_cleanup_active_branch(void)
{			
	if(!ppg_context->current_token) { return; }
	
// 	PPG_PRINTF("Triggering action of most recent token\n");
	
	PPG_Token__ *cur_token = ppg_context->current_token;
	
	while(cur_token) {
		
		PPG_PRINTF("Cleaning up children of token 0x%"
			PRIXPTR "\n", (uintptr_t)cur_token);
		
		// Reset all children (data structures and state)
		//
		ppg_token_reset_children(cur_token);

		cur_token = cur_token->parent;
	}
}

void ppg_delete_stored_events(void)
{
	ppg_event_buffer_init(&ppg_context->event_buffer);	
}