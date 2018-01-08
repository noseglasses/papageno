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

#include "papageno_char_strings.h"

#include <stdio.h>
#include <string.h>

#ifdef __AVR__

  // The following has been taken from the tmk and qmk projects

  #include <stdint.h>
  
  #ifndef TIMER_PRESCALER
  #   if F_CPU > 16000000
  #       define TIMER_PRESCALER      256
  #   elif F_CPU > 2000000
  #       define TIMER_PRESCALER      64
  #   elif F_CPU > 250000
  #       define TIMER_PRESCALER      8
  #   else
  #       define TIMER_PRESCALER      1
  #   endif
  #endif
  #define TIMER_RAW_FREQ      (F_CPU/TIMER_PRESCALER)
  #define TIMER_RAW           TCNT0
  #define TIMER_RAW_TOP       (TIMER_RAW_FREQ/1000)
  
  #if (TIMER_RAW_TOP > 255)
  #   error "Timer0 can't count 1ms at this clock freq. Use larger prescaler."
  #endif
  
  #define TIMER_DIFF(a, b, max)   ((a) >= (b) ?  (a) - (b) : (max) - (b) + (a))
  #define TIMER_DIFF_8(a, b)      TIMER_DIFF(a, b, UINT8_MAX)
  #define TIMER_DIFF_16(a, b)     TIMER_DIFF(a, b, UINT16_MAX)
  #define TIMER_DIFF_32(a, b)     TIMER_DIFF(a, b, UINT32_MAX)
  #define TIMER_DIFF_RAW(a, b)    TIMER_DIFF_8(a, b)
  
  static uint32_t current_time = 0;
  
  void timer_init(void) {current_time = 0;}
  
  void timer_clear(void) {current_time = 0;}
  
  uint16_t timer_read(void) { return current_time & 0xFFFF; }
  uint32_t timer_read32(void) { return current_time; }
  uint16_t timer_elapsed(uint16_t last) { return TIMER_DIFF_16(timer_read(), last); }
  uint32_t timer_elapsed32(uint32_t last) { return TIMER_DIFF_32(timer_read32(), last); }
  
  void set_time(uint32_t t) { current_time = t; }
  void advance_time(uint32_t ms) { current_time += ms; }
  
  void wait_ms(uint32_t ms) {
     advance_time(ms);
  }
#else

#include <sys/timeb.h> 
#include <unistd.h>
#endif

int ppg_cs_exceptions = PPG_CS_Action_Exception_None;

#ifdef __AVR__
static uint32_t ppg_cs_timeout_ms = 0;
static uint32_t ppg_cs_start_time_ms = 0;
#else
static long unsigned ppg_cs_timeout_ms = 0;
static long unsigned ppg_cs_start_time_s = 0;
static long unsigned ppg_cs_start_time_ms = 0;
#endif

static bool ppg_cs_test_success = true;

static char ppg_cs_flush_buffer[256];
static int ppg_cs_cur_flush_pos = 0;

static PPG_CS_Action_Expectation ppg_cs_action_expectation_queue[256];
static int ppg_cs_n_actions = 0;

#define MMG_MAX_ACTION_NAMES 100
static char *ppg_cs_action_names[MMG_MAX_ACTION_NAMES];
static int ppg_cs_next_action_id = 0;

void ppg_cs_init(void)
{
   ppg_cs_exceptions = PPG_CS_Action_Exception_None;

   ppg_cs_timeout_ms = 0;

   #ifndef __AVR__
   ppg_cs_start_time_s = 0;
   #endif
   ppg_cs_start_time_ms = 0;

   ppg_cs_test_success = true;

   ppg_cs_cur_flush_pos = 0;

   ppg_cs_n_actions = 0;

   ppg_cs_next_action_id = 0;
}

int ppg_cs_register_action(char *action_name)
{
   ppg_cs_action_names[ppg_cs_next_action_id] = action_name;
   return ppg_cs_next_action_id++;
}

