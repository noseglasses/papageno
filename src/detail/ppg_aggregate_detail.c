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

void *ppg_aggregate_new(void *aggregate__) {
    
   /* Explict call to parent constructor
    */
   ppg_token_new((PPG_Token__*)aggregate__);
   
   PPG_Aggregate *aggregate = (PPG_Aggregate*)aggregate__;
      
   /* Initialize the aggregate
    */
   aggregate->n_members = 0;
   aggregate->inputs = NULL;
   aggregate->member_active = NULL;
#ifdef PPG_PEDANTIC_TOKENS
   aggregate->all_activated = false;
#endif
   aggregate->n_inputs_active = 0;

   return aggregate;
}

void ppg_aggregate_reset(PPG_Aggregate *aggregate) 
{
   ppg_token_reset((PPG_Token__*)aggregate);
   
#ifdef PPG_PEDANTIC_TOKENS
   aggregate->all_activated = false;
#endif
   
   aggregate->n_inputs_active = 0;
   
   for(PPG_Count i = 0; i < aggregate->n_members; ++i) {
      aggregate->member_active[i] = false;
   }
}


static void ppg_aggregate_deallocate_member_storage(PPG_Aggregate *aggregate) {  
   
   if(aggregate->inputs) {
      free(aggregate->inputs);
      aggregate->inputs = NULL;
   }
   if(aggregate->member_active) {
      free(aggregate->member_active);
      aggregate->member_active = NULL;
   }
}

static void ppg_aggregate_resize(PPG_Aggregate *aggregate, 
                     PPG_Count n_members)
{
   ppg_aggregate_deallocate_member_storage(aggregate);
   
   aggregate->n_members = n_members;
   aggregate->inputs = (PPG_Input_Id *)malloc(n_members*sizeof(PPG_Input_Id));
   aggregate->member_active = (bool *)malloc(n_members*sizeof(bool));
#ifdef PPG_PEDANTIC_TOKENS
   aggregate->all_activated = false;
#endif
   aggregate->n_inputs_active = 0;
   
   for(PPG_Count i = 0; i < n_members; ++i) {
      aggregate->member_active[i] = false;
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
