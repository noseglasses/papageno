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

/* Some words about the code below:
 * 
 * Object oriented C is used to implement a polymorphic phrase class hierarchy.
 * Phrases that were initially implemented are notes, chords and clusters.
 * The phrase-family is extensible, so new types of phrases are easy to implement
 * as polymorphic classes.
 * 
 * All this would have been easier and more concise in C++ but as qmk seems
 * to be mostly written in C, we adhere to this choice of language, although it
 * is hard to live without type safety and all those neat C++ features as templates...
 * 
 * The idea of magic melodies is inspired by magic musical instruments from
 * phantasy fiction. Imagine a magic piano that does crazy things when certain
 * melodies are played. Melodies can consist of single notes, chords and note clusters
 * that must be played it a well defined order for the magic to happen.
 * Clusters are sets of notes that can be played in arbitrary order. It is only necessary
 * that every cluster member must have been played at least once for the cluster-phrase
 * to be accepted.
 * Going over to computer keyboards means that there is no music at all.
 * However, the basic concepts are transferable. A key on the piano 
 * here is a key on the keyboard. So a melody can be a combination of keystrokes.
 * 
 * Actions such as tmk/qmk keycodes can be associated with magic melodies as well as
 * user function calls that are fed with user data.
 * 
 * The implementation allows for simple definition of single note lines, isolated chords
 * or clusters as well as complex melodies that may consist of arbitrary combinations of
 * the afforementioned types of phrases.
 * 
 * A melody is generally associated with a layer and melody recognition works 
 * basically the same as layer selection. Only melodies that are associated with 
 * the current layer or those below are available. This also means that.
 * The same melody can be associated with a different action on a higher layer and
 * melodies can also be overriden with noops on higher layers.
 * 
 * To allow all this to work, magic melodies are independent of keymaps and implemented
 * on top of them. Because of this, phrases consist of keystroke sequences instead of
 * keycode sequences. Every member of a phrase is defined according to the 
 * row/column index of the key in the keyboard matrix.
 * 
 * If a melody completes, the associated action is triggered and
 * all keystrokes that happend to this point are consumed, i.e. not passed through
 * the keymap system. Only if a set of keystrokes does not match any defined melody, 
 * all keystrokes (keydowns/keyups) are passed in the exact order to the keymap processing
 * system as if they just happend, thereby also conserving the temporal order and the
 * time interval between key events.
 * 
 * The implementation is based on a search tree. Tree nodes represent 
 * phrases, i.e. notes, chords or clusters. Every newly defined melody is
 * integrated into the search tree.
 * Once a keystroke happens, the tree search tries to determine whether
 * the key is associated with any melody by determining a matching phrase on the 
 * current level of the search tree. Therefore, keystrokes are
 * passed to the sucessors of the current phrase/tree node to let the 
 * dedicated phrase implementation decide if the key is part of their definition. 
 * Successor phrases signal completion or invalid state. The latter happens 
 * as soon as a keystroke happens that is not part of the respective successor phrase.
 * If one or more suitable successor phrases complete, the most
 * suitable one is selected with respect to the current layer 
 * and replaces the current phrase. I may also happen that no successor phrase 
 * signals completion or invalidation e.g. if all successors are clusters or chords
 * that are not yet copleted.
 * If the most suitable successor that just copleted is a leaf node of the search tree, 
 * the current magic melody is considered as completed and the action 
 * that is associated with the melody is triggered. 
 */

#include "papageno.h"

#include <assert.h>
#include <inttypes.h>

#define PG_MAX_KEYCHANGES 100

#define PG_DEFAULT_KEYPRESS_TIMEOUT 200

#ifdef DEBUG_PAPAGENO
#include "debug.h"
#define PG_PRINTF(...) \
	if(pg_state.printf) {	\
		uprintf(__VA_ARGS__);	\
	}
#define PG_ERROR(...) PG_PRINTF("*** Error: " __VA_ARGS__)
#else
#define PG_PRINTF(...)
#define PG_ERROR(...)
#endif

