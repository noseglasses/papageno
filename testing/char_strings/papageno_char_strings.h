/* Copyright 2017 noseglasses <shinynoseglasses@gmail.com>
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

#ifdef __AVR__
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define PPG_STDOUT_REDIRECTION_SETUP \
          \
__NL__     static int uart_putchar(char c, FILE *stream) { \
__NL__        if (c == '\n') \
__NL__           uart_putchar('\r', stream); \
__NL__       while(!(UCSR0A & _BV(UDRE0))); \
__NL__            UDR1 = c; \
__NL__        return 0; \
__NL__     } \
__NL__      \
__NL__     static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, \
                                          _FDEV_SETUP_WRITE);

#define PPG_REDIRECT_STDOUT \
   stdout = &mystdout;
   
#else // #ifdef __AVR__
   
#define PPG_STDOUT_REDIRECTION_SETUP
#define PPG_REDIRECT_STDOUT
   
#endif // #ifdef __AVR__

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

enum { PPG_CS_Timeout_MS = 200 };

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

typedef struct {
   PPG_Count action_id;
   bool activated;
} PPG_CS_Action_Expectation;

extern int ppg_cs_result;

// Returns the action id
//
int ppg_cs_register_action(char *action_name);

char *ppg_cs_get_action_name(int action_id);

#define PPG_CS_PREVENT_UNUSED_WARNING(SYMBOL) \
   (void)SYMBOL;

#define PPG_CS_PASTE(A, B) A##B

#define PPG_CS_ACTION_VAR(ACTION_NAME) \
      PPG_CS_PASTE(PPG_CS_Action_, ACTION_NAME)
      
#define PPG_CS_ACTION_EXPECTATION(ACTION_NAME, ACTIVATED) \
      (PPG_CS_Action_Expectation) { \
         .action_id = PPG_CS_ACTION_VAR(ACTION_NAME), \
         .activated = ACTIVATED \
      }
      
#define PPG_CS_A(ACTION_NAME) \
      PPG_CS_ACTION_EXPECTATION(ACTION_NAME, true), \
      PPG_CS_ACTION_EXPECTATION(ACTION_NAME, false)
      
#ifdef PPG_CS_NO_COMPRESSION
#define PPG_CS_ADD_COMPRESSION_SYMBOL(SYMBOL)
#else
#define PPG_CS_ADD_COMPRESSION_SYMBOL(SYMBOL) \
__NL__   if(cs_ccontext__) { \
__NL__      PPG_COMPRESSION_REGISTER_SYMBOL(cs_ccontext__, SYMBOL); \
__NL__   }   
#endif

#define PPG_CS_REGISTER_ACTION(ACTION_NAME) \
__NL__   int PPG_CS_ACTION_VAR(ACTION_NAME) \
__NL__      = ppg_cs_register_action(#ACTION_NAME); \
__NL__   \
__NL__   PPG_CS_ADD_COMPRESSION_SYMBOL(PPG_CS_ACTION_VAR(ACTION_NAME))
      
#define PPG_CS_REGISTER_ACTION_ANNONYMOUS(ACTION_NAME) \
   ppg_cs_register_action(#ACTION_NAME);
   
#define PPG_CS_EXPECT_NO_EXCEPTIONS \
   PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_Action_Exception_None)
   
#define PPG_CS_EXPECT_ACTION_SERIES(...) \
__NL__   ppg_cs_check_action_series(sizeof((PPG_CS_Action_Expectation[]){__VA_ARGS__})/sizeof(PPG_CS_Action_Expectation), \
__NL__                              (PPG_CS_Action_Expectation[]){__VA_ARGS__});
   
#define PPG_CS_EXPECT_NO_ACTIONS \
   PPG_CS_EXPECT_ACTION_SERIES()
   
void ppg_cs_init(void);

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

void ppg_cs_process_action(bool activation, void *user_data);

void ppg_cs_process_event(char the_char);

void ppg_cs_process_string(char *string);

void ppg_cs_check_flushed(char *expected);

void ppg_cs_check_action_series(int n_actions, PPG_CS_Action_Expectation* expected);

void ppg_cs_list_event_queue(void);

#define PPG_CS_EXPECT_EMPTY_FLUSH \
   ppg_cs_check_flushed("");

#define PPG_CS_EXPECT_FLUSH(FLUSHED) \
   ppg_cs_check_flushed(FLUSHED);
   
#define PPG_CS_EXPECT_EXCEPTIONS(EXPECTED) \
   ppg_cs_check_test_results(EXPECTED);

#define PPG_CS_CHECK_NO_PROCESS(...) \
__NL__   ppg_timeout_check(); \
__NL__   ppg_cs_separator(); \
__NL__   __VA_ARGS__ \
__NL__   ppg_cs_list_event_queue(); \
__NL__   ppg_cs_separator(); \
__NL__   ppg_cs_check_test_success(__FILE__, __LINE__); \
__NL__   ppg_cs_separator(); \
__NL__   \
__NL__   if(automatically_reset_testing_system) { \
__NL__      ppg_cs_reset_testing_environment(); \
__NL__   } \
__NL__   else { \
__NL__      automatically_reset_testing_system = true; \
__NL__   }

#if PPG_PRINT_SELF_ENABLED
   #define PPG_CS_PRINT_TREE ppg_pattern_print_tree();
#else
   #define PPG_CS_PRINT_TREE
#endif

#if PPG_HAVE_DEBUGGING
#define PPG_CS_CHECK_CONSISTENCY \
__NL__      if(ppg_global_check_consistency()) { \
__NL__         PPG_LOG("Consistency check failed\n"); \
__NL__         PPG_CS_PRINT_TREE \
__NL__         abort(); \
__NL__      }
#else
#define PPG_CS_CHECK_CONSISTENCY
#endif
   
#define PPG_CS_PROCESS_STRING(STRING, ...) \
__NL__   PPG_CS_CHECK_CONSISTENCY \
__NL__   ppg_cs_output_test_info(__FILE__, __LINE__); \
__NL__   ppg_cs_process_string(STRING); \
__NL__   PPG_CS_CHECK_NO_PROCESS(__VA_ARGS__)

void ppg_cs_process_on_off(char *string);

#define PPG_CS_PROCESS_ON_OFF(STRING, ...) \
__NL__   PPG_CS_CHECK_CONSISTENCY \
__NL__   ppg_cs_output_test_info(__FILE__, __LINE__); \
__NL__   ppg_cs_process_on_off(STRING); \
__NL__   PPG_CS_CHECK_NO_PROCESS(__VA_ARGS__)   
   
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
__NL__   (PPG_Action) {   \
__NL__      .callback = (PPG_Action_Callback) { \
__NL__         .func = (PPG_Action_Callback_Fun)ppg_cs_process_action,  \
__NL__         .user_data = (void*)(uintptr_t)PPG_CS_ACTION_VAR(ACTION_NAME) \
__NL__      } \
__NL__   }

#define PPG_CS_ACTION_CALLBACK(CALLBACK, USER_DATA) \
__NL__   (PPG_Action) {   \
__NL__      .callback = (PPG_Action_Callback) { \
__NL__         .func = (PPG_Action_Callback_Fun)CALLBACK,  \
__NL__         .user_data = (void*)USER_DATA \
__NL__      } \
__NL__   }
   
#define PPG_CS_PREPARE_CONTEXT \
   \
__NL__   /*ppg_global_set_number_of_inputs(255); */\
__NL__   \
__NL__   ppg_global_set_default_event_processor((PPG_Event_Processor_Fun)ppg_cs_process_event_callback); \
__NL__   \
__NL__   ppg_global_set_time_manager( \
__NL__      (PPG_Time_Manager) { \
__NL__         .time = (PPG_Time_Fun)ppg_cs_time, \
__NL__         .time_difference = (PPG_Time_Difference_Fun)ppg_cs_time_difference, \
__NL__         .compare_times = (PPG_Time_Comparison_Fun)ppg_cs_time_comparison \
__NL__      } \
__NL__   ); \
__NL__   \
__NL__   ppg_global_set_signal_callback( \
__NL__      (PPG_Signal_Callback) { \
__NL__         .func = (PPG_Signal_Callback_Fun)ppg_cs_on_signal,  \
__NL__         .user_data = NULL \
__NL__      } \
__NL__   ); \
__NL__    \
__NL__   /*ppg_global_set_abort_trigger(PPG_CS_CHAR('z')); */\
__NL__    \
__NL__   ppg_cs_set_timeout_ms(PPG_CS_Timeout_MS);

