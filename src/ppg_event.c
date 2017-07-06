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

#include "ppg_event.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_global_detail.h"
#include "detail/ppg_furcation_detail.h"
#include "detail/ppg_event_buffer_detail.h"
#include "ppg_debug.h"

#if 0
bool ppg_event_process(PPG_Event *event)
{ 
	if(!ppg_context->papageno_enabled) {
		return true;
	}
	
	/* When a pattern could not be finished, all inputstrokes are
	 * processed through the process_record_quantum method.
	 * To prevent infinite recursion, we have to temporarily disable 
	 * processing patterns.
	 */
	if(ppg_context->papageno_temporarily_disabled) { return true; }
	
	/* Early exit if no pattern was registered 
	 */
	if(!ppg_context->pattern_root_initialized) { return true; }
	
	/* Check if the pattern is being aborted
	 */
	if(ppg_context->input_id_equal(ppg_context->abort_input.input_id, event->input_id)) {
		
		/* If a pattern is in progress, we abort it and consume the abort input.
		 */
		if(ppg_context->current_token) {
			PPG_PRINTF("Processing melodies interrupted by user\n");
			ppg_global_abort_pattern_matching();
			return false;
		}
	
		return false;
	}

// 	PPG_PRINTF("Starting inputprocessing\n");
	
	if(!ppg_context->current_token) {
		
// 		PPG_PRINTF("New pattern \n");
		
		/* Start of pattern processing
		 */
		ppg_event_buffer_init(&ppg_context->event_buffer);
		
		ppg_context->current_token = &ppg_context->pattern_root;
		
		ppg_context->time(&ppg_context->time_last_event);
	}
	else {
		
		if(ppg_timeout_check()) {
			
			/* Timeout hit. Cleanup already done.
			 */
			return false;
		}
		else {
			ppg_context->time(&ppg_context->time_last_event);
		}
	}
	
	PPG_Processing_State result 
		= ppg_token_match_event(	
										&ppg_context->current_token,
										event,
										ppg_context->layer
								);
		
	switch(result) {
		
		case PPG_Token_In_Progress:
			
			return false;
			
		case PPG_Pattern_Matches:
			
			ppg_recurse_and_process_actions(PPG_On_Pattern_Matches);
			
			ppg_context->current_token = NULL;
			
			ppg_delete_stored_events();
											  
			return false;
			
		case PPG_Token_Invalid:
			
			PPG_PRINTF("-\n");
		
			ppg_global_abort_pattern_matching();
			
			return false; /* The input(s) have been already processed */
			//return true; // Why does this require true to work and 
			// why is t not written?
	}
	
	return true;
}
#endif
	
/** @brief Check if a branch matches the current event chain
 * 
 * @returns PPG_Token_State
 */
static PPG_Count ppg_branch_check_match(
										PPG_Token__ *start_token,
										PPG_Event *event)
{
	/* Accept only paths through the search tree whose
		* nodes' ppg_context->layer tags are lower or equal the current ppg_context->layer
		*/
	if(start_token->layer > ppg_context->layer) { return PPG_Branch_Invalid; }
	
	if(start_token->state == PPG_Token_Invalid) {
		return PPG_Branch_Invalid;
	}
	
	return start_token
				->vtable->match_event(	
							start_token, 
							event
					);
}

/** @brief Cleanup a branch going backward the token chain
 *         
 * Every token on the way is cleaned up for reuse in 
 * future pattern matching.
 * The final token, end_token is excluded.
 * 
 * @returns The last token before end_token in backward direction
 */
static PPG_Token__ *ppg_branch_cleanup(
								PPG_Token__ *start_token,
								PPG_Token__ *end_token)
{
	PPG_Token__ *cur_token = start_token;
	PPG_Token__ *return_token;
	
	// Unwind and cleanup back to the current furcation or to the
	// root node
	// 
	while(cur_token != end_token) {
		
		PPG_CALL_VIRT_METHOD(cur_token, reset);
		
		ppg_token_reset(cur_token);
		
		return_token = cur_token;
		
		cur_token = cur_token->parent;
	}
	
	return return_token;
}

/** @brief Reverts to the next possible furcation
 *  
 * Cleans up on its way. Every token is initialized if it is
 * left. If a possible furcation is detected, 
 * any node that was already traversed is marked as invalid,
 * possible branches are represented by branches that are not
 * invalid.
 * 
 * @returns True if a possible furcation was found
 */
