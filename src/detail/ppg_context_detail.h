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

#ifndef PPG_CONTEXT_DETAIL_H
#define PPG_CONTEXT_DETAIL_H

#include "ppg_event.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_event_buffer_detail.h"
#include "detail/ppg_event_buffer_detail.h"
#include "detail/ppg_furcation_detail.h"
#include "ppg_signal_callback.h"

#include <stddef.h>

typedef struct PPG_Context_Struct
{
   PPG_Event_Buffer event_buffer;
   PPG_Furcation_Stack furcation_stack;
   PPG_Bitfield active_inputs;
   
   PPG_Token__ pattern_root;

   PPG_Token__ *current_token;

   bool timeout_enabled;
   bool papageno_enabled;
   
   PPG_Count tree_depth;
   
   PPG_Layer layer;

   PPG_Input_Id abort_input;

   PPG_Time time_last_event;

   PPG_Time event_timeout;
   
   PPG_Event_Processor_Fun event_processor;
   
   PPG_Time_Manager time_manager;
   
   PPG_Signal_Callback signal_callback;
  
} PPG_Context;

PPG_Context *ppg_context;

void ppg_global_initialize_context(PPG_Context *context);

/* The following macros influence the build

PAPAGENO_DISABLE_CONTEXT_SWITCHING 
      Disables the capability to switch contexts 

*/

#endif