#ifndef PPG_CS_NO_AUTOMATIC_CONTEXT_HANDLING
#define PPG_CS_INIT_GLOBAL_CONTEXT \
__NL__   ppg_global_init(); \
__NL__   \
__NL__   PPG_CS_PREPARE_CONTEXT

#define PPG_CS_FINALIZE_GLOBAL_CONTEXT \
__NL__   ppg_global_set_current_context(cs_test_context); \
__NL__   ppg_global_finalize();
#else
#define PPG_CS_INIT_GLOBAL_CONTEXT
#define PPG_CS_FINALIZE_GLOBAL_CONTEXT
#endif

#define PPG_CS_INIT \
   \
         PPG_REDIRECT_STDOUT \
   \
__NL__   ppg_cs_init(); \
__NL__   \
__NL__   PPG_CS_REGISTER_ACTION_ANNONYMOUS(Initialized)   \
__NL__   PPG_CS_REGISTER_ACTION_ANNONYMOUS(Exception_Timeout)   \
__NL__   PPG_CS_REGISTER_ACTION_ANNONYMOUS(Exception_Aborted) \
__NL__   \
__NL__   PPG_CS_INIT_GLOBAL_CONTEXT \
__NL__   \
__NL__   void *cs_test_context = ppg_global_get_current_context(); \
__NL__   PPG_CS_PREVENT_UNUSED_WARNING(cs_test_context) \
__NL__   \
__NL__   bool automatically_reset_testing_system = true; \
__NL__   PPG_CS_PREVENT_UNUSED_WARNING(automatically_reset_testing_system) \
__NL__   \
__NL__   PPG_Compression_Context cs_ccontext__ = NULL; \
__NL__   PPG_CS_PREVENT_UNUSED_WARNING(cs_ccontext__)