char *ppg_cs_get_action_name(int action_id)
{
   if(   (action_id < 0)
      || (action_id >= ppg_cs_next_action_id)) {
      return NULL;
   }
   
   return ppg_cs_action_names[action_id];
}

static void ppg_cs_store_start_time(void)
{
   #ifdef __AVR__
   ppg_cs_start_time_ms = timer_read32();
   #else
   struct timeb a_timeb;
   
   ftime(&a_timeb);
   
   ppg_cs_start_time_s = a_timeb.time;
   ppg_cs_start_time_ms = a_timeb.millitm;
   #endif
}

static long unsigned ppg_cs_run_time_ms(void)
{
   #ifdef __AVR__
   return timer_read32() - ppg_cs_start_time_ms;
   #else
   struct timeb a_timeb;
   
   ftime(&a_timeb);
 
   return (a_timeb.time - ppg_cs_start_time_s)*1000 + a_timeb.millitm - ppg_cs_start_time_ms;
   #endif
}

void ppg_cs_reset_testing_environment(void)
{
   ppg_cs_exceptions = PPG_CS_Action_Exception_None;
   ppg_cs_cur_flush_pos = 0;
   ppg_cs_test_success = true;
   ppg_cs_n_actions = 0;
}

void ppg_cs_process_event_callback( 
                              PPG_Event *event,
                              void *user_data)
{ 
   // Ignore events that were considered
   //
   if(event->flags & PPG_Event_Considered) { return; }
   
   char the_char = (char)(uintptr_t)event->input;
   
   if((uintptr_t)event->flags & PPG_Event_Active) {
      the_char = toupper(the_char);
   }
   
   ppg_cs_flush_buffer[ppg_cs_cur_flush_pos] = the_char;
   ++ppg_cs_cur_flush_pos;
   
//    PPG_LOG("%c", the_char);
}

void ppg_cs_list_flush_buffer(void)
{
   PPG_LOG("Flushed: \'%s\'\n", ppg_cs_flush_buffer);
}

void ppg_cs_flush_events(void)
{  
   ppg_event_buffer_iterate(
      ppg_cs_process_event_callback,
      NULL
   );
}

void ppg_cs_process_action(bool activation, void *user_data) {
   
   PPG_Count action_id = *((uintptr_t*)&user_data);
   
   PPG_LOG("***** Action: %s\n", ppg_cs_action_names[action_id]);
   
   ppg_cs_action_expectation_queue[ppg_cs_n_actions] 
      = (PPG_CS_Action_Expectation) {
            .action_id = action_id,
            .activated = activation
      };
      
   ++ppg_cs_n_actions;
}

static void ppg_cs_break(void) {

   PPG_LOG("\n");
}

void ppg_cs_separator(void) {

   PPG_LOG("***************************************************************************\n");
}

// inline
// static int my_isalpha_lower(int c) {
//     return ((c >= 'a' && c <= 'z')); } 

inline
static int my_isalpha_upper(int c) {
        return ((c >= 'A' && c <= 'Z')); } 

void ppg_cs_process_event(char the_char)
{
   if(!isalpha(the_char)) { return; }
   
   char lower_char = tolower(the_char);
   
   PPG_LOG("\n");
   ppg_cs_separator();
   PPG_LOG("Sending char %c, active: %d\n", the_char, my_isalpha_upper(the_char));
   ppg_cs_separator();
   
   PPG_Event event = {
      .input = (PPG_Input_Id)(uintptr_t)lower_char,
      .time = (PPG_Time)ppg_cs_run_time_ms(),
      .flags = (my_isalpha_upper(the_char)) ? PPG_Event_Active : PPG_Event_Flags_Empty
   };
   
   ppg_event_process(&event);
}

