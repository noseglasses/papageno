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

static bool ppg_note_match_event(   
                                 PPG_Note *note,
                                 PPG_Event *event,
                                 bool modify_only_if_consuming
                                ) 
{  
   PPG_LOG("Nt 0x%" PRIXPTR ", input 0x%d, ppg_note_match_event\n", (uintptr_t)note, note->input);
   
   PPG_Count note_flags 
      = note->super.misc.flags;
   
   // Assert that the note requires either activation or deactivation
   //
   PPG_ASSERT(note_flags != 0);
   
   switch(note_flags & (   PPG_Note_Flag_Match_Activation
                        | PPG_Note_Flag_Match_Deactivation)) {
     
      case     (PPG_Note_Flag_Match_Activation
            | PPG_Note_Flag_Match_Deactivation):
            
            if(note->input != event->input) {
               if(   (note_flags & PPG_Token_Flags_Pedantic) 
                  || (event->flags & PPG_Event_Active)) {
                  
                  if(!modify_only_if_consuming) {
                     note->super.misc.state = PPG_Token_Invalid;
                  }
               }

               return false;
            }
               
            if(event->flags & PPG_Event_Active) {
               
               PPG_LOG("I act\n");
               
               // Mark the note as active
               //
               note->super.misc.flags |= PPG_Note_Type_Active;
               
               PPG_LOG("Nt 0x%" PRIXPTR " fin\n", (uintptr_t)note);
               
               if(note_flags & PPG_Token_Flags_Pedantic) {
                  note->super.misc.state = PPG_Token_Activation_In_Progress;
               }
               else {
                  note->super.misc.state = PPG_Token_Matches;
               }
            }
            else {
               
               if(note_flags & PPG_Note_Type_Active) {
                  PPG_LOG("I deact\n");
               
                  if(note_flags & PPG_Token_Flags_Pedantic) {
                     PPG_LOG("Nt fin\n");
         //             PPG_LOG("N");
                     note->super.misc.state = PPG_Token_Matches;
                  }
                  else {
                     note->super.misc.state = PPG_Token_Finalized;
                  }
               }
               else {
               
                  // The input is not active but this is a deactivation 
                  // event. Thus, we ignore it as it belongs 
                  // to the activation of another token.
                  
                  // Note: Even the deactivation of the associated input must
                  //       be ignored here.
                  //
                  return false;
               }
            }
            break;
            
      case PPG_Note_Flag_Match_Activation:
         
         PPG_LOG("Only act\n");
         if(   (note->input == event->input)
            && (event->flags & PPG_Event_Active)) {
            PPG_LOG("I mtch\n");
            note->super.misc.state = PPG_Token_Matches;
            note->super.misc.flags |= PPG_Token_Flags_Done;
            return true;
         }
         
         PPG_LOG("I inact\n");
         if(!modify_only_if_consuming) {
            note->super.misc.state = PPG_Token_Invalid;
         }
         
         return false;
         
      case PPG_Note_Flag_Match_Deactivation:
      
         if(   (note->input == event->input)
            && ((event->flags & PPG_Event_Active) == 0)) {
            note->super.misc.state = PPG_Token_Finalized;
            
            note->super.misc.flags |= PPG_Token_Flags_Done;
            return true;
         }
         
         if(!modify_only_if_consuming) {
            note->super.misc.state = PPG_Token_Invalid;
         }
         
         return false;
   }
   
   return true;
}

static void ppg_note_reset(PPG_Note *note) 
{
   ppg_token_reset_control_state((PPG_Token__*)note);
   
   // Notes that only match deactivation start in matching state
   //
   if((note->super.misc.flags 
                     & PPG_Note_Flag_Match_Activation) == 0) {
      note->super.misc.state = PPG_Token_Matches;
   }
   
   // Clear the activation state
   //
   note->super.misc.flags 
         &= (PPG_Count)~PPG_Note_Type_Active;
}

static bool ppg_note_equals(PPG_Note *n1, PPG_Note *n2) 
{
   return n1->input == n2->input;
}

static PPG_Count ppg_note_token_precedence(PPG_Token__ *token)
{
   PPG_Note *note = (PPG_Note *)token;
   
   PPG_Count note_flags = note->super.misc.flags;
   
   if(   (note_flags & PPG_Note_Flag_Match_Activation)
      && (note_flags & PPG_Note_Flag_Match_Deactivation)) {
      return PPG_Token_Precedence_Note;
   }
   
   return PPG_Token_Precedence_Explicit_Note;
}

