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

#include "papageno_char_strings.h"

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/timeb.h> 
#include <string.h>

int ppg_cs_result = PPG_CS_Action_Initialized;

static long unsigned ppg_cs_timeout_ms = 0;

static long unsigned ppg_cs_start_time_s = 0;
static long unsigned ppg_cs_start_time_ms = 0;

char ppg_cs_flush_buffer[256];
int ppg_cs_cur_flush_pos = 0;

#define MMG_MAX_ACTION_NAMES 100
static char *ppg_cs_action_names[MMG_MAX_ACTION_NAMES];
static int ppg_cs_next_action_id = 0;

int ppg_cs_register_action(char *action_name)
{
	ppg_cs_action_names[ppg_cs_next_action_id] = action_name;
	return ppg_cs_next_action_id++;
}

char *ppg_cs_get_action_name(int action_id)
{
	if(	(action_id < 0)
		||	(action_id >= ppg_cs_next_action_id)) {
		return NULL;
	}
	
	return ppg_cs_action_names[action_id];
}

static void ppg_cs_store_start_time(void)
{
	struct timeb a_timeb;
	
	ftime(&a_timeb);
	
	ppg_cs_start_time_s = a_timeb.time;
	ppg_cs_start_time_ms = a_timeb.millitm;
}

static long unsigned ppg_cs_run_time_ms(void)
{
	struct timeb a_timeb;
	
	ftime(&a_timeb);
	
// 	if(ppg_cs_start_time_ms >= a_timeb.millitm) {
 
		return (a_timeb.time - ppg_cs_start_time_s)*1000 + a_timeb.millitm - ppg_cs_start_time_ms;
// 	}
// 
// 	return (a_timeb.time - ppg_cs_start_time_s - 1)*1000 + a_timeb.millitm - ppg_cs_start_time_ms;
}

void ppg_cs_reset_result(void)
{
	ppg_cs_result = PPG_CS_Action_Initialized;
	ppg_cs_cur_flush_pos = 0;
}

void ppg_cs_process_event_callback(	
										PPG_Event *event,
										void *user_data)
{ 
	// Ignore events that were considered
	//
	if(event->flags & PPG_Event_Considered) { return; }
	
	char the_char = (char)(uintptr_t)event->input_id;
	
	if((uintptr_t)event->flags & PPG_Event_Active) {
		the_char = toupper(the_char);
	}
	
	ppg_cs_flush_buffer[ppg_cs_cur_flush_pos] = the_char;
	++ppg_cs_cur_flush_pos;
	
	printf("%c", the_char);
}

void ppg_cs_list_flush_buffer(void)
{
	printf("Flushed: \'%s\'\n", ppg_cs_flush_buffer);
}

void ppg_cs_flush_events(void)
{	
	ppg_event_buffer_iterate(
		ppg_cs_process_event_callback,
		NULL
	);
}

void ppg_cs_process_action(void *user_data) {
	
	PPG_Count action_id = (int)(uintptr_t)user_data;
	
	ppg_cs_result |= (action_id << PPG_CS_Flag_Offset);
	
	printf("***** Action: %s\n", ppg_cs_action_names[action_id]);
}

static void ppg_cs_break(void) {

	printf("\n");
}
static void ppg_cs_separator(void) {

	printf("***************************************************************************\n");
}

// inline
// static int my_isalpha_lower(int c) {
//     return ((c >= 'a' && c <= 'z')); } 

inline
static int my_isalpha_upper(int c) {
        return ((c >= 'A' && c <= 'Z')); } 

bool ppg_cs_process_event(char the_char)
{
	if(!isalpha(the_char)) { return false; }
	
	char lower_char = tolower(the_char);
	
	printf("\nSending char %c, is upper: %d\n", the_char, my_isalpha_upper(the_char));
	
	PPG_Event event = {
		.input_id = (PPG_Input_Id)(uintptr_t)lower_char,
		.time = (PPG_Time)ppg_cs_run_time_ms(),
		.flags = (my_isalpha_upper(the_char)) ? PPG_Event_Active : PPG_Event_Flags_Empty
	};
	
	return ppg_event_process(&event);
}

bool ppg_cs_check_and_process_control_char(char c)
{
	bool is_control_char = true;
	
	switch(c) {
		case PPG_CS_CC_Short_Delay:
		{
			int microseconds = (int)((double)ppg_cs_timeout_ms*0.3*1000);
			printf("Short delay: %d\n", microseconds);
			usleep(microseconds);
		}
			break;
			
		case PPG_CS_CC_Long_Delay:
		{
			int microseconds = (int)((double)ppg_cs_timeout_ms*3*1000);
			printf("Long delay: %d\n", microseconds);
			usleep(microseconds);
		}
			break;
		case PPG_CS_CC_Noop:
			break;
		default:
			is_control_char = false;
			break;
	}
	
	return is_control_char;
}

void ppg_cs_process_string(char *string)
{
	ppg_cs_break();
	ppg_cs_separator();
	printf("Sending control string \"%s\"\n", string);
	ppg_cs_separator();
	
	int i = 0;
	while(string[i] != '\0') {
		
		if(!ppg_cs_check_and_process_control_char(string[i])) {
			ppg_cs_process_event(string[i]);
		}
		++i;
	}
}

