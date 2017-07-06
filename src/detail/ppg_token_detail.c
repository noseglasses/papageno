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

#include "detail/ppg_token_detail.h"
#include "ppg_global.h"
#include "ppg_debug.h"
#include "ppg_action.h"
#include "ppg_action.h"
#include "ppg_action_flags.h"
#include "ppg_slots.h"
#include "detail/ppg_context_detail.h"

#include <stdlib.h>

void ppg_token_store_action(PPG_Token__ *token, 
											  PPG_Action action)
{
	token->action = action; 
	
	PPG_PRINTF("Action of token 0x%" PRIXPTR ": 0x%" PRIXPTR "\n",
				  (uintptr_t)token, (uintptr_t)token->action.user_callback.user_data);
}

void ppg_token_reset(	PPG_Token__ *token)
{
	token->state = PPG_Token_In_Progress;
}

/* Tokens have states. This method resets the states
 * after processing a pattern. On our way through the
 * pattern tree we only need to reset thoses tokens
 * that were candidates.
 */
void ppg_token_reset_children(PPG_Token__ *token)
{
	/* Reset all child tokens if there are any
	 */
	for(PPG_Count i = 0; i < token->n_children; ++i) {
		PPG_CALL_VIRT_METHOD(token->children[i], reset);
	}
}

bool ppg_token_trigger_action(PPG_Token__ *token, PPG_Slot_Id slot_id) {
	
	/* Actions of matching tokens have already been triggered during 
	 * pattern processing.
	 */
	if(	(slot_id != PPG_On_Token_Matches)
		&&	(token->action.flags & PPG_Action_Immediate)) {
		return false;
	}
			
	if(token->action.user_callback.func) {

// 		PPG_PRINTF("Action of token 0x%" PRIXPTR ": %u\n",
// 				  (uintptr_t)token, (uintptr_t)token->action.user_callback.user_data);
		
		PPG_PRINTF("*\n");
	
		token->action.user_callback.func(
			slot_id,
			token->action.user_callback.user_data);
		
		return true;
	}
	
	return false;
}

PPG_Processing_State ppg_token_match_event(	
												PPG_Token__ **current_token,
												PPG_Event *event) 
{
	/* Loop over all tokens and inform them about the 
	 * event 
	 */
	bool all_children_invalid = true;
	
	PPG_Token__ *a_current_token = *current_token;
	
	ppg_event_buffer_store_event(event);
	
// 	PPG_PRINTF("Processing input\n");
	
// 	#if DEBUG_PAPAGENO
// 	if(event->pressed) {
// 		PPG_PRINTF("v");
// 	}
// 	else {
// 		PPG_PRINTF("^");
// 	}
// 	#endif
	
	bool any_token_matches = false;
		
	/* Pass the inputpress to the tokens and let them decide it they 
	 * can use it. If a token cannot it becomes invalid and is not
	 * processed further on this node level. This speeds up processing.
	 */
	for(PPG_Count i = 0; i < a_current_token->n_children; ++i) {
		
		// PPG_CALL_VIRT_METHOD(a_current_token->children[i], print_self);
		
		/* Accept only paths through the search tree whose
		 * nodes' ppg_context->layer tags are lower or equal the current ppg_context->layer
		 */
		if(a_current_token->children[i]->layer > ppg_context->layer) { continue; }
		
		if(a_current_token->children[i]->state == PPG_Token_Invalid) {
			continue;
		}
		
		PPG_Processing_State child_process_result 
			= a_current_token->children[i]
					->vtable->match_event(	
								a_current_token->children[i], 
								event
						);
								
		switch(child_process_result) {
			
			case PPG_Token_In_Progress:
				all_children_invalid = false;
				
				break;
				
			case PPG_Token_Matches:
				
				all_children_invalid = false;
				any_token_matches = true;

				break;
			case PPG_Token_Invalid:
// 				PPG_PRINTF("Token invalid");
				break;
		}
	}
	
	/* If all children are invalid, the inputstroke chain does not
	 * match any defined pattern.
	 */
	if(all_children_invalid) {
					
		return PPG_Token_Invalid;
	}
	
	/* If any token matches we have to find the most suitable one
	 * and either terminate pattern processing if the matching child
	 * is a leaf node, or replace the current token to await further 
	 * inputstrokes.
	 */
	else if(any_token_matches) {
		
		PPG_Layer highest_layer = -1;
		PPG_Id match_id = -1;
		
		/* Find the most suitable token with respect to the current ppg_context->layer.
		 */
		for(PPG_Count i = 0; i < a_current_token->n_children; ++i) {
		
// 			PPG_CALL_VIRT_METHOD(a_current_token->children[i], print_self);
		
			/* Accept only paths through the search tree whose
			* nodes' ppg_context->layer tags are lower or equal the current ppg_context->layer
			*/
			if(a_current_token->children[i]->layer > ppg_context->layer) { continue; }
			
			if(a_current_token->children[i]->state != PPG_Token_Matches) {
				continue;
			}
			
			if(a_current_token->children[i]->layer > highest_layer) {
				highest_layer = a_current_token->children[i]->layer;
				match_id = i;
			}
		}
		
		PPG_ASSERT(match_id >= 0);
				
		/* Cleanup children of the current node for further pattern processing.
		 */
		ppg_token_reset_children(a_current_token);
		
		/* Replace the current token. From this point on
		 * a_current_token references the child token
		*/
		*current_token = a_current_token->children[match_id];
		a_current_token = *current_token;
		
		if(a_current_token->action.flags & PPG_Action_Immediate) {
			ppg_token_trigger_action(a_current_token, PPG_On_Token_Matches);
		}
			
		if(0 == a_current_token->n_children) {
			
			return PPG_Pattern_Matches;
		}
		else {
			
			/* The pattern is still in progress. We continue with the 
			 * new current node as soon as the next inputstroke happens.
			 */
			return PPG_Token_In_Progress;
		}
	}
	
	return PPG_Token_In_Progress;
}


