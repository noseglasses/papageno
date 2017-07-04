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

#include "detail/ppg_token_detail.h"
#include "ppg_debug.h"
#include "ppg_global.h"
#include "detail/ppg_pattern_detail.h"

#if PAPAGENO_PRINT_SELF_ENABLED
static void ppg_recursively_print_pattern(PPG_Token__ *p)
{
	if(	p->parent
		&& p->parent != &ppg_context->pattern_root) {
		
		ppg_recursively_print_pattern(p->parent);
	}
	
	PPG_CALL_VIRT_METHOD(p, print_self);
}
#endif


PPG_Token ppg_pattern(		
							PPG_Layer layer, 
							PPG_Count n_tokens,
							PPG_Token tokens[])
{ 
	PPG_PRINTF("Adding pattern\n");
	
	ppg_init();
		
	return ppg_pattern_from_list(layer, n_tokens, tokens);
}

#if PAPAGENO_PRINT_SELF_ENABLED
static void ppg_recursively_print_pattern(PPG_Token__ *p)
{
	if(	p->parent
		&& p->parent != &ppg_context->pattern_root) {
		
		ppg_recursively_print_pattern(p->parent);
	}
	
	PPG_CALL_VIRT_METHOD(p, print_self);
}
#endif