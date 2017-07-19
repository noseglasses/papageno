/* Copyright 2017 Florian Fleissner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PAPAGENO_CHAR_STRINGS_H
#define PAPAGENO_CHAR_STRINGS_H

#include "papageno.h"

#include <stdlib.h>
#include <ctype.h>

// Note: Preprocessor macro functions can be 
//       hard to debug.
//
//       One approach is to take a look at
//       the preprocessed code to find out 
//       what goes wrong. 
//
//       Unfortunatelly macro replacement cannot deal with newlines.
//       Because of this, code that emerges from excessive macro
//       replacement looks very unreadable due to the absence of 
//       any line breaks.
//
//       To deal with this restriction, comment the
//       definition of the __NL__ macro below, during
//       preprocessor debugging. In that case, the
//       occurrences of __NL__ will not be replaced by
//       and empty string as in the compiled version but
//       will still be present in the preprocessed code.
//       Replace all occurrences of __NL__ in the preprocessed
//       with newlines using a text editor to gain a
//       readable version of the generated code.

#define __NL__

enum { PPG_CS_Timeout_MS = 20 };

enum { 
   PPG_CS_Action_Exception_None = 0,
   PPG_CS_Action_Exception_Aborted = (1 << 0),
   PPG_CS_Action_Exception_Timeout = (1 << 1),
   PPG_CS_Action_Exception_Match_Failed = (1 << 2)
};

enum {
   PPG_CS_EA = PPG_CS_Action_Exception_Aborted,
   PPG_CS_ET = PPG_CS_Action_Exception_Timeout,
   PPG_CS_EMF = PPG_CS_Action_Exception_Match_Failed
};

extern int ppg_cs_result;

// Returns the action id
//
int ppg_cs_register_action(char *action_name);

char *ppg_cs_get_action_name(int action_id);

#define PPG_CS_PASTE(A, B) A##B

#define PPG_CS_ACTION_VAR(ACTION_NAME) \
      PPG_CS_PASTE(PPG_CS_Action_, ACTION_NAME)
   
#define PPG_CS_A(ACTION_NAME) PPG_CS_ACTION_VAR(ACTION_NAME)

#define PPG_CS_REGISTER_ACTION(ACTION_NAME) \
   int PPG_CS_ACTION_VAR(ACTION_NAME) \
      = ppg_cs_register_action(#ACTION_NAME);
      
#define PPG_CS_REGISTER_ACTION_ANNONYMOUS(ACTION_NAME) \
   ppg_cs_register_action(#ACTION_NAME);
   
#define PPG_CS_EXPECT_NO_EXCEPTIONS \
   PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_Action_Exception_None)
   
#define PPG_CS_EXPECT_ACTION_SERIES(...) \
   ppg_cs_check_action_series(sizeof((int[]){__VA_ARGS__})/sizeof(int), \
                              (int[]){__VA_ARGS__});
   
#define PPG_CS_EXPECT_NO_ACTIONS \
   PPG_CS_EXPECT_ACTION_SERIES()

void ppg_cs_reset_testing_environment(void);

void ppg_cs_list_flush_buffer(void);

void ppg_cs_check_test_results(uint8_t expected);

void ppg_cs_compile(void);

// Control characters
//
enum { PPG_CS_CC_Short_Delay = '_' };
enum { PPG_CS_CC_Long_Delay = '|' };
enum { PPG_CS_CC_Noop = ' ' };

void ppg_cs_process_event_callback(   
                        PPG_Event *event,
                        void *user_data);

void ppg_cs_flush_key_events(void);

void ppg_cs_process_action(void *user_data);

void ppg_cs_process_event(char the_char);

void ppg_cs_process_string(char *string);

void ppg_cs_check_flushed(char *expected);

void ppg_cs_check_action_series(int n_actions, int* expected);

void ppg_cs_list_event_queue(void);

#define PPG_CS_EXPECT_EMPTY_FLUSH \
   ppg_cs_check_flushed("");

#define PPG_CS_EXPECT_FLUSH(FLUSHED) \
   ppg_cs_check_flushed(FLUSHED);
   
#define PPG_CS_EXPECT_EXCEPTIONS(EXPECTED) \
   ppg_cs_check_test_results(EXPECTED);

#define PPG_CS_CHECK_NO_PROCESS(...) \
   ppg_timeout_check(); \
   ppg_cs_separator(); \
   __VA_ARGS__ \
   ppg_cs_list_event_queue(); \
   ppg_cs_separator(); \
   ppg_cs_check_test_success(__FILE__, __LINE__); \
   ppg_cs_separator(); \
   ppg_cs_reset_testing_environment();
   
#define PPG_CS_PROCESS_STRING(STRING, ...) \
   ppg_cs_output_test_info(__FILE__, __LINE__); \
   ppg_cs_process_string(STRING); \
   PPG_CS_CHECK_NO_PROCESS(__VA_ARGS__)

void ppg_cs_process_on_off(char *string);

#define PPG_CS_PROCESS_ON_OFF(STRING, ...) \
   ppg_cs_output_test_info(__FILE__, __LINE__); \
   ppg_cs_process_on_off(STRING); \
   PPG_CS_CHECK_NO_PROCESS(__VA_ARGS__)   
   
void ppg_cs_time(         PPG_Time *time);

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

uint16_t ppg_cs_input_id_from_keypos(uint8_t row, uint8_t col);

void ppg_cs_check_test_success(char *file, int line);
void ppg_cs_output_test_info(char *file, int line);

void ppg_cs_separator(void);
   
#define PPG_CS_CHAR(CHAR) \
   (PPG_Input_Id)(uintptr_t)tolower(CHAR)

#define PPG_CS_ACTION(ACTION_NAME) \
   (PPG_Action) {   \
      .flags = PPG_Action_Default, \
      .callback = (PPG_Action_Callback) { \
         .func = (PPG_Action_Callback_Fun)ppg_cs_process_action,  \
         .user_data = (void*)(uintptr_t)PPG_CS_ACTION_VAR(ACTION_NAME) \
      } \
   }
   
#define PPG_CS_PREPARE_CONTEXT \
   \
   ppg_global_set_number_of_inputs(255); \
   \
   ppg_global_set_default_event_processor((PPG_Event_Processor_Fun)ppg_cs_process_event_callback); \
   \
   ppg_global_set_time_manager( \
      (PPG_Time_Manager) { \
         .time = (PPG_Time_Fun)ppg_cs_time, \
         .time_difference = (PPG_Time_Difference_Fun)ppg_cs_time_difference, \
         .compare_times = (PPG_Time_Comparison_Fun)ppg_cs_time_comparison \
      } \
   ); \
   \
   ppg_global_set_signal_callback( \
      (PPG_Signal_Callback) { \
         .func = (PPG_Signal_Callback_Fun)ppg_cs_on_signal,  \
         .user_data = NULL \
      } \
   ); \
    \
   /*ppg_global_set_abort_trigger(PPG_CS_CHAR('z')); */\
    \
   ppg_cs_set_timeout_ms(PPG_CS_Timeout_MS); \

#define PPG_CS_INIT \
   \
   ppg_global_init(); \
   \
   PPG_CS_PREPARE_CONTEXT
   
#define PPG_CS_START_TEST \
\
int main(int argc, char **argv) \
{ \
   \
   PPG_CS_REGISTER_ACTION_ANNONYMOUS(Initialized)   \
   PPG_CS_REGISTER_ACTION_ANNONYMOUS(Exception_Timeout)   \
   PPG_CS_REGISTER_ACTION_ANNONYMOUS(Exception_Aborted) \
   \
   PPG_CS_INIT \
   
#define PPG_CS_END_TEST \
   \
   ppg_global_finalize();   \
   \
   return 0; \
}

#define PPG_CS_N(CHAR) \
   ppg_note_create_standard(PPG_CS_CHAR(CHAR))
   
#define PPG_CS_N_A(CHAR) \
   ppg_note_create(PPG_CS_CHAR(CHAR), PPG_Note_Type_Match_Activation)
   
#define PPG_CS_N_D(CHAR) \
   ppg_note_create(PPG_CS_CHAR(CHAR), PPG_Note_Type_Match_Deactivation)

#endif
