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

#include "ppg_note.h"
#include "ppg_debug.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_note_detail.h"
#include "detail/ppg_pattern_detail.h"

PPG_Token ppg_note_create(PPG_Input_Id input, PPG_Count flags)
{
    PPG_Note *note = (PPG_Note*)ppg_note_new(ppg_note_alloc());
    
    note->input = input;
    
    note->super.misc.flags = flags;
    
    // Return the new end of the pattern
    //
    return note;
}

PPG_Token ppg_note_create_standard(PPG_Input_Id input)
{
   return ppg_note_create(input, PPG_Note_Flags_A_N_D);
}

PPG_Token ppg_single_note_line(  
                     PPG_Layer layer,
                     PPG_Action action,
                     PPG_Count n_inputs,
                     PPG_Input_Id inputs[])
{
//    PPG_LOG("Adding single note line\n");
  
   PPG_Token__ *tokens[n_inputs];
      
   for (PPG_Count i = 0; i < n_inputs; i++) {

      tokens[i] = ppg_note_create_standard(inputs[i]);
   }
   
   ppg_token_store_action(tokens[n_inputs - 1], action);
   
   PPG_Token__ *leaf_token 
      = ppg_pattern_from_list(NULL, layer, n_inputs, tokens);
  
   return leaf_token;
}
