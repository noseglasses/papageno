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

#include "quantum.h"
#include "tmk_core/common/keyboard.h"

/* This function is defined in quantum/keymap_common.c 
 */
action_t action_for_configured_keycode(uint16_t keycode);

typedef struct {
	uint16_t time_offset;
} PG_QMK_Keycode_Data;

bool pg_process_qmk_key_event(	PG_Key_Event *key_event,
										uint8_t state_flag, 
										void *user_data)
{
	PG_QMK_Keycode_Data *kd = (PG_QMK_Keycode_Data *)user_data;
	
	if(kd->time_offset == 0) {
		kd->time_offset = timer_read() - key_event->time
	};
	
	keypos_t keypos = (keypos_t)key_event->key_id;
	
	keyrecord_t record = {
		.event = {
			.time = key_event->time + time_offset,
			.key = keypos,
			.pressed = key_event->pressed
		}
	};
		
	PG_PRINTF("Issuing keystroke at %d, %d\n", record.event.key.row, record.event.key.col);
		
	process_record_quantum(&record);
	
	return true;
}

void pg_process_qmk_keycode(void *user_data) {
	
	uint16_t keycode = (uint16_t)user_data;
	
	if(keycode != 0) {
		
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
			* modifier keys or MO(...), etc.is
			*/
		
		uint16_t configured_keycode = keycode_configkeycode);
		
		action_t action = action_for_configured_keycode(configured_keycode); 
	
		process_action(&record, action);
		
		record.event.pressed = false;
		record.event.time = timer_read();
		
		process_action(&record, action);
	}
}

bool pg_qmk_process_key_event(uint16_t keycode, keyrecord_t *record)
{
	PG_Key_Event key_event = {
		.key_id = (PG_Key_Id)record.event.key;
		.time = (PG_Time_Id)record.event.time;
		.pressed = record.event.pressed
	};
	
	return pg_process_key_event(key_event);
}