static bool ppg_branch_revert_to_next_possible_furcation(PPG_Token__ *start_token)
{
	#define PPG_CUR_FUR PPG_FB.furcations[PPG_FB.cur_furcation]
	
	// If there is no current furcation, we at least clean up to 
	// the root node. Otherwise we cleanup to the current furcation.
	//
	PPG_Token__ *termination_token 
		= (PPG_FB.cur_furcation == -1) ? NULL : PPG_CUR_FUR.branch;
	
	ppg_branch_cleanup(start_token, termination_token);
	
	// If there is no current furcation, we can't do anything else
	//
	if(PPG_FB.cur_furcation == -1) { return false; }
	
	// Mark the branch we decended from as invalid
	//
	PPG_CUR_FUR.branch->state = PPG_Token_Invalid;
	
	if(PPG_CUR_FUR.n_possible_branches == 0) {
		
		// There are no possibilities left, so we cleanup the children
		// of the furcation node
		//
		ppg_token_reset_children(PPG_CUR_FUR.token);
		
		// Mark the furcation node as invalid
		//
		PPG_CUR_FUR.token->state = PPG_Token_Invalid;
		
		// Store the token that is associated with the current
		// furcation to preserve it when decrementing the furcation below
		//
		PPG_Token__ *fur_token = PPG_CUR_FUR.token;
		
		// Replace the current furcation with the previous one 
		//
		--PPG_FB.cur_furcation;
		
		// Recursively try to go up to the next furcation and cleanup
		// on the way. 
		//
		// Note: This is necessary for proper cleanup even though 
		//       PPG_FB.cur_furcation is -1 at this point
		//
		return ppg_branch_revert_to_next_possible_furcation(fur_token);
	}
	
	// After reverting it may be possible that no suitable furcation
	// could be found
	//
	if(PPG_FB.cur_furcation == -1) {
		return false;
	}
	
	return true;
}

static PPG_Id ppg_token_get_most_appropriate_branch(PPG_Token__ *parent_token)
{
	PPG_Layer highest_layer = -1;
	PPG_Id match_id = -1;
	PPG_Count precedence = 0;
	
	/* Find the most suitable token with respect to the current ppg_context->layer.
		*/
	for(PPG_Count i = 0; i < parent_token->n_children; ++i) {
	
// 			PPG_CALL_VIRT_METHOD(parent_token->children[i], print_self);
	
		/* Accept only paths through the search tree whose
		* nodes' ppg_context->layer tags are lower or equal the current ppg_context->layer
		*/
		if(parent_token->children[i]->layer > ppg_context->layer) { continue; }
		
		if(parent_token->children[i]->state != PPG_Token_Matches) {
			continue;
		}
		
		PPG_Count cur_precedence 
				= parent_token->children[i]
						->vtable->token_precedence();
				
		if(cur_precedence > precedence) {
			precedence = cur_precedence;
			match_id = i;
		}
		else {
			
			if(parent_token->children[i]->layer > highest_layer) {
				highest_layer = parent_token->children[i]->layer;
				match_id = i;
			}
		}
	}
	
	PPG_ASSERT(match_id >= 0);
	
	return match_id;
}

/** @brief Checks if an arriving event can possibly be ignored
 */
static bool ppg_check_ignore_event(PPG_Event *event, bool *swallow_event)
{
	*swallow_event = false;
	
	if(!ppg_context->papageno_enabled) {
		return true;
	}
	
	/* When a pattern could not be finished, all inputstrokes are
	 * processed through the process_record_quantum method.
	 * To prevent infinite recursion, we have to temporarily disable 
	 * processing patterns.
	 */
	if(ppg_context->papageno_temporarily_disabled) { return true; }
	
	/* Early exit if no pattern was registered 
	 */
	if(!ppg_context->pattern_root_initialized) { return true; }
	
	/* Check if the pattern is being aborted
	 */
	if(ppg_context->input_id_equal(ppg_context->abort_input.input_id, event->input_id)) {
		
		/* If a pattern is in progress, we abort it and consume the abort input.
		 */
		if(ppg_context->current_token) {
			PPG_PRINTF("Processing melodies interrupted by user\n");
			
			*swallow_event = true;
			ppg_global_abort_pattern_matching();
			return true;
		}
	
		return true;
	}
	
	return false;
}
	
