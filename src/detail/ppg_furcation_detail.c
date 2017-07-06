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

#include "detail/ppg_furcation_detail.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_pattern_detail.h"
#include "ppg_debug.h"

#include <stdlib.h>

void ppg_furcation_buffer_init(PPG_Furcation_Buffer *buffer)
{
	buffer->furcations = NULL;
	buffer->n_furcations = 0;
	buffer->cur_furcation = -1;
}

void ppg_furcation_push_or_update(PPG_Count n_tokens_in_progress,
											PPG_Token__ *branch)
{
	// Check if there is already a furcation open for the current node
	//
	if(	(PPG_FB.cur_furcation != -1)
		&& (PPG_FB.furcations[PPG_FB.cur_furcation].token != ppg_context->current_token)) {
		
		// If not, we open a new furcation
	
		PPG_ASSERT(PPG_FB.cur_furcation < PPG_FB.n_furcations - 1);
		
		++PPG_FB.cur_furcation;
		
		PPG_FB.furcations[PPG_FB.cur_furcation].n_possible_branches = n_tokens_in_progress;
		PPG_FB.furcations[PPG_FB.cur_furcation].event_id = PPG_EB.cur;
		PPG_FB.furcations[PPG_FB.cur_furcation].token = ppg_context->current_token;
		PPG_FB.furcations[PPG_FB.cur_furcation].branch = branch;
	}
	else {
		
		// Update the furcation information
		//
		PPG_FB.furcations[PPG_FB.cur_furcation].n_possible_branches = n_tokens_in_progress;
		PPG_FB.furcations[PPG_FB.cur_furcation].branch = branch;
	}
}

void ppg_furcation_buffer_resize(void)
{
	if(ppg_context->furcation_buffer.furcations) { return; }
	
	PPG_Count depth = 1 + ppg_pattern_tree_depth();
	
	ppg_context->furcation_buffer.furcations 
			= (PPG_Furcation*)malloc(depth*sizeof(PPG_Furcation));
}

void ppg_furcation_buffer_free(PPG_Furcation_Buffer *buffer)
{
	if(!buffer->furcations) { return; }
	
	free(buffer->furcations);
	
	buffer->furcations = NULL;
}
	