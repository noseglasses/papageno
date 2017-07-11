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
bool ppg_recurse_and_process_actions(PPG_Slot_Id slot_id)
{			
	if(!ppg_context->current_token) { return false; }
	
// 	PPG_PRINTF("Triggering action of most recent token\n");
	
	PPG_Token__ *cur_token = ppg_context->current_token;
	
	while(cur_token) {
		
		PPG_PRINTF("Triggering actions of token 0x%"
			PRIXPTR "\n", (uintptr_t)cur_token);
		
		bool action_present = ppg_token_trigger_action(cur_token, slot_id);

		if(action_present) {
			if(cur_token->action.flags &= PPG_Action_Fall_Through) {
				cur_token = cur_token->parent;
			}
			else {
				return true;
			}
		}
		else {
			if(cur_token->action.flags &= PPG_Action_Fall_Back) {
				cur_token = cur_token->parent;
			}
			else {
				return false;
			}
		}
	}
	
	PPG_PRINTF("Done\n");
	
	return false;
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