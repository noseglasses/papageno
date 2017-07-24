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

#include "detail/ppg_note_detail.h"
#include "ppg_debug.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_input_detail.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_token_precedence_detail.h"

#include <stdlib.h>

static void ppg_note_match_event(   
                                 PPG_Note *note,
                                 PPG_Event *event) 
{  
   PPG_ASSERT(
         (ppg_token_get_state(note) == PPG_Token_In_Progress)
      || (ppg_token_get_state(note) == PPG_Token_Initialized)
   );

   PPG_Count activation_flags 
      = ppg_token_get_flags(note, PPG_Token_Match_Activation | PPG_Token_Match_Deactivation);
   
   // Assert that the note requires either activation or deactivation
   //
   PPG_ASSERT(activation_flags != 0);
   
   if(note->flags & PPG_Token_Match_Activation) {
      
      if(note->flags & PPG_Token_Match_Deactivation) {
   
         /* Set state appropriately 
         */
         if(note->input == event->input) {
               
            if(event->flags & PPG_Event_Active) {
               
               PPG_LOG("I act\n");
               
               note->flags |= PPG_Note_Type_Active;
               
      #if PPG_PEDANTIC_TOKENS
               ppg_token_set_state(note, PPG_Token_In_Progress);
      #else 
               
               PPG_LOG("Nt 0x%" PRIXPTR " fin\n", 
             (uintptr_t)note);
      //             PPG_LOG("N");
               ppg_token_set_state(note, PPG_Token_Matches);
      #endif
            }
            else {
               
               if(note->flags & PPG_Note_Type_Active) {
                  PPG_LOG("I deact\n");
               
      #if PPG_PEDANTIC_TOKENS
                  PPG_LOG("Nt fin\n");
      //             PPG_LOG("N");
                  ppg_token_set_state(note, PPG_Token_Matches);
      #endif
               }
               else {
                  
                  // The input is not active but this is a deactivation 
                  // event. Thus, we ignore it as it belongs 
                  // to the activation of another token.
                  //
                  return;
               }
            }
         }
         else {
      //       PPG_LOG("note invalid\n");
            
      #ifndef PPG_PEDANTIC_TOKENS
            if(event->flags & PPG_Event_Active) {
               
               // Only if the non matching input is activated, we
               // complain
               //
      #endif
               ppg_token_set_state(note, PPG_Token_Invalid);
      #ifndef PPG_PEDANTIC_TOKENS
            }
      #endif
            return;
         }
      }
      else {
         
         PPG_LOG("Only act\n");
         
         // Only activation
         
         if(note->input != event->input) {
            
            PPG_LOG("I wrg\n");
            ppg_token_set_state(note, PPG_Token_Invalid);
            return;
         }
         
         if(event->flags & PPG_Event_Active) {
            PPG_LOG("I mtch\n");
            ppg_token_set_state(note, PPG_Token_Matches);
            return;
         }
         
         PPG_LOG("I inact\n");
         ppg_token_set_state(note, PPG_Token_Invalid);
         
         return;
      }
   }
   else if(note->flags & PPG_Token_Match_Deactivation) {
         
      // Only deactivation
      
      if(note->input != event->input) {
         ppg_token_set_state(note, PPG_Token_Invalid);
         return;
      }
      
      if((event->flags & PPG_Event_Active) == 0) {
         ppg_token_set_state(note, PPG_Token_Matches);
         return;
      }
      
      ppg_token_set_state(note, PPG_Token_Invalid);
         
      return;
   }
   
   return;
}

static void ppg_note_reset(PPG_Note *note) 
{
   ppg_token_reset((PPG_Token__*)note);
   
   // Clear the activation state
   //
   note->flags &= (PPG_Count)~PPG_Note_Type_Active;
}

static bool ppg_note_equals(PPG_Note *n1, PPG_Note *n2) 
{
   return n1->input == n2->input;
}

static PPG_Count ppg_note_token_precedence(PPG_Token__ *token)
{
   PPG_Note *note = (PPG_Note *)token;
   
   if(   (note->flags & PPG_Token_Match_Activation)
      && (note->flags & PPG_Token_Match_Deactivation)) {
      return PPG_Token_Precedence_Note;
   }
   
   return PPG_Token_Precedence_Explicit_Note;
}

#if PPG_PRINT_SELF_ENABLED
static void ppg_note_print_self(PPG_Note *p, PPG_Count indent, bool recurse)
{
   PPG_I PPG_LOG("<*** nt (0x%" PRIXPTR ") ***>\n", (uintptr_t)p);
   ppg_token_print_self_start((PPG_Token__*)p, indent);
   PPG_I PPG_LOG("\tI: 0x%" PRIXPTR "\n", (uintptr_t)p->input);
   PPG_I PPG_LOG("\tA: %d\n", (p->flags & PPG_Note_Type_Active));
   PPG_I PPG_LOG("\tm a: %d\n", (bool)(p->flags & PPG_Token_Match_Activation));
   PPG_I PPG_LOG("\tm d: %d\n", (bool)(p->flags & PPG_Token_Match_Deactivation));
   ppg_token_print_self_end((PPG_Token__*)p, indent, recurse);
}
#endif

static PPG_Token_Vtable ppg_note_vtable =
{
   .match_event 
      = (PPG_Token_Match_Event_Fun) ppg_note_match_event,
   .reset 
      = (PPG_Token_Reset_Fun) ppg_note_reset,
   .destroy 
      = (PPG_Token_Destroy_Fun) ppg_token_destroy,
   .equals
      = (PPG_Token_Equals_Fun) ppg_note_equals,
   .token_precedence
      = (PPG_Token_Precedence_Fun)ppg_note_token_precedence
      
   #if PPG_PRINT_SELF_ENABLED
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
    
    note->flags = PPG_Note_Type_Undefined;
    
    ppg_global_init_input(&note->input);
    
    return note;
}

PPG_Note *ppg_note_alloc(void) {
    return (PPG_Note*)malloc(sizeof(PPG_Note));
}