#define PPG_CS_INIT_COMPRESSION(CCONTEXT_PTR) \
\
__NL__   PPG_Compression_Context CCONTEXT_PTR \
__NL__               = ppg_compression_init(); \
\
__NL__   cs_ccontext__ = CCONTEXT_PTR; \
__NL__   \
__NL__   if(cs_ccontext__) { \
__NL__      PPG_COMPRESSION_REGISTER_SYMBOL(cs_ccontext__, ppg_cs_process_action) \
__NL__      PPG_COMPRESSION_REGISTER_SYMBOL(cs_ccontext__, ppg_cs_on_signal) \
__NL__      PPG_COMPRESSION_REGISTER_SYMBOL(cs_ccontext__, ppg_cs_process_event_callback) \
__NL__      PPG_COMPRESSION_REGISTER_SYMBOL(cs_ccontext__, ppg_cs_time) \
__NL__      PPG_COMPRESSION_REGISTER_SYMBOL(cs_ccontext__, ppg_cs_time_difference) \
__NL__      PPG_COMPRESSION_REGISTER_SYMBOL(cs_ccontext__, ppg_cs_time_comparison) \
__NL__      PPG_COMPRESSION_REGISTER_SYMBOL(cs_ccontext__, ppg_cs_on_signal) \
__NL__   }
   
#define PPG_CS_START_TEST \
         \
__NL__   PPG_STDOUT_REDIRECTION_SETUP \
__NL__   \
__NL__   int main(int argc, char **argv) \
__NL__   { \
__NL__      \
__NL__      PPG_CS_INIT \

#ifdef __AVR__
#define PPG_MAIN_EXIT \
__NL__   	cli(); \
__NL__   	sleep_mode(); \
__NL__      \
__NL__      abort();
#else
#define PPG_MAIN_EXIT \
__NL__   return 0;
#endif
   
#define PPG_CS_END_TEST \
   \
__NL__   PPG_CS_FINALIZE_GLOBAL_CONTEXT   \
__NL__   \
__NL__   PPG_MAIN_EXIT \
__NL__}

#define PPG_CS_N(CHAR) \
   ppg_note_create_standard(PPG_CS_CHAR(CHAR))
   
#define PPG_CS_N_A(CHAR) \
   ppg_note_create(PPG_CS_CHAR(CHAR), PPG_Note_Flag_Match_Activation)
   
#define PPG_CS_N_D(CHAR) \
   ppg_note_create(PPG_CS_CHAR(CHAR), PPG_Note_Flag_Match_Deactivation)
   
//##############################################################################
// Definitions for Papagenos glockenspiel compiler interface
//##############################################################################
//
#define m_GLS_ENABLE_ACTIONS_LOCAL_INITIALIZATION_ALL
#define m_GLS_ENABLE_INPUTS_LOCAL_INITIALIZATION_ALL
#define m_GLS_NO_AUTOMATIC_LOCAL_INITIALIZATION
   
#define m_GLS_INPUT_INITIALIZE___(UNIQUE_ID, USER_ID, CHAR) \
   PPG_CS_CHAR(CHAR)
   
#define m_GLS_ACTION_INITIALIZE___(UNIQUE_ID, USER_ID) \
   PPG_CS_ACTION(USER_ID)
   
#define m_GLS_ACTION_CONFIGURE_LOCAL___(UNIQUE_ID, USER_ID) \
   PPG_CS_REGISTER_ACTION(USER_ID)

#endif
