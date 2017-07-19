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

#include "ppg_chord.h"
#include "ppg_debug.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_aggregate_detail.h"
#include "detail/ppg_pattern_detail.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_token_precedence_detail.h"

typedef PPG_Aggregate PPG_Chord;

static void ppg_chord_match_event(  
                                 PPG_Chord *chord,
                                 PPG_Event *event) 
{
   PPG_LOG("Ch. chord\n");
   
   bool input_part_of_chord = false;
   
   PPG_ASSERT(chord->n_members != 0);
   
   PPG_ASSERT(
         (chord->super.state == PPG_Token_In_Progress)
      || (chord->super.state == PPG_Token_Initialized)
   );
   
   /* Check if the input is part of the current chord 
    */
   for(PPG_Count i = 0; i < chord->n_members; ++i) {
      
      if(chord->inputs[i] == event->input) {
         
         input_part_of_chord = true;
         
         if(event->flags & PPG_Event_Active) {
            if(!ppg_bitfield_get_bit(&chord->member_active, i)) {
               ppg_bitfield_set_bit(&chord->member_active, i, true);
               ++chord->n_inputs_active;
            }
         }
         else {
            if(ppg_bitfield_get_bit(&chord->member_active, i)) {
               ppg_bitfield_set_bit(&chord->member_active, i, false);
               --chord->n_inputs_active;
            }
            else {
               
               // The event deactivates an input that
               // was not previously registered as active.
               // Thus, we conclude that the event must be related
               // to a previous match. Ignore the event.
               //
               return;
            }
         }
         break;
      }
   }
   
#ifdef PPG_HAVE_DEBUG
   for(PPG_Count i = 0; i < chord->n_members; ++i) {
      PPG_LOG("%d = %d\n", i, 
                 ppg_bitfield_get_bit(&chord->member_active, i));
   }
#endif
   
   if(!input_part_of_chord) {
      if(event->flags & PPG_Event_Active) {
         chord->super.state = PPG_Token_Invalid;
      }
      
      // Chords ignore unmatching deactivation events
      
      return;
   }
   
   chord->super.state = PPG_Token_In_Progress;
   
   if(chord->n_inputs_active == chord->n_members) {
      
#ifdef PPG_PEDANTIC_TOKENS
      chord->all_activated = true;
#else
      
      /* Chord matches
       */
      chord->super.state = PPG_Token_Matches;
//       PPG_LOG("C");
#endif
   }
#ifdef PPG_PEDANTIC_TOKENS
   else if(chord->n_inputs_active == 0) {
      
      if(chord->all_activated) {
      
         /* Chord matches
         */
         chord->super.state = PPG_Token_Matches;
//          PPG_LOG("C");
      }
   }
#endif
   
   return;
}

static PPG_Count ppg_chord_token_precedence(PPG_Token__ *token)
{
   return PPG_Token_Precedence_Chord;
}

#ifdef PPG_PRINT_SELF_ENABLED
static void ppg_chord_print_self(PPG_Chord *c, PPG_Count indent, bool recurse)
{
   PPG_I PPG_LOG("<*** chrd (0x%" PRIXPTR ") ***>\n", (uintptr_t)c);
   ppg_token_print_self_start((PPG_Token__*)c, indent);
   PPG_I PPG_LOG("\tn mem: %d\n", c->n_members);
   PPG_I PPG_LOG("\tn I actv: %d\n", c->n_inputs_active);
   
   for(PPG_Count i = 0; i < c->n_members; ++i) {
      PPG_I PPG_LOG("\t\tI: 0x%" PRIXPTR ", actv: %d\n", 
              (uintptr_t)c->inputs[i],
               ppg_bitfield_get_bit(&c->member_active, i));
   }
   ppg_token_print_self_end((PPG_Token__*)c, indent, recurse);
}
#endif

static PPG_Token_Vtable ppg_chord_vtable =
{
   .match_event 
      = (PPG_Token_Match_Event_Fun) ppg_chord_match_event,
   .reset 
      = (PPG_Token_Reset_Fun) ppg_aggregate_reset,
   .destroy 
      = (PPG_Token_Destroy_Fun) ppg_aggregate_destroy,
   .equals
      = (PPG_Token_Equals_Fun) ppg_aggregates_equal,
   .token_precedence
      = (PPG_Token_Precedence_Fun)ppg_chord_token_precedence
   #ifdef PPG_PRINT_SELF_ENABLED
   ,
   .print_self
      = (PPG_Token_Print_Self_Fun) ppg_chord_print_self
   #endif
};

PPG_Token ppg_chord_create(   
                        PPG_Count n_inputs,
                        PPG_Input_Id inputs[])
{
   PPG_Chord *chord = (PPG_Chord*)ppg_aggregate_new(ppg_aggregate_alloc());
   
   chord->super.vtable = &ppg_chord_vtable;
   
//    PPG_LOG("in def: 0x%" PRIXPTR "\n", (uintptr_t)ppg_chord_match_event);
   
   return ppg_global_initialize_aggregate(chord, n_inputs, inputs);
}

PPG_Token ppg_chord(    
                     PPG_Layer layer, 
                     PPG_Action action, 
                     PPG_Count n_inputs,
                     PPG_Input_Id inputs[])
{     
//    PPG_LOG("Adding chord\n");
   
   PPG_Token__ *token = 
      (PPG_Token__ *)ppg_chord_create(n_inputs, inputs);
      
   token->action = action;
      
   PPG_Token tokens[1] = { token };
   
   PPG_Token__ *leaf_token 
      = ppg_pattern_from_list(layer, 1, tokens);
      
   return leaf_token;
}
