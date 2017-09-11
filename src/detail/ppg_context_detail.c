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

#include "detail/ppg_context_detail.h"
#include "detail/ppg_input_detail.h"
#include "ppg_statistics.h"
#include "ppg_debug.h"

PPG_Context *ppg_context = NULL;

/** @brief This function initializes a signal callback
 *
 * @param cb A pointer to the callback struct
 */
inline
static void ppg_signal_callback_init(PPG_Signal_Callback *cb)
{
   cb->func = NULL;
   cb->user_data = NULL;
}

void ppg_global_initialize_context(PPG_Context *context) {
   
//    PPG_LOG("Initializing context\n");
   
   ppg_event_buffer_init(&context->event_buffer);
   ppg_furcation_stack_init(&context->furcation_stack);
   ppg_active_tokens_init(&context->active_tokens);
   
//    ppg_bitfield_init(&context->active_inputs);
   
   context->properties.timeout_enabled = true;
   context->properties.papageno_enabled = true;
      
   #if PPG_HAVE_LOGGING
   context->properties.logging_enabled = true;
   #endif
   
   context->properties.destruction_enabled = true;
   
   context->tree_depth = 0;
   context->layer = 0;
   ppg_global_init_input(&context->abort_input);
   context->time_last_event = 0;
   context->event_timeout = 0;
   context->event_processor = NULL;
   
   ppg_time_manager_init(&context->time_manager);
   
   ppg_signal_callback_init(&context->signal_callback);
   
   context->pattern_root = ppg_token_alloc();

   /* Initialize the pattern root
   */
   ppg_token_new(context->pattern_root);
   
   context->pattern_root->misc.state = PPG_Token_Initialized;
   
   context->current_token = NULL;
   
   #if PPG_HAVE_STATISTICS
   ppg_statistics_clear(&context->statistics);
   #endif
};

size_t ppg_context_get_size_requirements(PPG_Context *context)
{
   return sizeof(PPG_Context)
         + context->furcation_stack.n_furcations*sizeof(PPG_Furcation);
}

char *ppg_context_copy(PPG_Context *context, void *target__)
{
   char *target = target__;
   
   PPG_Context *target_context = (PPG_Context*)target;
   
   *target_context = *ppg_context;
   
   target += sizeof(PPG_Context);
   
   target_context->furcation_stack.furcations = (PPG_Furcation *)target;
   
   target += context->furcation_stack.n_furcations*sizeof(PPG_Furcation);
   
   return target;
}
