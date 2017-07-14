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

#ifndef PPG_AGGREGATE_DETAIL_H
#define PPG_AGGREGATE_DETAIL_H

#include "detail/ppg_token_detail.h"
#include "ppg_input.h"
#include "ppg_settings.h"

typedef struct {
	
	PPG_Token__ super;
	 
	PPG_Count n_members;
	PPG_Input_Id *inputs;
	bool *member_active;
	
#ifdef PPG_PEDANTIC_TOKENS
	bool all_activated;
#endif
	PPG_Count n_inputs_active;
	 
} PPG_Aggregate;

PPG_Token ppg_global_initialize_aggregate(	
								PPG_Aggregate *aggregate,
								PPG_Count n_inputs,
								PPG_Input_Id inputs[]);

void *ppg_aggregate_new(void *aggregate__);

void ppg_aggregate_reset(PPG_Aggregate *aggregate);

PPG_Aggregate* ppg_aggregate_destroy(PPG_Aggregate *aggregate);

PPG_Aggregate *ppg_aggregate_alloc(void);

bool ppg_aggregates_equal(PPG_Aggregate *c1, PPG_Aggregate *c2);

#endif
