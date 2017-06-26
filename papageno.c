/* Copyright 2016 Jack Humbert
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

#ifdef MAGIC_MELODIES_ENABLE

#include "process_magic_melodies.h"
#include "assert.h"
#include <inttypes.h>

#define MM_MAX_KEYCHANGES 100

#define MM_DEFAULT_KEYPRESS_TIMEOUT 200

#ifdef DEBUG_MAGIC_MELODIES
#include "debug.h"
#define MM_PRINTF(...) uprintf(__VA_ARGS__)
#define MM_ERROR(...) uprintf("*** Error: " __VA_ARGS__)
#else
#define MM_PRINTF(...)
#define MM_ERROR(...)
#endif

#define MM_CALL_VIRT_METHOD(THIS, METHOD, ...) \
	THIS->vtable->METHOD(THIS, ##__VA_ARGS__);

/* This function is defined in quantum/keymap_common.c 
 */
action_t action_for_configured_keycode(uint16_t keycode);

static bool mm_keypos_equal(keypos_t kp1, keypos_t kp2)
{
// 	MM_PRINTF("kp1: %d, %d\n", kp1.row, kp1.col);
// 	MM_PRINTF("kp2: %d, %d\n", kp2.row, kp2.col);
	
	return 		(kp1.row == kp2.row)
				&&	(kp1.col == kp2.col);
}

struct MM_PhraseStruct;

typedef uint8_t (*MM_Phrase_Consider_Keychange_Fun)(	struct MM_PhraseStruct *a_This, 
														uint16_t keycode, 
														keyrecord_t *record);

typedef uint8_t (*MM_Phrase_Successor_Consider_Keychange_Fun)(
														struct MM_PhraseStruct *a_This, 
														uint16_t keycode, 
														keyrecord_t *record);

typedef void (*MM_Phrase_Reset_Fun)(	struct MM_PhraseStruct *a_This);

typedef void (*MM_Phrase_Trigger_Action_Fun)(	struct MM_PhraseStruct *a_This);

typedef struct MM_PhraseStruct * (*MM_Phrase_Destroy_Fun)(struct MM_PhraseStruct *a_This);

typedef bool (*MM_Phrase_Equals_Fun)(struct MM_PhraseStruct *p1, struct MM_PhraseStruct *p2);

typedef void (*MM_Phrase_Print_Self_Fun)(struct MM_PhraseStruct *p);

typedef struct {
	
	MM_Phrase_Consider_Keychange_Fun 
									consider_keychange;
									
	MM_Phrase_Successor_Consider_Keychange_Fun 
									successor_consider_keychange;
									
	MM_Phrase_Reset_Fun
									reset;
									
	MM_Phrase_Trigger_Action_Fun
									trigger_action;
									
	MM_Phrase_Destroy_Fun	
									destroy;
			
	MM_Phrase_Equals_Fun
									equals;
									
	MM_Phrase_Print_Self_Fun
									print_self;
									
} MM_Phrase_Vtable;

typedef struct MM_PhraseStruct {

	MM_Phrase_Vtable *vtable;
	
	struct MM_PhraseStruct *parent;
	
	struct MM_PhraseStruct **successors;
	
	uint8_t n_allocated_successors;
	uint8_t n_successors;
	
	MM_Action action;
	
	uint8_t state;
	
	uint8_t layer;
	 
} MM_Phrase;

typedef struct
{
	uint16_t n_keys_changed;
	bool pressed[MM_MAX_KEYCHANGES];
	keypos_t keypos[MM_MAX_KEYCHANGES];
	uint16_t keytimes[MM_MAX_KEYCHANGES];

	MM_Phrase melody_root;
	bool melody_root_initialized;

	MM_Phrase *current_phrase;

	bool magic_melodies_temporarily_disabled;

	keypos_t abort_keypos;

	uint16_t time_last_keypress;

	uint16_t keypress_timeout;
  
} MM_Magic_Melody_State;

static MM_Magic_Melody_State mm_state = 
{
	.n_keys_changed = 0,
	.melody_root_initialized = false,
	.current_phrase = NULL,
	.magic_melodies_temporarily_disabled = false,
	.abort_keypos = { .row = 100, .col = 100 },
	.keypress_timeout = MM_DEFAULT_KEYPRESS_TIMEOUT
};

enum {
	MM_Phrase_In_Progress = 0,
	MM_Phrase_Completed,
	MM_Phrase_Invalid
};

static void mm_store_keychange(uint16_t keycode, 
										keyrecord_t *record)
{
	assert(mm_state.n_keys_changed < MM_MAX_KEYCHANGES);
	
	mm_state.pressed[mm_state.n_keys_changed] = record->event.pressed;
	mm_state.keypos[mm_state.n_keys_changed].row = record->event.key.row;
	mm_state.keypos[mm_state.n_keys_changed].col = record->event.key.col;
	mm_state.keytimes[mm_state.n_keys_changed] = record->event.time;
	
	++mm_state.n_keys_changed;
}

static void mm_phrase_store_action(MM_Phrase *a_phrase, 
											  MM_Action action)
{
	a_phrase->action.type = action.type;
	
	a_phrase->action.data.user_callback.func = NULL;
	a_phrase->action.data.user_callback.user_data = NULL;
	a_phrase->action.data.keycode = 0;
	
	switch(action.type) {
		case MM_Action_Keycode:
			a_phrase->action.data.keycode = action.data.keycode;
			break;
		case MM_Action_User_Callback:
			a_phrase->action.data.user_callback.func 
				= action.data.user_callback.func;
			a_phrase->action.data.user_callback.user_data 
				= action.data.user_callback.user_data;
			break;
	}
}

