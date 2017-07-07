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

#include <stdlib.h>

enum { PPG_Timeout_MS = 20 };

enum { 
	PPG_Action_Initialized = 0,
	PPG_Action_Exception_Timeout,
	PPG_Action_Exception_Aborted
};

extern int ppg_cs_result;

// Returns the action id
//
int ppg_cs_register_action(char *action_name);

char *ppg_cs_get_action_name(int action_id);

#define PPG_CS_ACTION_VAR(ACTION_NAME) PPG_Action_##ACTION_NAME

#define PPG_CS_REGISTER_ACTION(ACTION_NAME) \
	int PPG_CS_ACTION_VAR(ACTION_NAME) \
		= ppg_cs_register_action(#ACTION_NAME);
		
#define PPG_CS_REGISTER_ACTION_ANNONYMOUS(ACTION_NAME) \
	ppg_cs_register_action(#ACTION_NAME);

void ppg_cs_reset_result(void);

void ppg_cs_print_action_names(int expected, int actual);

#define PPG_CS_ASSERT_RESULT(EXPECTED) \
{ \
	if(ppg_cs_result != (EXPECTED)) { \
		printf("Result assertion failed in %s, line %d\n", \
				 __FILE__, __LINE__); \
		\
		ppg_cs_print_action_names((EXPECTED), ppg_cs_result); \
		\
		exit(1); \
	} \
	else { \
		printf("Expected result verified: %s\n", \
				 ppg_cs_get_action_name(ppg_cs_result)); \
	}\
}

#define PPG_CS_N(CHAR) \
	ppg_note_create(PPG_CS_CHAR(CHAR))

void ppg_cs_compile(void);

// Control characters
//
enum { PPG_CS_CC_Short_Delay = '_' };
enum { PPG_CS_CC_Long_Delay = '|' };
enum { PPG_CS_CC_Noop = ' ' };

bool ppg_cs_process_event_callback(	
								PPG_Event *key_event,
								uint8_t slot_id, 
								void *user_data);

void ppg_cs_flush_key_events(void);

void ppg_cs_process_action(
								uint8_t slot_id, 
								void *user_data);

bool ppg_cs_process_event(char the_char);

void ppg_cs_process_string(char *string);

#define PPG_CS_PROCESS_STRING(STRING, EXPECTED_ACTION_NAME) \
	ppg_cs_process_string(STRING); \
	ppg_timeout_check(); \
	PPG_CS_ASSERT_RESULT(PPG_CS_ACTION_VAR(EXPECTED_ACTION_NAME)) \
	ppg_cs_reset_result();

void ppg_cs_process_on_off(char *string);

#define PPG_CS_PROCESS_ON_OFF(STRING, EXPECTED_ACTION_NAME) \
	ppg_cs_process_on_off(STRING); \
	ppg_timeout_check(); \
	PPG_CS_ASSERT_RESULT(PPG_CS_ACTION_VAR(EXPECTED_ACTION_NAME)) \
	ppg_cs_reset_result();
	
void ppg_cs_time(			PPG_Time *time);

void ppg_cs_time_difference(
								PPG_Time time1,
								PPG_Time time2,
								PPG_Time *delta);

int8_t ppg_cs_time_comparison(
								PPG_Time time1,
								PPG_Time time2);

void ppg_cs_set_timeout_ms(long unsigned timeout_ms);
int ppg_cs_get_timeout_ms(void);

/* Call this to flush key events that
 * were encountered by papageno
 */
void ppg_cs_flush_key_events(void);

bool ppg_cs_check_char_uppercase(PPG_Input_Id input_id,
										PPG_Input_State state);

void ppg_cs_on_timeout(
								uint8_t slot_id, 
								void *user_data);

void ppg_cs_on_abort(
								uint8_t slot_id, 
								void *user_data);

bool ppg_cs_input_id_equal(PPG_Input_Id input_id1, PPG_Input_Id input_id2);

uint16_t ppg_cs_input_id_from_keypos(uint8_t row, uint8_t col);
	
#define PPG_CS_CHAR(CHAR) \
	(PPG_Input) { \
		.input_id = (PPG_Input_Id)(uintptr_t)CHAR, \
		.check_active = (PPG_Input_Active_Check_Fun)ppg_cs_check_char_uppercase \
	}	

#define PPG_CS_ACTION(ACTION_NAME) \
	(PPG_Action) {	\
		.flags = PPG_Action_Default, \
		.user_callback = (PPG_User_Callback) { \
			.func = (PPG_User_Callback_Fun)ppg_cs_process_action,  \
			.user_data = (void*)(uintptr_t)PPG_CS_ACTION_VAR(ACTION_NAME) \
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
	ppg_global_set_input_id_equal_function((PPG_Input_Id_Equal_Fun)ppg_cs_input_id_equal); \
	\
	ppg_global_set_timeout_action( \
		(PPG_User_Callback) { \
			.func = (PPG_User_Callback_Fun)ppg_cs_on_timeout,  \
			.user_data = NULL \
		} \
	); \
	ppg_global_set_abort_action( \
		(PPG_User_Callback) { \
			.func = (PPG_User_Callback_Fun)ppg_cs_on_abort,  \
			.user_data = NULL \
		} \
	);
	
#define PPG_CS_START_TEST \
\
int main(int argc, char **argv) \
{ \
	PPG_CS_INIT \
	 \
	ppg_global_set_abort_trigger(PPG_CS_CHAR('z')); \
	 \
	ppg_cs_set_timeout_ms(PPG_Timeout_MS); \
	\
	PPG_CS_REGISTER_ACTION_ANNONYMOUS(Initialized)	\
	PPG_CS_REGISTER_ACTION_ANNONYMOUS(Exception_Timeout)	\
	PPG_CS_REGISTER_ACTION_ANNONYMOUS(Exception_Aborted)
	
#define PPG_CS_END_TEST \
	return 0; \
}

#endif
