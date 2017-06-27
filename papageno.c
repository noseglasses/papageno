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

#include "papageno.h"

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>

#define PPG_MAX_KEYCHANGES 100

#ifdef DEBUG_PAPAGENO
#include "debug.h"
#define PPG_PRINTF(...) \
	if(ppg_state.printf) {	\
		ppg_state.printf(__VA_ARGS__);	\
	}
#define PPG_ERROR(...) PPG_PRINTF("*** Error: ", ##__VA_ARGS__)
#else
#define PPG_PRINTF(...)
#define PPG_ERROR(...)
#endif

#define PPG_CALL_VIRT_METHOD(THIS, METHOD, ...) \
	THIS->vtable->METHOD(THIS, ##__VA_ARGS__);

static bool ppg_key_id_simple_equal(PPG_Key_Id kp1, PPG_Key_Id kp2)
{
	return 	kp1 == kp2;
}

struct PPG_PhraseStruct;

typedef uint8_t (*PPG_Phrase_Consider_Keychange_Fun)(	
														struct PPG_PhraseStruct *a_This, 
														PPG_Key_Event *key_event,
														uint8_t cur_layer
																	);

typedef uint8_t (*PPG_Phrase_Successor_Consider_Keychange_Fun)(
														struct PPG_PhraseStruct *a_This, 
														PPG_Key_Event *key_event
 													);

typedef void (*PPG_Phrase_Reset_Fun)(	struct PPG_PhraseStruct *a_This);

typedef bool (*PPG_Phrase_Trigger_Action_Fun)(	struct PPG_PhraseStruct *a_This, uint8_t slot_id);

typedef struct PPG_PhraseStruct * (*PPG_Phrase_Destroy_Fun)(struct PPG_PhraseStruct *a_This);

typedef bool (*PPG_Phrase_Equals_Fun)(struct PPG_PhraseStruct *p1, struct PPG_PhraseStruct *p2);

typedef void (*PPG_Phrase_Print_Self_Fun)(struct PPG_PhraseStruct *p);

typedef struct {
	
	PPG_Phrase_Consider_Keychange_Fun 
									consider_key_event;
									
	PPG_Phrase_Successor_Consider_Keychange_Fun 
									successor_consider_key_event;
									
	PPG_Phrase_Reset_Fun
									reset;
									
	PPG_Phrase_Trigger_Action_Fun
									trigger_action;
									
	PPG_Phrase_Destroy_Fun	
									destroy;
			
	PPG_Phrase_Equals_Fun
									equals;
									
	PPG_Phrase_Print_Self_Fun
									print_self;
									
} PPG_Phrase_Vtable;

typedef struct PPG_PhraseStruct {

	PPG_Phrase_Vtable *vtable;
	
	struct PPG_PhraseStruct *parent;
	
	struct PPG_PhraseStruct **successors;
	
	uint8_t n_allocated_successors;
	uint8_t n_successors;
	
	PPG_Action action;
	
	uint8_t state;
	
	uint8_t layer;
	 
} PPG_Phrase__;

typedef struct
{
	bool initialized;
	uint16_t n_key_events;
	PPG_Key_Event key_events[PPG_MAX_KEYCHANGES];

	PPG_Phrase__ melody_root;
	bool melody_root_initialized;
	bool process_actions_if_aborted;

	PPG_Phrase__ *current_phrase;

	bool papageno_enabled;
	bool papageno_temporarily_disabled;

	PPG_Key_Id abort_key_id;

	PPG_Time time_last_keypress;

	PPG_Time keypress_timeout;
	
	PPG_Key_Id_Equal_Fun key_id_equal;
	
	PPG_Key_Event_Processor_Fun key_processor;
	
	PPG_Time_Fun time;
	PPG_Time_Difference_Fun time_difference;
	PPG_Time_Comparison_Fun time_comparison;
	
	#ifdef DEBUG_PAPAGENO
	PPG_Printf_Fun printf;
	#endif	
  
} PPG_Magic_Melody_State;

static void ppg_default_time(PPG_Time *time)
{
	*time = 0;
}

static void ppg_default_time_difference(PPG_Time time1, PPG_Time time2, PPG_Time *delta)
{
	*delta = 0;
}

int8_t ppg_default_time_comparison(
								PPG_Time time1,
								PPG_Time time2)
{
	return 0;
}

static void ppg_initialize_state() {
	
	ppg_state.initialized = true;
	ppg_state.n_key_events = 0;
	ppg_state.melody_root_initialized = false;
	ppg_state.process_actions_if_aborted = false;
	ppg_state.current_phrase = NULL;
	ppg_state.papageno_enabled = true;
	ppg_state.papageno_temporarily_disabled = false;
	ppg_state.abort_key_id = (PPG_Key_Id)((int16_t)-1);
	ppg_state.keypress_timeout = NULL;
	ppg_state.key_id_equal = (PPG_Key_Id_Equal_Fun)ppg_key_id_simple_equal;
	ppg_state.time = ppg_default_time;
	ppg_state.time_difference = ppg_default_time_difference;
	ppg_state.time_comparison = ppg_default_time_comparison;
	ppg_state.printf = NULL;
};

static PPG_Magic_Melody_State ppg_state = { .initialized = false; }

enum {
	PPG_Phrase_In_Progress = 0,
	PPG_Phrase_Completed,
	PPG_Phrase_Invalid
};

static void ppg_store_key_event(PPG_Key_Event *key_event)
{
	assert(ppg_state.n_key_events < PPG_MAX_KEYCHANGES);
	
	ppg_state.key_events[ppg_state.n_key_events] = *key_event;
	
	++ppg_state.n_key_events;
}

static void ppg_phrase_store_action(PPG_Phrase__ *a_phrase, 
											  PPG_Action action)
{
	a_phrase->action = action; 
}

void ppg_flush_stored_key_events(
								uint8_t slot_id, 
								PPG_Key_Event_Processor_Fun key_processor,
								void *user_data)
{
	if(ppg_state.n_key_events == 0) { return; }
	
	PPG_Key_Event_Processor_Fun kp	=
		(key_processor) ? key_processor : ppg_state.key_processor;
	
	if(!kp) { return; }
	
	/* Make sure that no recursion is possible if the key processor 
	 * invokes ppg_process_key_event 
	 */
	ppg_state.papageno_temporarily_disabled = true;
       
// 	uint16_t cur_time = timer_read();
// 	
// 	uint16_t time_offset = cur_time - ppg_state.keytimes[ppg_state.n_key_events - 1];
	
	for(uint16_t i = 0; i < ppg_state.n_key_events; ++i) {
		
		if(!kp(&ppg_state.key_events[i], slot_id, user_data)) { 
			break;
		}
		
// 		record.event.time = ppg_state.keytimes[i] + time_offset;
// 		
// 		record.event.key.row = ppg_state.key_id[i].row;
// 		record.event.key.col = ppg_state.key_id[i].col;
// 		record.event.pressed = ppg_state.pressed[i];
// 		
// 		PPG_PRINTF("Issuing keystroke at %d, %d\n", record.event.key.row, record.event.key.col);
// 		
// 		process_record_quantum(&record);
	}
	
	ppg_state.papageno_temporarily_disabled = false;
	
	ppg_state.n_key_events = 0;
}

static void ppg_phrase_reset(	PPG_Phrase__ *a_This)
{
	a_This->state = PPG_Phrase_In_Progress;
}

/* Phrases have states. This method resets the states
 * after processing a melody. On our way through the
 * melody tree we only need to reset thoses phrases
 * that were candidates.
 */
static void ppg_phrase_reset_successors(PPG_Phrase__ *a_This)
{
	/* Reset all successor phrases if there are any
	 */
	for(uint8_t i = 0; i < a_This->n_successors; ++i) {
		PPG_CALL_VIRT_METHOD(a_This->successors[i], reset);
	}
}
	
static void ppg_abort_magic_melody(void)
{		
	if(!ppg_state.current_phrase) { return; }
	
	PPG_PRINTF("Aborting magic melody\n");
	
	/* The frase could not be parsed. Reset any successor phrases.
	*/
	ppg_phrase_reset_successors(ppg_state.current_phrase);
	
	if(pgg_state.process_actions_if_aborted) {
		ppg_recurse_and_process_actions(PPG_On_Abort);
	}
	
	/* Cleanup and issue all keypresses as if they happened without parsing a melody
	*/
	ppg_flush_stored_key_events(	PPG_On_Abort, 
										NULL /* key_processor */, 
										NULL /* user data */);
	
	ppg_state.current_phrase = NULL;
}

static bool ppg_phrase_trigger_action(PPG_Phrase__ *a_PG_Phrase, uint8_t slot_id) {
	
	PPG_PRINTF("*\n");
	
// 	switch(a_PG_Phrase->action.type) {
// 		case PPG_Action_Keycode:
// 	
// 			if(a_PG_Phrase->action.data.keycode != 0) {
// 				
// 				/* Construct a dummy record
// 				*/
// 				keyrecord_t record;
// 					record.event.key.row = 0;
// 					record.event.key.col = 0;
// 					record.event.pressed = true;
// 					record.event.time = timer_read();
// 					
// 				/* Use the quantum/tmk system to trigger the action
// 				 * thereby using a fictituous a key (0, 0) with which the action 
// 				 * keycode is associated. We pretend that the respective key
// 				 * was hit and released to make sure that any action that
// 				 * requires both events is correctly processed.
// 				 * Unfortunatelly this means that some actions that
// 				 * require keys to be held do not make sense, e.g.
// 				 * modifier keys or MO(...), etc.
// 				 */
// 				
// 				uint16_t configured_keycode = keycode_config(a_PG_Phrase->action.data.keycode);
// 				
// 				action_t action = action_for_configured_keycode(configured_keycode); 
// 			
// 				process_action(&record, action);
// 				
// 				record.event.pressed = false;
// 				record.event.time = timer_read();
// 				
// 				process_action(&record, action);
// 			}
// 			break;
	
	/* Actions of completed subphrases have already been triggered during 
	 * melody processing.
	 */
	if(	(slot_id != PPG_On_Subphrase_Completed)
		&&	(a_PG_Phrase->action.flags & PPG_Action_Immediate)) {
		return false;
	}
			
	if(a_PG_Phrase->action.user_callback.func) {
		a_PG_Phrase->action.user_callback.func(
			a_PG_Phrase->action.user_callback.user_data);
		
		return true;
	}
	
	return false;
}

/* Returns if an action has been triggered.
 */
static bool ppg_recurse_and_process_actions(uint8_t slot_id)
{			
	if(!ppg_state.current_phrase) { return false; }
	
	PPG_PRINTF("Triggering action of most recent phrase\n");
	
	PPG_Phrase__ *cur_phrase = ppg_state.current_phrase;
	
	while(cur_phrase) {
		
		bool action_present = ppg_phrase_trigger_action(cur_phrase, slot_id);
		
		if(action_present) {
			if(cur_phrase->action.flags &= PPG_Action_Fallthrough) {
				cur_phrase = cur_phrase->parent;
			}
			else {
				return true;
			}
		}
		else {
			if(cur_phrase->action.flags &= PPG_Action_Noop_Fallthrough) {
				cur_phrase = cur_phrase->parent;
			}
			else {
				return false;
			}
		}
	}
	
	return false;
}

static void ppg_on_timeout(void)
{
	if(!ppg_state.current_phrase) { return; }
	
	/* The frase could not be parsed. Reset any previous phrases.
	*/
	ppg_phrase_reset_successors(ppg_state.current_phrase);
	
	/* It timeout was hit, we either trigger the most recent action
	 * (e.g. necessary for tap dances) or flush the keyevents
	 * that happend until this point
	 */
	
	bool action_triggered 
		= ppg_recurse_and_process_actions(PPG_On_Timeout);
	
	/* Cleanup and issue all keypresses as if they happened without parsing a melody
	*/
	if(action_triggered) {
		ppg_flush_stored_key_events(	PPG_On_Timeout, 
											NULL /* key_processor */, 
											NULL /* user data */);
	}
	
	ppg_state.current_phrase = NULL;
}

static uint8_t ppg_phrase_consider_key_event(	
												PPG_Phrase__ **current_phrase,
												PPG_Key_Event *key_event,
												uint8_t cur_layer
 														) 
{
	/* Loop over all phrases and inform them about the 
	 * key_event 
	 */
	bool all_successors_invalid = true;
	
	PPG_Phrase__ *a_current_phrase = *current_phrase;
	
	ppg_store_key_event(key_event);
	
// 	PPG_PRINTF("Processing key\n");
	
	#if DEBUG_PAPAGENO
	if(key_event->pressed) {
		PPG_PRINTF("v");
	}
	else {
		PPG_PRINTF("^");
	}
	#endif
	
	bool any_phrase_completed = false;
		
	/* Pass the keypress to the phrases and let them decide it they 
	 * can use it. If a phrase cannot it becomes invalid and is not
	 * processed further on this node level. This speeds up processing.
	 */
	for(uint8_t i = 0; i < a_current_phrase->n_successors; ++i) {
		
		// PPG_CALL_VIRT_METHOD(a_current_phrase->successors[i], print_self);
		
		/* Accept only paths through the search tree whose
		 * nodes' cur_layer tags are lower or equal the current cur_layer
		 */
		if(a_current_phrase->successors[i]->layer > cur_layer) { continue; }
		
		if(a_current_phrase->successors[i]->state == PPG_Phrase_Invalid) {
			continue;
		}
		
		uint8_t successor_process_result 
			= a_current_phrase->successors[i]
					->vtable->successor_consider_key_event(	
								a_current_phrase->successors[i], 
								key_event
						);
								
		switch(successor_process_result) {
			
			case PPG_Phrase_In_Progress:
				all_successors_invalid = false;
				
				break;
				
			case PPG_Phrase_Completed:
				
				all_successors_invalid = false;
				any_phrase_completed = true;

				break;
			case PPG_Phrase_Invalid:
// 				PPG_PRINTF("Phrase invalid");
				break;
		}
	}
	
	/* If all successors are invalid, the keystroke chain does not
	 * match any defined melody.
	 */
	if(all_successors_invalid) {
					
		return PPG_Phrase_Invalid;
	}
	
	/* If any phrase completed we have to find the most suitable one
	 * and either terminate melody processing if the matching successor
	 * is a leaf node, or replace the current phrase to await further 
	 * keystrokes.
	 */
	else if(any_phrase_completed) {
		
		int8_t highest_layer = -1;
		int8_t match_id = -1;
		
		/* Find the most suitable phrase with respect to the current cur_layer.
		 */
		for(uint8_t i = 0; i < a_current_phrase->n_successors; ++i) {
		
// 			PPG_CALL_VIRT_METHOD(a_current_phrase->successors[i], print_self);
		
			/* Accept only paths through the search tree whose
			* nodes' cur_layer tags are lower or equal the current cur_layer
			*/
			if(a_current_phrase->successors[i]->layer > cur_layer) { continue; }
			
			if(a_current_phrase->successors[i]->state != PPG_Phrase_Completed) {
				continue;
			}
			
			if(a_current_phrase->successors[i]->layer > highest_layer) {
				highest_layer = a_current_phrase->successors[i]->layer;
				match_id = i;
			}
		}
		
		assert(match_id >= 0);
				
		/* Cleanup successors of the current node for further melody processing.
		 */
		ppg_phrase_reset_successors(a_current_phrase);
		
		if(a_current_phrase->action.flags & PPG_Action_Immediate) {
			ppg_phrase_trigger_action(a_current_phrase, PPG_On_Subphrase_Completed);
		}
		
		/* Replace the current phrase.
		*/
		*current_phrase = a_current_phrase->successors[match_id];
		a_current_phrase = *current_phrase;
			
		if(0 == a_current_phrase->n_successors) {
			
			/* The melody is completed. Trigger the action that is associated
			* with the leaf node.
			*/
			PPG_CALL_VIRT_METHOD(a_current_phrase, trigger_action);
			
			ppg_state.current_phrase = NULL;
			
			return PPG_Phrase_Completed;
		}
		else {
			
			/* The melody is still in progress. We continue with the 
			 * new current node as soon as the next keystroke happens.
			 */
			return PPG_Phrase_In_Progress;
		}
	}
	
	return PPG_Phrase_In_Progress;
}

static void ppg_phrase_allocate_successors(PPG_Phrase__ *a_This, uint8_t n_successors) {

	 a_This->successors 
		= (struct PPG_PhraseStruct **)malloc(n_successors*sizeof(struct PPG_PhraseStruct*));
	 a_This->n_allocated_successors = n_successors;
}

static void ppg_phrase_grow_successors(PPG_Phrase__ *a_This) {

	if(a_This->n_allocated_successors == 0) {
		
		ppg_phrase_allocate_successors(a_This, 1);
	}
	else {
		PPG_Phrase__ **oldSucessors = a_This->successors;
		
		ppg_phrase_allocate_successors(a_This, 2*a_This->n_allocated_successors);
			
		for(uint8_t i = 0; i < a_This->n_successors; ++i) {
			a_This->successors[i] = oldSucessors[i];
		}
		
		free(oldSucessors); 
	}
}

static void ppg_phrase_add_successor(PPG_Phrase__ *a_This, PPG_Phrase__ *successor) {
	
	if(a_This->n_allocated_successors == a_This->n_successors) {
		ppg_phrase_grow_successors(a_This);
	}
	
	a_This->successors[a_This->n_successors] = successor;
	
	successor->parent = a_This;
	
	++a_This->n_successors;
}

static void ppg_phrase_free(PPG_Phrase__ *a_This);

static void ppg_phrase_free_successors(PPG_Phrase__ *a_This)
{
	if(!a_This->successors) { return; }
	
	for(uint8_t i = 0; i < a_This->n_allocated_successors; ++i) {
		
		ppg_phrase_free(a_This->successors[i]);
	}
	
	free(a_This->successors);
	
	a_This->successors = NULL;
	a_This->n_allocated_successors = 0;
}

static PPG_Phrase__* ppg_phrase_destroy(PPG_Phrase__ *a_PG_Phrase) {

	ppg_phrase_free_successors(a_PG_Phrase);
	
	return a_PG_Phrase;
}

static bool ppg_phrase_equals(PPG_Phrase__ *p1, PPG_Phrase__ *p2) 
{
	if(p1->vtable != p2->vtable) { return false; }
	
	return p1->vtable->equals(p1, p2);
}

static void ppg_phrase_free(PPG_Phrase__ *a_This) {
	
	PPG_CALL_VIRT_METHOD(a_This, destroy);

	free(a_This);
}

static PPG_Phrase__* ppg_phrase_get_equivalent_successor(
														PPG_Phrase__ *a_PG_Phrase,
														PPG_Phrase__ *sample) {
	
	if(a_PG_Phrase->n_successors == 0) { return NULL; }
	
	for(uint8_t i = 0; i < a_PG_Phrase->n_successors; ++i) {
		if(ppg_phrase_equals(
											a_PG_Phrase->successors[i], 
											sample)
		  ) {
			return a_PG_Phrase->successors[i];
		}
	}
	
	return NULL;
}

static void ppg_phrase_print_self(PPG_Phrase__ *p)
{
	PPG_PRINTF("phrase (0x%" PRIXPTR ")\n", (void*)p);
	PPG_PRINTF("   parent: 0x%" PRIXPTR "\n", (void*)&p->parent);
	PPG_PRINTF("   successors: 0x%" PRIXPTR "\n", (void*)&p->successors);
	PPG_PRINTF("   n_allocated_successors: %d\n", p->n_allocated_successors);
	PPG_PRINTF("   n_successors: %d\n", p->n_successors);
	PPG_PRINTF("   action.flags: %d\n", p->action.flags);
	PPG_PRINTF("   action_user_func: %0x%" PRIXPTR "\n", p->action.user_callback.func);
	PPG_PRINTF("   action_user_data: %0x%" PRIXPTR "\n", p->action.user_callback.user_data);
	PPG_PRINTF("   state: %d\n", p->state);
	PPG_PRINTF("   layer: %d\n", p->layer);
}

static PPG_Phrase_Vtable ppg_phrase_vtable =
{
	.consider_key_event 
		= (PPG_Phrase_Consider_Keychange_Fun) ppg_phrase_consider_key_event,
	.successor_consider_key_event 
		= NULL,
	.reset 
		= (PPG_Phrase_Reset_Fun) ppg_phrase_reset,
	.trigger_action 
		= (PPG_Phrase_Trigger_Action_Fun) ppg_phrase_trigger_action,
	.destroy 
		= (PPG_Phrase_Destroy_Fun) ppg_phrase_destroy,
	.equals
		= NULL,
	.print_self
		= (PPG_Phrase_Print_Self_Fun) ppg_phrase_print_self,
};

static PPG_Phrase__ *ppg_phrase_new(PPG_Phrase__ *a_PG_Phrase) {
	
    a_PG_Phrase->vtable = &ppg_phrase_vtable;
	 
    a_PG_Phrase->parent = NULL;
    a_PG_Phrase->successors = NULL;
	 a_PG_Phrase->n_allocated_successors = 0;
    a_PG_Phrase->n_successors = 0;
    a_PG_Phrase->action.flags = PPG_Action_Undefined;
    a_PG_Phrase->action.user_callback.func = NULL;
    a_PG_Phrase->action.user_callback.user_data = NULL;
    a_PG_Phrase->state = PPG_Phrase_In_Progress;
    a_PG_Phrase->layer = 0;
	 
    return a_PG_Phrase;
}

/*****************************************************************************
 Notes 
*****************************************************************************/

typedef struct {
	
	PPG_Phrase__ phrase_inventory;
	 
	PPG_Key_Id key_id;
	bool pressed;
	 
} PPG_Note;

static uint8_t ppg_note_successor_consider_key_event(	
											PPG_Note *a_This,
											PPG_Key_Event *key_event) 
{	
	assert(a_This->phrase_inventory.state == PPG_Phrase_In_Progress);
	
	/* Set state appropriately 
	 */
	if(ppg_state.key_id_equal(a_This->key_id, key_event->key_id)) {
		
		if(key_event->pressed) {
			a_This->pressed = true;
			a_This->phrase_inventory.state = PPG_Phrase_In_Progress;
		}
		else {
			if(a_This->pressed) {
	// 		PPG_PRINTF("note successfully finished\n");
				PPG_PRINTF("N");
				a_This->phrase_inventory.state = PPG_Phrase_Completed;
			}
		}
	}
	else {
// 		PPG_PRINTF("note invalid\n");
		a_This->phrase_inventory.state = PPG_Phrase_Invalid;
	}
	
	return a_This->phrase_inventory.state;
}

static void ppg_note_reset(PPG_Note *a_This) 
{
	ppg_phrase_reset((PPG_Phrase__*)a_This);
	
	a_This->pressed = false;
}

static PPG_Note *ppg_note_alloc(void) {
    return (PPG_Note*)malloc(sizeof(PPG_Note));
}

static bool ppg_note_equals(PPG_Note *n1, PPG_Note *n2) 
{
	return ppg_state.key_id_equal(n1->key_id, n2->key_id);
}

static void ppg_note_print_self(PPG_Note *p)
{
	ppg_phrase_print_self((PPG_Phrase__*)p);
	
	PPG_PRINTF("note\n");
	PPG_PRINTF("   key_id: %d\n", p->key_id);
}

static PPG_Phrase_Vtable ppg_note_vtable =
{
	.consider_key_event 
		= (PPG_Phrase_Consider_Keychange_Fun) ppg_phrase_consider_key_event,
	.successor_consider_key_event 
		= (PPG_Phrase_Successor_Consider_Keychange_Fun) ppg_note_successor_consider_key_event,
	.reset 
		= (PPG_Phrase_Reset_Fun) ppg_note_reset,
	.trigger_action 
		= (PPG_Phrase_Trigger_Action_Fun) ppg_phrase_trigger_action,
	.destroy 
		= (PPG_Phrase_Destroy_Fun) ppg_phrase_destroy,
	.equals
		= (PPG_Phrase_Equals_Fun) ppg_note_equals,
	.print_self
		= (PPG_Phrase_Print_Self_Fun) ppg_note_print_self
};

static PPG_Note *ppg_note_new(PPG_Note *a_note)
{
    /* Explict call to parent constructor 
	  */
    ppg_phrase_new((PPG_Phrase__*)a_note);

    a_note->phrase_inventory.vtable = &ppg_note_vtable;
		
	 a_note->key_id = (PPG_Key_Id)((uint16_t)-1);
	 
	 a_note->pressed = false;
	 
    return a_note;
}

/*****************************************************************************
 Chords 
*****************************************************************************/

typedef struct {
	
	PPG_Phrase__ phrase_inventory;
	 
	uint8_t n_members;
	PPG_Key_Id *key_id;
	bool *member_active;
	uint8_t n_chord_keys_pressed;
	 
} PPG_Chord;

static uint8_t ppg_chord_successor_consider_key_event(	
											PPG_Chord *a_This,
											PPG_Key_Event *key_event) 
{
	bool key_part_of_chord = false;
	
	assert(a_This->n_members != 0);
	
	assert(a_This->phrase_inventory.state == PPG_Phrase_In_Progress);
	
	/* Check it the key is part of the current chord 
	 */
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		
		if(ppg_state.key_id_equal(a_This->key_id[i], key_event->key_id)) {
			
			key_part_of_chord = true;
			
			if(key_event->pressed) {
				if(!a_This->member_active[i]) {
					a_This->member_active[i] = true;
					++a_This->n_chord_keys_pressed;
				}
			}
			else {
				if(a_This->member_active[i]) {
					a_This->member_active[i] = false;
					--a_This->n_chord_keys_pressed;
				}
			}
			break;
		}
	}
	
	if(!key_part_of_chord) {
		if(key_event->pressed) {
			a_This->phrase_inventory.state = PPG_Phrase_Invalid;
			return a_This->phrase_inventory.state;
		}
	}
	
	if(a_This->n_chord_keys_pressed == a_This->n_members) {
		
		/* Chord completed
		 */
		a_This->phrase_inventory.state = PPG_Phrase_Completed;
 		PPG_PRINTF("C");
	}
	
	return a_This->phrase_inventory.state;
}

