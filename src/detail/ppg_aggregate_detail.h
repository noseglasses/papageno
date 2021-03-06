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

#ifndef PPG_AGGREGATE_DETAIL_H
#define PPG_AGGREGATE_DETAIL_H

#include "detail/ppg_token_detail.h"
#include "ppg_input.h"
#include "ppg_settings.h"
#include "ppg_bitfield.h"

typedef struct {
   
   PPG_Token__ super;
    
   PPG_Count n_members;
   PPG_Input_Id *inputs;
   
   PPG_Bitfield member_active;
   
   PPG_Count n_inputs_active;
    
} PPG_Aggregate;

// Careful: Keep this in sync with flags for chords or clusters
//
enum {
   PPG_Aggregate_All_Active = 1 << 2
};

PPG_Token ppg_global_initialize_aggregate(   
                        PPG_Aggregate *aggregate,
                        PPG_Count n_inputs,
                        PPG_Input_Id inputs[]);

void *ppg_aggregate_new(void *aggregate__);

void ppg_aggregate_reset(PPG_Aggregate *aggregate);

size_t ppg_aggregate_dynamic_member_size(PPG_Aggregate *aggregate);
  
#if PPG_HAVE_DEBUGGING
bool ppg_aggregate_check_initialized(PPG_Token__ *token);
#endif

PPG_Aggregate* ppg_aggregate_destroy(PPG_Aggregate *aggregate);

PPG_Aggregate *ppg_aggregate_alloc(void);

bool ppg_aggregates_equal(PPG_Aggregate *c1, PPG_Aggregate *c2);

char *ppg_aggregate_copy_dynamic_members(PPG_Token__ *source,
                                         PPG_Token__ *target,
                                         char *buffer);

void ppg_aggregate_addresses_to_relative(  PPG_Token__ *token,
                                       void *begin_of_buffer);

void ppg_aggregate_addresses_to_absolute(  PPG_Token__ *token,
                                       void *begin_of_buffer);

#endif
