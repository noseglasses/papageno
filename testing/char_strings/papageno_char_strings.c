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

bool ppg_cs_process_event_callback(	
										PPG_Event *event,
										uint8_t slot_id, 
										void *user_data)
{ 
	char the_char = (char)(uintptr_t)event->input_id;
	
	if((uintptr_t)event->state) {
		the_char = toupper(the_char);
	}
	
	printf("%c", the_char);
	
	return true;
}

void ppg_cs_flush_events(void)
{	
	ppg_event_buffer_flush(
		PPG_On_User,
		ppg_cs_process_event_callback,
		NULL
	);
}

void ppg_cs_process_string(uint8_t slot_id, void *user_data) {
	
	char *the_string = (char*)(uintptr_t)user_data;
	
	printf("Action: %s\n", the_string);
}

inline
static int my_isalpha_lower(int c) {
    return ((c >= 'a' && c <= 'z')); } 

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
		.time = (PPG_Time)clock(),
		.state = (PPG_Input_State)(uintptr_t)my_isalpha_upper(the_char)
	};
	
	return ppg_event_process(&event);
}

void ppg_cs_process_events_from_string(char *string)
{
	int i = 0;
	while(string[i] != '\0') {
		ppg_cs_process_event(string[i]);
		++i;
	}
}

void ppg_cs_time(PPG_Time *time)
{
	*time = clock();
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

void ppg_cs_set_timeout_ms(int timeout_ms)
{
	ppg_global_set_timeout((PPG_Time)CLOCKS_PER_SEC*timeout_ms);
}

inline
static uint16_t ppg_cs_get_event_state(PPG_Input_State *state)
{
	return *(uint16_t*)&state;
}

bool ppg_cs_check_char_uppercase(PPG_Input_Id input_id,
										PPG_Input_State state)
{
	return (ppg_cs_get_event_state(state) == true);
}

bool ppg_cs_input_id_equal(PPG_Input_Id input_id1, PPG_Input_Id input_id2)
{
	char the_char_1 = toupper((char)(uintptr_t)input_id1);
	char the_char_2 = toupper((char)(uintptr_t)input_id2);
	
	//printf("checking equality: c1 = %c, c2 = %c\n", the_char_1, the_char_2);
	
	return the_char_1 == the_char_2;
}