static void mm_flush_stored_keyevents(void)
{
	if(mm_state.n_keys_changed == 0) { return; }
	
	/* Process all events as if they had happened just now. Use a time
	 * offset to achieve this.
	 */
	
	mm_state.magic_melodies_temporarily_disabled = true;
       
	uint16_t cur_time = timer_read();
	
	uint16_t time_offset = cur_time - mm_state.keytimes[mm_state.n_keys_changed - 1];
	
	for(uint16_t i = 0; i < mm_state.n_keys_changed; ++i) {
		
		keyrecord_t record;
		
		record.event.time = mm_state.keytimes[i] + time_offset;
		
		record.event.key.row = mm_state.keypos[i].row;
		record.event.key.col = mm_state.keypos[i].col;
		record.event.pressed = mm_state.pressed[i];
		
		MM_PRINTF("Issuing keystroke at %d, %d\n", record.event.key.row, record.event.key.col);
		
		process_record_quantum(&record);
	}
	
	mm_state.magic_melodies_temporarily_disabled = false;
}

static void mm_phrase_reset(	MM_Phrase *a_This)
{
	a_This->state = MM_Phrase_In_Progress;
}

/* Phrases have states. This method resets the states
 * after processing a melody. On our way through the
 * melody tree we only need to reset thoses phrases
 * that were candidates.
 */
static void mm_phrase_reset_successors(MM_Phrase *a_This)
{
	/* Reset all successor phrases if there are any
	 */
	for(uint8_t i = 0; i < a_This->n_successors; ++i) {
		MM_CALL_VIRT_METHOD(a_This->successors[i], reset);
	}
}
	
static void mm_abort_magic_melody(void)
{		
	if(!mm_state.current_phrase) { return; }
	
	MM_PRINTF("Aborting magic melody\n");
	
	/* The frase could not be parsed. Reset any previous phrases.
	*/
	mm_phrase_reset_successors(mm_state.current_phrase);
	
	/* Cleanup and issue all keypresses as if they happened without parsing a melody
	*/
	mm_flush_stored_keyevents();
	
	mm_state.n_keys_changed = 0;
	mm_state.current_phrase = NULL;
}

static void mm_phrase_trigger_action(MM_Phrase *a_MM_Phrase) {
	
	MM_PRINTF("*\n");
	
	switch(a_MM_Phrase->action.type) {
		case MM_Action_Keycode:
	
			if(a_MM_Phrase->action.data.keycode != 0) {
				
				/* Construct a dummy record
				*/
				keyrecord_t record;
					record.event.key.row = 0;
					record.event.key.col = 0;
					record.event.pressed = true;
					record.event.time = timer_read();
					
				/* Use the quantum/tmk system to trigger the action
				 * thereby using a fictituous a key (0, 0) with which the action 
				 * keycode is associated. We pretend that the respective key
				 * was hit and released to make sure that any action that
				 * requires both events is correctly processed.
				 * Unfortunatelly this means that some actions that
				 * require keys to be held do not make sense, e.g.
				 * modifier keys or MO(...), etc.
				 */
				
				uint16_t configured_keycode = keycode_config(a_MM_Phrase->action.data.keycode);
				
				action_t action = action_for_configured_keycode(configured_keycode); 
			
				process_action(&record, action);
				
				record.event.pressed = false;
				record.event.time = timer_read();
				
				process_action(&record, action);
			}
			break;
		case MM_Action_User_Callback:
			
			if(a_MM_Phrase->action.data.user_callback.func) {
				a_MM_Phrase->action.data.user_callback.func(
					a_MM_Phrase->action.data.user_callback.user_data);
			}
			break;
	}
}

static bool mm_trigger_action_of_most_recent_phrase(void)
{			
	if(!mm_state.current_phrase) { return false; }
	
	MM_PRINTF("Triggering action of most recent phrase\n");
	
	MM_Phrase *cur_phrase = mm_state.current_phrase;
	
	while(cur_phrase) {
		
		switch(cur_phrase->action.type) {
			case MM_Action_Undefined:
			case MM_Action_None:
				return false;
				break;
			case MM_Action_Transparent:
				cur_phrase = cur_phrase->parent;
				break;
			default:
				mm_phrase_trigger_action(cur_phrase);
				return true;
		}
	}
	
	return false;
}

static void mm_on_timeout(void)
{
	if(!mm_state.current_phrase) { return; }
	
	/* The frase could not be parsed. Reset any previous phrases.
	*/
	mm_phrase_reset_successors(mm_state.current_phrase);
	
	/* It timeout was hit, we either trigger the most recent action
	 * (e.g. necessary for tap dances) or flush the keyevents
	 * that happend until this point
	 */
	
	bool action_triggered 
		= mm_trigger_action_of_most_recent_phrase();
	
	/* Cleanup and issue all keypresses as if they happened without parsing a melody
	*/
	if(action_triggered) {
		mm_flush_stored_keyevents();
	}
	
	mm_state.n_keys_changed = 0;
	mm_state.current_phrase = NULL;
}

