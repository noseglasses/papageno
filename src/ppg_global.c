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

#include "ppg_global.h"
#include "ppg_token.h"
#include "ppg_debug.h"
#include "ppg_context.h"
#include "ppg_action_flags.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_global_detail.h"
#include "detail/ppg_event_buffer_detail.h"
#include "detail/ppg_signal_detail.h"
#include "detail/ppg_pattern_detail.h"

#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdarg.h>

void ppg_global_init(void) {

   if(!ppg_context) {
      
      ppg_context = (PPG_Context *)ppg_context_create();
   }
}

void ppg_global_compile(void)
{
   ppg_context->tree_depth = ppg_pattern_tree_depth();
   
   // Initialize the furcation buffer to ensure correct size (the maximum
   // search tree depth)
   //
   ppg_furcation_stack_resize();
}

void ppg_global_finalize(void) {
   
   if(!ppg_context) { return; }
   
   ppg_context_destroy(ppg_context);
   
   ppg_context = NULL;
}

void ppg_global_reset(void)
{
   ppg_global_finalize();
   
   ppg_context = (PPG_Context *)ppg_context_create();
}

/* Use this function to define a input that always aborts a pattern
 */
PPG_Input_Id ppg_global_set_abort_trigger(PPG_Input_Id input)
{
   PPG_Input_Id old_input = ppg_context->abort_input;
   
   ppg_context->abort_input = input;
   
   return old_input;
}

PPG_Input_Id ppg_global_get_abort_trigger(void)
{
   return ppg_context->abort_input;
}

PPG_Time ppg_global_set_timeout(PPG_Time timeout)
{
   PPG_Time old_value = ppg_context->event_timeout;
   
   ppg_context->event_timeout = timeout;
   
   return old_value;
}

PPG_Time ppg_global_get_timeout(void)
{
   return ppg_context->event_timeout;
}

PPG_Event_Processor_Fun ppg_global_set_default_event_processor(PPG_Event_Processor_Fun event_processor)
{
   PPG_Event_Processor_Fun old_event_processor = ppg_context->event_processor;
   
   ppg_context->event_processor = event_processor;
   
   return old_event_processor;
}

bool ppg_global_set_enabled(bool state)
{
   bool old_state = ppg_context->properties.papageno_enabled;
   
   ppg_context->properties.papageno_enabled = state;

   return old_state;
}

bool ppg_global_set_timeout_enabled(bool state)
{
   bool previous_state = ppg_context->properties.timeout_enabled;
   
   ppg_context->properties.timeout_enabled = state;
   
   return previous_state;
}

bool ppg_global_get_timeout_enabled(void)
{
   return ppg_context->properties.timeout_enabled;
}

PPG_Layer ppg_global_set_layer(PPG_Layer layer)
{
   PPG_Layer previous_layer = ppg_context->layer;
   
   if(previous_layer != layer) {
      if(ppg_context->current_token) {
         ppg_global_abort_pattern_matching();
      }
   }
   
   ppg_context->layer = layer;
   
   return previous_layer;
}

PPG_Layer ppg_global_get_layer(void)
{
   return ppg_context->layer;
}

PPG_Signal_Callback ppg_global_set_signal_callback(PPG_Signal_Callback callback)
{
   PPG_Signal_Callback previous_callback = ppg_context->signal_callback;
   
   ppg_context->signal_callback = callback;
   
   return previous_callback;
}

PPG_Signal_Callback ppg_global_get_signal_callback(void)
{
   return ppg_context->signal_callback;
}

void ppg_global_abort_pattern_matching(void)
{     
   if(!ppg_context->current_token) { return; }
   
//    PPG_LOG("Abrt pttrn\n");

   // Note: It is on the user to read back any 
   //       events that were stored from the PPG_On_Abort signal callback
         
   ppg_signal(PPG_On_Abort);
   
   ppg_delete_stored_events();
   
   ppg_reset_pattern_matching_engine();
}

#if PPG_HAVE_DEBUGGING

bool ppg_global_check_consistency(void)
{
   return false;//ppg_token_recurse_check_initialized(&ppg_context->pattern_root);
}

#endif
