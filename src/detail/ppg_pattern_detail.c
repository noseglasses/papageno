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

#include "detail/ppg_pattern_detail.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_context_detail.h"
#include "ppg_debug.h"
#include "ppg_action.h"

PPG_Token ppg_pattern_from_list(	
												PPG_Layer layer,
												PPG_Count n_tokens,
												PPG_Token tokens[])
{ 	
	PPG_Token__ *parent_token = &ppg_context->pattern_root;
	
	PPG_PRINTF("   %d members\n", n_tokens);
	
	for (PPG_Count i = 0; i < n_tokens; i++) { 
		
		PPG_Token__ *cur_token = (PPG_Token__*)tokens[i];
		
		PPG_Token__ *equivalent_child 
			= ppg_token_get_equivalent_child(parent_token, cur_token);
			
		PPG_PRINTF("   member %d: ", i);
		
		if(	equivalent_child
			
			/* Only share interior nodes and ...
			 */
			&& equivalent_child->children
			
			/* ... only if the 
			 * newly registered node on the respective level is 
			 * not a leaf node.
			 */ 
			&& (i < (n_tokens - 1))
			
			// And only if the actions associated with both nodes are equal
			//
			&& (cur_token->action.callback.func 
						== equivalent_child->action.callback.func)
			
			&& (cur_token->action.callback.user_data 
						== equivalent_child->action.callback.user_data)
		) {
			
			PPG_PRINTF("already present: 0x%" PRIXPTR "\n", (uintptr_t)equivalent_child);
			
			parent_token = equivalent_child;
			
			if(layer < equivalent_child->layer) {
				
				equivalent_child->layer = layer;
			}
			
			/* The child is already registered in the search tree. Delete the newly created version.
			 */			
			ppg_token_free(cur_token);
		}
		else {
			
			PPG_PRINTF("newly defined: 0x%" PRIXPTR "\n", (uintptr_t)cur_token);
			
			#if DEBUG_PAPAGENO
			
			/* Detect pattern ambiguities
			 */
			if(equivalent_child) {
				if(	
					/* Melodies are ambiguous if ...
					 * the conflicting nodes/tokens are both leaf tokens
					 */
						(i == (n_tokens - 1))
					&&	!equivalent_child->children
					
					/* And defined for the same layer
					 */
					&& (equivalent_child->layer == layer)
				) {
					PPG_ERROR("Conflicting melodies detected.\n")
					
					#ifdef PAPAGENO_PRINT_SELF_ENABLED
					PPG_ERROR(
						"The tokens of the conflicting melodies are listed below.\n");
					
					PPG_ERROR("Previously defined:\n");
					
					PPG_CALL_VIRT_METHOD(equivalent_child, print_self, 0, false /* do not recurse */);
					
					PPG_ERROR("Conflicting:\n");
					for (PPG_Count i = 0; i < n_tokens; i++) {
						PPG_CALL_VIRT_METHOD(tokens[i], print_self, 0, false /* do not recurse */);
					}
					#endif
				}
			}
			#endif /* if DEBUG_PAPAGENO */
					
			cur_token->layer = layer;
			
			ppg_token_add_child(parent_token, cur_token);
			
			parent_token = cur_token;
		}
	}
	
	/* Return the leaf token 
	 */
	return parent_token;
}

PPG_Count ppg_branch_depth(PPG_Token__ *token)
{
	if(!token) { return 0; }
	
	PPG_Count max_depth = 0;
	
	for(PPG_Count i = 0; i < token->n_children; ++i) {
		
		PPG_Count cur_depth = 
			ppg_branch_depth(token->children[i]);
			
		if(cur_depth > max_depth) {
			max_depth = cur_depth;
		}
	}
	
	return 1 + max_depth;
}

PPG_Count ppg_pattern_tree_depth(void)
{
	return ppg_branch_depth(&ppg_context->pattern_root);
}
