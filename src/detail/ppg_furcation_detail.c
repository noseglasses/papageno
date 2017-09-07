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

#include "detail/ppg_furcation_detail.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_pattern_detail.h"
#include "ppg_debug.h"

#include <stdlib.h>

void ppg_furcation_stack_init(PPG_Furcation_Stack *stack)
{
   stack->furcations = NULL;
   stack->n_furcations = 0;
   stack->cur_furcation = -1;
}

void ppg_furcation_stack_resize(void)
{
   if(PPG_FB.furcations) { return; }
   
   PPG_FB.n_furcations = ppg_context->tree_depth;
   
   PPG_FB.furcations 
         = (PPG_Furcation*)calloc(1, ppg_context->tree_depth*sizeof(PPG_Furcation));
}

void ppg_furcation_stack_free(PPG_Furcation_Stack *stack)
{
   if(!stack->furcations) { return; }
   
   free(stack->furcations);
   
   stack->furcations = NULL;
}
   