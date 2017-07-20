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

#include "ppg_context.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_furcation_detail.h"
#include "ppg_debug.h"

#include <stdlib.h>

void* ppg_context_create(void)
{
//    PPG_LOG("Cr. new cntxt\n");
   
   PPG_Context *context = (PPG_Context *)malloc(sizeof(PPG_Context));
   
   ppg_global_initialize_context(context);
   
   return context;
}

void ppg_context_destroy(void *context)
{
   PPG_Context *context__ = (PPG_Context *)context;
   
   ppg_furcation_stack_free(&context__->furcation_stack);
   
   ppg_bitfield_destroy(&context__->active_inputs);
   
   ppg_token_free_children(&context__->pattern_root);
   
   free(context__);
}

#if !PPG_DISABLE_CONTEXT_SWITCHING

void* ppg_global_set_current_context(void *context)
{
   void *old_context = ppg_context;
   
   ppg_context = (PPG_Context *)context;
   
   return old_context;
}

void* ppg_global_get_current_context(void)
{
   return ppg_context;
}

#endif