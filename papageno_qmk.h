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

#ifndef PAPAGENO_QMK_H
#define PAPAGENO_QMK_H

#include "papageno.h"

bool pg_process_qmk_key_event(	PG_Key_Event *key_event,
										uint8_t state_flag, 
										void *user_data);

void pg_process_qmk_keycode(void *user_data);

bool pg_qmk_process_key_event(uint16_t keycode, keyrecord_t *record);

#define PG_QMK_ACTION_KEYCODE(KK) \
	(PG_Action) {	\
		.flags = PG_Action_Undefined, \
		.user_callback = (PG_User_Callback) { \
			.func = pg_process_qmk_keycode,  \
			.user_data = (void*)KK \
		} \
	}
	
#ifdef DEBUG_PAPAGENO
#define PG_QMK_SET_PRINTF \
	pg_set_printf((PG_Printf_Fun)__xprintf);
#else
#define PG_QMK_SET_PRINTF
#endif

#define QMK_INIT_PAPAGENO \
	\
	pg_set_key_processor((PG_Key_Event_Processor_Fun)pg_process_qmk_key_event);\
	\
	PG_QMK_SET_PRINTF

#define PG_QMK_KEYPOS_HEX(COL, ROW) (PG_Key_Id)((keypos_t){ .row = 0x##ROW, .col = 0x##COL})

#define PG_QMK_

#endif
