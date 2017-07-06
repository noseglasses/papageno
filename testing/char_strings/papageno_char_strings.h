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

#ifndef PAPAGENO_CHAR_STRINGS_H
#define PAPAGENO_CHAR_STRINGS_H

#include "papageno.h"

bool ppg_cs_process_event_callback(	
								PPG_Event *key_event,
								uint8_t slot_id, 
								void *user_data);

void ppg_cs_flush_key_events(void);

void ppg_cs_process_string(
								uint8_t slot_id, 
								void *user_data);

bool ppg_cs_process_event(char the_char);

void ppg_cs_process_events_from_string(char *sting);

void ppg_cs_time(			PPG_Time *time);

void  ppg_cs_time_difference(
								PPG_Time time1,
								PPG_Time time2,
								PPG_Time *delta);

int8_t ppg_cs_time_comparison(
								PPG_Time time1,
								PPG_Time time2);

void ppg_cs_set_timeout_ms(int timeout_ms);

/* Call this to flush key events that
 * were encountered by papageno
 */
void ppg_cs_flush_key_events(void);

bool ppg_cs_check_char_uppercase(PPG_Input_Id input_id,
										PPG_Input_State state);


bool ppg_cs_input_id_equal(PPG_Input_Id input_id1, PPG_Input_Id input_id2);

uint16_t ppg_cs_input_id_from_keypos(uint8_t row, uint8_t col);
	
#define PPG_CS_CHAR(CHAR) \
	(PPG_Input) { \
		.input_id = (PPG_Input_Id)(uintptr_t)CHAR, \
		.check_active = (PPG_Input_Active_Check_Fun)ppg_cs_check_char_uppercase \
	}	

#define PPG_CS_ACTION_STRING(STRING) \
	(PPG_Action) {	\
		.flags = PPG_Action_Default, \
		.user_callback = (PPG_User_Callback) { \
			.func = (PPG_User_Callback_Fun)ppg_cs_process_string,  \
			.user_data = (void*)(uintptr_t)(char*)STRING \
		} \
	}

#define PPG_CS_INIT \
	\
	ppg_global_init(); \
	\
	ppg_global_set_default_event_processor((PPG_Event_Processor_Fun)ppg_cs_process_event_callback); \
	\
	ppg_global_set_time_function((PPG_Time_Fun)ppg_cs_time); \
	ppg_global_set_time_difference_function((PPG_Time_Difference_Fun)ppg_cs_time_difference); \
	ppg_global_set_time_comparison_function((PPG_Time_Comparison_Fun)ppg_cs_time_comparison); \
	ppg_global_set_input_id_equal_function((PPG_Input_Id_Equal_Fun)ppg_cs_input_id_equal); 

#endif
