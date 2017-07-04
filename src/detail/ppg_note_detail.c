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

#include "detail/ppg_note_detail.h"
#include "ppg_debug.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_input_detail.h"

#include <stdlib.h>

static PPG_Processing_State ppg_note_match_event(	
											PPG_Note *note,
											PPG_Event *event) 
{	
	PPG_ASSERT(note->super.state == PPG_Token_In_Progress);
	
	/* Set state appropriately 
	 */
	if(ppg_context->input_id_equal(note->input.input_id, event->input_id)) {
		
		bool input_active 
			= note->input.check_active(note->input.input_id, event->state);
			
		if(input_active) {
			note->active = true;
			note->super.state = PPG_Token_In_Progress;
		}
		else {
			if(note->active) {
	// 		PPG_PRINTF("note successfully finished\n");
				PPG_PRINTF("N");
				note->super.state = PPG_Token_Completed;
			}
		}
	}
	else {
// 		PPG_PRINTF("note invalid\n");
		note->super.state = PPG_Token_Invalid;
	}
	
	return note->super.state;
}

static void ppg_note_reset(PPG_Note *note) 
{
	ppg_token_reset((PPG_Token__*)note);
	
	note->active = false;
}

static bool ppg_note_equals(PPG_Note *n1, PPG_Note *n2) 
{
	return ppg_context->input_id_equal(n1->input.input_id, n2->input.input_id);
}

#if PAPAGENO_PRINT_SELF_ENABLED
static void ppg_note_print_self(PPG_Note *p)
{
	ppg_token_print_self((PPG_Token__*)p);
	
	PPG_PRINTF("note\n");
	PPG_PRINTF("   input_id: %d\n", p->input.input_id);
	PPG_PRINTF("   active: %d\n", p->active);
}
#endif

static PPG_Token_Vtable ppg_note_vtable =
{
	.match_event 
		= (PPG_Token_Match_Event_Fun) ppg_note_match_event,
	.reset 
		= (PPG_Token_Reset_Fun) ppg_note_reset,
	.trigger_action 
		= (PPG_Token_Trigger_Action_Fun) ppg_token_trigger_action,
	.destroy 
		= (PPG_Token_Destroy_Fun) ppg_token_destroy,
	.equals
		= (PPG_Token_Equals_Fun) ppg_note_equals
		
	#if PAPAGENO_PRINT_SELF_ENABLED
	,
	.print_self
		= (PPG_Token_Print_Self_Fun) ppg_note_print_self
	#endif
};

PPG_Note *ppg_note_new(PPG_Note *note)
{
    /* Explict call to parent constructor 
	  */
    ppg_token_new((PPG_Token__*)note);

    note->super.vtable = &ppg_note_vtable;
	 
	 ppg_init_input(&note->input);
	 
    return note;
}

PPG_Note *ppg_note_alloc(void) {
    return (PPG_Note*)malloc(sizeof(PPG_Note));
}