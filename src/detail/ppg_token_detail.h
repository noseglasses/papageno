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

#ifndef PPG_TOKEN_DETAIL_H
#define PPG_TOKEN_DETAIL_H

#include "ppg_token.h"
#include "ppg_event.h"
#include "ppg_action.h"
#include "ppg_layer.h"

struct PPG_TokenStruct;

typedef uint8_t PPG_Processing_State;

typedef PPG_Processing_State (*PPG_Token_Match_Event_Fun)(
														struct PPG_TokenStruct *token, 
														PPG_Event *event
 													);

typedef void (*PPG_Token_Reset_Fun)(	struct PPG_TokenStruct *token);

typedef bool (*PPG_Token_Trigger_Action_Fun)(	struct PPG_TokenStruct *token, PPG_Slot_Id slot_id);

typedef struct PPG_TokenStruct * (*PPG_Token_Destroy_Fun)(struct PPG_TokenStruct *token);

typedef bool (*PPG_Token_Equals_Fun)(struct PPG_TokenStruct *p1, struct PPG_TokenStruct *p2);

#if PAPAGENO_PRINT_SELF_ENABLED
typedef void (*PPG_Token_Print_Self_Fun)(struct PPG_TokenStruct *p);
#endif

typedef struct {
									
	PPG_Token_Match_Event_Fun 
									match_event;
									
	PPG_Token_Reset_Fun
									reset;
									
	PPG_Token_Trigger_Action_Fun
									trigger_action;
									
	PPG_Token_Destroy_Fun	
									destroy;
			
	PPG_Token_Equals_Fun
									equals;
									
	#if PAPAGENO_PRINT_SELF_ENABLED
	PPG_Token_Print_Self_Fun
									print_self;
	#endif								
} PPG_Token_Vtable;

#define PPG_CALL_VIRT_METHOD(THIS, METHOD, ...) \
	THIS->vtable->METHOD(THIS, ##__VA_ARGS__);

typedef struct PPG_TokenStruct {

	PPG_Token_Vtable *vtable;
	
	struct PPG_TokenStruct *parent;
	
	struct PPG_TokenStruct **successors;
	
	PPG_Count n_allocated_successors;
	PPG_Count n_successors;
	
	PPG_Action action;
	
	PPG_Processing_State state;
	
	PPG_Layer layer;
	 
} PPG_Token__;

enum PPG_Processing_State {
	PPG_Token_In_Progress = 0,
	PPG_Token_Completed,
	PPG_Token_Invalid,
	PPG_Pattern_Completed
};

void ppg_token_free_successors(PPG_Token__ *a_This);

void ppg_token_store_action(PPG_Token__ *token, 
											  PPG_Action action);

void ppg_token_reset(	PPG_Token__ *token);

void ppg_token_reset_successors(PPG_Token__ *token);

bool ppg_token_trigger_action(PPG_Token__ *token, PPG_Slot_Id slot_id);

PPG_Processing_State ppg_token_match_event(	
												PPG_Token__ **current_token,
												PPG_Event *event,
												PPG_Layer cur_layer
 										);

PPG_Token__ *ppg_token_new(PPG_Token__ *token);

PPG_Token__* ppg_token_destroy(PPG_Token__ *token);

void ppg_token_free(PPG_Token__ *token);

void ppg_token_add_successor(PPG_Token__ *token, PPG_Token__ *successor);

PPG_Token__* ppg_token_get_equivalent_successor(
														PPG_Token__ *parent_token,
														PPG_Token__ *sample);

#endif
