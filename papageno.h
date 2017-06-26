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
#ifndef PAPAGENO_H
#define PAPAGENO_H

#include <stdbool.h>
#include <inttypes.h>

#ifdef DEBUG_PAPAGENO
typedef void (*PG_Printf_Fun)(const char *, ...);
void pg_set_printf(PG_Printf_Fun printf_fun);
#endif

/* Note: All definition functions return a pointer to the melody that can be 
 *       passed e.g. to pg_set_action.
 */
enum PG_Action_Flags {
	PG_Action_Undefined = 0,
	PG_Action_Fallthrough = 1 << 0,
	PG_Action_Noop_Fallthrough = 1 << 2,
};

enum PG_Key_Flush {
	PG_Key_Flush_Abort = 0,
	PG_Key_Flush_Timeout,
	PG_Key_Flush_Melody_Completed,
	PG_Key_Flush_User
};

typedef void (*PG_User_Callback_Fun)(void *);

typedef struct {
	PG_User_Callback_Fun func;
	void *	user_data;
} PG_User_Callback;

typedef struct {
	PG_User_Callback user_callback;
	uint8_t flags;
} PG_Action;
	
typedef void * PG_Key_Id;
typedef void * PG_Time_Id;

typedef struct {
	PG_Key_Id key_id;
	PG_Time_Id time;
	bool pressed;
} PG_Key_Event;

/* Define single note lines.
 */
void *pg_single_note_line(
							uint8_t layer, 
							PG_Action action, 
							int count, 
							...);

/* Define a chord (all members must be activated/pressed simultaneously).
 */
void *pg_chord(		uint8_t layer,
							PG_Action action,
							PG_Key_Id *key_id,
							uint8_t n_members);

/* Define a cluste (all members must be activated/pressed at least once for
 * the cluster to be considered as completed).
 */
void *pg_cluster(		uint8_t layer, 
							PG_Action action, 
							PG_Key_Id *key_id,
							uint8_t n_members);

/* Define tap dances (great thanks to algernon for the inspiration).
 * The variadic arguments are pairs of keystroke counts and associated actions.
 */
void *pg_tap_dance(	uint8_t layer,
							PG_Key_Id curKeypos,
							uint8_t default_action_flags,
							uint8_t n_tap_definitions,
							...);

#define PG_N_TAPS(S) (2*S)

/* Use the following functions to create complex melodies as sequences of
 * notes, chords and clusters.
 */
void *pg_create_note(PG_Key_Id key_id);

void *pg_create_chord(	
							PG_Key_Id *key_id,
							uint8_t n_members);

void *pg_create_cluster(
							PG_Key_Id *key_id,
							uint8_t n_members);

void *pg_melody(		uint8_t layer,  
							int count,
							...);

/* Use this to modify actions after definition. The function
 * returns the phrase pointer.
 */
void *pg_set_action(
									void *phrase,
									PG_Action action);

/* Configuration functions */

typedef bool (*PG_Key_Id_Equal_Fun)(PG_Key_Id, PG_Key_Id);

/* Define a custom key id comparison function.
 * The default is to just compare the PG_Key_Id 
 * as raw data.
 */
void pg_set_key_id_equal_function(PG_Key_Id_Equal_Fun fun);

/* Define a key that aborts melody processing 
 */
void pg_set_abort_key_id(PG_Key_Id key_id);

/* Set the melody processing timeout 
 */
void pg_set_timeout(PG_Time_Id timeout);

typedef bool (*PG_Key_Event_Processor_Fun)(PG_Key_Event *key_event,
														 uint8_t state_flag, 
														 void *user_data);

void pg_set_key_processor(PG_Key_Event_Processor_Fun fun);

typedef void (*PG_Time_Fun)(PG_Time_Id *time);

void pg_set_time_function(PG_Time_Fun fun);

typedef void (*PG_Time_Difference_Fun)(PG_Time_Id time1, PG_Time_Id time2, PG_Time_Id *delta);

void pg_set_time_difference_function(PG_Time_Difference_Fun fun);

/* Returns a positive value when time1 is greater time2, a negative value it time 1 is less time 2 and zero if both are equal.
 */
typedef int8_t (*PG_Time_Comparison_Fun)(PG_Time_Id time1, PG_Time_Id time2);

void pg_set_time_comparison_function(PG_Time_Comparison_Fun fun);

void pg_set_enabled(bool state);

/* Is finalization required?
 */
void pg_finalize(void);

/* Call this function from process_record_user to enable processing magic melodies
 */
// bool pg_process_magic_melodies(uint16_t keycode, keyrecord_t *record);
bool pg_process_key_event(PG_Key_Event *key_event,
								  uint8_t cur_layer);

/* Add this function to your implementation of matrix_scan_user.
 * It returns true on timeout.
 */
bool pg_check_timeout(void);

void pg_flush_stored_key_events(
								uint8_t state_flag, 
								PG_Key_Event_Processor_Fun key_processor,
								void *user_data);

#define PG_NO_ACTION KC_NO

#define PG_NUM_KEYS(S) \
	(sizeof(S)/sizeof(PG_Key_Id))
	
#define PG_ACTION_USER_CALLBACK(FUNC, USER_DATA) \
	(PG_Action) { \
		.flags = PG_Action_Undefined, \
		.user_callback = (PG_User_Callback) { \
			.func = FUNC, \
			.user_data = USER_DATA \
		} \
	}
	
#define PG_ACTION_NOOP \
	(PG_Action) { \
		.flags = PG_Action_None, \
		.user_callback = (PG_User_Callback) { \
			.func = NULL, \
			.user_data = NULL \
		} \
	}

#endif
