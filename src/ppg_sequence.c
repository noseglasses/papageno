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

#include "ppg_sequence.h"
#include "ppg_debug.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_sequence_detail.h"
#include "detail/ppg_pattern_detail.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_token_precedence_detail.h"

#define S_AGGREGATE sequence->aggregate

static bool ppg_sequence_match_event(  
                                 PPG_Sequence *sequence,
                                 PPG_Event *event,
                                 bool modify_only_if_consuming) 
{
   PPG_LOG("Ch. sequence\n");
   
   PPG_LOG("++++++++++++++Sequence 0x%" PRIXPTR " %d/%d, flags: %d, state: %d\n", (uintptr_t)sequence, sequence->next_member, S_AGGREGATE.n_members,S_AGGREGATE.super.misc.flags, S_AGGREGATE.super.misc.state);
   
   bool input_part_of_sequence = false;
   
   PPG_ASSERT(S_AGGREGATE.n_members != 0);
   
   if(event->flags & PPG_Event_Active) {
      if(S_AGGREGATE.inputs[sequence->next_member] == event->input) {
         ppg_bitfield_set_bit(&S_AGGREGATE.member_active, sequence->next_member, true);
         ++sequence->next_member;
         ++S_AGGREGATE.n_inputs_active;
         input_part_of_sequence = true;
      }
      else {
         
         if(!modify_only_if_consuming) {
            S_AGGREGATE.super.misc.state = PPG_Token_Invalid;
         }
               
         return false;
      }
   }
   else {
      for(PPG_Count i = 0; i < sequence->next_member; ++i) {
         
         if(S_AGGREGATE.inputs[i] == event->input) {
            
            if(ppg_bitfield_get_bit(&S_AGGREGATE.member_active, i)) {
               ppg_bitfield_set_bit(&S_AGGREGATE.member_active, i, false);
               --S_AGGREGATE.n_inputs_active;
               input_part_of_sequence = true;
               break;
            }
         }
      }
   }
   
#if PPG_HAVE_LOGGING
   PPG_LOG("inputs active: %d\n", S_AGGREGATE.n_inputs_active);
   for(PPG_Count i = 0; i < S_AGGREGATE.n_members; ++i) {
      PPG_LOG("%d: 0x%d = %d\n", 
              i, 
              S_AGGREGATE.inputs[i],
              ppg_bitfield_get_bit(&S_AGGREGATE.member_active, i)
      );
   }
#endif
   
   if(!input_part_of_sequence) {
         
      if(event->flags & PPG_Event_Active) {
         if(!modify_only_if_consuming) {
            S_AGGREGATE.super.misc.state = PPG_Token_Invalid;
         }
      }
      
      // Sequences ignore non-matching deactivation events
      
      PPG_LOG("sequence ignores non matching deactivation event\n");
      return false;
   }
   
   S_AGGREGATE.super.misc.state = PPG_Token_Activation_In_Progress;
   
   if(   (sequence->next_member >= S_AGGREGATE.n_members) 
      // Only enter this clause once when the last input of the sequence activated
      && !(S_AGGREGATE.super.misc.flags & PPG_Aggregate_All_Active)) {
      
      S_AGGREGATE.super.misc.flags |= PPG_Aggregate_All_Active;
      
      if(!(S_AGGREGATE.super.misc.flags & PPG_Token_Flags_Pedantic)) {
         /* Sequence matches
         */
         S_AGGREGATE.super.misc.state = PPG_Token_Matches;
      }
   }
   else if(S_AGGREGATE.n_inputs_active == 0) {
      
      if(S_AGGREGATE.super.misc.flags & PPG_Aggregate_All_Active) {
      
         if(S_AGGREGATE.super.misc.flags & PPG_Token_Flags_Pedantic) {
            /* Sequence matches
            */
            S_AGGREGATE.super.misc.state = PPG_Token_Matches;
         }
         else {
            S_AGGREGATE.super.misc.state = PPG_Token_Finalized;
         }
      }
   }
   else {
      if(S_AGGREGATE.super.misc.flags & PPG_Aggregate_All_Active) {
         S_AGGREGATE.super.misc.state = PPG_Token_Deactivation_In_Progress;
      }
   }
   
   PPG_LOG("================Sequence 0x%" PRIXPTR " %d/%d, flags: %d, state: %d\n", 
          (uintptr_t)sequence, sequence->next_member, S_AGGREGATE.n_members,S_AGGREGATE.super.misc.flags, S_AGGREGATE.super.misc.state);
   
   return true;
}

