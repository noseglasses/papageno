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

#include "detail/ppg_furcation_detail.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_pattern_detail.h"
#include "ppg_debug.h"

#include <stdlib.h>

void ppg_furcation_stack_init(PPG_Furcation_Stack *stack)
{
   stack->furcations = NULL;
   stack->n_furcations = 0;
   stack->cur_furcation = -1;
}

void ppg_furcation_create_or_update(PPG_Count n_branch_candidates,
                                 PPG_Token__ *branch)
{
   // Check if there is already a furcation open for the current node
   //
   if(   PPG_FB.cur_furcation == -1
      || (PPG_FB.furcations[PPG_FB.cur_furcation].token 
                  != ppg_context->current_token)
   ) {
      PPG_LOG("Crt furc for tk 0x%" PRIXPTR "\n", 
             (uintptr_t)ppg_context->current_token);
  
      // If not, we open a new furcation
   
      PPG_ASSERT(PPG_FB.cur_furcation < PPG_FB.n_furcations - 1);
      
      ++PPG_FB.cur_furcation;
      
      // The number of branch candidates is decreased when reverting
      //
      PPG_FB.furcations[PPG_FB.cur_furcation].n_branch_candidates 
                  = n_branch_candidates;
                  
      PPG_FB.furcations[PPG_FB.cur_furcation].event_id = PPG_EB.cur;
      PPG_FB.furcations[PPG_FB.cur_furcation].token = ppg_context->current_token;
      PPG_FB.furcations[PPG_FB.cur_furcation].branch = branch;
   }
   else {
      PPG_LOG("Upd. furc for tk 0x%" PRIXPTR "\n", 
             (uintptr_t)ppg_context->current_token);
                  
      PPG_FB.furcations[PPG_FB.cur_furcation].branch = branch;
   }
   
   PPG_LOG("br 0x%" PRIXPTR ", n. c. %u\n", 
             (uintptr_t)branch, n_branch_candidates);
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
      
//       PPG_CALL_VIRT_METHOD(cur_token, reset);
//       
//       ppg_token_reset(cur_token);
      
      // Reset all children (data structures and state)
      //
      ppg_token_reset_children(cur_token);
      
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
bool ppg_furcation_revert(PPG_Token__ *start_token)
{
   // If there is no current furcation, we at least clean up to 
   // the root node. Otherwise we cleanup to the current furcation.
   //
   PPG_Token__ *termination_token = NULL;
      
   while(1) {
      
      termination_token
         = (PPG_FB.cur_furcation == -1) ? NULL : PPG_CUR_FUR.branch;
   
      // Recursively reset all tokens of the branch we are on
      // wich the respect to the current furcation 
      // back until the first token of the branch.
      // 
      ppg_branch_cleanup(start_token, termination_token);
      
      // If there is no current furcation, we can't do anything else
      //
      if(PPG_FB.cur_furcation == -1) { 
         
         // By returning false we signal that there is no
         // reversion to another furcation possible.
         //
         return false;
      }
      
      // Mark the current branch (i.e. the branch's root node) as invalid
      //
      PPG_CUR_FUR.branch->state = PPG_Token_Invalid;
      
      // If we already tried all possible branches of the current furcation
      //
      if(PPG_CUR_FUR.n_branch_candidates == 0) {
         
         // ... we cleanup all child nodes
         //
         ppg_token_reset_children(PPG_CUR_FUR.token);
         
         // Mark the furcation node itself as invalid
         //
         PPG_CUR_FUR.token->state = PPG_Token_Invalid;
         
         // Store the token that is associated with the current
         // furcation to preserve it when decrementing the furcation below
         //
         start_token = PPG_CUR_FUR.token;
         
         // Replace the current furcation with the previous one (if possible)
         //
         --PPG_FB.cur_furcation;
      }
      else {
         
         // There are still branches left of the current furcation.
         // However, we just tried one that was not successful.
         //
         --PPG_CUR_FUR.n_branch_candidates;
         
         break;
      }
   }
   
   // Reset the current token for further processing
   //
   ppg_context->current_token = PPG_CUR_FUR.token;
   
   // Reset the current event that is used for further processing
   //
   PPG_EB.cur = PPG_CUR_FUR.event_id;
   
   return true;
}

void ppg_furcation_stack_resize(void)
{
   if(PPG_FB.furcations) { return; }
   
   PPG_FB.n_furcations = ppg_context->tree_depth;
   
   PPG_FB.furcations 
         = (PPG_Furcation*)malloc(ppg_context->tree_depth*sizeof(PPG_Furcation));
}

void ppg_furcation_stack_free(PPG_Furcation_Stack *stack)
{
   if(!stack->furcations) { return; }
   
   free(stack->furcations);
   
   stack->furcations = NULL;
}
   