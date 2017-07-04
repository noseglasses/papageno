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

#include "ppg_context.h"
#include "detail/ppg_context_detail.h"
#include "ppg_debug.h"

#include <stdlib.h>

void* ppg_create_context(void)
{
	PPG_PRINTF("Creating new context\n");
	
	PPG_Context *context = (PPG_Context *)malloc(sizeof(PPG_Context));
	
	ppg_initialize_context(context);
	
	return context;
}

#ifndef PAPAGENO_DISABLE_CONTEXT_SWITCHING

void ppg_destroy_context(void *context_void)
{
	PPG_Context *context = (PPG_Context *)context_void;
	
	ppg_token_free_successors(&context->pattern_root);
	
	free(context);
}

void* ppg_set_context(void *context_void)
{
	void *old_context = ppg_context;
	
	ppg_context = (PPG_Context *)context_void;
	
	return old_context;
}

void* ppg_get_current_context(void)
{
	return ppg_context;
}

#endif