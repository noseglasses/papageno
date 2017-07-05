/* Copyright 2017 Florian Fleissner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ppg_cluster.h"
#include "ppg_debug.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_aggregate_detail.h"
#include "detail/ppg_pattern_detail.h"

typedef PPG_Aggregate PPG_Cluster;

static PPG_Processing_State ppg_cluster_match_event(	
											PPG_Cluster *cluster,
											PPG_Event *event) 
{
	bool input_part_of_cluster = false;
	
	PPG_ASSERT(cluster->n_members != 0);
	
	PPG_ASSERT(cluster->super.state == PPG_Token_In_Progress);
	
	/* Check it the input is part of the current chord 
	 */
	for(PPG_Count i = 0; i < cluster->n_members; ++i) {
		
		if(ppg_context->input_id_equal(cluster->inputs[i].input_id, event->input_id)) {
			
			input_part_of_cluster = true;
			
			bool input_active 
				= cluster->inputs[i].check_active(cluster->inputs[i].input_id, event->state);
			
			if(input_active) {
				if(!cluster->member_active[i]) {
					cluster->member_active[i] = true;
					++cluster->n_inputs_active;
				}
			}
			/* Note: We do not care for released inputs here. Every cluster member must be pressed only once
			 */

			break;
		}
	}
	
	if(!input_part_of_cluster) {
// 		if(event->pressed) {
			cluster->super.state = PPG_Token_Invalid;
			return cluster->super.state;
// 		}
	}
	
	if(cluster->n_inputs_active == cluster->n_members) {
		
		/* Cluster matches
		 */
		cluster->super.state = PPG_Token_Matches;
 		PPG_PRINTF("O");
	}
	
	return cluster->super.state;
}

static PPG_Count ppg_cluster_token_precedence(void)
{
	return 70;
}

#if PAPAGENO_PRINT_SELF_ENABLED
static void ppg_cluster_print_self(PPG_Cluster *c)
{
	ppg_token_print_self((PPG_Token__*)c);
	
	PPG_PRINTF("cluster\n");
	PPG_PRINTF("   n_members: %d\n", c->n_members);
	PPG_PRINTF("   n_inputs_active: %d\n", c->n_inputs_active);
	
	for(PPG_Count i = 0; i < c->n_members; ++i) {
		PPG_PRINTF("      input_id: %d, active: %d\n", 
				  c->inputs[i].input_id, c->member_active[i]);
	}
}
#endif

static PPG_Token_Vtable ppg_cluster_vtable =
{
	.match_event 
		= (PPG_Token_Match_Event_Fun) ppg_cluster_match_event,
	.reset 
		= (PPG_Token_Reset_Fun) ppg_aggregate_reset,
	.trigger_action 
		= (PPG_Token_Trigger_Action_Fun) ppg_token_trigger_action,
	.destroy 
		= (PPG_Token_Destroy_Fun) ppg_aggregate_destroy,
	.equals
		= (PPG_Token_Equals_Fun) ppg_aggregates_equal,
	.token_precedence
		= (PPG_Token_Precedence_Fun)ppg_cluster_token_precedence
	#if PAPAGENO_PRINT_SELF_ENABLED
	,
	.print_self
		= (PPG_Token_Print_Self_Fun) ppg_cluster_print_self
	#endif
};
	
PPG_Token ppg_cluster_create(
								PPG_Count n_inputs,
									PPG_Input inputs[])
{
	PPG_Cluster *cluster = (PPG_Cluster*)ppg_aggregate_new(ppg_aggregate_alloc());
	 
	cluster->super.vtable = &ppg_cluster_vtable;
	
	return ppg_global_initialize_aggregate(cluster, n_inputs, inputs);
}

PPG_Token ppg_cluster(		
							PPG_Layer layer, 
							PPG_Action action, 
							PPG_Count n_inputs,
							PPG_Input inputs[])
{   	
	PPG_PRINTF("Adding cluster\n");
	
	PPG_Token__ *token = 
		(PPG_Token__ *)ppg_cluster_create(n_inputs, inputs);
		
	token->action = action;
	
	PPG_Token tokens[1] = { token };
	
	PPG_Token__ *leaf_token 
		= ppg_pattern_from_list(layer, 1, tokens);
		
	return leaf_token;
}