bool ppg_cs_check_and_process_control_char(char c)
{
   bool is_control_char = true;
   
   switch(c) {
      case PPG_CS_CC_Short_Delay:
      {
         #ifdef __AVR__
         wait_ms(ppg_cs_timeout_ms);
         PPG_LOG("Short delay: %lu\n", ppg_cs_timeout_ms);
         #else
         int microseconds = (int)((double)ppg_cs_timeout_ms*0.3*1000);
         PPG_LOG("Short delay: %d\n", microseconds);
         usleep(microseconds);
         #endif
      }
         break;
         
      case PPG_CS_CC_Long_Delay:
      {
         #ifdef __AVR__
         wait_ms(ppg_cs_timeout_ms*3);
         PPG_LOG("Long delay: %lu\n", ppg_cs_timeout_ms*3);
         #else
         int microseconds = (int)((double)ppg_cs_timeout_ms*3*1000);
         PPG_LOG("Long delay: %d\n", microseconds);
         usleep(microseconds);
         #endif
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
   PPG_LOG("Sending control string \"%s\"\n", string);
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
   PPG_LOG("Sending control string \"%s\" in on-off mode\n", string);
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
   
//    PPG_LOG("time [ms]: %ld\n", *time);
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
   
   if(time1 == time2) {
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

void ppg_cs_compile(void)
{
   // Store the start time as a reference
   //
   ppg_cs_store_start_time();
   
   ppg_global_compile();
   
   PPG_PATTERN_PRINT_TREE
}

void ppg_cs_on_signal(
                        uint8_t slot_id, 
                        void *user_data)
{
//    PPG_LOG("Slot id: %d\n", slot_id);
   
   ppg_cs_break();
   
   switch(slot_id) {
      case PPG_On_Abort:
         PPG_LOG("# Registering abortion\n");
         ppg_cs_exceptions |= PPG_CS_Action_Exception_Aborted;
         ppg_cs_flush_events();
         break;
      case PPG_On_Timeout:
         PPG_LOG("# Registering timeout\n");
         ppg_cs_exceptions |= PPG_CS_Action_Exception_Timeout;
         ppg_cs_flush_events();
         break;
      case PPG_On_Match_Failed:
         PPG_LOG("# Registering match failed\n");
         ppg_cs_exceptions |= PPG_CS_Action_Exception_Match_Failed;
         break;
      case PPG_On_Flush_Events:
         PPG_LOG("# Registering flush events\n");
         ppg_cs_flush_events();
         break;
   }
}

void ppg_cs_check_test_results(uint8_t expected)
{
   ppg_cs_break();
   
   if(ppg_cs_exceptions != (expected)) {
      PPG_LOG("! Exception state mismatch\n");

      ppg_cs_test_success = false;
   }
   else {
      PPG_LOG("Exception state verified\n");
   }
   
   PPG_LOG("   aborted: expected: %d, actual: %d\n",
      ((expected) & PPG_CS_Action_Exception_Aborted),
      (ppg_cs_exceptions & PPG_CS_Action_Exception_Aborted));
   
   PPG_LOG("   timeout: expected: %d, actual: %d\n",
      ((expected) & PPG_CS_Action_Exception_Timeout),
      (ppg_cs_exceptions & PPG_CS_Action_Exception_Timeout)
   );
   
   PPG_LOG("   match failed: expected: %d, actual: %d\n",
      ((expected) & PPG_CS_Action_Exception_Match_Failed),
      (ppg_cs_exceptions & PPG_CS_Action_Exception_Match_Failed)
   );
   
   ppg_token_list_all_active();
}

void ppg_cs_check_flushed(char *expected)
{
   ppg_cs_flush_buffer[ppg_cs_cur_flush_pos] = '\0';
   ++ppg_cs_cur_flush_pos;
   
   if(strcmp(ppg_cs_flush_buffer, expected)) {
      PPG_LOG("! Flush buffer mismatch\n");
      PPG_LOG("   expected: \'%s\'\n", expected);
      PPG_LOG("   actual:   \'%s\'\n", ppg_cs_flush_buffer);
      
      ppg_cs_test_success = false;
   }
   else {
      PPG_LOG("Flush buffer test successfully passed (\'%s\')\n", expected);
   }
}

void ppg_cs_output_test_info(char *file, int line)
{
   PPG_LOG("Next test at %s: %d\n", file, line);
}

void ppg_cs_check_test_success(char *file, int line)
{
   if(!ppg_cs_test_success) {
      PPG_LOG("! %s: %d\n", file, line);
      PPG_LOG("Test failed. Aborting.\n");
      
      #ifdef __AVR__
      printf("__PAPAGENO_TEST_FAILED__\n");
      #endif
      abort();
   }
   else {
      PPG_LOG("All assertions passed.\n");
   }
}

void ppg_cs_check_action_series(int n_actions, 
                                PPG_CS_Action_Expectation* expected)
{
   ppg_cs_break();
   
   bool n_actions_ok = true;
   bool actions_test_success = true;
   
   if(n_actions < ppg_cs_n_actions) {
      PPG_LOG("! There occurred more actions than expected\n");
      n_actions_ok = false;
   }
   else if(n_actions > ppg_cs_n_actions) {
      PPG_LOG("! There occurred less actions than expected\n");
      n_actions_ok = false;
   }
   else {
      for(int i = 0; i < ppg_cs_n_actions; ++i) {
         if(   (ppg_cs_action_expectation_queue[i].action_id 
                  != expected[i].action_id)
            || (ppg_cs_action_expectation_queue[i].activated 
                  != expected[i].activated)) 
         {
            PPG_LOG("! Action mismatch of action %d\n", i);
            PPG_LOG("   expected: %s, activated: %d\n",        
               ppg_cs_get_action_name(expected[i].action_id),
               expected[i].activated
            );
            PPG_LOG("   actual:   %s, activated: %d\n",
               ppg_cs_get_action_name(
                  ppg_cs_action_expectation_queue[i].action_id),
               ppg_cs_action_expectation_queue[i].activated
            );
            
            actions_test_success = false;
         }
      }
   }
   
   actions_test_success = (actions_test_success && n_actions_ok);
   
   if(!n_actions_ok) {
      PPG_LOG("expected:\n");
      
      for(int i = 0; i < n_actions; ++i) {
         PPG_LOG("   %s, activated: %d\n",
            ppg_cs_get_action_name(expected[i].action_id),
            expected[i].activated
         );
      }
      
      PPG_LOG("occurred:\n");
      
      for(int i = 0; i < ppg_cs_n_actions; ++i) {
         PPG_LOG("   %s, activated: %d\n",
            ppg_cs_get_action_name(
               ppg_cs_action_expectation_queue[i].action_id),
               ppg_cs_action_expectation_queue[i].activated
         );
      }
   }
   
   if(actions_test_success) {
      PPG_LOG("Actions test successfully passed\n");
      for(int i = 0; i < n_actions; ++i) {
         PPG_LOG("   %s, activated: %d\n", 
               ppg_cs_get_action_name(expected[i].action_id),
               expected[i].activated);
      }
   }
   else {
      ppg_cs_test_success = false;
   }
}

void ppg_cs_event_char_callback( 
                              PPG_Event *event,
                              void *user_data)
{ 
   char the_char = (char)(uintptr_t)event->input;
   
   if((uintptr_t)event->flags & PPG_Event_Active) {
      the_char = toupper(the_char);
   }
   else {
      the_char = tolower(the_char);
   }
   
   PPG_LOG("%c", the_char);
}

void ppg_cs_event_considered_callback( 
                              PPG_Event *event,
                              void *user_data)
{ 
   #if PPG_HAVE_LOGGING
   char c_char = 0;
   
   if(event->flags & PPG_Event_Considered) {
      c_char = '+';
   }
   else {
      c_char = '-';
   }
   
   PPG_LOG("%c", c_char);
   #endif
}

void ppg_cs_list_event_queue(void)
{
   PPG_LOG("\nEvent queue:\n");
   
   PPG_LOG("   \'");
   
   ppg_event_buffer_iterate((PPG_Event_Processor_Fun)ppg_cs_event_char_callback, 
                            NULL);
   
   PPG_LOG("\'\n");
   
   PPG_LOG("    ");
   
   ppg_event_buffer_iterate((PPG_Event_Processor_Fun)ppg_cs_event_considered_callback, 
                            NULL);
   PPG_LOG("\n");
}