static size_t ppg_note_dynamic_size(PPG_Token__ *token)
{
   return   sizeof(PPG_Note)
         +  ppg_token_dynamic_member_size(token);
}

static char *ppg_note_placement_clone(PPG_Token__ *token, char *buffer)
{
   PPG_Note *note = (PPG_Note *)token;
   
   *((PPG_Note *)buffer) = *note;
   
   PPG_Token__ *clone = (PPG_Token__ *)buffer;
   
//    printf("Replacing children pointer %p with %p\n", clone->children, (PPG_Token__ **)(buffer + sizeof(PPG_Note)));
   
   clone->children = (PPG_Token__ **)(buffer + sizeof(PPG_Note));
   
   return ppg_token_copy_dynamic_members(token, buffer + sizeof(PPG_Note));
}

#if PPG_PRINT_SELF_ENABLED
static void ppg_note_print_self(PPG_Note *p, PPG_Count indent, bool recurse)
{
   PPG_I PPG_LOG("<*** nt (0x%" PRIXPTR ") ***>\n", (uintptr_t)p);
   ppg_token_print_self_start((PPG_Token__*)p, indent);
   PPG_I PPG_LOG("\tI: 0x%" PRIXPTR "\n", (uintptr_t)p->input);
   PPG_I PPG_LOG("\tA: %d\n", (p->super.misc.flags & PPG_Note_Type_Active));
   PPG_I PPG_LOG("\tm a: %d\n", (bool)(p->super.misc.flags & PPG_Note_Flag_Match_Activation));
   PPG_I PPG_LOG("\tm d: %d\n", (bool)(p->super.misc.flags & PPG_Note_Flag_Match_Deactivation));
   ppg_token_print_self_end((PPG_Token__*)p, indent, recurse);
}
#endif

#if PPG_HAVE_DEBUGGING
static bool ppg_note_check_initialized(PPG_Token__ *token)
{
   PPG_Note *note = (PPG_Note *)token;
  
   bool assertion_failed = false;
   
   // Skip the parent call as we need a specialized check
//    assertion_failed |= ppg_token_check_initialized(token);
   
   if((note->super.misc.flags 
                     & PPG_Note_Flag_Match_Activation) == 0) {
      PPG_ASSERT_WARN(note->super.misc.state == PPG_Token_Matches);
   }
   else {
      PPG_ASSERT_WARN(note->super.misc.state == PPG_Token_Initialized);
   }
   
   PPG_ASSERT_WARN(token->misc.action_state == PPG_Action_Disabled);
   
   PPG_ASSERT_WARN(
      (note->super.misc.flags 
         & PPG_Note_Type_Active) == 0);
   
   return assertion_failed;
}
#endif

PPG_Token_Vtable ppg_note_vtable =
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
      = (PPG_Token_Precedence_Fun)ppg_note_token_precedence,
   .dynamic_size 
      = (PPG_Token_Dynamic_Size_Requirement_Fun)ppg_note_dynamic_size,
   .placement_clone
      = (PPG_Token_Placement_Clone_Fun)ppg_note_placement_clone,
   .register_ptrs_for_compression
      = (PPG_Token_Register_Pointers_For_Compression)ppg_token_register_pointers_for_compression
      
   #if PPG_PRINT_SELF_ENABLED
   ,
   .print_self
      = (PPG_Token_Print_Self_Fun) ppg_note_print_self
   #endif
   
   #if PPG_HAVE_DEBUGGING
   ,
   .check_initialized
      = (PPG_Token_Check_Initialized_Fun)ppg_note_check_initialized
   #endif
};

PPG_Note *ppg_note_new(PPG_Note *note)
{
    /* Explict call to parent constructor 
     */
    ppg_token_new((PPG_Token__*)note);

    note->super.vtable = &ppg_note_vtable;
    
    note->super.misc.flags = PPG_Token_Flags_Empty;
    
    ppg_global_init_input(&note->input);
    
    return note;
}

PPG_Note *ppg_note_alloc(void) {
    return (PPG_Note*)malloc(sizeof(PPG_Note));
}
