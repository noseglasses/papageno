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

enum { PPG_CS_Timeout_MS = 20 };

enum { 
	PPG_CS_Action_Initialized = 0,
	PPG_CS_Action_Exception_Aborted = (1 << 0),
	PPG_CS_Action_Exception_Timeout = (1 << 1),
	PPG_CS_Action_Exception_Match_Failed = (1 << 2)
};

enum {
	PPG_CS_EA = PPG_CS_Action_Exception_Aborted,
	PPG_CS_ET = PPG_CS_Action_Exception_Timeout,
	PPG_CS_EMF = PPG_CS_Action_Exception_Match_Failed
};

enum { PPG_CS_Flag_Offset = 3 };

extern int ppg_cs_result;

// Returns the action id
//
int ppg_cs_register_action(char *action_name);

char *ppg_cs_get_action_name(int action_id);

#define PPG_CS_PASTE(A, B) A##B

#define PPG_CS_ACTION_VAR(ACTION_NAME) \
		PPG_CS_PASTE(PPG_CS_Action_, ACTION_NAME)

#define PPG_CS_REGISTER_ACTION(ACTION_NAME) \
	int PPG_CS_ACTION_VAR(ACTION_NAME) \
		= ppg_cs_register_action(#ACTION_NAME);
		
#define PPG_CS_REGISTER_ACTION_ANNONYMOUS(ACTION_NAME) \
	ppg_cs_register_action(#ACTION_NAME);

void ppg_cs_reset_result(void);

void ppg_cs_list_flush_buffer(void);

void ppg_cs_print_action_names(int expected, int actual);

#define PPG_CS_EXPECT_ACTION(S) (PPG_CS_ACTION_VAR(S) << PPG_CS_Flag_Offset)
#define PPG_CS_EXP(S) PPG_CS_EXPECT_ACTION(S)

void ppg_cs_check_test_result(uint8_t expected);

#define PPG_CS_N(CHAR) \
	ppg_note_create(PPG_CS_CHAR(CHAR))

void ppg_cs_compile(void);

// Control characters
//
enum { PPG_CS_CC_Short_Delay = '_' };
enum { PPG_CS_CC_Long_Delay = '|' };
enum { PPG_CS_CC_Noop = ' ' };

void ppg_cs_process_event_callback(	
								PPG_Event *key_event,
								void *user_data);

void ppg_cs_flush_key_events(void);

void ppg_cs_process_action(void *user_data);

bool ppg_cs_process_event(char the_char);

void ppg_cs_process_string(char *string);

void ppg_cs_check_flushed(char *string);

#define PPG_CS_EMPTY_FLUSH ""

#define PPG_CS_PROCESS_STRING(STRING, FLUSHED, EXPECTED) \
	ppg_cs_process_string(STRING); \
	ppg_timeout_check(); \
	printf("Expected: %s = %d\n", #EXPECTED, (EXPECTED)); \
	ppg_cs_check_test_result(EXPECTED); \
	ppg_cs_check_flushed(FLUSHED); \
	ppg_cs_reset_result();

void ppg_cs_process_on_off(char *string);

#define PPG_CS_PROCESS_ON_OFF(STRING, FLUSHED, EXPECTED) \
	ppg_cs_process_on_off(STRING); \
	ppg_timeout_check(); \
	printf("Expected: %s = %d\n", #EXPECTED, (EXPECTED)); \
	ppg_cs_check_test_result(EXPECTED); \
	ppg_cs_check_flushed(FLUSHED); \
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

void ppg_cs_on_signal(
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
	}	

#define PPG_CS_ACTION(ACTION_NAME) \
	(PPG_Action) {	\
		.flags = PPG_Action_Default, \
		.callback = (PPG_Action_Callback) { \
			.func = (PPG_Action_Callback_Fun)ppg_cs_process_action,  \
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
	ppg_global_set_signal_callback( \
		(PPG_Signal_Callback) { \
			.func = (PPG_Signal_Callback_Fun)ppg_cs_on_signal,  \
			.user_data = NULL \
		} \
	); \
	
#define PPG_CS_START_TEST \
\
int main(int argc, char **argv) \
{ \
	PPG_CS_INIT \
	 \
	ppg_global_set_abort_trigger(PPG_CS_CHAR('z')); \
	 \
	ppg_cs_set_timeout_ms(PPG_CS_Timeout_MS); \
	\
	PPG_CS_REGISTER_ACTION_ANNONYMOUS(Initialized)	\
	PPG_CS_REGISTER_ACTION_ANNONYMOUS(Exception_Timeout)	\
	PPG_CS_REGISTER_ACTION_ANNONYMOUS(Exception_Aborted)
	
#define PPG_CS_END_TEST \
	return 0; \
}

#endif
