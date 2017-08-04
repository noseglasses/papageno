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

#include "ppg_leader_sequences.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_note_detail.h"
#include "detail/ppg_pattern_detail.h"

void ppg_alphabetic_leader_sequences(
            PPG_Layer layer,
            PPG_Token leader_token,
            uint8_t n_sequences,
            PPG_Leader_Functions functions,
            bool allow_fallback)
{
   PPG_Token__ *leader = (PPG_Token__ *)leader_token;
   
   PPG_Token__ *leafs[n_sequences];

   for(uint8_t seq = 0; seq < n_sequences; ++seq) {
      
      char buffer[PPG_MAX_SEQUENCE_LENGTH];
      
      PPG_Token__ *tokens[PPG_MAX_SEQUENCE_LENGTH];
   
      // Create a note sequence
      //
      functions.retreive_string(seq, buffer, PPG_MAX_SEQUENCE_LENGTH);
      
      PPG_Count n_tokens = 0;
      
      for(int i = 0; buffer[i]; i++) {
         
         PPG_Input_Id input = functions.input_from_char(buffer[i]);
         
         tokens[i] = ppg_note_create_standard(input);
         
         ++n_tokens;
      }
      
      PPG_Action action 
         = functions.retreive_action(seq);
      
      ppg_token_store_action(tokens[n_tokens - 1], action);
      
      leafs[seq] = tokens[n_tokens - 1];
      
      ppg_pattern_from_list(leader, layer, n_tokens, tokens);
   }
   
   if(!allow_fallback) { return; }
   
   // Check all the leafs and walk back along the branches 
   // to the first furcation and add fallback tags
   //
   for(uint8_t seq = 0; seq < n_sequences; ++seq) {
      
      PPG_Token__ *token = leafs[seq];
      PPG_Token__ *child = NULL;
      
      while(   (token != leader)
            && (token->n_children <= 1)) {
         
         if(!child) { 
            
            child = token;
         
            token = token->parent;
            
            continue; 
         }
         
         token->misc.action_flags |= PPG_Action_Fallback;
         
         token->action = child->action;
         
         child = token;
         
         token = token->parent;
      }
   } 
}