static uint8_t mm_phrase_consider_keychange(	MM_Phrase **current_phrase,
												uint16_t keycode, 
												keyrecord_t *record) 
{
	/* Loop over all phrases and inform them about the 
	 * keychange 
	 */
	bool all_successors_invalid = true;
	
	MM_Phrase *a_current_phrase = *current_phrase;
	
	mm_store_keychange(keycode, record);
	
	uint8_t layer = biton32(layer_state);
	
// 	MM_PRINTF("Processing key\n");
	
	#if DEBUG_MAGIC_MELODIES
	if(record->event.pressed) {
		MM_PRINTF("v");
	}
	else {
		MM_PRINTF("^");
	}
	#endif
	
	bool any_phrase_completed = false;
		
	/* Pass the keypress to the phrases and let them decide it they 
	 * can use it. If a phrase cannot it becomes invalid and is not
	 * processed further on this node level. This speeds up processing.
	 */
	for(uint8_t i = 0; i < a_current_phrase->n_successors; ++i) {
		
		// MM_CALL_VIRT_METHOD(a_current_phrase->successors[i], print_self);
		
		/* Accept only paths through the search tree whose
		 * nodes' layer tags are lower or equal the current layer
		 */
		if(a_current_phrase->successors[i]->layer > layer) { continue; }
		
		if(a_current_phrase->successors[i]->state == MM_Phrase_Invalid) {
			continue;
		}
		
		uint8_t successor_process_result 
			= a_current_phrase->successors[i]
					->vtable->successor_consider_keychange(	
								a_current_phrase->successors[i], 
								keycode,
								record
						);
								
		switch(successor_process_result) {
			
			case MM_Phrase_In_Progress:
				all_successors_invalid = false;
				
				break;
				
			case MM_Phrase_Completed:
				
				all_successors_invalid = false;
				any_phrase_completed = true;

				break;
			case MM_Phrase_Invalid:
// 				MM_PRINTF("Phrase invalid");
				break;
		}
	}
	
	/* If all successors are invalid, the keystroke chain does not
	 * match any defined melody.
	 */
	if(all_successors_invalid) {
					
		return MM_Phrase_Invalid;
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
		
// 			MM_CALL_VIRT_METHOD(a_current_phrase->successors[i], print_self);
		
			/* Accept only paths through the search tree whose
			* nodes' layer tags are lower or equal the current layer
			*/
			if(a_current_phrase->successors[i]->layer > layer) { continue; }
			
			if(a_current_phrase->successors[i]->state != MM_Phrase_Completed) {
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
		mm_phrase_reset_successors(a_current_phrase);
		
		/* Replace the current phrase.
		*/
		*current_phrase = a_current_phrase->successors[match_id];
		a_current_phrase = *current_phrase;
			
		if(0 == a_current_phrase->n_successors) {
			
			/* The melody is completed. Trigger the action that is associated
			* with the leaf node.
			*/
			MM_CALL_VIRT_METHOD(a_current_phrase, trigger_action);
			
			mm_state.current_phrase = NULL;
			
			return MM_Phrase_Completed;
		}
		else {
			
			/* The melody is still in progress. We continue with the 
			 * new current node as soon as the next keystroke happens.
			 */
			return MM_Phrase_In_Progress;
		}
	}
	
	return MM_Phrase_In_Progress;
}

static void mm_phrase_allocate_successors(MM_Phrase *a_This, uint8_t n_successors) {

	 a_This->successors 
		= (struct MM_PhraseStruct **)malloc(n_successors*sizeof(struct MM_PhraseStruct*));
	 a_This->n_allocated_successors = n_successors;
}

static void mm_phrase_grow_successors(MM_Phrase *a_This) {

	if(a_This->n_allocated_successors == 0) {
		
		mm_phrase_allocate_successors(a_This, 1);
	}
	else {
		MM_Phrase **oldSucessors = a_This->successors;
		
		mm_phrase_allocate_successors(a_This, 2*a_This->n_allocated_successors);
			
		for(uint8_t i = 0; i < a_This->n_successors; ++i) {
			a_This->successors[i] = oldSucessors[i];
		}
		
		free(oldSucessors); 
	}
}

static void mm_phrase_add_successor(MM_Phrase *a_This, MM_Phrase *successor) {
	
	if(a_This->n_allocated_successors == a_This->n_successors) {
		mm_phrase_grow_successors(a_This);
	}
	
	a_This->successors[a_This->n_successors] = successor;
	
	successor->parent = a_This;
	
	++a_This->n_successors;
}

static void mm_phrase_free(MM_Phrase *a_This);

static void mm_phrase_free_successors(MM_Phrase *a_This)
{
	if(!a_This->successors) { return; }
	
	for(uint8_t i = 0; i < a_This->n_allocated_successors; ++i) {
		
		mm_phrase_free(a_This->successors[i]);
	}
	
	free(a_This->successors);
	
	a_This->successors = NULL;
	a_This->n_allocated_successors = 0;
}

static MM_Phrase* mm_phrase_destroy(MM_Phrase *a_MM_Phrase) {

	mm_phrase_free_successors(a_MM_Phrase);
	
	return a_MM_Phrase;
}

static bool mm_phrase_equals(MM_Phrase *p1, MM_Phrase *p2) 
{
	if(p1->vtable != p2->vtable) { return false; }
	
	return p1->vtable->equals(p1, p2);
}

static void mm_phrase_free(MM_Phrase *a_This) {
	
	MM_CALL_VIRT_METHOD(a_This, destroy);

	free(a_This);
}

static MM_Phrase* mm_phrase_get_equivalent_successor(
														MM_Phrase *a_MM_Phrase,
														MM_Phrase *sample) {
	
	if(a_MM_Phrase->n_successors == 0) { return NULL; }
	
	for(uint8_t i = 0; i < a_MM_Phrase->n_successors; ++i) {
		if(mm_phrase_equals(
											a_MM_Phrase->successors[i], 
											sample)
		  ) {
			return a_MM_Phrase->successors[i];
		}
	}
	
	return NULL;
}

static void mm_phrase_print_self(MM_Phrase *p)
{
	MM_PRINTF("phrase (0x%" PRIXPTR ")\n", (void*)p);
	MM_PRINTF("   parent: 0x%" PRIXPTR "\n", (void*)&p->parent);
	MM_PRINTF("   successors: 0x%" PRIXPTR "\n", (void*)&p->successors);
	MM_PRINTF("   n_allocated_successors: %d\n", p->n_allocated_successors);
	MM_PRINTF("   n_successors: %d\n", p->n_successors);
	MM_PRINTF("   action.type: %d\n", p->action.type);
	MM_PRINTF("   action_keycode: %d\n", p->action.data.keycode);
	MM_PRINTF("   action_user_func: %0x%" PRIXPTR "\n", p->action.data.user_callback.func);
	MM_PRINTF("   action_user_data: %0x%" PRIXPTR "\n", p->action.data.user_callback.user_data);
	MM_PRINTF("   state: %d\n", p->state);
	MM_PRINTF("   layer: %d\n", p->layer);
}

static MM_Phrase_Vtable mm_phrase_vtable =
{
	.consider_keychange 
		= (MM_Phrase_Consider_Keychange_Fun) mm_phrase_consider_keychange,
	.successor_consider_keychange 
		= NULL,
	.reset 
		= (MM_Phrase_Reset_Fun) mm_phrase_reset,
	.trigger_action 
		= (MM_Phrase_Trigger_Action_Fun) mm_phrase_trigger_action,
	.destroy 
		= (MM_Phrase_Destroy_Fun) mm_phrase_destroy,
	.equals
		= NULL,
	.print_self
		= (MM_Phrase_Print_Self_Fun) mm_phrase_print_self,
};

static MM_Phrase *mm_phrase_new(MM_Phrase *a_MM_Phrase) {
	
    a_MM_Phrase->vtable = &mm_phrase_vtable;
	 
    a_MM_Phrase->parent = NULL;
    a_MM_Phrase->successors = NULL;
	 a_MM_Phrase->n_allocated_successors = 0;
    a_MM_Phrase->n_successors = 0;
    a_MM_Phrase->action.data.keycode = 0;
    a_MM_Phrase->action.type = MM_Action_Undefined;
    a_MM_Phrase->action.data.user_callback.func = NULL;
    a_MM_Phrase->action.data.user_callback.user_data = NULL;
    a_MM_Phrase->state = MM_Phrase_In_Progress;
    a_MM_Phrase->layer = 0;
	 
    return a_MM_Phrase;
}

/*****************************************************************************
 Notes 
*****************************************************************************/

typedef struct {
	
	MM_Phrase phrase_inventory;
	 
	keypos_t keypos;
	bool pressed;
	 
} MM_Note;

static uint8_t mm_note_successor_consider_keychange(	
											MM_Note *a_This,
											uint16_t keycode, 
											keyrecord_t *record) 
{	
	assert(a_This->phrase_inventory.state == MM_Phrase_In_Progress);
	
	/* Set state appropriately 
	 */
	if(mm_keypos_equal(a_This->keypos, record->event.key)) {
		
		if(record->event.pressed) {
			a_This->pressed = true;
			a_This->phrase_inventory.state = MM_Phrase_In_Progress;
		}
		else {
			if(a_This->pressed) {
	// 		MM_PRINTF("note successfully finished\n");
				MM_PRINTF("N");
				a_This->phrase_inventory.state = MM_Phrase_Completed;
			}
		}
	}
	else {
// 		MM_PRINTF("note invalid\n");
		a_This->phrase_inventory.state = MM_Phrase_Invalid;
	}
	
	return a_This->phrase_inventory.state;
}

static void mm_note_reset(MM_Note *a_This) 
{
	mm_phrase_reset((MM_Phrase*)a_This);
	
	a_This->pressed = false;
}

static MM_Note *mm_note_alloc(void) {
    return (MM_Note*)malloc(sizeof(MM_Note));
}

static bool mm_note_equals(MM_Note *n1, MM_Note *n2) 
{
	return mm_keypos_equal(n1->keypos, n2->keypos);
}

static void mm_note_print_self(MM_Note *p)
{
	mm_phrase_print_self((MM_Phrase*)p);
	
	MM_PRINTF("note\n");
	MM_PRINTF("   row: %d\n", p->keypos.row);
	MM_PRINTF("   col: %d\n", p->keypos.col);
}

static MM_Phrase_Vtable mm_note_vtable =
{
	.consider_keychange 
		= (MM_Phrase_Consider_Keychange_Fun) mm_phrase_consider_keychange,
	.successor_consider_keychange 
		= (MM_Phrase_Successor_Consider_Keychange_Fun) mm_note_successor_consider_keychange,
	.reset 
		= (MM_Phrase_Reset_Fun) mm_note_reset,
	.trigger_action 
		= (MM_Phrase_Trigger_Action_Fun) mm_phrase_trigger_action,
	.destroy 
		= (MM_Phrase_Destroy_Fun) mm_phrase_destroy,
	.equals
		= (MM_Phrase_Equals_Fun) mm_note_equals,
	.print_self
		= (MM_Phrase_Print_Self_Fun) mm_note_print_self
};

static MM_Note *mm_note_new(MM_Note *a_note)
{
    /* Explict call to parent constructor 
	  */
    mm_phrase_new((MM_Phrase*)a_note);

    a_note->phrase_inventory.vtable = &mm_note_vtable;
		
	 a_note->keypos.row = 100;
	 a_note->keypos.col = 100;
	 
	 a_note->pressed = false;
	 
    return a_note;
}

/*****************************************************************************
 Chords 
*****************************************************************************/

typedef struct {
	
	MM_Phrase phrase_inventory;
	 
	uint8_t n_members;
	keypos_t *keypos;
	bool *member_active;
	uint8_t n_chord_keys_pressed;
	 
} MM_Chord;

static uint8_t mm_chord_successor_consider_keychange(	
											MM_Chord *a_This,
											uint16_t keycode, 
											keyrecord_t *record) 
{
	bool key_part_of_chord = false;
	
	assert(a_This->n_members != 0);
	
	assert(a_This->phrase_inventory.state == MM_Phrase_In_Progress);
	
	/* Check it the key is part of the current chord 
	 */
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		
		if(mm_keypos_equal(a_This->keypos[i], record->event.key)) {
			
			key_part_of_chord = true;
			
			if(record->event.pressed) {
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
		if(record->event.pressed) {
			a_This->phrase_inventory.state = MM_Phrase_Invalid;
			return a_This->phrase_inventory.state;
		}
	}
	
	if(a_This->n_chord_keys_pressed == a_This->n_members) {
		
		/* Chord completed
		 */
		a_This->phrase_inventory.state = MM_Phrase_Completed;
 		MM_PRINTF("C");
	}
	
	return a_This->phrase_inventory.state;
}

static void mm_chord_reset(MM_Chord *a_This) 
{
	mm_phrase_reset((MM_Phrase*)a_This);
	
	a_This->n_chord_keys_pressed = 0;
	
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static void mm_chord_deallocate_member_storage(MM_Chord *a_This) {	
	
	if(a_This->keypos) {
		free(a_This->keypos);
		a_This->keypos = NULL;
	}
	if(a_This->member_active) {
		free(a_This->member_active);
		a_This->member_active = NULL;
	}
}

static void mm_chord_resize(MM_Chord *a_This, 
							uint8_t n_members)
{
	mm_chord_deallocate_member_storage(a_This);
	
	a_This->n_members = n_members;
	a_This->keypos = (keypos_t *)malloc(n_members*sizeof(keypos_t));
	a_This->member_active = (bool *)malloc(n_members*sizeof(bool));
	a_This->n_chord_keys_pressed = 0;
	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static MM_Chord* mm_chord_destroy(MM_Chord *a_This) {
	
	mm_chord_deallocate_member_storage(a_This);

	return a_This;
}

static MM_Chord *mm_chord_alloc(void){
    return (MM_Chord*)malloc(sizeof(MM_Chord));
}

static bool mm_chord_equals(MM_Chord *c1, MM_Chord *c2) 
{
	if(c1->n_members != c2->n_members) { return false; }
	
	for(uint8_t i = 0; i < c1->n_members; ++i) {
		if(!mm_keypos_equal(c1->keypos[i], c2->keypos[i])) { return false; }
	}
	
	return true;
}

static void mm_chord_print_self(MM_Chord *c)
{
	mm_phrase_print_self((MM_Phrase*)c);
	
	MM_PRINTF("chord\n");
	MM_PRINTF("   n_members: %d\n", c->n_members);
	MM_PRINTF("   n_chord_keys_pressed: %d\n", c->n_chord_keys_pressed);
	
	for(uint8_t i = 0; i < c->n_members; ++i) {
		MM_PRINTF("      row: %d, col: %d, active: %d\n", 
				  c->keypos[i].row, c->keypos[i].col, c->member_active[i]);
	}
}

static MM_Phrase_Vtable mm_chord_vtable =
{
	.consider_keychange 
		= (MM_Phrase_Consider_Keychange_Fun) mm_phrase_consider_keychange,
	.successor_consider_keychange 
		= (MM_Phrase_Successor_Consider_Keychange_Fun) mm_chord_successor_consider_keychange,
	.reset 
		= (MM_Phrase_Reset_Fun) mm_chord_reset,
	.trigger_action 
		= (MM_Phrase_Trigger_Action_Fun) mm_phrase_trigger_action,
	.destroy 
		= (MM_Phrase_Destroy_Fun) mm_chord_destroy,
	.equals
		= (MM_Phrase_Equals_Fun) mm_chord_equals,
	.print_self
		= (MM_Phrase_Print_Self_Fun) mm_chord_print_self
};

static MM_Chord *mm_chord_new(MM_Chord *a_chord){
    
	/* Explict call to parent constructor
	 */
	mm_phrase_new((MM_Phrase*)a_chord);

	a_chord->phrase_inventory.vtable = &mm_chord_vtable;
		
	/* Initialize the chord
	 */
	a_chord->n_members = 0;
	a_chord->keypos = NULL;
	a_chord->member_active = NULL;
	a_chord->n_chord_keys_pressed = 0;

	return a_chord;
}

/*****************************************************************************
 Clusters 
*****************************************************************************/

typedef struct {
	
	MM_Phrase phrase_inventory;
	 
	uint8_t n_members;
	keypos_t *keypos;
	bool *member_active;
	uint8_t n_cluster_keys_pressed;
	 
} MM_Cluster;

static uint8_t mm_cluster_successor_consider_keychange(	
											MM_Cluster *a_This,
											uint16_t keycode, 
											keyrecord_t *record) 
{
	bool key_part_of_cluster = false;
	
	assert(a_This->n_members != 0);
	
	/* Only react on pressed keys. Allow cluster members to be released. 
	 * Every key only must be pressed once.
	 */
	if(!record->event.pressed) { return MM_Phrase_In_Progress; }
	
	assert(a_This->phrase_inventory.state == MM_Phrase_In_Progress);
	
	/* Check it the key is part of the current chord 
	 */
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		
		if(mm_keypos_equal(a_This->keypos[i], record->event.key)) {
			
			key_part_of_cluster = true;
			
			if(record->event.pressed) {
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
		if(record->event.pressed) {
			a_This->phrase_inventory.state = MM_Phrase_Invalid;
			return a_This->phrase_inventory.state;
		}
	}
	
	if(a_This->n_cluster_keys_pressed == a_This->n_members) {
		
		/* Cluster completed
		 */
		a_This->phrase_inventory.state = MM_Phrase_Completed;
 		MM_PRINTF("O");
	}
	
	return a_This->phrase_inventory.state;
}

static void mm_cluster_reset(MM_Cluster *a_This) 
{
	mm_phrase_reset((MM_Phrase*)a_This);
	
	a_This->n_cluster_keys_pressed = 0;
	
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static void mm_cluster_deallocate_member_storage(MM_Cluster *a_This) {	
	
	if(a_This->keypos) {
		free(a_This->keypos);
		a_This->keypos = NULL;
	}
	if(a_This->member_active) {
		free(a_This->member_active);
		a_This->member_active = NULL;
	}
}

static void mm_cluster_resize(MM_Cluster *a_This, 
							uint8_t n_members)
{
	mm_cluster_deallocate_member_storage(a_This);
	
	a_This->n_members = n_members;
	a_This->keypos = (keypos_t *)malloc(n_members*sizeof(keypos_t));
	a_This->member_active = (bool *)malloc(n_members*sizeof(bool));
	a_This->n_cluster_keys_pressed = 0;
	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static bool mm_is_cluster_member(MM_Cluster *c, keypos_t key)
{
	for(uint8_t i = 0; i < c->n_members; ++i) {
		if(mm_keypos_equal(c->keypos[i], key)) {
			return true;
		}
	}
	
	return false;
}

static bool mm_cluster_equals(MM_Cluster *c1, MM_Cluster *c2) 
{
	if(c1->n_members != c2->n_members) { return false; }
	
	for(uint8_t i = 0; i < c1->n_members; ++i) {
		if(!mm_is_cluster_member(c1, c2->keypos[i])) { return false; }
	}
	
	return true;
}

static MM_Cluster* mm_cluster_destroy(MM_Cluster *a_This) {
	
	mm_cluster_deallocate_member_storage(a_This);

    return a_This;
}

static MM_Cluster *mm_cluster_alloc(void){
    return (MM_Cluster*)malloc(sizeof(MM_Cluster));
} 

static void mm_cluster_print_self(MM_Cluster *c)
{
	mm_phrase_print_self((MM_Phrase*)c);
	
	MM_PRINTF("cluster\n");
	MM_PRINTF("   n_members: %d\n", c->n_members);
	MM_PRINTF("   n_cluster_keys_pressed: %d\n", c->n_cluster_keys_pressed);
	
	for(uint8_t i = 0; i < c->n_members; ++i) {
		MM_PRINTF("      row: %d, col: %d, active: %d\n", 
				  c->keypos[i].row, c->keypos[i].col, c->member_active[i]);
	}
}

static MM_Phrase_Vtable mm_cluster_vtable =
{
	.consider_keychange 
		= (MM_Phrase_Consider_Keychange_Fun) mm_phrase_consider_keychange,
	.successor_consider_keychange 
		= (MM_Phrase_Successor_Consider_Keychange_Fun) mm_cluster_successor_consider_keychange,
	.reset 
		= (MM_Phrase_Reset_Fun) mm_cluster_reset,
	.trigger_action 
		= (MM_Phrase_Trigger_Action_Fun) mm_phrase_trigger_action,
	.destroy 
		= (MM_Phrase_Destroy_Fun) mm_cluster_destroy,
	.equals
		= (MM_Phrase_Equals_Fun) mm_cluster_equals,
	.print_self
		= (MM_Phrase_Print_Self_Fun) mm_cluster_print_self
};

static MM_Cluster *mm_cluster_new(MM_Cluster *a_cluster) {
	
	/*Explict call to parent constructor
	*/
	mm_phrase_new((MM_Phrase*)a_cluster);

	a_cluster->phrase_inventory.vtable = &mm_cluster_vtable;
	
	a_cluster->n_members = 0;
	a_cluster->keypos = NULL;
	a_cluster->member_active = NULL;
	a_cluster->n_cluster_keys_pressed = 0;
	
	return a_cluster;
}

/*****************************************************************************
 * Public access functions for creation and destruction of the melody tree
 *****************************************************************************/

static void mm_init(void) {

	if(!mm_state.melody_root_initialized) {
		
		/* Initialize the melody root
		 */
		mm_phrase_new(&mm_state.melody_root);
		
		mm_state.melody_root_initialized = true;
	}
}

void mm_finalize_magic_melodies(void) {
	mm_phrase_free_successors(&mm_state.melody_root);
}

void *mm_create_note(keypos_t keypos)
{
    MM_Note *a_note = (MM_Note*)mm_note_new(mm_note_alloc());
	 
	 a_note->keypos = keypos;
	 
	 /* Return the new end of the melody */
	 return a_note;
}

void *mm_create_chord(	keypos_t *keypos,
								uint8_t n_members)
{
	MM_Chord *a_chord = (MM_Chord*)mm_chord_new(mm_chord_alloc());
	 	 
	mm_chord_resize(a_chord, n_members);
	 
	for(uint8_t i = 0; i < n_members; ++i) {
		a_chord->keypos[i] = keypos[i];
	}
	 
	/* Return the new end of the melody */
	return a_chord;
}

void *mm_create_cluster(
									keypos_t *keypos,
									uint8_t n_members)
{
	MM_Cluster *a_cluster = (MM_Cluster*)mm_cluster_new(mm_cluster_alloc());
	 
	mm_cluster_resize(a_cluster, n_members);
	 	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_cluster->keypos[i] = keypos[i];
	}
	
	/* Return the new end of the melody */
	return a_cluster;
}

#if DEBUG_MAGIC_MELODIES
static void mm_recursively_print_melody(MM_Phrase *p)
{
	if(	p->parent
		&& p->parent != &mm_state.melody_root) {
		
		mm_recursively_print_melody(p->parent);
	}
	
	MM_CALL_VIRT_METHOD(p, print_self);
}
#endif

static void *mm_melody_from_list(	uint8_t layer,
												MM_Phrase **phrases,
												int n_phrases)
{ 
	MM_Phrase *parent_phrase = &mm_state.melody_root;
	
	MM_PRINTF("   %d members\n", n_phrases);
	
	for (int i = 0; i < n_phrases; i++) { 
		
		MM_Phrase *cur_phrase = phrases[i];
		
		/* If the action type is undefined, we set action 
		 * type none, which means that no fall through happens
		 * in case of timeout.
		 */
		if(cur_phrase->action.type == MM_Action_Undefined) {
			cur_phrase->action.type = MM_Action_None;
		}
		
		MM_Phrase *equivalent_successor 
			= mm_phrase_get_equivalent_successor(parent_phrase, cur_phrase);
			
		MM_PRINTF("   member %d: ", i);
		
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
			
			#if DEBUG_MAGIC_MELODIES
			if(cur_phrase->action.type != equivalent_successor->action.type) {
				MM_ERROR("Incompatible action types detected\n");
			}
			#endif
			
			MM_PRINTF("already present\n");
			
			parent_phrase = equivalent_successor;
			
			if(layer < equivalent_successor->layer) {
				
				equivalent_successor->layer = layer;
			}
			
			/* The successor is already registered in the search tree. Delete the newly created version.
			 */			
			mm_phrase_free(cur_phrase);
		}
		else {
			
			MM_PRINTF("newly defined\n");
			
			#if DEBUG_MAGIC_MELODIES
			
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
					MM_ERROR("Conflicting melodies detected. "
						"The phrases of the conflicting melodies are listed below.\n");
					
					MM_ERROR("Previously defined:\n");
					mm_recursively_print_melody(equivalent_successor);
					
					MM_ERROR("Conflicting:\n");
					for (int i = 0; i < n_phrases; i++) {
						MM_CALL_VIRT_METHOD(phrases[i], print_self);
					}
				}
			}
			#endif /* if DEBUG_MAGIC_MELODIES */
					
			cur_phrase->layer = layer;
			
			mm_phrase_add_successor(parent_phrase, cur_phrase);
			
			parent_phrase = cur_phrase;
		}
	}
	
	/* Return the leaf phrase 
	 */
	return parent_phrase;
}

void *mm_melody(		uint8_t layer, 
							int count, 
							...)
{ 
	MM_PRINTF("Adding magic melody\n");
	
	mm_init();
	
	va_list ap;

	va_start (ap, count);         /* Initialize the argument list. */
  	
	MM_Phrase **phrases 
		= (MM_Phrase **)malloc(count*sizeof(MM_Phrase *));
	
	for (int i = 0; i < count; i++) { 
		
		phrases[i] = va_arg (ap, MM_Phrase *);
	}
	
	MM_Phrase *leafPhrase 
		= mm_melody_from_list(layer, phrases, count);
	
	free(phrases);

	va_end (ap);                  /* Clean up. */
	
	return leafPhrase;
}

void *mm_chord(		uint8_t layer, 
							MM_Action action, 
							keypos_t *keypos,
							uint8_t n_members)
{   	
	MM_PRINTF("Adding chord\n");
	
	MM_Phrase *cPhrase = 
		(MM_Phrase *)mm_create_chord(keypos, n_members);
		
	cPhrase->action = action;
		
	MM_Phrase *phrases[1] = { cPhrase };
	
	MM_Phrase *leafPhrase 
		= mm_melody_from_list(layer, phrases, 1);
		
	return leafPhrase;
}

void *mm_cluster(		uint8_t layer, 
							MM_Action action, 
							keypos_t *keypos,
							uint8_t n_members)
{   	
	MM_PRINTF("Adding cluster\n");
	
	MM_Phrase *cPhrase = 
		(MM_Phrase *)mm_create_cluster(keypos, n_members);
		
	cPhrase->action = action;
	
	MM_Phrase *phrases[1] = { cPhrase };
	
	MM_Phrase *leafPhrase 
		= mm_melody_from_list(layer, phrases, 1);
		
	return leafPhrase;
}

void *mm_single_note_line(	uint8_t layer,
							MM_Action action, 
							int count, ...)
{
	MM_PRINTF("Adding single note line\n");
	
	mm_init();
	
	va_list ap;

	va_start (ap, count);         /* Initialize the argument list. */
  
	MM_Phrase **phrases 
		= (MM_Phrase **)malloc(count*sizeof(MM_Phrase *));
		
	for (int i = 0; i < count; i++) {
		
		keypos_t curKeypos = va_arg (ap, keypos_t); 

		void *new_note = mm_create_note(curKeypos);
		
		phrases[i] = (MM_Phrase *)new_note;
	}
	
	mm_phrase_store_action(phrases[count - 1], action);
	
	MM_Phrase *leaf_phrase 
		= mm_melody_from_list(layer, phrases, count);
	
	free(phrases);

	va_end (ap);                  /* Clean up. */
  
	return leaf_phrase;
}

void *mm_tap_dance(	uint8_t layer,
							keypos_t curKeypos,
							uint8_t default_action_type,
							uint8_t n_vargs,
							...
							)
{
	MM_PRINTF("Adding tap dance\n");
	
	mm_init();
	
	va_list ap;

	va_start (ap, n_vargs);         /* Initialize the argument list. */
	
	uint8_t n_tap_definitions = n_vargs/2;
		
	int n_taps = 0;
	for (uint8_t i = 0; i < n_tap_definitions; i++) {
		
		int tap_count = va_arg (ap, int); 
		va_arg (ap, MM_Action); /* not needed here */
		
		if(tap_count > n_taps) {
			n_taps = tap_count;
		}
	}
	
	if(n_taps == 0) { return NULL; }
	
	MM_Phrase **phrases 
		= (MM_Phrase **)malloc(n_taps*sizeof(MM_Phrase *));
	
	for (int i = 0; i < n_taps; i++) {
		
		void *new_note = mm_create_note(curKeypos);
		
		phrases[i] = (MM_Phrase *)new_note;
		
		phrases[i]->action.type = default_action_type;
	}
	
	va_start (ap, n_vargs);         /* Initialize the argument list. */
	
	for (uint8_t i = 0; i < n_tap_definitions; i++) {
		
		int tap_count = va_arg (ap, int); 
		MM_Action action = va_arg (ap, MM_Action);

		mm_phrase_store_action(phrases[tap_count - 1], action);
	}
			
	MM_Phrase *leafPhrase 
		= mm_melody_from_list(layer, phrases, n_taps);
	
	free(phrases);
	
	return leafPhrase;
}

/* Returns phrase__
 */
void *mm_set_action(	void *phrase__,
							MM_Action action)
{
	MM_Phrase *phrase = (MM_Phrase *)phrase__;
	
	mm_phrase_store_action(phrase, action);
	
	return phrase__;
}

bool mm_check_timeout(void)
{
	if(mm_state.current_phrase
		&& (timer_elapsed(mm_state.time_last_keypress) 
				> mm_state.keypress_timeout)
	  ) {
		
		MM_PRINTF("Magic melody timeout hit\n");
	
		/* Too late...
			*/
		mm_on_timeout();
	
		return true;
	}
	
	return false;
}

bool mm_process_magic_melodies(uint16_t keycode, 
										 keyrecord_t *record)
{
	/* When a melody could not be finished, all keystrokes are
	 * processed through the process_record_quantum method.
	 * To prevent infinite recursion, we have to temporarily disable 
	 * processing magic melodies.
	 */
	if(mm_state.magic_melodies_temporarily_disabled) { return true; }
	
	/* Early exit if no melody was registered 
	 */
	if(!mm_state.melody_root_initialized) { return true; }
			
	/* If there is no melody processed, we ignore keyups.
	*/	
	if(!mm_state.current_phrase && !record->event.pressed) {
// 		MM_PRINTF("Keyup ignored as no melody currently processed\n");
		return true;
	}
	
	/* Check if the melody is being aborted
	 */
	if(mm_keypos_equal(mm_state.abort_keypos, record->event.key)) {
		
		/* If a melody is in progress, we abort it and consume the abort key.
		 */
		if(mm_state.current_phrase) {
			MM_PRINTF("Processing melodies interrupted by user\n");
			mm_abort_magic_melody();
			return false;
		}
	
		return false;
	}
	
	MM_PRINTF("Starting keyprocessing\n");
	
	if(!mm_state.current_phrase) {
		
// 		MM_PRINTF("New melody \n");
		
		mm_state.current_phrase = &mm_state.melody_root;
		mm_state.time_last_keypress = timer_read();
	}
	else {
		
		if(mm_check_timeout()) {
			
			/* Timeout hit. Cleanup already done.
			 */
			return false;
		}
		else {
			mm_state.time_last_keypress = timer_read();
		}
	}
	
	uint8_t result 
		= mm_phrase_consider_keychange(	&mm_state.current_phrase,
											keycode,
											record);
		
	switch(result) {
		case MM_Phrase_In_Progress:
		case MM_Phrase_Completed:
			return false;
		case MM_Phrase_Invalid:
			
			MM_PRINTF("-\n");
		
			mm_abort_magic_melody();
			
// 			return false; /* The key(s) have been already processed */
			return true; // Why does this require true to work and 
			// why is t not written?
	}
	
	return true;
}

/* Use this function to define a keypos that always aborts a magic melody
 */
void mm_set_abort_keypos(keypos_t keypos)
{
	mm_state.abort_keypos = keypos;
}

void mm_set_timeout_ms(uint16_t timeout)
{
	mm_state.keypress_timeout = timeout;
}

#endif /*ifdef MAGIC_MELODIES_ENABLE*/