static void ppg_chord_reset(PPG_Chord *a_This) 
{
	ppg_phrase_reset((PPG_Phrase__*)a_This);
	
	a_This->n_chord_keys_pressed = 0;
	
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static void ppg_chord_deallocate_member_storage(PPG_Chord *a_This) {	
	
	if(a_This->key_id) {
		free(a_This->key_id);
		a_This->key_id = NULL;
	}
	if(a_This->member_active) {
		free(a_This->member_active);
		a_This->member_active = NULL;
	}
}

static void ppg_chord_resize(PPG_Chord *a_This, 
							uint8_t n_members)
{
	ppg_chord_deallocate_member_storage(a_This);
	
	a_This->n_members = n_members;
	a_This->key_id = (PPG_Key_Id *)malloc(n_members*sizeof(PPG_Key_Id));
	a_This->member_active = (bool *)malloc(n_members*sizeof(bool));
	a_This->n_chord_keys_pressed = 0;
	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static PPG_Chord* ppg_chord_destroy(PPG_Chord *a_This) {
	
	ppg_chord_deallocate_member_storage(a_This);

	return a_This;
}

static PPG_Chord *ppg_chord_alloc(void){
    return (PPG_Chord*)malloc(sizeof(PPG_Chord));
}

static bool ppg_chord_equals(PPG_Chord *c1, PPG_Chord *c2) 
{
	if(c1->n_members != c2->n_members) { return false; }
	
	for(uint8_t i = 0; i < c1->n_members; ++i) {
		if(!ppg_state.key_id_equal(c1->key_id[i], c2->key_id[i])) { return false; }
	}
	
	return true;
}

static void ppg_chord_print_self(PPG_Chord *c)
{
	ppg_phrase_print_self((PPG_Phrase__*)c);
	
	PPG_PRINTF("chord\n");
	PPG_PRINTF("   n_members: %d\n", c->n_members);
	PPG_PRINTF("   n_chord_keys_pressed: %d\n", c->n_chord_keys_pressed);
	
	for(uint8_t i = 0; i < c->n_members; ++i) {
		PPG_PRINTF("      key_id: %d, active: %d\n", 
				  c->key_id[i], c->member_active[i]);
	}
}

static PPG_Phrase_Vtable ppg_chord_vtable =
{
	.consider_key_event 
		= (PPG_Phrase_Consider_Keychange_Fun) ppg_phrase_consider_key_event,
	.successor_consider_key_event 
		= (PPG_Phrase_Successor_Consider_Keychange_Fun) ppg_chord_successor_consider_key_event,
	.reset 
		= (PPG_Phrase_Reset_Fun) ppg_chord_reset,
	.trigger_action 
		= (PPG_Phrase_Trigger_Action_Fun) ppg_phrase_trigger_action,
	.destroy 
		= (PPG_Phrase_Destroy_Fun) ppg_chord_destroy,
	.equals
		= (PPG_Phrase_Equals_Fun) ppg_chord_equals,
	.print_self
		= (PPG_Phrase_Print_Self_Fun) ppg_chord_print_self
};

static PPG_Chord *ppg_chord_new(PPG_Chord *a_chord){
    
	/* Explict call to parent constructor
	 */
	ppg_phrase_new((PPG_Phrase__*)a_chord);

	a_chord->phrase_inventory.vtable = &ppg_chord_vtable;
		
	/* Initialize the chord
	 */
	a_chord->n_members = 0;
	a_chord->key_id = NULL;
	a_chord->member_active = NULL;
	a_chord->n_chord_keys_pressed = 0;

	return a_chord;
}

/*****************************************************************************
 Clusters 
*****************************************************************************/

typedef struct {
	
	PPG_Phrase__ phrase_inventory;
	 
	uint8_t n_members;
	PPG_Key_Id *key_id;
	bool *member_active;
	uint8_t n_cluster_keys_pressed;
	 
} PPG_Cluster;

static uint8_t ppg_cluster_successor_consider_key_event(	
											PPG_Cluster *a_This,
											PPG_Key_Event *key_event) 
{
	bool key_part_of_cluster = false;
	
	assert(a_This->n_members != 0);
	
	/* Only react on pressed keys. Allow cluster members to be released. 
	 * Every key only must be pressed once.
	 */
	if(!key_event->pressed) { return PPG_Phrase_In_Progress; }
	
	assert(a_This->phrase_inventory.state == PPG_Phrase_In_Progress);
	
	/* Check it the key is part of the current chord 
	 */
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		
		if(ppg_state.key_id_equal(a_This->key_id[i], key_event->key_id)) {
			
			key_part_of_cluster = true;
			
			if(key_event->pressed) {
				if(!a_This->member_active[i]) {
					a_This->member_active[i] = true;
					++a_This->n_cluster_keys_pressed;
				}
			}
			/* Note: We do not care for released keys here. Every cluster member must be pressed only once
			 */

			break;
		}
	}
	
	if(!key_part_of_cluster) {
		if(key_event->pressed) {
			a_This->phrase_inventory.state = PPG_Phrase_Invalid;
			return a_This->phrase_inventory.state;
		}
	}
	
	if(a_This->n_cluster_keys_pressed == a_This->n_members) {
		
		/* Cluster completed
		 */
		a_This->phrase_inventory.state = PPG_Phrase_Completed;
 		PPG_PRINTF("O");
	}
	
	return a_This->phrase_inventory.state;
}

static void ppg_cluster_reset(PPG_Cluster *a_This) 
{
	ppg_phrase_reset((PPG_Phrase__*)a_This);
	
	a_This->n_cluster_keys_pressed = 0;
	
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static void ppg_cluster_deallocate_member_storage(PPG_Cluster *a_This) {	
	
	if(a_This->key_id) {
		free(a_This->key_id);
		a_This->key_id = NULL;
	}
	if(a_This->member_active) {
		free(a_This->member_active);
		a_This->member_active = NULL;
	}
}

static void ppg_cluster_resize(PPG_Cluster *a_This, 
							uint8_t n_members)
{
	ppg_cluster_deallocate_member_storage(a_This);
	
	a_This->n_members = n_members;
	a_This->key_id = (PPG_Key_Id *)malloc(n_members*sizeof(PPG_Key_Id));
	a_This->member_active = (bool *)malloc(n_members*sizeof(bool));
	a_This->n_cluster_keys_pressed = 0;
	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static bool ppg_is_cluster_member(PPG_Cluster *c, PPG_Key_Id key)
{
	for(uint8_t i = 0; i < c->n_members; ++i) {
		if(ppg_state.key_id_equal(c->key_id[i], key)) {
			return true;
		}
	}
	
	return false;
}

static bool ppg_cluster_equals(PPG_Cluster *c1, PPG_Cluster *c2) 
{
	if(c1->n_members != c2->n_members) { return false; }
	
	for(uint8_t i = 0; i < c1->n_members; ++i) {
		if(!ppg_is_cluster_member(c1, c2->key_id[i])) { return false; }
	}
	
	return true;
}

static PPG_Cluster* ppg_cluster_destroy(PPG_Cluster *a_This) {
	
	ppg_cluster_deallocate_member_storage(a_This);

    return a_This;
}

static PPG_Cluster *ppg_cluster_alloc(void){
    return (PPG_Cluster*)malloc(sizeof(PPG_Cluster));
} 

static void ppg_cluster_print_self(PPG_Cluster *c)
{
	ppg_phrase_print_self((PPG_Phrase__*)c);
	
	PPG_PRINTF("cluster\n");
	PPG_PRINTF("   n_members: %d\n", c->n_members);
	PPG_PRINTF("   n_cluster_keys_pressed: %d\n", c->n_cluster_keys_pressed);
	
	for(uint8_t i = 0; i < c->n_members; ++i) {
		PPG_PRINTF("      key_id: %d, active: %d\n", 
				  c->key_id[i], c->member_active[i]);
	}
}

static PPG_Phrase_Vtable ppg_cluster_vtable =
{
	.consider_key_event 
		= (PPG_Phrase_Consider_Keychange_Fun) ppg_phrase_consider_key_event,
	.successor_consider_key_event 
		= (PPG_Phrase_Successor_Consider_Keychange_Fun) ppg_cluster_successor_consider_key_event,
	.reset 
		= (PPG_Phrase_Reset_Fun) ppg_cluster_reset,
	.trigger_action 
		= (PPG_Phrase_Trigger_Action_Fun) ppg_phrase_trigger_action,
	.destroy 
		= (PPG_Phrase_Destroy_Fun) ppg_cluster_destroy,
	.equals
		= (PPG_Phrase_Equals_Fun) ppg_cluster_equals,
	.print_self
		= (PPG_Phrase_Print_Self_Fun) ppg_cluster_print_self
};

static PPG_Cluster *ppg_cluster_new(PPG_Cluster *a_cluster) {
	
	/*Explict call to parent constructor
	*/
	ppg_phrase_new((PPG_Phrase__*)a_cluster);

	a_cluster->phrase_inventory.vtable = &ppg_cluster_vtable;
	
	a_cluster->n_members = 0;
	a_cluster->key_id = NULL;
	a_cluster->member_active = NULL;
	a_cluster->n_cluster_keys_pressed = 0;
	
	return a_cluster;
}

/*****************************************************************************
 * Public access functions for creation and destruction of the melody tree
 *****************************************************************************/

static void ppg_init(void) {

	if(!ppg_state.initialized) {
		ppg_initialize_state();
	}
	
	if(!ppg_state.melody_root_initialized) {
		
		/* Initialize the melody root
		 */
		ppg_phrase_new(&ppg_state.melody_root);
		
		ppg_state.melody_root_initialized = true;
	}
}

void ppg_finalize(void) {
	ppg_phrase_free_successors(&ppg_state.melody_root);
}

void ppg_reset(void)
{
	ppg_finalize();
	ppg_initialize_state();
}

PPG_Phrase ppg_create_note(PPG_Key_Id key_id)
{
    PPG_Note *a_note = (PPG_Note*)ppg_note_new(ppg_note_alloc());
	 
	 a_note->key_id = key_id;
	 
	 /* Return the new end of the melody */
	 return a_note;
}

PPG_Phrase ppg_create_chord(	PPG_Key_Id *key_id,
								uint8_t n_members)
{
	PPG_Chord *a_chord = (PPG_Chord*)ppg_chord_new(ppg_chord_alloc());
	 	 
	ppg_chord_resize(a_chord, n_members);
	 
	for(uint8_t i = 0; i < n_members; ++i) {
		a_chord->key_id[i] = key_id[i];
	}
	 
	/* Return the new end of the melody */
	return a_chord;
}

PPG_Phrase ppg_create_cluster(
									PPG_Key_Id *key_id,
									uint8_t n_members)
{
	PPG_Cluster *a_cluster = (PPG_Cluster*)ppg_cluster_new(ppg_cluster_alloc());
	 
	ppg_cluster_resize(a_cluster, n_members);
	 	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_cluster->key_id[i] = key_id[i];
	}
	
	/* Return the new end of the melody */
	return a_cluster;
}

#if DEBUG_PAPAGENO
static void ppg_recursively_print_melody(PPG_Phrase__ *p)
{
	if(	p->parent
		&& p->parent != &ppg_state.melody_root) {
		
		ppg_recursively_print_melody(p->parent);
	}
	
	PPG_CALL_VIRT_METHOD(p, print_self);
}
#endif

static PPG_Phrase ppg_melody_from_list(	uint8_t layer,
												PPG_Phrase__ **phrases,
												int n_phrases)
{ 
	PPG_Phrase__ *parent_phrase = &ppg_state.melody_root;
	
	PPG_PRINTF("   %d members\n", n_phrases);
	
	for (int i = 0; i < n_phrases; i++) { 
		
		PPG_Phrase__ *cur_phrase = phrases[i];
		
		/* If the action type is undefined, we set action 
		 * type none, which means that no fall through happens
		 * in case of timeout.
		 */
// 		if(cur_phrase->action.type == PPG_Action_Undefined) {
// 			cur_phrase->action.flags = PPG_Action_Undefined;
// 		}
		
		PPG_Phrase__ *equivalent_successor 
			= ppg_phrase_get_equivalent_successor(parent_phrase, cur_phrase);
			
		PPG_PRINTF("   member %d: ", i);
		
		if(	equivalent_successor
			
			/* Only share interior nodes and ...
			 */
			&& equivalent_successor->successors
			/* ... only if the 
			 * newly registered node on the respective level is 
			 * not a leaf node.
			 */ 
			&& (i < (n_phrases - 1))
		) {
			
// 			#if DEBUG_PAPAGENO
// 			if(cur_phrase->action.type != equivalent_successor->action.type) {
// 				PPG_ERROR("Incompatible action types detected\n");
// 			}
// 			#endif
			
			PPG_PRINTF("already present\n");
			
			parent_phrase = equivalent_successor;
			
			if(layer < equivalent_successor->layer) {
				
				equivalent_successor->layer = layer;
			}
			
			/* The successor is already registered in the search tree. Delete the newly created version.
			 */			
			ppg_phrase_free(cur_phrase);
		}
		else {
			
			PPG_PRINTF("newly defined\n");
			
			#if DEBUG_PAPAGENO
			
			/* Detect melody ambiguities
			 */
			if(equivalent_successor) {
				if(	
					/* Melodies are ambiguous if ...
					 * the conflicting nodes/phrases are both leaf phrases
					 */
						(i == (n_phrases - 1))
					&&	!equivalent_successor->successors
					
					/* And defined for the same layer
					 */
					&& (equivalent_successor->layer == layer)
				) {
					PPG_ERROR("Conflicting melodies detected. "
						"The phrases of the conflicting melodies are listed below.\n");
					
					PPG_ERROR("Previously defined:\n");
					ppg_recursively_print_melody(equivalent_successor);
					
					PPG_ERROR("Conflicting:\n");
					for (int i = 0; i < n_phrases; i++) {
						PPG_CALL_VIRT_METHOD(phrases[i], print_self);
					}
				}
			}
			#endif /* if DEBUG_PAPAGENO */
					
			cur_phrase->layer = layer;
			
			ppg_phrase_add_successor(parent_phrase, cur_phrase);
			
			parent_phrase = cur_phrase;
		}
	}
	
	/* Return the leaf phrase 
	 */
	return parent_phrase;
}

PPG_Phrase ppg_melody(		uint8_t layer, 
							int count, 
							...)
{ 
	PPG_PRINTF("Adding magic melody\n");
	
	ppg_init();
	
	va_list ap;

	va_start (ap, count);         /* Initialize the argument list. */
  	
	PPG_Phrase__ **phrases 
		= (PPG_Phrase__ **)malloc(count*sizeof(PPG_Phrase__ *));
	
	for (int i = 0; i < count; i++) { 
		
		phrases[i] = va_arg (ap, PPG_Phrase__ *);
	}
	
	PPG_Phrase__ *leafPhrase 
		= ppg_melody_from_list(layer, phrases, count);
	
	free(phrases);

	va_end (ap);                  /* Clean up. */
	
	return leafPhrase;
}

PPG_Phrase ppg_chord(		uint8_t layer, 
							PPG_Action action, 
							PPG_Key_Id *key_ids,
							uint8_t n_members)
{   	
	PPG_PRINTF("Adding chord\n");
	
	PPG_Phrase__ *cPhrase = 
		(PPG_Phrase__ *)ppg_create_chord(key_ids, n_members);
		
	cPhrase->action = action;
		
	PPG_Phrase__ *phrases[1] = { cPhrase };
	
	PPG_Phrase__ *leafPhrase 
		= ppg_melody_from_list(layer, phrases, 1);
		
	return leafPhrase;
}

PPG_Phrase ppg_cluster(		uint8_t layer, 
							PPG_Action action, 
							PPG_Key_Id *key_ids,
							uint8_t n_members)
{   	
	PPG_PRINTF("Adding cluster\n");
	
	PPG_Phrase__ *cPhrase = 
		(PPG_Phrase__ *)ppg_create_cluster(key_ids, n_members);
		
	cPhrase->action = action;
	
	PPG_Phrase__ *phrases[1] = { cPhrase };
	
	PPG_Phrase__ *leafPhrase 
		= ppg_melody_from_list(layer, phrases, 1);
		
	return leafPhrase;
}

PPG_Phrase ppg_single_note_line(	
							uint8_t layer,
							PPG_Action action, 
							int count, 
							...)
{
	PPG_PRINTF("Adding single note line\n");
	
	ppg_init();
	
	va_list ap;

	va_start (ap, count);         /* Initialize the argument list. */
  
	PPG_Phrase__ **phrases 
		= (PPG_Phrase__ **)malloc(count*sizeof(PPG_Phrase__ *));
		
	for (int i = 0; i < count; i++) {
		
		PPG_Key_Id curKeypos = va_arg (ap, PPG_Key_Id); 

		PPG_Phrase new_note = ppg_create_note(curKeypos);
		
		phrases[i] = (PPG_Phrase__ *)new_note;
	}
	
	ppg_phrase_store_action(phrases[count - 1], action);
	
	PPG_Phrase__ *leaf_phrase 
		= ppg_melody_from_list(layer, phrases, count);
	
	free(phrases);

	va_end (ap);                  /* Clean up. */
  
	return leaf_phrase;
}

PPG_Phrase ppg_tap_dance(	uint8_t layer,
							PPG_Key_Id key_id,
							uint8_t default_action_flags,
							uint8_t n_tap_definition_varargs,
							...
							)
{
	PPG_PRINTF("Adding tap dance\n");
	
	ppg_init();
	
	va_list ap;

	va_start (ap, n_tap_definition_varargs);         /* Initialize the argument list. */
	
	uint8_t n_tap_definitions = n_tap_definition_varargs/2;
		
	int n_taps = 0;
	for (uint8_t i = 0; i < n_tap_definitions; i++) {
		
		int tap_count = va_arg (ap, int); 
		va_arg (ap, PPG_Action); /* not needed here */
		
		if(tap_count > n_taps) {
			n_taps = tap_count;
		}
	}
	
	if(n_taps == 0) { return NULL; }
	
	PPG_Phrase__ **phrases 
		= (PPG_Phrase__ **)malloc(n_taps*sizeof(PPG_Phrase__ *));
	
	for (int i = 0; i < n_taps; i++) {
		
		PPG_Phrase new_note = ppg_create_note(key_id);
		
		phrases[i] = (PPG_Phrase__ *)new_note;
		
		phrases[i]->action.flags = default_action_flags;
	}
	
	va_start (ap, n_tap_definition_varargs);         /* Initialize the argument list. */
	
	for (uint8_t i = 0; i < n_tap_definitions; i++) {
		
		int tap_count = va_arg (ap, int); 
		PPG_Action action = va_arg (ap, PPG_Action);

		ppg_phrase_store_action(phrases[tap_count - 1], action);
	}
			
	PPG_Phrase__ *leafPhrase 
		= ppg_melody_from_list(layer, phrases, n_taps);
	
	free(phrases);
	
	return leafPhrase;
}

/* Returns phrase__
 */
PPG_Phrase ppg_phrase_set_action(	
							PPG_Phrase phrase__,
							PPG_Action action)
{
	PPG_Phrase__ *phrase = (PPG_Phrase__ *)phrase__;
	
	ppg_phrase_store_action(phrase, action);
	
	return phrase__;
}

PPG_Action ppg_phrase_get_action(PPG_Phrase phrase__)
{
	PPG_Phrase__ *phrase = (PPG_Phrase__ *)phrase__;
	
	return phrase->action;
}

PPG_Phrase ppg_phrase_set_action_flags(
									PPG_Phrase phrase,
									uint8_t action_flags)
{
	PPG_Phrase__ *phrase = (PPG_Phrase__ *)phrase__;
	
	phrase->action.flags = action_flags;
}

uint8_t ppg_phrase_get_action_flags(PPG_Phrase phrase)
{
	PPG_Phrase__ *phrase = (PPG_Phrase__ *)phrase__;
	
	return action_flags;
}

bool ppg_check_timeout(void)
{
	PPG_Time cur_time;
	
	ppg_state.time(&cur_time);
	
	PPG_Time delta;
	ppg_state.time_difference(
					ppg_state.time_last_keypress, 
					cur_time, 
					&delta);
	
	if(ppg_state.current_phrase
		&& (ppg_state.time_comparison(
					delta,
					ppg_state.keypress_timeout
			) > 0)
	  ) {
		
		PPG_PRINTF("Magic melody timeout hit\n");
	
		/* Too late...
			*/
		ppg_on_timeout();
	
		return true;
	}
	
	return false;
}

bool ppg_process_key_event(PPG_Key_Event *key_event,
								  uint8_t cur_layer)
{ 
	if(!ppg_state.papageno_enabled) {
		return true;
	}
	
	/* When a melody could not be finished, all keystrokes are
	 * processed through the process_record_quantum method.
	 * To prevent infinite recursion, we have to temporarily disable 
	 * processing magic melodies.
	 */
	if(ppg_state.papageno_temporarily_disabled) { return true; }
	
	/* Early exit if no melody was registered 
	 */
	if(!ppg_state.melody_root_initialized) { return true; }
			
	/* If there is no melody processed, we ignore keyups.
	*/	
	if(!ppg_state.current_phrase && !key_event->pressed) {
// 		PPG_PRINTF("Keyup ignored as no melody currently processed\n");
		return true;
	}
	
	/* Check if the melody is being aborted
	 */
	if(ppg_state.key_id_equal(ppg_state.abort_key_id, key_event->key_id)) {
		
		/* If a melody is in progress, we abort it and consume the abort key.
		 */
		if(ppg_state.current_phrase) {
			PPG_PRINTF("Processing melodies interrupted by user\n");
			ppg_abort_magic_melody();
			return false;
		}
	
		return false;
	}
	
	PPG_PRINTF("Starting keyprocessing\n");
	
	if(!ppg_state.current_phrase) {
		
// 		PPG_PRINTF("New melody \n");
		
		/* Start of melody processing
		 */
		ppg_state.n_key_events = 0;
		ppg_state.current_phrase = &ppg_state.melody_root;
		
		ppg_state.time(ppg_state.time_last_keypress);
	}
	else {
		
		if(ppg_check_timeout()) {
			
			/* Timeout hit. Cleanup already done.
			 */
			return false;
		}
		else {
			ppg_state.time(ppg_state.time_last_keypress);
		}
	}
	
	uint8_t result 
		= ppg_phrase_consider_key_event(	
										&ppg_state.current_phrase,
										key_event,
										cur_layer
								);
		
	switch(result) {
		case PPG_Phrase_In_Progress:
		case PPG_Phrase_Completed:
			ppg_flush_stored_key_events(	PPG_On_Melody_Completed, 
												NULL /* key_processor */, 
												NULL /* user data */);
											  
			return false;
			
		case PPG_Phrase_Invalid:
			
			PPG_PRINTF("-\n");
		
			ppg_abort_magic_melody();
			
// 			return false; /* The key(s) have been already processed */
			return true; // Why does this require true to work and 
			// why is t not written?
	}
	
	return true;
}

/* Use this function to define a key_id that always aborts a magic melody
 */
PPG_Key_Id ppg_set_abort_key_id(PPG_Key_Id key_id)
{
	PPG_Key_Id old_key_id = ppg_state.abort_key_id;
	
	ppg_state.abort_key_id = key_id;
	
	return old_key_id;
}

PPG_Key_Id ppg_get_abort_key_id(void)
{
	return ppg_state.abort_key_id;
}

bool pgg_set_process_actions_if_aborted(bool state)
{
	bool old_value = pgg.process_actions_if_aborted;
	
	pgg.process_actions_if_aborted = state;
	
	return old_value;
}

bool pgg_get_process_actions_if_aborted(void)
{
	return pgg.process_actions_if_aborted;
}

PPG_Time ppg_set_timeout(PPG_Time timeout)
{
	PPG_Time old_value = ppg_state.keypress_timeout;
	
	ppg_state.keypress_timeout = timeout;
	
	return old_value;
}

PPG_Time ppg_get_timeout(void)
{
	return ppg_state.keypress_timeout;
}

void ppg_set_key_id_equal_function(PPG_Key_Id_Equal_Fun fun)
{
	ppg_state.key_id_equal = fun;
}

PPG_Key_Event_Processor_Fun ppg_set_default_key_processor(PPG_Key_Event_Processor_Fun key_processor)
{
	PPG_Key_Event_Processor_Fun old_key_processor = ppg_state.key_processor;
	
	ppg_state.key_processor = key_processor;
	
	return old_key_processor;
}

bool ppg_set_enabled(bool state)
{
	bool old_state = ppg_state.papageno_enabled;
	
	ppg_state.papageno_enabled = state;
	
	return old_state;
}

PPG_Time_Fun ppg_set_time_function(PPG_Time_Fun fun)
{
	PPG_Time_Fun old_fun = fun;
	
	ppg_state.time = fun;
	
	return old_fun;
}

PPG_Time_Difference_Fun ppg_set_time_difference_function(PPG_Time_Difference_Fun fun)
{
	PPG_Time_Difference_Fun old_fun = ppg_state.time_difference;
	
	ppg_state.time_difference = fun;
	
	return PPG_Time_Difference_Fun;
}

PPG_Time_Comparison_Fun ppg_set_time_comparison_function(PPG_Time_Comparison_Fun fun)
{
	PPG_Time_Comparison_Fun old_fun = ppg_state.time_comparison;
	
	ppg_state.time_comparison = fun;
	
	return old_fun;
}

#ifdef DEBUG_PAPAGENO
void ppg_set_printf(PPG_Printf_Fun printf_fun)
{
	ppg_state.printf = printf_fun;
}
#endif