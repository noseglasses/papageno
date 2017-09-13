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

#include "detail/ppg_aggregate_detail.h"
#include "detail/ppg_input_detail.h"
#include "detail/ppg_context_detail.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void *ppg_aggregate_new(void *aggregate__) {
    
   /* Explict call to parent constructor
    */
   ppg_token_new((PPG_Token__*)aggregate__);
   
   PPG_Aggregate *aggregate = (PPG_Aggregate*)aggregate__;
      
   /* Initialize the aggregate
    */
   aggregate->n_members = 0;
   aggregate->inputs = NULL;
   
   ppg_bitfield_init(&aggregate->member_active);

   aggregate->n_inputs_active = 0;

   return aggregate;
}

void ppg_aggregate_reset(PPG_Aggregate *aggregate) 
{
   ppg_token_reset_control_state((PPG_Token__*)aggregate);
   
   aggregate->n_inputs_active = 0;
   
   for(PPG_Count i = 0; i < aggregate->n_members; ++i) {
      ppg_bitfield_set_bit(&aggregate->member_active,
                           i,
                           false);
   }
   
   // Clear the activation state
   //
   aggregate->super.misc.flags 
         &= (PPG_Count)~PPG_Aggregate_All_Active;
}

static void ppg_aggregate_deallocate_member_storage(PPG_Aggregate *aggregate) {  
   
   if(aggregate->inputs) {
      free(aggregate->inputs);
      aggregate->inputs = NULL;
   }
   
   ppg_bitfield_destroy(&aggregate->member_active);
}

static void ppg_aggregate_resize(PPG_Aggregate *aggregate, 
                     PPG_Count n_members)
{
   ppg_aggregate_deallocate_member_storage(aggregate);
   
   aggregate->n_members = n_members;
   
   aggregate->inputs = (PPG_Input_Id *)malloc(n_members*sizeof(PPG_Input_Id));
	
   ppg_bitfield_resize(&aggregate->member_active,
                       n_members,
                       false /*drop old bits*/);

   aggregate->n_inputs_active = 0;
      
   for(PPG_Count i = 0; i < n_members; ++i) {
      ppg_global_init_input(&aggregate->inputs[i]);
   }
}

PPG_Aggregate* ppg_aggregate_destroy(PPG_Aggregate *aggregate) {
   
   ppg_aggregate_deallocate_member_storage(aggregate);

   return aggregate;
}

PPG_Aggregate *ppg_aggregate_alloc(void) {
    return (PPG_Aggregate*)malloc(sizeof(PPG_Aggregate));
}

bool ppg_aggregates_equal(PPG_Aggregate *c1, PPG_Aggregate *c2) 
{
   if(c1->n_members != c2->n_members) { return false; }
   
   PPG_Count n_equalities = 0;
   
   for(PPG_Count i = 0; i < c1->n_members; ++i) {
      for(PPG_Count j = 0; j < c1->n_members; ++j) {
         if(c1->inputs[i] == c2->inputs[j]) {
            ++n_equalities;
            break;
         }
      }
   }
   
   return n_equalities == c1->n_members;
}

PPG_Token ppg_global_initialize_aggregate(   
                        PPG_Aggregate *aggregate,
                        PPG_Count n_inputs,
                        PPG_Input_Id inputs[])
{
   ppg_aggregate_resize(aggregate, n_inputs);
    
   for(PPG_Count i = 0; i < n_inputs; ++i) {
      aggregate->inputs[i] = inputs[i];
   }
    
   /* Return the new end of the pattern 
    */
   return aggregate;
}

size_t ppg_aggregate_dynamic_member_size(PPG_Aggregate *aggregate)
{
   return   ppg_token_dynamic_member_size((PPG_Token__*)aggregate)
         +  aggregate->n_members*sizeof(PPG_Input_Id)
         +  ppg_bitfield_get_num_cells(&aggregate->member_active)
               *sizeof(PPG_Bitfield_Storage_Type);
}

char *ppg_aggregate_copy_dynamic_members(PPG_Token__ *source, 
                                         PPG_Token__ *target, 
                                         char *buffer)
{
   PPG_Aggregate *aggregate = (PPG_Aggregate *)source;
   
   PPG_Aggregate *copy_of_aggregate = (PPG_Aggregate *)target;
   
   buffer = ppg_token_copy_dynamic_members(source, buffer);
   
   size_t n_bytes = aggregate->n_members*sizeof(PPG_Input_Id);
   
   memcpy(buffer, (void*)aggregate->inputs, n_bytes);
   
   copy_of_aggregate->inputs = (PPG_Input_Id *)buffer;
   
   buffer += n_bytes;
   
   n_bytes = ppg_bitfield_get_num_cells(&aggregate->member_active)
               *sizeof(PPG_Bitfield_Storage_Type);
   
   memcpy(buffer, (void*)aggregate->member_active.bitarray, n_bytes);
   
   copy_of_aggregate->member_active.bitarray = (PPG_Bitfield_Storage_Type *)buffer;
   
   return buffer + n_bytes;
}

void ppg_aggregate_addresses_to_relative(  PPG_Token__ *token,
                                       void *begin_of_buffer
) 
{
   ppg_token_addresses_to_relative(token, begin_of_buffer);
   
   PPG_Aggregate *aggregate = (PPG_Aggregate *)token;
   
   aggregate->inputs = (PPG_Input_Id *)((char*)aggregate->inputs
                                 - (char*)begin_of_buffer);
   
   aggregate->member_active.bitarray = (PPG_Bitfield_Storage_Type *)
                        ((char*)aggregate->member_active.bitarray
                                 - (char*)begin_of_buffer);
}

void ppg_aggregate_addresses_to_absolute(  PPG_Token__ *token,
                                       void *begin_of_buffer
) 
{
   ppg_token_addresses_to_absolute(token, begin_of_buffer);
   
   PPG_Aggregate *aggregate = (PPG_Aggregate *)token;
   
   aggregate->inputs = (PPG_Input_Id *)((char*)begin_of_buffer
                              + (size_t)aggregate->inputs);
   
   aggregate->member_active.bitarray = (PPG_Bitfield_Storage_Type *)
         ((char*)begin_of_buffer
               + (size_t)aggregate->member_active.bitarray);
}

#if PPG_HAVE_DEBUGGING
bool ppg_aggregate_check_initialized(PPG_Token__ *token)
{
   PPG_Aggregate *aggregate = (PPG_Aggregate*)token;
   
   bool assertion_failed = false;
   
   assertion_failed |= ppg_token_check_initialized(token);
   
   PPG_ASSERT_WARN((aggregate->super.misc.flags 
         & PPG_Aggregate_All_Active) == 0);
   
   PPG_ASSERT_WARN(aggregate->n_inputs_active == 0);
   
   for(PPG_Count i = 0; i < aggregate->n_members; ++i) {
      PPG_ASSERT_WARN(ppg_bitfield_get_bit(&aggregate->member_active,
                           i) == false);
   }
   
   return assertion_failed;
}
#endif
