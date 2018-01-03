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

static bool ppg_chord_match_event(  
                                 PPG_Chord *chord,
                                 PPG_Event *event,
                                 bool modify_only_if_consuming) 
{
   PPG_LOG("Ch. chord\n");
   
   bool input_part_of_chord = false;
   
   PPG_ASSERT(chord->n_members != 0);
   
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
            
            if(   ((chord->super.misc.flags & PPG_Aggregate_All_Active) == 0)
               && (chord->super.misc.flags &          
                        PPG_Chord_Flags_Disallow_Input_Deactivation)) {
               
               if(!modify_only_if_consuming) {
                  chord->super.misc.state = PPG_Token_Invalid;
               }
               
               return false;
            }
            
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
               return false;
            }
         }
         break;
      }
   }
   
#if PPG_HAVE_LOGGING
   for(PPG_Count i = 0; i < chord->n_members; ++i) {
      PPG_LOG("%d: 0x%d = %d\n", 
              i, 
              chord->inputs[i],
              ppg_bitfield_get_bit(&chord->member_active, i)
      );
   }
#endif
   
   if(!input_part_of_chord) {
      if(event->flags & PPG_Event_Active) {
         
         if(!modify_only_if_consuming) {
            chord->super.misc.state = PPG_Token_Invalid;
         }
      }
      
      // Chords ignore unmatching deactivation events
      
      return false;
   }
   
   chord->super.misc.state = PPG_Token_Activation_In_Progress;
   
   if(chord->n_inputs_active == chord->n_members) {
      
      chord->super.misc.flags |= PPG_Aggregate_All_Active;
      
      /* Chord matches
       */
      chord->super.misc.state = PPG_Token_Matches;
//       PPG_LOG("C");
   }
   else if(chord->n_inputs_active == 0) {
      
      if(chord->super.misc.flags & PPG_Aggregate_All_Active) {
      
         if(chord->super.misc.flags & PPG_Token_Flags_Pedantic) {
            /* Chord matches
            */
            chord->super.misc.state = PPG_Token_Matches;
         }
         else {
            chord->super.misc.state = PPG_Token_Finalized;
         }
      }
   }
   else {
      if(chord->super.misc.flags & PPG_Aggregate_All_Active) {
         chord->super.misc.state = PPG_Token_Deactivation_In_Progress;
      }
   }
   
   return true;
}

static PPG_Count ppg_chord_token_precedence(PPG_Token__ *token)
{
   PPG_UNUSED(token);
   return PPG_Token_Precedence_Chord;
}

static size_t ppg_chord_dynamic_member_size(PPG_Token *token)
{
   return   sizeof(PPG_Chord)
         +  ppg_aggregate_dynamic_member_size((PPG_Aggregate *)token);
}

static char *ppg_chord_placement_clone(PPG_Token__ *token, char *buffer)
{
   PPG_Chord *chord = (PPG_Chord *)token;
   
   *((PPG_Chord *)buffer) = *chord;
   
   PPG_Token__ *clone = (PPG_Token__ *)buffer;
   
//    printf("Replacing children pointer %p with %p\n", clone->children, (PPG_Token__ **)(buffer + sizeof(PPG_Chord)));
   
   clone->children = (PPG_Token__ **)(buffer + sizeof(PPG_Chord));
   
   return ppg_aggregate_copy_dynamic_members(token, clone, buffer + sizeof(PPG_Chord));
}

#if PPG_PRINT_SELF_ENABLED
static void ppg_chord_print_self(PPG_Chord *c, PPG_Count indent, bool recurse)
{
   PPG_I PPG_LOG("<*** chrd (0x%" PRIXPTR ") ***>\n", (uintptr_t)c);
   ppg_token_print_self_start((PPG_Token__*)c, indent);
   PPG_I PPG_LOG("\tn mem: %d\n", c->n_members);
   PPG_I PPG_LOG("\tn I actv: %d\n", c->n_inputs_active);
   
   for(PPG_Count i = 0; i < c->n_members; ++i) {
      PPG_I PPG_LOG("\t\tI: 0x%d, actv: %d\n", 
              c->inputs[i],
               ppg_bitfield_get_bit(&c->member_active, i));
   }
   ppg_token_print_self_end((PPG_Token__*)c, indent, recurse);
}
#endif

PPG_Token_Vtable ppg_chord_vtable =
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
      = (PPG_Token_Precedence_Fun)ppg_chord_token_precedence,
   .dynamic_size
      = (PPG_Token_Dynamic_Size_Requirement_Fun)ppg_chord_dynamic_member_size,
   .placement_clone
      = (PPG_Token_Placement_Clone_Fun)ppg_chord_placement_clone,
   .register_ptrs_for_compression
      = (PPG_Token_Register_Pointers_For_Compression)ppg_token_register_pointers_for_compression,
   .addresses_to_relative
      = (PPG_Token_Addresses_To_Relative)ppg_aggregate_addresses_to_relative,
   .addresses_to_absolute
      = (PPG_Token_Addresses_To_Absolute)ppg_aggregate_addresses_to_absolute
      
   #if PPG_PRINT_SELF_ENABLED
   ,
   .print_self
      = (PPG_Token_Print_Self_Fun) ppg_chord_print_self
   #endif
   
   #if PPG_HAVE_DEBUGGING
   ,
   .check_initialized
      = (PPG_Token_Check_Initialized_Fun)ppg_aggregate_check_initialized
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
      
   PPG_Token__ *tokens[1] = { token };
   
   PPG_Token__ *leaf_token 
      = ppg_pattern_from_list(NULL, layer, 1, tokens);
      
   return leaf_token;
}
