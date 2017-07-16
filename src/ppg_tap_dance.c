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

#include "ppg_tap_dance.h"
#include "ppg_debug.h"
#include "ppg_note.h"
#include "ppg_global.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_pattern_detail.h"

#include <stddef.h>

PPG_Token ppg_tap_dance(   
                     PPG_Layer layer,
                     PPG_Input_Id input,
                     PPG_Count n_tap_definitions,
                     PPG_Tap_Definition tap_definitions[])
{
//    PPG_LOG("Adding tap dance\n");
   
   ppg_global_init();
   
   PPG_Count n_taps = 0;
   for (PPG_Count i = 0; i < n_tap_definitions; i++) { 
      
      if(tap_definitions[i].tap_count > n_taps) {
         n_taps = tap_definitions[i].tap_count;
      }
   }
   
   if(n_taps == 0) { return NULL; }
      
   PPG_Token tokens[n_taps];
   
   for (PPG_Count i = 0; i < n_taps; i++) {
      
      PPG_Token__ *new_note = (PPG_Token__*)ppg_note_create_standard(input);
         new_note->action.flags = PPG_Action_Fallback;
      
      tokens[i] = new_note;
   }
   
   for (PPG_Count i = 0; i < n_tap_definitions; i++) {
      
      ppg_token_store_action(
               tokens[tap_definitions[i].tap_count - 1], 
               tap_definitions[i].action);
   }
         
   PPG_Token__ *leafToken 
      = ppg_pattern_from_list(layer, n_taps, tokens);
   
   return leafToken;
}
