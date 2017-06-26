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
#ifndef PROCESS_MAGIC_MELODIES_H
#define PROCESS_MAGIC_MELODIES_H

#ifdef MAGIC_MELODIES_ENABLE

#include "quantum.h"
#include "tmk_core/common/keyboard.h"

/* Note: All definition functions return a pointer to the melody that can be 
 *       passed e.g. to mm_set_action.
 */

enum {
	MM_Action_Undefined = 0,
	MM_Action_None,
	MM_Action_Transparent,
	MM_Action_Keycode,
	MM_Action_User_Callback
};

typedef void (*MM_User_Callback_Fun)(void *);

typedef struct {
	MM_User_Callback_Fun func;
	void *	user_data;
} MM_User_Callback;

typedef union {
	uint16_t keycode;
	MM_User_Callback user_callback;
} MM_ActionDataUnion;

typedef struct {
	MM_ActionDataUnion data;
	uint8_t type;
} MM_Action;
	
#define MM_ACTION_KEYCODE(KK) \
	(MM_Action) {	\
		.type = MM_Action_Keycode, \
		.data = (MM_ActionDataUnion) { \
			.keycode = KK  \
		} \
	}
	
#define MM_ACTION_USER_CALLBACK(FUNC, USER_DATA) \
	(MM_Action) { \
		.type = MM_Action_User_Callback, \
		.data = (MM_ActionDataUnion) { \
			.user_callback = (MM_User_Callback) { \
				.func = FUNC, \
				.user_data = USER_DATA \
			} \
		} \
	}
	
#define MM_ACTION_NOOP \
	(MM_Action) { \
		.type = MM_Action_None, \
		.data = (MM_ActionDataUnion) { \
			.user_callback = (MM_User_Callback) { \
				.func = NULL, \
				.user_data = NULL \
			} \
		} \
	}

/* Define single note lines.
 */
void *mm_single_note_line(
							uint8_t layer, 
							MM_Action action, 
							int count, ...);

/* Define a chord (all members must be activated/pressed simultaneously).
 */
void *mm_chord(		uint8_t layer,
							MM_Action action,
							keypos_t *keypos,
							uint8_t n_members);

/* Define a cluste (all members must be activated/pressed at least once for
 * the cluster to be considered as completed).
 */
void *mm_cluster(		uint8_t layer, 
							MM_Action action, 
							keypos_t *keypos,
							uint8_t n_members);

/* Define tap dances (great thanks to algernon for the inspiration).
 * The variadic arguments are pairs of keystroke counts and associated actions.
 */
void *mm_tap_dance(	uint8_t layer,
							keypos_t curKeypos,
							uint8_t default_action_type,
							uint8_t n_tap_definitions,
							...);

#define MM_N_TAPS(S) (2*S)

/* Use the following functions to create complex melodies as sequences of
 * notes, chords and clusters.
 */
void *mm_create_note(keypos_t keypos);

void *mm_create_chord(	
							keypos_t *keypos,
							uint8_t n_members);

void *mm_create_cluster(
							keypos_t *keypos,
							uint8_t n_members);

void *mm_melody(		uint8_t layer,  
							int count, ...);

/* Use this to modify actions after definition. The function
 * returns the phrase pointer.
 */
void *mm_set_action(
									void *phrase,
									MM_Action action);

/* Configuration functions */

/* Define a key that aborts melody processing 
 */
void mm_set_abort_keypos(keypos_t keypos);

/* Set the melody processing timeout 
 */
void mm_set_timeout_ms(uint16_t timeout);

/* Is finalization required?
 */
void mm_finalize_magic_melodies(void);

/* Call this function from process_record_user to enable processing magic melodies
 */
bool mm_process_magic_melodies(uint16_t keycode, keyrecord_t *record);

/* Add this function to your implementation of matrix_scan_user.
 * It returns true on timeout.
 */
bool mm_check_timeout(void);

#define MM_NO_ACTION KC_NO

#define MM_KEYPOS_HEX(COL, ROW) (keypos_t){.row = 0x##ROW, .col = 0x##COL}

#define MM_NUM_KEYS(S) \
	(sizeof(S)/sizeof(keypos_t))

#endif /*ifdef MAGIC_MELODIES_ENABLE*/

#endif