void ppg_cs_process_on_off(char *string)
{
	ppg_cs_break();
	ppg_cs_separator();
	printf("Sending control string \"%s\" in on-off mode\n", string);
	ppg_cs_separator();
	
	int i = 0;
	while(string[i] != '\0') {
				
		if(!ppg_cs_check_and_process_control_char(string[i])) {
			ppg_cs_process_event(toupper(string[i]));
			ppg_cs_process_event(tolower(string[i]));
		}
		++i;
	}
}

void ppg_cs_time(PPG_Time *time)
{
	*time = ppg_cs_run_time_ms();
	
// 	printf("time [ms]: %ld\n", *time);
}

void  ppg_cs_time_difference(PPG_Time time1, PPG_Time time2, PPG_Time *delta)
{
	*delta = time2 - time1; 
}

int8_t ppg_cs_time_comparison(
								PPG_Time time1,
								PPG_Time time2)
{
	if(time1 > time2) {
		return 1;
	}
	else if(time1 == time2) {
		return 0;
	}
	 
	return -1;
}

void ppg_cs_set_timeout_ms(long unsigned timeout_ms)
{	
	ppg_cs_timeout_ms = timeout_ms;
	ppg_global_set_timeout((PPG_Time)timeout_ms);
}

int ppg_cs_get_timeout_ms(void)
{
	return ppg_cs_timeout_ms;
}

bool ppg_cs_input_id_equal(PPG_Input_Id input_id1, PPG_Input_Id input_id2)
{
	char the_char_1 = toupper((char)(uintptr_t)input_id1);
	char the_char_2 = toupper((char)(uintptr_t)input_id2);
	
	//printf("checking equality: c1 = %c, c2 = %c\n", the_char_1, the_char_2);
	
	return the_char_1 == the_char_2;
}

void ppg_cs_compile(void)
{
	// Store the start time as a reference
	//
	ppg_cs_store_start_time();
	
	ppg_global_compile();
	
	PPG_PATTERN_PRINT_TREE
}

void ppg_cs_print_action_names(int expected, int actual)
{
	PPG_Count action_id = (ppg_cs_result >> PPG_CS_Flag_Offset);
	PPG_Count exp_action_id = (expected >> PPG_CS_Flag_Offset);
	
	char *expected_action_name = ppg_cs_get_action_name(exp_action_id);
	char *actual_action_name = ppg_cs_get_action_name(action_id);
	
	printf("   expected: ");
	if(expected_action_name) {
		printf("%s\n", expected_action_name);
	}
	else {
		printf("%d\n", exp_action_id);
	}
	
	printf("   actual: ");
	if(actual_action_name) {
		printf("%s\n", actual_action_name);
	}
	else {
		printf("%d\n", exp_action_id);
	}
}

void ppg_cs_on_signal(
								uint8_t slot_id, 
								void *user_data)
{
// 	printf("Slot id: %d\n", slot_id);
	
	ppg_cs_break();
	ppg_cs_separator();
	
	switch(slot_id) {
		case PPG_On_Abort:
			printf("Registering abortion\n");
			ppg_cs_result |= PPG_CS_Action_Exception_Aborted;
			break;
		case PPG_On_Timeout:
			printf("Registering timeout\n");
			ppg_cs_result |= PPG_CS_Action_Exception_Timeout;
			
			ppg_cs_flush_events();
			break;
		case PPG_On_Match_Failed:
			printf("Registering match failed\n");
			ppg_cs_result |= PPG_CS_Action_Exception_Match_Failed;
			break;
	}

	ppg_cs_separator();
}

void ppg_cs_check_test_result(uint8_t expected)
{
	bool failed = false;
	
	ppg_cs_break();
	ppg_cs_separator();
	
	if(ppg_cs_result != (expected)) {
		printf("Result assertion failed in %s, line %d\n",
				 __FILE__, __LINE__);
		
		ppg_cs_print_action_names((expected), ppg_cs_result);
		
		failed = true;
	}
	else {
		printf("Expected result verified: %s\n",
				 ppg_cs_get_action_name((ppg_cs_result >> PPG_CS_Flag_Offset)));
	}
	
	printf("   aborted: expected: %d, actual: %d\n",
		((expected) & PPG_CS_Action_Exception_Aborted),
		(ppg_cs_result & PPG_CS_Action_Exception_Aborted));
	
	printf("   timeout: expected: %d, actual: %d\n",
		((expected) & PPG_CS_Action_Exception_Timeout),
		(ppg_cs_result & PPG_CS_Action_Exception_Timeout)
	);
	
	printf("   match failed: expected: %d, actual: %d\n",
		((expected) & PPG_CS_Action_Exception_Match_Failed),
		(ppg_cs_result & PPG_CS_Action_Exception_Match_Failed)
	);
	
	ppg_input_list_all_active();
	
	ppg_cs_separator();
	
	if(failed) {
		exit(1);
	}
}

void ppg_cs_check_flushed(char *expected)
{
	ppg_cs_flush_buffer[ppg_cs_cur_flush_pos] = '\0';
	++ppg_cs_cur_flush_pos;
	
	if(strcmp(ppg_cs_flush_buffer, expected)) {
		printf("Flush buffer mismatch\n");
		printf("   expected: \'%s\'\n", expected);
		printf("   actual:   \'%s\'\n", ppg_cs_flush_buffer);
	}
	else {
		printf("Flush buffer success\n");
	}
}