static PPG_Count ppg_process_next_event(void)
{	
	//PPG_PRINTF("ppg_process_next_event\n");
	
	if(!ppg_context->current_token) {
		ppg_context->current_token = &ppg_context->pattern_root;
	}

	PPG_Count n_tokens_in_progress = 0;
		
	bool any_token_matches = false;
	
	PPG_PRINTF("Checking %d children of token 0x%" PRIXPTR "\n", 
				 ppg_context->current_token->n_children, (uintptr_t)ppg_context->current_token);
	
	PPG_PRINTF("start: %u, cur: %u, end: %u, size: %u\n", 
				  PPG_EB.start, PPG_EB.cur, PPG_EB.end, PPG_EB.size);
	
	// Check the subtokens of the current branch
	//  to find a match based on the current event
	//
	for(PPG_Count i = 0; i < ppg_context->current_token->n_children; ++i) {
		
		PPG_Event *event = &PPG_EB.events[PPG_EB.cur];
		
		PPG_Count event_check_result = ppg_branch_check_match(
										ppg_context->current_token->children[i],
										event);
		
		switch(event_check_result) {
			
			case PPG_Token_In_Progress:
				++n_tokens_in_progress;
				break;
				
			case PPG_Token_Matches:
				any_token_matches = true;
				++n_tokens_in_progress;

				break;
		}
	}
	
	PPG_Id branch_id = -1;
	
	PPG_PRINTF("any_token_matches: %d\n", any_token_matches);
	PPG_PRINTF("n_tokens_in_progress: %d\n", n_tokens_in_progress);
	
	if(any_token_matches) {
		
		// Find the most appropriate branch to continue with
		//
		branch_id = ppg_token_get_most_appropriate_branch(
												ppg_context->current_token);
	}
	
		// 2) If there are several nodes that are not invalid,
		//    increase the current event id and 
		//    signal furcation by returning an appiopriate value
		//    (this pushes a furcation node on the furcation stack
	if(n_tokens_in_progress > 1) {
		
		ppg_furcation_push_or_update(
						n_tokens_in_progress,
						ppg_context->current_token->children[branch_id]);
	}

	if(any_token_matches) {
		
		PPG_PRINTF("Advancing with child token\n");
		
		ppg_context->current_token 
				= ppg_context->current_token->children[branch_id];
		
		// The current node has no children, this means we found a matching
		// pattern
		//
		if(0 == ppg_context->current_token->n_children) {
			
			return PPG_Pattern_Matches;
		}
		else {

			return PPG_Token_Matches;
		}
	}
	else if(n_tokens_in_progress == 0) {
		return PPG_Token_Invalid;
	}
			
	return PPG_Token_In_Progress;
}

static void ppg_event_process_all_possible(void)
{
	//PPG_PRINTF("ppg_event_process_all_possible\n");
	
	while(ppg_event_buffer_events_left()) {
		
		PPG_Count process_event_result = ppg_process_next_event();
		
		switch(process_event_result) {
			
			case PPG_Pattern_Matches:
				
				ppg_recurse_and_process_actions(PPG_On_Pattern_Matches);
				
				ppg_recurse_and_cleanup_active_branch();
				
				// Even though the pattern matches, it is possible that not
				// all events were consumed as there might have been a
				// a tree furcation traverse involved. This might leave events
				// beyond the current event that might be part of
				// a following match.
				// Thus, we remove all events that were consumed and leave the
				// rest.
				//
				ppg_event_buffer_truncate_at_front();
				
				ppg_context->current_token = NULL;
				
				break;
				
			case PPG_Token_Matches:
				
				// TODO: Trigger event on token match
				
				// No break statement to fall through to PPG_Token_In_Progress
				
			case PPG_Token_In_Progress:
				
				// The current_token has already been advanced
				
				ppg_event_buffer_advance();
				
				break;
				
			case PPG_Token_Invalid:
			{
				// Token invalid means that we found a dead branch
				//
				bool furcation_found 
					= ppg_branch_revert_to_next_possible_furcation(
									ppg_context->current_token);
					
				if(!furcation_found) {
					
					// If no furcation was found, there is no chance
					// for a match. Thus we remove the first stored event
					// and rerun the overall pattern matching based on a
					// new first event.
					//
					ppg_even_buffer_flush_and_remove_first_event(PPG_On_Match_Failed);
					
					ppg_context->current_token = NULL;
				}
				else {
					
					// Reset the current token for further processing
					//
					ppg_context->current_token = PPG_CUR_FUR.token;
					
					// Reset the current event that is used for further processing
					//
					PPG_EB.cur = PPG_CUR_FUR.event_id;
				}
			}
			break;
		}
	}
}

bool ppg_event_process(PPG_Event *event)
{
	// The return value signals the calling instance to 
	// process the event (true) or to ignore it (false)
	//
	bool swallow_event;
	if(ppg_check_ignore_event(event, &swallow_event)) {
		return !swallow_event;
	}
	
	ppg_event_buffer_store_event(event);
	
	if(ppg_timeout_check()) {
			
		// Timeout hit. Cleanup already done.
		//
		return false;
	}
	
	ppg_event_process_all_possible();
	
	return false;
}