#define PG_CALL_VIRT_METHOD(THIS, METHOD, ...) \
	THIS->vtable->METHOD(THIS, ##__VA_ARGS__);

static bool pg_key_id_simple_equal(PG_Key_Id kp1, PG_Key_Id kp2)
{
	return 	kp1 == kp2;
}

struct PG_PhraseStruct;

typedef uint8_t (*PG_Phrase_Consider_Keychange_Fun)(	
														struct PG_PhraseStruct *a_This, 
														PG_Key_Event *key_event);

typedef uint8_t (*PG_Phrase_Successor_Consider_Keychange_Fun)(
														struct PG_PhraseStruct *a_This, 
														PG_Key_Event *key_event);

typedef void (*PG_Phrase_Reset_Fun)(	struct PG_PhraseStruct *a_This);

typedef void (*PG_Phrase_Trigger_Action_Fun)(	struct PG_PhraseStruct *a_This);

typedef struct PG_PhraseStruct * (*PG_Phrase_Destroy_Fun)(struct PG_PhraseStruct *a_This);

typedef bool (*PG_Phrase_Equals_Fun)(struct PG_PhraseStruct *p1, struct PG_PhraseStruct *p2);

typedef void (*PG_Phrase_Print_Self_Fun)(struct PG_PhraseStruct *p);

typedef struct {
	
	PG_Phrase_Consider_Keychange_Fun 
									consider_key_event;
									
	PG_Phrase_Successor_Consider_Keychange_Fun 
									successor_consider_key_event;
									
	PG_Phrase_Reset_Fun
									reset;
									
	PG_Phrase_Trigger_Action_Fun
									trigger_action;
									
	PG_Phrase_Destroy_Fun	
									destroy;
			
	PG_Phrase_Equals_Fun
									equals;
									
	PG_Phrase_Print_Self_Fun
									print_self;
									
} PG_Phrase_Vtable;

typedef struct PG_PhraseStruct {

	PG_Phrase_Vtable *vtable;
	
	struct PG_PhraseStruct *parent;
	
	struct PG_PhraseStruct **successors;
	
	uint8_t n_allocated_successors;
	uint8_t n_successors;
	
	PG_Action action;
	
	uint8_t state;
	
	uint8_t layer;
	 
} PG_Phrase;

typedef struct
{
	uint16_t n_key_events;
	PG_Key_Event key_events[PG_MAX_KEYCHANGES];

	PG_Phrase melody_root;
	bool melody_root_initialized;

	PG_Phrase *current_phrase;

	bool papageno_enabled;
	bool papageno_temporarily_disabled;

	PG_Key_Id abort_key_id;

	uint16_t time_last_keypress;

	uint16_t keypress_timeout;
	
	PG_Key_Id_Equal_Fun key_id_equal;
	
	PG_Key_Event_Processor_Fun key_processor;
	
	#ifdef DEBUG_PAPAGENO
	PG_Printf_Fun printf;
	#endif	
  
} PG_Magic_Melody_State;

static PG_Magic_Melody_State pg_state = 
{
	.n_key_events = 0,
	.melody_root_initialized = false,
	.current_phrase = NULL,
	.papageno_enabled = true,
	.papageno_temporarily_disabled = false,
	.abort_key_id = { .row = 100, .col = 100 },
	.keypress_timeout = PG_DEFAULT_KEYPRESS_TIMEOUT,
	.key_id_equal = (PG_Key_Id_Equal_Fun)pg_key_id_simple_equal,
	.printf = NULL
};

enum {
	PG_Phrase_In_Progress = 0,
	PG_Phrase_Completed,
	PG_Phrase_Invalid
};

static void pg_store_key_event(PG_Key_Event *key_event)
{
	assert(pg_state.n_key_events < PG_MAX_KEYCHANGES);
	
	pg_state.key_events[pg_state.n_key_events] = *key_event;
	
	++pg_state.n_key_events;
}

static void pg_phrase_store_action(PG_Phrase *a_phrase, 
											  PG_Action action)
{
	a_phrase->action = action; 
}

void pg_flush_stored_keyevents(
								uint8_t state_flag, 
								PG_Key_Event_Processor_Fun key_processor,
								void *user_data)
{
	if(pg_state.n_key_events == 0) { return; }
	
	PG_Key_Event_Processor_Fun kp	=
		(key_processor) ? key_processor : pstate.key_processor;
	
	if(!key_processor) { return; }
	
	/* Process all events as if they had happened just now. Use a time
	 * offset to achieve this.
	 */
	
	pg_state.papageno_temporarily_disabled = true;
       
// 	uint16_t cur_time = timer_read();
// 	
// 	uint16_t time_offset = cur_time - pg_state.keytimes[pg_state.n_key_events - 1];
	
	for(uint16_t i = 0; i < pg_state.n_key_events; ++i) {
		
		if(!kp(&pg_state.key_events[i], state_flag, user_data)) { 
			break;
		}
		
// 		record.event.time = pg_state.keytimes[i] + time_offset;
// 		
// 		record.event.key.row = pg_state.key_id[i].row;
// 		record.event.key.col = pg_state.key_id[i].col;
// 		record.event.pressed = pg_state.pressed[i];
// 		
// 		PG_PRINTF("Issuing keystroke at %d, %d\n", record.event.key.row, record.event.key.col);
// 		
// 		process_record_quantum(&record);
	}
	
	pg_state.papageno_temporarily_disabled = false;
	
	pg_state.n_key_events = 0;
}

static void pg_phrase_reset(	PG_Phrase *a_This)
{
	a_This->state = PG_Phrase_In_Progress;
}

/* Phrases have states. This method resets the states
 * after processing a melody. On our way through the
 * melody tree we only need to reset thoses phrases
 * that were candidates.
 */
static void pg_phrase_reset_successors(PG_Phrase *a_This)
{
	/* Reset all successor phrases if there are any
	 */
	for(uint8_t i = 0; i < a_This->n_successors; ++i) {
		PG_CALL_VIRT_METHOD(a_This->successors[i], reset);
	}
}
	
static void pg_abort_magic_melody(void)
{		
	if(!pg_state.current_phrase) { return; }
	
	PG_PRINTF("Aborting magic melody\n");
	
	/* The frase could not be parsed. Reset any previous phrases.
	*/
	pg_phrase_reset_successors(pg_state.current_phrase);
	
	/* Cleanup and issue all keypresses as if they happened without parsing a melody
	*/
	pg_flush_stored_keyevents(	PG_Key_Flush_Abort, 
										NULL /* key_processor */, 
										NULL /* user data */);
	
	pg_state.current_phrase = NULL;
}

static void pg_phrase_trigger_action(PG_Phrase *a_PG_Phrase) {
	
	PG_PRINTF("*\n");
	
// 	switch(a_PG_Phrase->action.type) {
// 		case PG_Action_Keycode:
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
			
	if(a_PG_Phrase->action.user_callback.func) {
		a_PG_Phrase->action.user_callback.func(
			a_PG_Phrase->action.user_callback.user_data);
	}
}

static bool pg_recurse_and_process_actions(void)
{			
	if(!pg_state.current_phrase) { return false; }
	
	PG_PRINTF("Triggering action of most recent phrase\n");
	
	PG_Phrase *cur_phrase = pg_state.current_phrase;
	
	while(cur_phrase) {
		
		pg_phrase_trigger_action(cur_phrase);
		
		if(cur_phrase->action.flag &= PG_Action_Fallthrough) {
			cur_phrase = cur_phrase->parent;
		}
		else {
			return true;
		}
	}
	
	return false;
}

static void pg_on_timeout(void)
{
	if(!pg_state.current_phrase) { return; }
	
	/* The frase could not be parsed. Reset any previous phrases.
	*/
	pg_phrase_reset_successors(pg_state.current_phrase);
	
	/* It timeout was hit, we either trigger the most recent action
	 * (e.g. necessary for tap dances) or flush the keyevents
	 * that happend until this point
	 */
	
	bool action_triggered 
		= pg_recurse_and_process_actions();
	
	/* Cleanup and issue all keypresses as if they happened without parsing a melody
	*/
	if(action_triggered) {
		pg_flush_stored_keyevents(	PG_Key_Flush_Timeout, 
											NULL /* key_processor */, 
											NULL /* user data */);
	}
	
	pg_state.current_phrase = NULL;
}

static uint8_t pg_phrase_consider_key_event(	
												PG_Phrase **current_phrase,
												PG_Key_Event *key_event) 
{
	/* Loop over all phrases and inform them about the 
	 * key_event 
	 */
	bool all_successors_invalid = true;
	
	PG_Phrase *a_current_phrase = *current_phrase;
	
	pg_store_key_event(key_event);
	
	uint8_t layer = biton32(layer_state);
	
// 	PG_PRINTF("Processing key\n");
	
	#if DEBUG_PAPAGENO
	if(key_event->pressed) {
		PG_PRINTF("v");
	}
	else {
		PG_PRINTF("^");
	}
	#endif
	
	bool any_phrase_completed = false;
		
	/* Pass the keypress to the phrases and let them decide it they 
	 * can use it. If a phrase cannot it becomes invalid and is not
	 * processed further on this node level. This speeds up processing.
	 */
	for(uint8_t i = 0; i < a_current_phrase->n_successors; ++i) {
		
		// PG_CALL_VIRT_METHOD(a_current_phrase->successors[i], print_self);
		
		/* Accept only paths through the search tree whose
		 * nodes' layer tags are lower or equal the current layer
		 */
		if(a_current_phrase->successors[i]->layer > layer) { continue; }
		
		if(a_current_phrase->successors[i]->state == PG_Phrase_Invalid) {
			continue;
		}
		
		uint8_t successor_process_result 
			= a_current_phrase->successors[i]
					->vtable->successor_consider_key_event(	
								a_current_phrase->successors[i], 
								key_event
						);
								
		switch(successor_process_result) {
			
			case PG_Phrase_In_Progress:
				all_successors_invalid = false;
				
				break;
				
			case PG_Phrase_Completed:
				
				all_successors_invalid = false;
				any_phrase_completed = true;

				break;
			case PG_Phrase_Invalid:
// 				PG_PRINTF("Phrase invalid");
				break;
		}
	}
	
	/* If all successors are invalid, the keystroke chain does not
	 * match any defined melody.
	 */
	if(all_successors_invalid) {
					
		return PG_Phrase_Invalid;
	}
	
	/* If any phrase completed we have to find the most suitable one
	 * and either terminate melody processing if the matching successor
	 * is a leaf node, or replace the current phrase to await further 
	 * keystrokes.
	 */
	else if(any_phrase_completed) {
		
		int8_t highest_layer = -1;
		int8_t match_id = -1;
		
		/* Find the most suitable phrase with respect to the current layer.
		 */
		for(uint8_t i = 0; i < a_current_phrase->n_successors; ++i) {
		
// 			PG_CALL_VIRT_METHOD(a_current_phrase->successors[i], print_self);
		
			/* Accept only paths through the search tree whose
			* nodes' layer tags are lower or equal the current layer
			*/
			if(a_current_phrase->successors[i]->layer > layer) { continue; }
			
			if(a_current_phrase->successors[i]->state != PG_Phrase_Completed) {
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
		pg_phrase_reset_successors(a_current_phrase);
		
		/* Replace the current phrase.
		*/
		*current_phrase = a_current_phrase->successors[match_id];
		a_current_phrase = *current_phrase;
			
		if(0 == a_current_phrase->n_successors) {
			
			/* The melody is completed. Trigger the action that is associated
			* with the leaf node.
			*/
			PG_CALL_VIRT_METHOD(a_current_phrase, trigger_action);
			
			pg_state.current_phrase = NULL;
			
			return PG_Phrase_Completed;
		}
		else {
			
			/* The melody is still in progress. We continue with the 
			 * new current node as soon as the next keystroke happens.
			 */
			return PG_Phrase_In_Progress;
		}
	}
	
	return PG_Phrase_In_Progress;
}

static void pg_phrase_allocate_successors(PG_Phrase *a_This, uint8_t n_successors) {

	 a_This->successors 
		= (struct PG_PhraseStruct **)malloc(n_successors*sizeof(struct PG_PhraseStruct*));
	 a_This->n_allocated_successors = n_successors;
}

static void pg_phrase_grow_successors(PG_Phrase *a_This) {

	if(a_This->n_allocated_successors == 0) {
		
		pg_phrase_allocate_successors(a_This, 1);
	}
	else {
		PG_Phrase **oldSucessors = a_This->successors;
		
		pg_phrase_allocate_successors(a_This, 2*a_This->n_allocated_successors);
			
		for(uint8_t i = 0; i < a_This->n_successors; ++i) {
			a_This->successors[i] = oldSucessors[i];
		}
		
		free(oldSucessors); 
	}
}

static void pg_phrase_add_successor(PG_Phrase *a_This, PG_Phrase *successor) {
	
	if(a_This->n_allocated_successors == a_This->n_successors) {
		pg_phrase_grow_successors(a_This);
	}
	
	a_This->successors[a_This->n_successors] = successor;
	
	successor->parent = a_This;
	
	++a_This->n_successors;
}

static void pg_phrase_free(PG_Phrase *a_This);

static void pg_phrase_free_successors(PG_Phrase *a_This)
{
	if(!a_This->successors) { return; }
	
	for(uint8_t i = 0; i < a_This->n_allocated_successors; ++i) {
		
		pg_phrase_free(a_This->successors[i]);
	}
	
	free(a_This->successors);
	
	a_This->successors = NULL;
	a_This->n_allocated_successors = 0;
}

static PG_Phrase* pg_phrase_destroy(PG_Phrase *a_PG_Phrase) {

	pg_phrase_free_successors(a_PG_Phrase);
	
	return a_PG_Phrase;
}

static bool pg_phrase_equals(PG_Phrase *p1, PG_Phrase *p2) 
{
	if(p1->vtable != p2->vtable) { return false; }
	
	return p1->vtable->equals(p1, p2);
}

static void pg_phrase_free(PG_Phrase *a_This) {
	
	PG_CALL_VIRT_METHOD(a_This, destroy);

	free(a_This);
}

static PG_Phrase* pg_phrase_get_equivalent_successor(
														PG_Phrase *a_PG_Phrase,
														PG_Phrase *sample) {
	
	if(a_PG_Phrase->n_successors == 0) { return NULL; }
	
	for(uint8_t i = 0; i < a_PG_Phrase->n_successors; ++i) {
		if(pg_phrase_equals(
											a_PG_Phrase->successors[i], 
											sample)
		  ) {
			return a_PG_Phrase->successors[i];
		}
	}
	
	return NULL;
}

static void pg_phrase_print_self(PG_Phrase *p)
{
	PG_PRINTF("phrase (0x%" PRIXPTR ")\n", (void*)p);
	PG_PRINTF("   parent: 0x%" PRIXPTR "\n", (void*)&p->parent);
	PG_PRINTF("   successors: 0x%" PRIXPTR "\n", (void*)&p->successors);
	PG_PRINTF("   n_allocated_successors: %d\n", p->n_allocated_successors);
	PG_PRINTF("   n_successors: %d\n", p->n_successors);
	PG_PRINTF("   action.flag: %d\n", p->action.flag);
	PG_PRINTF("   action_user_func: %0x%" PRIXPTR "\n", p->action.user_callback.func);
	PG_PRINTF("   action_user_data: %0x%" PRIXPTR "\n", p->action.user_callback.user_data);
	PG_PRINTF("   state: %d\n", p->state);
	PG_PRINTF("   layer: %d\n", p->layer);
}

static PG_Phrase_Vtable pg_phrase_vtable =
{
	.consider_key_event 
		= (PG_Phrase_Consider_Keychange_Fun) pg_phrase_consider_key_event,
	.successor_consider_key_event 
		= NULL,
	.reset 
		= (PG_Phrase_Reset_Fun) pg_phrase_reset,
	.trigger_action 
		= (PG_Phrase_Trigger_Action_Fun) pg_phrase_trigger_action,
	.destroy 
		= (PG_Phrase_Destroy_Fun) pg_phrase_destroy,
	.equals
		= NULL,
	.print_self
		= (PG_Phrase_Print_Self_Fun) pg_phrase_print_self,
};

static PG_Phrase *pg_phrase_new(PG_Phrase *a_PG_Phrase) {
	
    a_PG_Phrase->vtable = &pg_phrase_vtable;
	 
    a_PG_Phrase->parent = NULL;
    a_PG_Phrase->successors = NULL;
	 a_PG_Phrase->n_allocated_successors = 0;
    a_PG_Phrase->n_successors = 0;
    a_PG_Phrase->action.flag = PG_Action_Undefined;
    a_PG_Phrase->action.user_callback.func = NULL;
    a_PG_Phrase->action.user_callback.user_data = NULL;
    a_PG_Phrase->state = PG_Phrase_In_Progress;
    a_PG_Phrase->layer = 0;
	 
    return a_PG_Phrase;
}

/*****************************************************************************
 Notes 
*****************************************************************************/

typedef struct {
	
	PG_Phrase phrase_inventory;
	 
	PG_Key_Id key_id;
	bool pressed;
	 
} PG_Note;

static uint8_t pg_note_successor_consider_key_event(	
											PG_Note *a_This,
											PG_Key_Event *key_event) 
{	
	assert(a_This->phrase_inventory.state == PG_Phrase_In_Progress);
	
	/* Set state appropriately 
	 */
	if(pg_state.key_id_equal(a_This->key_id, record->event.key)) {
		
		if(key_event->pressed) {
			a_This->pressed = true;
			a_This->phrase_inventory.state = PG_Phrase_In_Progress;
		}
		else {
			if(a_This->pressed) {
	// 		PG_PRINTF("note successfully finished\n");
				PG_PRINTF("N");
				a_This->phrase_inventory.state = PG_Phrase_Completed;
			}
		}
	}
	else {
// 		PG_PRINTF("note invalid\n");
		a_This->phrase_inventory.state = PG_Phrase_Invalid;
	}
	
	return a_This->phrase_inventory.state;
}

static void pg_note_reset(PG_Note *a_This) 
{
	pg_phrase_reset((PG_Phrase*)a_This);
	
	a_This->pressed = false;
}

static PG_Note *pg_note_alloc(void) {
    return (PG_Note*)malloc(sizeof(PG_Note));
}

static bool pg_note_equals(PG_Note *n1, PG_Note *n2) 
{
	return pg_state.key_id_equal(n1->key_id, n2->key_id);
}

static void pg_note_print_self(PG_Note *p)
{
	pg_phrase_print_self((PG_Phrase*)p);
	
	PG_PRINTF("note\n");
	PG_PRINTF("   row: %d\n", p->key_id.row);
	PG_PRINTF("   col: %d\n", p->key_id.col);
}

static PG_Phrase_Vtable pg_note_vtable =
{
	.consider_key_event 
		= (PG_Phrase_Consider_Keychange_Fun) pg_phrase_consider_key_event,
	.successor_consider_key_event 
		= (PG_Phrase_Successor_Consider_Keychange_Fun) pg_note_successor_consider_key_event,
	.reset 
		= (PG_Phrase_Reset_Fun) pg_note_reset,
	.trigger_action 
		= (PG_Phrase_Trigger_Action_Fun) pg_phrase_trigger_action,
	.destroy 
		= (PG_Phrase_Destroy_Fun) pg_phrase_destroy,
	.equals
		= (PG_Phrase_Equals_Fun) pg_note_equals,
	.print_self
		= (PG_Phrase_Print_Self_Fun) pg_note_print_self
};

static PG_Note *pg_note_new(PG_Note *a_note)
{
    /* Explict call to parent constructor 
	  */
    pg_phrase_new((PG_Phrase*)a_note);

    a_note->phrase_inventory.vtable = &pg_note_vtable;
		
	 a_note->key_id.row = 100;
	 a_note->key_id.col = 100;
	 
	 a_note->pressed = false;
	 
    return a_note;
}

/*****************************************************************************
 Chords 
*****************************************************************************/

typedef struct {
	
	PG_Phrase phrase_inventory;
	 
	uint8_t n_members;
	PG_Key_Id *key_id;
	bool *member_active;
	uint8_t n_chord_keys_pressed;
	 
} PG_Chord;

static uint8_t pg_chord_successor_consider_key_event(	
											PG_Chord *a_This,
											PG_Key_Event *key_event) 
{
	bool key_part_of_chord = false;
	
	assert(a_This->n_members != 0);
	
	assert(a_This->phrase_inventory.state == PG_Phrase_In_Progress);
	
	/* Check it the key is part of the current chord 
	 */
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		
		if(pg_state.key_id_equal(a_This->key_id[i], record->event.key)) {
			
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
			a_This->phrase_inventory.state = PG_Phrase_Invalid;
			return a_This->phrase_inventory.state;
		}
	}
	
	if(a_This->n_chord_keys_pressed == a_This->n_members) {
		
		/* Chord completed
		 */
		a_This->phrase_inventory.state = PG_Phrase_Completed;
 		PG_PRINTF("C");
	}
	
	return a_This->phrase_inventory.state;
}

static void pg_chord_reset(PG_Chord *a_This) 
{
	pg_phrase_reset((PG_Phrase*)a_This);
	
	a_This->n_chord_keys_pressed = 0;
	
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static void pg_chord_deallocate_member_storage(PG_Chord *a_This) {	
	
	if(a_This->key_id) {
		free(a_This->key_id);
		a_This->key_id = NULL;
	}
	if(a_This->member_active) {
		free(a_This->member_active);
		a_This->member_active = NULL;
	}
}

static void pg_chord_resize(PG_Chord *a_This, 
							uint8_t n_members)
{
	pg_chord_deallocate_member_storage(a_This);
	
	a_This->n_members = n_members;
	a_This->key_id = (PG_Key_Id *)malloc(n_members*sizeof(PG_Key_Id));
	a_This->member_active = (bool *)malloc(n_members*sizeof(bool));
	a_This->n_chord_keys_pressed = 0;
	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static PG_Chord* pg_chord_destroy(PG_Chord *a_This) {
	
	pg_chord_deallocate_member_storage(a_This);

	return a_This;
}

static PG_Chord *pg_chord_alloc(void){
    return (PG_Chord*)malloc(sizeof(PG_Chord));
}

static bool pg_chord_equals(PG_Chord *c1, PG_Chord *c2) 
{
	if(c1->n_members != c2->n_members) { return false; }
	
	for(uint8_t i = 0; i < c1->n_members; ++i) {
		if(!pg_state.key_id_equal(c1->key_id[i], c2->key_id[i])) { return false; }
	}
	
	return true;
}

static void pg_chord_print_self(PG_Chord *c)
{
	pg_phrase_print_self((PG_Phrase*)c);
	
	PG_PRINTF("chord\n");
	PG_PRINTF("   n_members: %d\n", c->n_members);
	PG_PRINTF("   n_chord_keys_pressed: %d\n", c->n_chord_keys_pressed);
	
	for(uint8_t i = 0; i < c->n_members; ++i) {
		PG_PRINTF("      row: %d, col: %d, active: %d\n", 
				  c->key_id[i].row, c->key_id[i].col, c->member_active[i]);
	}
}

static PG_Phrase_Vtable pg_chord_vtable =
{
	.consider_key_event 
		= (PG_Phrase_Consider_Keychange_Fun) pg_phrase_consider_key_event,
	.successor_consider_key_event 
		= (PG_Phrase_Successor_Consider_Keychange_Fun) pg_chord_successor_consider_key_event,
	.reset 
		= (PG_Phrase_Reset_Fun) pg_chord_reset,
	.trigger_action 
		= (PG_Phrase_Trigger_Action_Fun) pg_phrase_trigger_action,
	.destroy 
		= (PG_Phrase_Destroy_Fun) pg_chord_destroy,
	.equals
		= (PG_Phrase_Equals_Fun) pg_chord_equals,
	.print_self
		= (PG_Phrase_Print_Self_Fun) pg_chord_print_self
};

static PG_Chord *pg_chord_new(PG_Chord *a_chord){
    
	/* Explict call to parent constructor
	 */
	pg_phrase_new((PG_Phrase*)a_chord);

	a_chord->phrase_inventory.vtable = &pg_chord_vtable;
		
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
	
	PG_Phrase phrase_inventory;
	 
	uint8_t n_members;
	PG_Key_Id *key_id;
	bool *member_active;
	uint8_t n_cluster_keys_pressed;
	 
} PG_Cluster;

static uint8_t pg_cluster_successor_consider_key_event(	
											PG_Cluster *a_This,
											PG_Key_Event *key_event) 
{
	bool key_part_of_cluster = false;
	
	assert(a_This->n_members != 0);
	
	/* Only react on pressed keys. Allow cluster members to be released. 
	 * Every key only must be pressed once.
	 */
	if(!key_event->pressed) { return PG_Phrase_In_Progress; }
	
	assert(a_This->phrase_inventory.state == PG_Phrase_In_Progress);
	
	/* Check it the key is part of the current chord 
	 */
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		
		if(pg_state.key_id_equal(a_This->key_id[i], record->event.key)) {
			
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
			a_This->phrase_inventory.state = PG_Phrase_Invalid;
			return a_This->phrase_inventory.state;
		}
	}
	
	if(a_This->n_cluster_keys_pressed == a_This->n_members) {
		
		/* Cluster completed
		 */
		a_This->phrase_inventory.state = PG_Phrase_Completed;
 		PG_PRINTF("O");
	}
	
	return a_This->phrase_inventory.state;
}

static void pg_cluster_reset(PG_Cluster *a_This) 
{
	pg_phrase_reset((PG_Phrase*)a_This);
	
	a_This->n_cluster_keys_pressed = 0;
	
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static void pg_cluster_deallocate_member_storage(PG_Cluster *a_This) {	
	
	if(a_This->key_id) {
		free(a_This->key_id);
		a_This->key_id = NULL;
	}
	if(a_This->member_active) {
		free(a_This->member_active);
		a_This->member_active = NULL;
	}
}

static void pg_cluster_resize(PG_Cluster *a_This, 
							uint8_t n_members)
{
	pg_cluster_deallocate_member_storage(a_This);
	
	a_This->n_members = n_members;
	a_This->key_id = (PG_Key_Id *)malloc(n_members*sizeof(PG_Key_Id));
	a_This->member_active = (bool *)malloc(n_members*sizeof(bool));
	a_This->n_cluster_keys_pressed = 0;
	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static bool pg_is_cluster_member(PG_Cluster *c, PG_Key_Id key)
{
	for(uint8_t i = 0; i < c->n_members; ++i) {
		if(pg_state.key_id_equal(c->key_id[i], key)) {
			return true;
		}
	}
	
	return false;
}

static bool pg_cluster_equals(PG_Cluster *c1, PG_Cluster *c2) 
{
	if(c1->n_members != c2->n_members) { return false; }
	
	for(uint8_t i = 0; i < c1->n_members; ++i) {
		if(!pg_is_cluster_member(c1, c2->key_id[i])) { return false; }
	}
	
	return true;
}

static PG_Cluster* pg_cluster_destroy(PG_Cluster *a_This) {
	
	pg_cluster_deallocate_member_storage(a_This);

    return a_This;
}

static PG_Cluster *pg_cluster_alloc(void){
    return (PG_Cluster*)malloc(sizeof(PG_Cluster));
} 

static void pg_cluster_print_self(PG_Cluster *c)
{
	pg_phrase_print_self((PG_Phrase*)c);
	
	PG_PRINTF("cluster\n");
	PG_PRINTF("   n_members: %d\n", c->n_members);
	PG_PRINTF("   n_cluster_keys_pressed: %d\n", c->n_cluster_keys_pressed);
	
	for(uint8_t i = 0; i < c->n_members; ++i) {
		PG_PRINTF("      row: %d, col: %d, active: %d\n", 
				  c->key_id[i].row, c->key_id[i].col, c->member_active[i]);
	}
}

static PG_Phrase_Vtable pg_cluster_vtable =
{
	.consider_key_event 
		= (PG_Phrase_Consider_Keychange_Fun) pg_phrase_consider_key_event,
	.successor_consider_key_event 
		= (PG_Phrase_Successor_Consider_Keychange_Fun) pg_cluster_successor_consider_key_event,
	.reset 
		= (PG_Phrase_Reset_Fun) pg_cluster_reset,
	.trigger_action 
		= (PG_Phrase_Trigger_Action_Fun) pg_phrase_trigger_action,
	.destroy 
		= (PG_Phrase_Destroy_Fun) pg_cluster_destroy,
	.equals
		= (PG_Phrase_Equals_Fun) pg_cluster_equals,
	.print_self
		= (PG_Phrase_Print_Self_Fun) pg_cluster_print_self
};

static PG_Cluster *pg_cluster_new(PG_Cluster *a_cluster) {
	
	/*Explict call to parent constructor
	*/
	pg_phrase_new((PG_Phrase*)a_cluster);

	a_cluster->phrase_inventory.vtable = &pg_cluster_vtable;
	
	a_cluster->n_members = 0;
	a_cluster->key_id = NULL;
	a_cluster->member_active = NULL;
	a_cluster->n_cluster_keys_pressed = 0;
	
	return a_cluster;
}

/*****************************************************************************
 * Public access functions for creation and destruction of the melody tree
 *****************************************************************************/

static void pg_init(void) {

	if(!pg_state.melody_root_initialized) {
		
		/* Initialize the melody root
		 */
		pg_phrase_new(&pg_state.melody_root);
		
		pg_state.melody_root_initialized = true;
	}
}

void pg_finalize(void) {
	pg_phrase_free_successors(&pg_state.melody_root);
}

void *pg_create_note(PG_Key_Id key_id)
{
    PG_Note *a_note = (PG_Note*)pg_note_new(pg_note_alloc());
	 
	 a_note->key_id = key_id;
	 
	 /* Return the new end of the melody */
	 return a_note;
}

void *pg_create_chord(	PG_Key_Id *key_id,
								uint8_t n_members)
{
	PG_Chord *a_chord = (PG_Chord*)pg_chord_new(pg_chord_alloc());
	 	 
	pg_chord_resize(a_chord, n_members);
	 
	for(uint8_t i = 0; i < n_members; ++i) {
		a_chord->key_id[i] = key_id[i];
	}
	 
	/* Return the new end of the melody */
	return a_chord;
}

void *pg_create_cluster(
									PG_Key_Id *key_id,
									uint8_t n_members)
{
	PG_Cluster *a_cluster = (PG_Cluster*)pg_cluster_new(pg_cluster_alloc());
	 
	pg_cluster_resize(a_cluster, n_members);
	 	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_cluster->key_id[i] = key_id[i];
	}
	
	/* Return the new end of the melody */
	return a_cluster;
}

#if DEBUG_PAPAGENO
static void pg_recursively_print_melody(PG_Phrase *p)
{
	if(	p->parent
		&& p->parent != &pg_state.melody_root) {
		
		pg_recursively_print_melody(p->parent);
	}
	
	PG_CALL_VIRT_METHOD(p, print_self);
}
#endif

static void *pg_melody_from_list(	uint8_t layer,
												PG_Phrase **phrases,
												int n_phrases)
{ 
	PG_Phrase *parent_phrase = &pg_state.melody_root;
	
	PG_PRINTF("   %d members\n", n_phrases);
	
	for (int i = 0; i < n_phrases; i++) { 
		
		PG_Phrase *cur_phrase = phrases[i];
		
		/* If the action type is undefined, we set action 
		 * type none, which means that no fall through happens
		 * in case of timeout.
		 */
		if(cur_phrase->action.type == PG_Action_Undefined) {
			cur_phrase->action.type = PG_Action_None;
		}
		
		PG_Phrase *equivalent_successor 
			= pg_phrase_get_equivalent_successor(parent_phrase, cur_phrase);
			
		PG_PRINTF("   member %d: ", i);
		
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
			
			#if DEBUG_PAPAGENO
			if(cur_phrase->action.type != equivalent_successor->action.type) {
				PG_ERROR("Incompatible action types detected\n");
			}
			#endif
			
			PG_PRINTF("already present\n");
			
			parent_phrase = equivalent_successor;
			
			if(layer < equivalent_successor->layer) {
				
				equivalent_successor->layer = layer;
			}
			
			/* The successor is already registered in the search tree. Delete the newly created version.
			 */			
			pg_phrase_free(cur_phrase);
		}
		else {
			
			PG_PRINTF("newly defined\n");
			
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
					PG_ERROR("Conflicting melodies detected. "
						"The phrases of the conflicting melodies are listed below.\n");
					
					PG_ERROR("Previously defined:\n");
					pg_recursively_print_melody(equivalent_successor);
					
					PG_ERROR("Conflicting:\n");
					for (int i = 0; i < n_phrases; i++) {
						PG_CALL_VIRT_METHOD(phrases[i], print_self);
					}
				}
			}
			#endif /* if DEBUG_PAPAGENO */
					
			cur_phrase->layer = layer;
			
			pg_phrase_add_successor(parent_phrase, cur_phrase);
			
			parent_phrase = cur_phrase;
		}
	}
	
	/* Return the leaf phrase 
	 */
	return parent_phrase;
}

void *pg_melody(		uint8_t layer, 
							int count, 
							...)
{ 
	PG_PRINTF("Adding magic melody\n");
	
	pg_init();
	
	va_list ap;

	va_start (ap, count);         /* Initialize the argument list. */
  	
	PG_Phrase **phrases 
		= (PG_Phrase **)malloc(count*sizeof(PG_Phrase *));
	
	for (int i = 0; i < count; i++) { 
		
		phrases[i] = va_arg (ap, PG_Phrase *);
	}
	
	PG_Phrase *leafPhrase 
		= pg_melody_from_list(layer, phrases, count);
	
	free(phrases);

	va_end (ap);                  /* Clean up. */
	
	return leafPhrase;
}

void *pg_chord(		uint8_t layer, 
							PG_Action action, 
							PG_Key_Id *key_ids,
							uint8_t n_members)
{   	
	PG_PRINTF("Adding chord\n");
	
	PG_Phrase *cPhrase = 
		(PG_Phrase *)pg_create_chord(key_ids, n_members);
		
	cPhrase->action = action;
		
	PG_Phrase *phrases[1] = { cPhrase };
	
	PG_Phrase *leafPhrase 
		= pg_melody_from_list(layer, phrases, 1);
		
	return leafPhrase;
}

void *pg_cluster(		uint8_t layer, 
							PG_Action action, 
							PG_Key_Id *key_ids,
							uint8_t n_members)
{   	
	PG_PRINTF("Adding cluster\n");
	
	PG_Phrase *cPhrase = 
		(PG_Phrase *)pg_create_cluster(key_ids, n_members);
		
	cPhrase->action = action;
	
	PG_Phrase *phrases[1] = { cPhrase };
	
	PG_Phrase *leafPhrase 
		= pg_melody_from_list(layer, phrases, 1);
		
	return leafPhrase;
}

void *pg_single_note_line(	
							uint8_t layer,
							PG_Action action, 
							int count, 
							...)
{
	PG_PRINTF("Adding single note line\n");
	
	pg_init();
	
	va_list ap;

	va_start (ap, count);         /* Initialize the argument list. */
  
	PG_Phrase **phrases 
		= (PG_Phrase **)malloc(count*sizeof(PG_Phrase *));
		
	for (int i = 0; i < count; i++) {
		
		PG_Key_Id curKeypos = va_arg (ap, PG_Key_Id); 

		void *new_note = pg_create_note(curKeypos);
		
		phrases[i] = (PG_Phrase *)new_note;
	}
	
	pg_phrase_store_action(phrases[count - 1], action);
	
	PG_Phrase *leaf_phrase 
		= pg_melody_from_list(layer, phrases, count);
	
	free(phrases);

	va_end (ap);                  /* Clean up. */
  
	return leaf_phrase;
}

void *pg_tap_dance(	uint8_t layer,
							PG_Key_Id curKeypos,
							uint8_t default_action_type,
							uint8_t n_vargs,
							...
							)
{
	PG_PRINTF("Adding tap dance\n");
	
	pg_init();
	
	va_list ap;

	va_start (ap, n_vargs);         /* Initialize the argument list. */
	
	uint8_t n_tap_definitions = n_vargs/2;
		
	int n_taps = 0;
	for (uint8_t i = 0; i < n_tap_definitions; i++) {
		
		int tap_count = va_arg (ap, int); 
		va_arg (ap, PG_Action); /* not needed here */
		
		if(tap_count > n_taps) {
			n_taps = tap_count;
		}
	}
	
	if(n_taps == 0) { return NULL; }
	
	PG_Phrase **phrases 
		= (PG_Phrase **)malloc(n_taps*sizeof(PG_Phrase *));
	
	for (int i = 0; i < n_taps; i++) {
		
		void *new_note = pg_create_note(curKeypos);
		
		phrases[i] = (PG_Phrase *)new_note;
		
		phrases[i]->action.type = default_action_type;
	}
	
	va_start (ap, n_vargs);         /* Initialize the argument list. */
	
	for (uint8_t i = 0; i < n_tap_definitions; i++) {
		
		int tap_count = va_arg (ap, int); 
		PG_Action action = va_arg (ap, PG_Action);

		pg_phrase_store_action(phrases[tap_count - 1], action);
	}
			
	PG_Phrase *leafPhrase 
		= pg_melody_from_list(layer, phrases, n_taps);
	
	free(phrases);
	
	return leafPhrase;
}

/* Returns phrase__
 */
void *pg_set_action(	void *phrase__,
							PG_Action action)
{
	PG_Phrase *phrase = (PG_Phrase *)phrase__;
	
	pg_phrase_store_action(phrase, action);
	
	return phrase__;
}

bool pg_check_timeout(void)
{
	if(pg_state.current_phrase
		&& (timer_elapsed(pg_state.time_last_keypress) 
				> pg_state.keypress_timeout)
	  ) {
		
		PG_PRINTF("Magic melody timeout hit\n");
	
		/* Too late...
			*/
		pg_on_timeout();
	
		return true;
	}
	
	return false;
}

bool pg_process_key_event(PG_Key_Event *key_event)
{ 
	if(!pg_state.papageno_enabled) {
		return true;
	}
	
	/* When a melody could not be finished, all keystrokes are
	 * processed through the process_record_quantum method.
	 * To prevent infinite recursion, we have to temporarily disable 
	 * processing magic melodies.
	 */
	if(pg_state.papageno_temporarily_disabled) { return true; }
	
	/* Early exit if no melody was registered 
	 */
	if(!pg_state.melody_root_initialized) { return true; }
			
	/* If there is no melody processed, we ignore keyups.
	*/	
	if(!pg_state.current_phrase && !key_event->pressed) {
// 		PG_PRINTF("Keyup ignored as no melody currently processed\n");
		return true;
	}
	
	/* Check if the melody is being aborted
	 */
	if(pg_state.key_id_equal(pg_state.abort_key_id, record->event.key)) {
		
		/* If a melody is in progress, we abort it and consume the abort key.
		 */
		if(pg_state.current_phrase) {
			PG_PRINTF("Processing melodies interrupted by user\n");
			pg_abort_magic_melody();
			return false;
		}
	
		return false;
	}
	
	PG_PRINTF("Starting keyprocessing\n");
	
	if(!pg_state.current_phrase) {
		
// 		PG_PRINTF("New melody \n");
		
		/* Start of melody processing
		 */
		pg_state.n_key_events = 0;
		pg_state.current_phrase = &pg_state.melody_root;
		pg_state.time_last_keypress = timer_read();
	}
	else {
		
		if(pg_check_timeout()) {
			
			/* Timeout hit. Cleanup already done.
			 */
			return false;
		}
		else {
			pg_state.time_last_keypress = timer_read();
		}
	}
	
	uint8_t result 
		= pg_phrase_consider_key_event(	&pg_state.current_phrase,
											key_event);
		
	switch(result) {
		case PG_Phrase_In_Progress:
		case PG_Phrase_Completed:
			pg_flush_stored_keyevents(	PG_Key_Flush_Melody_Completed, 
												NULL /* key_processor */, 
												NULL /* user data */);
											  
			return false;
			
		case PG_Phrase_Invalid:
			
			PG_PRINTF("-\n");
		
			pg_abort_magic_melody();
			
// 			return false; /* The key(s) have been already processed */
			return true; // Why does this require true to work and 
			// why is t not written?
	}
	
	return true;
}

/* Use this function to define a key_id that always aborts a magic melody
 */
void pg_set_abort_key_id(PG_Key_Id key_id)
{
	pg_state.abort_key_id = key_id;
}

void pg_set_timeout_ms(uint16_t timeout)
{
	pg_state.keypress_timeout = timeout;
}

void pg_set_key_id_equal_function(PG_Key_Id_Equal_Fun fun)
{
	pg_state.key_id_equal = fun;
}

void pg_set_key_processor(PG_Key_Event_Processor_Fun key_processor)
{
	pg_state.key_processor = key_processor;
}

void pg_set_enabled(bool state)
{
	pg_state.papageno_enabled = state;
}

#ifdef DEBUG_PAPAGENO
void pg_set_printf(PG_Printf_Fun printf_fun)
{
	pg_state.printf = printf_fun;
}
#endif