static void ppg_token_allocate_children(PPG_Token__ *a_This, PPG_Count n_children) {

	 a_This->children 
		= (struct PPG_TokenStruct **)malloc(n_children*sizeof(struct PPG_TokenStruct*));
	 a_This->n_allocated_children = n_children;
}

static void ppg_token_grow_children(PPG_Token__ *a_This) {

	if(a_This->n_allocated_children == 0) {
		
		ppg_token_allocate_children(a_This, 1);
	}
	else {
		PPG_Token__ **oldSucessors = a_This->children;
		
		ppg_token_allocate_children(a_This, 2*a_This->n_allocated_children);
			
		for(PPG_Count i = 0; i < a_This->n_children; ++i) {
			a_This->children[i] = oldSucessors[i];
		}
		
		free(oldSucessors); 
	}
}

void ppg_token_add_child(PPG_Token__ *token, PPG_Token__ *child) {
	
	if(token->n_allocated_children == token->n_children) {
		ppg_token_grow_children(token);
	}
	
	token->children[token->n_children] = child;
	
	child->parent = token;
	
	++token->n_children;
}

void ppg_token_free_children(PPG_Token__ *token)
{
	if(!token->children) { return; }
	
	for(PPG_Count i = 0; i < token->n_allocated_children; ++i) {
		
		ppg_token_free(token->children[i]);
	}
	
	free(token->children);
	
	token->children = NULL;
	token->n_allocated_children = 0;
}

PPG_Token__* ppg_token_destroy(PPG_Token__ *token) {

	ppg_token_free_children(token);
	
	return token;
}

static bool ppg_token_equals(PPG_Token__ *p1, PPG_Token__ *p2) 
{
	if(p1->vtable != p2->vtable) { return false; }
	
	return p1->vtable->equals(p1, p2);
}

void ppg_token_free(PPG_Token__ *token) {
	
	PPG_CALL_VIRT_METHOD(token, destroy);

	free(token);
}

