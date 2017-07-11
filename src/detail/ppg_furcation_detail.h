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

#ifndef PPG_FURCATION_DETAIL_H
#define PPG_FURCATION_DETAIL_H

#include "ppg_settings.h"
#include "detail/ppg_token_detail.h"
#include "ppg_bitfield.h"

typedef struct {
	PPG_Count n_possible_branches; ///< The number of remaining
							// branches that have not been checked yet
							
	PPG_Count event_id; ///< The index of the first event 
							//	in the event buffer that is fed to 
							// one of the tokens in the branch
	PPG_Token__ *token;
	PPG_Token__ *branch;
	
	PPG_Bitfield	active_inputs;
} PPG_Furcation;

typedef struct {
	PPG_Furcation *furcations;
	PPG_Count n_furcations;
	PPG_Id cur_furcation;
} PPG_Furcation_Buffer;

#define PPG_FB ppg_context->furcation_buffer

// Initialize the furcation buffer with respect to the size of the 
// current pattern tree
//
void ppg_furcation_buffer_init(PPG_Furcation_Buffer *buffer);

void ppg_furcation_buffer_resize(void);

void ppg_furcation_push_or_update(PPG_Count n_tokens_in_progress,
											PPG_Token__ *branch);

void ppg_furcation_buffer_free(PPG_Furcation_Buffer *buffer);
#endif
