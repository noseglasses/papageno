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

#include "detail/ppg_furcation_detail.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_pattern_detail.h"
#include "ppg_debug.h"
#include "detail/ppg_malloc_detail.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

void ppg_furcation_stack_init(PPG_Furcation_Stack *stack)
{
   stack->furcations = NULL;
   stack->n_furcations = 0;
   stack->cur_furcation = -1;
   stack->max_furcations = 0;
}

void ppg_furcation_stack_resize(PPG_Furcation_Stack *stack, 
                                PPG_Count new_size)
{
   PPG_ASSERT(stack);
   
   if(new_size <= stack->max_furcations) { return; }
   
   PPG_Furcation *new_furcations
      = (PPG_Furcation*)PPG_MALLOC(sizeof(PPG_Furcation)*new_size);
      
   stack->max_furcations = new_size;
   
   if(stack->furcations && (stack->n_furcations > 0)) {
      memcpy(new_furcations, stack->furcations, 
             sizeof(PPG_Furcation)*stack->n_furcations);
   }
   
   stack->furcations = new_furcations;
}

void ppg_furcation_stack_restore(PPG_Furcation_Stack *stack)
{
   PPG_ASSERT(stack);
   
   PPG_Count saved_size = stack->max_furcations;
   
   // Enable this method to be used for initialization
   // during context compilation
   //
   if(ppg_context && ppg_context->tree_depth > saved_size) {
      saved_size = ppg_context->tree_depth;
   }
   
   stack->furcations = NULL;
   stack->max_furcations = 0;
   
   ppg_furcation_stack_resize(stack, saved_size);
}

void ppg_furcation_stack_free(PPG_Furcation_Stack *stack)
{
   if(!stack->furcations) { return; }
   
   free(stack->furcations);
   
   stack->furcations = NULL;
}
   