static PPG_Count ppg_sequence_token_precedence(PPG_Token__ *token)
{
   PPG_UNUSED(token);
   return PPG_Token_Precedence_Sequence;
}

static size_t ppg_sequence_dynamic_member_size(PPG_Token *token)
{
   return   sizeof(PPG_Sequence)
         +  ppg_aggregate_dynamic_member_size((PPG_Aggregate *)token);
}

static char *ppg_sequence_placement_clone(PPG_Token__ *token, char *buffer)
{
   PPG_Sequence *sequence = (PPG_Sequence *)token;
   
   *((PPG_Sequence *)buffer) = *sequence;
   
   PPG_Token__ *clone = (PPG_Token__ *)buffer;
   
//    printf("Replacing children pointer %p with %p\n", clone->children, (PPG_Token__ **)(buffer + sizeof(PPG_Sequence)));
   
   clone->children = (PPG_Token__ **)(buffer + sizeof(PPG_Sequence));
   
   return ppg_aggregate_copy_dynamic_members(token, clone, buffer + sizeof(PPG_Sequence));
}

static void ppg_sequence_reset(PPG_Sequence *sequence) 
{
   sequence->next_member = 0;
   ppg_aggregate_reset((PPG_Aggregate*)sequence);
}

#if PPG_PRINT_SELF_ENABLED
static void ppg_sequence_print_self(PPG_Sequence *c, PPG_Count indent, bool recurse)
{
   PPG_I PPG_LOG("<*** seq (0x%" PRIXPTR ") ***>\n", (uintptr_t)c);
   ppg_token_print_self_start((PPG_Token__*)c, indent);
   PPG_I PPG_LOG("\tnext member: %d\n", c->next_member);
   ppg_token_print_self_end((PPG_Token__*)c, indent, recurse);
}
#endif

PPG_Token_Vtable ppg_sequence_vtable =
{
   .match_event
      = (PPG_Token_Match_Event_Fun) ppg_sequence_match_event,
   .reset 
      = (PPG_Token_Reset_Fun) ppg_sequence_reset,
   .destroy 
      = (PPG_Token_Destroy_Fun) ppg_aggregate_destroy,
   .equals
      = (PPG_Token_Equals_Fun) ppg_aggregates_equal,
   .token_precedence
      = (PPG_Token_Precedence_Fun)ppg_sequence_token_precedence,
   .dynamic_size
      = (PPG_Token_Dynamic_Size_Requirement_Fun)ppg_sequence_dynamic_member_size,
   .placement_clone
      = (PPG_Token_Placement_Clone_Fun)ppg_sequence_placement_clone,
   .register_ptrs_for_compression
      = (PPG_Token_Register_Pointers_For_Compression)ppg_token_register_pointers_for_compression,
   .addresses_to_relative
      = (PPG_Token_Addresses_To_Relative)ppg_aggregate_addresses_to_relative,
   .addresses_to_absolute
      = (PPG_Token_Addresses_To_Absolute)ppg_aggregate_addresses_to_absolute
      
   #if PPG_PRINT_SELF_ENABLED
   ,
   .print_self
      = (PPG_Token_Print_Self_Fun) ppg_sequence_print_self
   #endif
   
   #if PPG_HAVE_DEBUGGING
   ,
   .check_initialized
      = (PPG_Token_Check_Initialized_Fun)ppg_aggregate_check_initialized
   #endif
};

PPG_Token ppg_sequence_create(   
                        PPG_Count n_inputs,
                        PPG_Input_Id inputs[])
{
   PPG_Sequence *sequence = (PPG_Sequence*)ppg_aggregate_new(ppg_aggregate_alloc());
   
   S_AGGREGATE.super.vtable = &ppg_sequence_vtable;
   
   sequence->next_member = 0;
   
//    PPG_LOG("in def: 0x%" PRIXPTR "\n", (uintptr_t)ppg_sequence_match_event);
   
   return ppg_global_initialize_aggregate((PPG_Aggregate*)sequence, n_inputs, inputs);
}

PPG_Token ppg_sequence(    
                     PPG_Layer layer, 
                     PPG_Action action, 
                     PPG_Count n_inputs,
                     PPG_Input_Id inputs[])
{     
//    PPG_LOG("Adding sequence\n");
   
   PPG_Token__ *token = 
      (PPG_Token__ *)ppg_sequence_create(n_inputs, inputs);
      
   token->action = action;
      
   PPG_Token__ *tokens[1] = { token };
   
   PPG_Token__ *leaf_token 
      = ppg_pattern_from_list(NULL, layer, 1, tokens);
      
   return leaf_token;
}
