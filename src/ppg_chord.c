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

#include "ppg_chord.h"
#include "ppg_aggregate.h"

typedef PPG_Aggregate PPG_Chord;

static uint8_t ppg_chord_successor_consider_event(	
											PPG_Chord *chord,
											PPG_Event *event) 
{
	bool input_part_of_chord = false;
	
	PPG_ASSERT(chord->n_members != 0);
	
	PPG_ASSERT(chord->token_inventory.state == PPG_Token_In_Progress);
	
	/* Check it the input is part of the current chord 
	 */
	for(uint8_t i = 0; i < chord->n_members; ++i) {
		
		if(ppg_context->input_id_equal(chord->inputs[i].input_id, event->input_id)) {
			
			input_part_of_chord = true;
			
			bool input_active 
				= chord->inputs[i].check_active(chord->inputs[i].input_id, event->state);
			
			if(input_active) {
				if(!chord->member_active[i]) {
					chord->member_active[i] = true;
					++chord->n_inputs_active;
				}
			}
			else {
				if(chord->member_active[i]) {
					chord->member_active[i] = false;
					--chord->n_inputs_active;
				}
			}
			break;
		}
	}
	
	if(!input_part_of_chord) {
// 		if(event->pressed) {
			chord->token_inventory.state = PPG_Token_Invalid;
			return chord->token_inventory.state;
// 		}
	}
	
	if(chord->n_inputs_active == chord->n_members) {
		
		/* Chord completed
		 */
		chord->token_inventory.state = PPG_Token_Completed;
 		PPG_PRINTF("C");
	}
	
	return chord->token_inventory.state;
}

#if PAPAGENO_PRINT_SELF_ENABLED
static void ppg_chord_print_self(PPG_Chord *c)
{
	ppg_token_print_self((PPG_Token__*)c);
	
	PPG_PRINTF("chord\n");
	PPG_PRINTF("   n_members: %d\n", c->n_members);
	PPG_PRINTF("   n_inputs_active: %d\n", c->n_inputs_active);
	
	for(uint8_t i = 0; i < c->n_members; ++i) {
		PPG_PRINTF("      input_id: %d, active: %d\n", 
				  c->inputs[i].input_id, c->member_active[i]);
	}
}
#endif

static PPG_Token_Vtable ppg_chord_vtable =
{
	.successor_consider_event 
		= (PPG_Token_Successor_Consider_Inputchange_Fun) ppg_chord_successor_consider_event,
	.reset 
		= (PPG_Token_Reset_Fun) ppg_aggregate_reset,
	.trigger_action 
		= (PPG_Token_Trigger_Action_Fun) ppg_token_trigger_action,
	.destroy 
		= (PPG_Token_Destroy_Fun) ppg_aggregate_destroy,
	.equals
		= (PPG_Token_Equals_Fun) ppg_aggregates_equal
	#if PAPAGENO_PRINT_SELF_ENABLED
	,
	.print_self
		= (PPG_Token_Print_Self_Fun) ppg_chord_print_self
	#endif
};

PPG_Token ppg_create_chord(	
								uint8_t n_inputs,
								PPG_Input inputs[])
{
	PPG_Chord *chord = (PPG_Chord*)ppg_aggregate_new(ppg_aggregate_alloc());
	
	chord->token_inventory.vtable = &ppg_chord_vtable;
	
	return ppg_initialize_aggregate(chord, n_inputs, inputs);
}

PPG_Token ppg_chord(		
							uint8_t layer, 
							PPG_Action action, 
							uint8_t n_inputs,
							PPG_Input inputs[])
{   	
	PPG_PRINTF("Adding chord\n");
	
	PPG_Token__ *token = 
		(PPG_Token__ *)ppg_create_chord(n_inputs, inputs);
		
	token->action = action;
		
	PPG_Token tokens[1] = { token };
	
	PPG_Token__ *leaf_token 
		= ppg_pattern_from_list(layer, 1, tokens);
		
	return leaf_token;
}
