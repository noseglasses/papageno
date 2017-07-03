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


struct PPG_TokenStruct;

typedef uint8_t (*PPG_Token_Consider_Inputchange_Fun)(	
														struct PPG_TokenStruct *a_This, 
														PPG_Event *event,
														uint8_t cur_layer
																	);

typedef uint8_t (*PPG_Token_Successor_Consider_Inputchange_Fun)(
														struct PPG_TokenStruct *a_This, 
														PPG_Event *event
 													);

typedef void (*PPG_Token_Reset_Fun)(	struct PPG_TokenStruct *a_This);

typedef bool (*PPG_Token_Trigger_Action_Fun)(	struct PPG_TokenStruct *a_This, uint8_t slot_id);

typedef struct PPG_TokenStruct * (*PPG_Token_Destroy_Fun)(struct PPG_TokenStruct *a_This);

typedef bool (*PPG_Token_Equals_Fun)(struct PPG_TokenStruct *p1, struct PPG_TokenStruct *p2);

#if PAPAGENO_PRINT_SELF_ENABLED
typedef void (*PPG_Token_Print_Self_Fun)(struct PPG_TokenStruct *p);
#endif

typedef struct {
									
	PPG_Token_Successor_Consider_Inputchange_Fun 
									successor_consider_event;
									
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

typedef struct PPG_TokenStruct {

	PPG_Token_Vtable *vtable;
	
	struct PPG_TokenStruct *parent;
	
	struct PPG_TokenStruct **successors;
	
	uint8_t n_allocated_successors;
	uint8_t n_successors;
	
	PPG_Action action;
	
	uint8_t state;
	
	uint8_t layer;
	 
} PPG_Token__;

enum {
	PPG_Token_In_Progress = 0,
	PPG_Token_Completed,
	PPG_Token_Invalid,
	PPG_Pattern_Completed
};

static void ppg_token_free_successors(PPG_Token__ *a_This);

void ppg_token_store_action(PPG_Token__ *token, 
											  PPG_Action action);

void ppg_token_reset(	PPG_Token__ *token);

void ppg_token_reset_successors(PPG_Token__ *token);

bool ppg_token_trigger_action(PPG_Token__ *token, uint8_t slot_id);

uint8_t ppg_token_consider_event(	
												PPG_Token__ **current_token,
												PPG_Event *event,
												uint8_t cur_layer
 										);