PPG_Token__* ppg_token_get_equivalent_child(
														PPG_Token__ *parent_token,
														PPG_Token__ *sample) {
	
	if(parent_token->n_children == 0) { return NULL; }
	
	for(PPG_Count i = 0; i < parent_token->n_children; ++i) {
		if(ppg_token_equals(
											parent_token->children[i], 
											sample)
		  ) {
			return parent_token->children[i];
		}
	}
	
	return NULL;
}

#if PAPAGENO_PRINT_SELF_ENABLED

void ppg_token_print_self_start(PPG_Token__ *p, PPG_Count indent)
{
	PPG_I PPG_PRINTF("   parent: 0x%" PRIXPTR "\n", (uintptr_t)p->parent);
	PPG_I PPG_PRINTF("   action.flags: %d\n", p->action.flags);
	PPG_I PPG_PRINTF("   action_user_func: 0x%" PRIXPTR "\n", (uintptr_t)p->action.user_callback.func);
	PPG_I PPG_PRINTF("   action_user_data: 0x%" PRIXPTR "\n", (uintptr_t)p->action.user_callback.user_data);
	PPG_I PPG_PRINTF("   state: %d\n", p->state);
	PPG_I PPG_PRINTF("   layer: %d\n", p->layer);
}

void ppg_token_print_self_end(PPG_Token__ *p, PPG_Count indent, bool recurse)
{
// 	PPG_I PPG_PRINTF("   children: 0x%" PRIXPTR "\n", (uintptr_t)&p->children);
	PPG_I PPG_PRINTF("   n_allocated_children: %d\n", p->n_allocated_children);
	PPG_I PPG_PRINTF("   n_children: %d\n", p->n_children);
	
	if(recurse) {
		for(PPG_Count i = 0; i < p->n_children; ++i) {
			PPG_I PPG_PRINTF("   child: %d\n", i);
			PPG_CALL_VIRT_METHOD(p->children[i], print_self, indent + 1, recurse);
		}
	}
}

static void ppg_token_print_self(PPG_Token__ *p, PPG_Count indent, bool recurse)
{
	PPG_I PPG_PRINTF("<*** token (0x%" PRIXPTR ") ***>\n", (uintptr_t)p);
	ppg_token_print_self_start(p, indent);
	ppg_token_print_self_end(p, indent, recurse);
}
#endif

static PPG_Token_Vtable ppg_token_vtable =
{
	.match_event 
		= NULL,
	.reset 
		= (PPG_Token_Reset_Fun) ppg_token_reset,
	.trigger_action 
		= (PPG_Token_Trigger_Action_Fun) ppg_token_trigger_action,
	.destroy 
		= (PPG_Token_Destroy_Fun) ppg_token_destroy,
	.equals
		= NULL
	
	#if PAPAGENO_PRINT_SELF_ENABLED
	,
	.print_self
		= (PPG_Token_Print_Self_Fun) ppg_token_print_self,
	#endif
};

PPG_Token__ *ppg_token_new(PPG_Token__ *token) {
	
    token->vtable = &ppg_token_vtable;
	 
    token->parent = NULL;
    token->children = NULL;
	 token->n_allocated_children = 0;
    token->n_children = 0;
    token->action.flags = PPG_Action_Default;
    token->action.user_callback.func = NULL;
    token->action.user_callback.user_data = NULL;
    token->state = PPG_Token_In_Progress;
    token->layer = 0;
	 
    return token;
}

PPG_Token ppg_token_set_action(	
							PPG_Token token__,
							PPG_Action action)
{
	PPG_Token__ *token = (PPG_Token__ *)token__;
	
	ppg_token_store_action(token, action);
	
	return token__;
}

PPG_Action ppg_token_get_action(PPG_Token token__)
{
	PPG_Token__ *token = (PPG_Token__ *)token__;
	
	return token->action;
}

PPG_Token ppg_token_set_action_flags(
									PPG_Token token__,
									PPG_Action_Flags_Type action_flags)
{
	PPG_Token__ *token = (PPG_Token__ *)token__;
	
	token->action.flags = action_flags;
	
	return token;
}

PPG_Action_Flags_Type ppg_token_get_action_flags(PPG_Token token__)
{
	PPG_Token__ *token = (PPG_Token__ *)token__;
	
	return token->action.flags;
}