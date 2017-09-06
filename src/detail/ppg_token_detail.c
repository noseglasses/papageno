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

#include "detail/ppg_token_detail.h"
#include "ppg_global.h"
#include "ppg_debug.h"
#include "ppg_action.h"
#include "ppg_action_flags.h"
#include "ppg_signals.h"
#include "detail/ppg_context_detail.h"
#include "ppg_input.h"
#include "detail/ppg_event_buffer_detail.h"

#include <stdlib.h>
#include <string.h>

void ppg_token_store_action(PPG_Token__ *token, 
                                   PPG_Action action)
{
   token->action = action; 
   
   PPG_LOG("A tk 0x%" PRIXPTR ": 0x%" PRIXPTR "\n",
              (uintptr_t)token, (uintptr_t)token->action.callback.user_data);
}

void ppg_token_reset_control_state(PPG_Token__ *token)
{
   PPG_LOG("Resetting tk 0x%" PRIXPTR "\n", (uintptr_t)token);
   
   token->misc.state = PPG_Token_Initialized;
   token->misc.action_state = PPG_Action_Disabled;
}

static void ppg_token_allocate_children(PPG_Token__ *token, PPG_Count n_children) {

    token->children 
      = (struct PPG_TokenStruct **)malloc(n_children*sizeof(struct PPG_TokenStruct*));
    token->n_allocated_children = n_children;
}

static void ppg_token_grow_children(PPG_Token__ *token) {

   if(token->n_allocated_children == 0) {
      
      ppg_token_allocate_children(token, 1);
   }
   else {
      PPG_Token__ **oldSucessors = token->children;
      
      ppg_token_allocate_children(token, 2*token->n_allocated_children);
         
      for(PPG_Count i = 0; i < token->n_children; ++i) {
         token->children[i] = oldSucessors[i];
      }
      
      free(oldSucessors); 
   }
}

void ppg_token_add_child(PPG_Token__ *token, PPG_Token__ *child) {
   
   if(token->n_allocated_children == token->n_children) {
      ppg_token_grow_children(token);
   }
   
   token->children[token->n_children] = child;
   
   child->parent = token;
   
   ++token->n_children;
}

void ppg_token_free_children(PPG_Token__ *token)
{
   if(!token->children) { return; }
   
   for(PPG_Count i = 0; i < token->n_children; ++i) {
      
      ppg_token_free(token->children[i]);
   }
   
   free(token->children);
   
   token->children = NULL;
   token->n_allocated_children = 0;
}

PPG_Token__* ppg_token_destroy(PPG_Token__ *token) {

   ppg_token_free_children(token);
   
   return token;
}

static bool ppg_token_equals(PPG_Token__ *p1, PPG_Token__ *p2) 
{
   if(p1->vtable != p2->vtable) { return false; }
   
   return p1->vtable->equals(p1, p2);
}

void ppg_token_free(PPG_Token__ *token) {
   
   PPG_CALL_VIRT_METHOD(token, destroy);

   free(token);
}

PPG_Token__* ppg_token_get_equivalent_child(
                                          PPG_Token__ *parent_token,
                                          PPG_Token__ *sample) 
{
   if(parent_token->n_children == 0) { return NULL; }
   
   for(PPG_Count i = 0; i < parent_token->n_children; ++i) {
      if(ppg_token_equals(
                                 parent_token->children[i], 
                                 sample)
        ) {
         return parent_token->children[i];
      }
   }
   
   return NULL;
}

size_t ppg_token_dynamic_member_size(PPG_Token__ *token)
{
   return token->n_children*sizeof(PPG_Token__ *);
}

size_t ppg_token_dynamic_size(PPG_Token__ *token)
{
   return   sizeof(PPG_Token__) 
         +  ppg_token_dynamic_member_size(token);
}

char *ppg_token_copy_dynamic_members(PPG_Token__ *token, char *buffer)
{
   size_t n_bytes = token->n_children*sizeof(PPG_Token__ *);
   
   memcpy((void*)buffer, (void*)token->children, n_bytes);
   
   return buffer + n_bytes;
}

char *ppg_token_placement_clone(PPG_Token__ *token, char *buffer)
{
   *((PPG_Token__ *)buffer) = *token;
   
   return ppg_token_copy_dynamic_members(token, buffer + sizeof(PPG_Token__));
}


void ppg_token_register_pointers_for_compression(
                                             PPG_Token__ *token,
                                             PPG_Compression_Context__ *ccontext)
{
   ppg_compression_context_register_vptr((void**)&token->vtable, ccontext);
   
   ppg_compression_context_register_symbol((void**)&token->action.callback.func, ccontext);
   
   if(token->action.callback.user_data) {
      ppg_compression_context_register_symbol((void**)&token->action.callback.user_data, ccontext);
   }
}

#if PPG_PRINT_SELF_ENABLED

void ppg_token_print_self_start(PPG_Token__ *p, PPG_Count indent)
{
   PPG_I PPG_LOG("\tprnt: 0x%" PRIXPTR "\n", (uintptr_t)p->parent);
   PPG_I PPG_LOG("\tst: %d\n", (PPG_Count)p->misc.state);
   PPG_I PPG_LOG("\tflgs: %d\n", (PPG_Count)p->misc.flags);
   PPG_I PPG_LOG("\ta.st: %d\n", (PPG_Count)p->misc.action_state);
   PPG_I PPG_LOG("\ta.flgs: %d\n", (PPG_Count)p->misc.action_flags);
   PPG_I PPG_LOG("\ta.u_f: 0x%" PRIXPTR "\n", (uintptr_t)p->action.callback.func);
   PPG_I PPG_LOG("\ta.u_d: 0x%" PRIXPTR "\n", (uintptr_t)p->action.callback.user_data);
   PPG_I PPG_LOG("\tst: %d\n", (PPG_Count)p->misc.state);
   PPG_I PPG_LOG("\tlyr: %d\n", p->layer);
}

void ppg_token_print_self_end(PPG_Token__ *p, PPG_Count indent, bool recurse)
{
//    PPG_I PPG_LOG("\tchldr: 0x%" PRIXPTR "\n", (uintptr_t)&p->children);
   PPG_I PPG_LOG("\tn alloc chld: %d\n", p->n_allocated_children);
   PPG_I PPG_LOG("\tn chld: %d\n", p->n_children);
   
   if(recurse) {
      for(PPG_Count i = 0; i < p->n_children; ++i) {
         PPG_I PPG_LOG("\tchld: %d\n", i);
         PPG_CALL_VIRT_METHOD(p->children[i], print_self, indent + 1, recurse);
      }
   }
}

static void ppg_token_print_self(PPG_Token__ *p, PPG_Count indent, bool recurse)
{
   PPG_I PPG_LOG("<*** tk (0x%" PRIXPTR ") ***>\n", (uintptr_t)p);
   ppg_token_print_self_start(p, indent);
   ppg_token_print_self_end(p, indent, recurse);
}
#endif

#if PPG_HAVE_DEBUGGING
bool ppg_token_check_initialized(PPG_Token__ *token)
{
   bool assertion_failed = false;
   
   PPG_ASSERT_WARN(token->misc.state == PPG_Token_Initialized);
   PPG_ASSERT_WARN(token->misc.action_state == PPG_Action_Disabled);
/*   
#if PPG_PRINT_SELF_ENABLED
   if(assertion_failed) {
      PPG_PRINT_TOKEN(token)
   }
#endif*/
   
   return assertion_failed;
}

bool ppg_token_recurse_check_initialized(PPG_Token__ *token)
{
   bool assertion_failed = false;
   
   assertion_failed 
         |= PPG_CALL_VIRT_METHOD(token, check_initialized);
   
   for(PPG_Count i = 0; i < token->n_children; ++i) {
      assertion_failed 
         |= ppg_token_recurse_check_initialized(token->children[i]);
   }
   
   return assertion_failed;
}

#endif

void ppg_token_traverse_tree(PPG_Token__ *token,
                             PPG_Token_Tree_Visitor visitor,
                             void *user_data)
{
   visitor(token, user_data);
   
   for(PPG_Count i = 0; i < token->n_children; ++i) {
      ppg_token_traverse_tree(token->children[i],
                              visitor,
                              user_data);
   }
}

PPG_Token_Vtable ppg_token_vtable =
{
   .match_event 
      = NULL,
   .reset 
      = (PPG_Token_Reset_Fun) ppg_token_reset_control_state,
   .destroy 
      = (PPG_Token_Destroy_Fun) ppg_token_destroy,
   .equals
      = NULL,
   .dynamic_size
      = (PPG_Token_Dynamic_Size_Requirement_Fun)ppg_token_dynamic_size,
   .placement_clone
      = (PPG_Token_Placement_Clone_Fun)ppg_token_placement_clone,
   .register_ptrs_for_compression
      = (PPG_Token_Register_Pointers_For_Compression)ppg_token_register_pointers_for_compression
   
   #if PPG_PRINT_SELF_ENABLED
   ,
   .print_self
      = (PPG_Token_Print_Self_Fun) ppg_token_print_self
   #endif
      
   #if PPG_HAVE_DEBUGGING
   ,
   .check_initialized
      = (PPG_Token_Check_Initialized_Fun)ppg_token_check_initialized
   #endif
};

PPG_Token__ *ppg_token_new(PPG_Token__ *token) {
   
    token->vtable = &ppg_token_vtable;
    
    token->misc = (PPG_Misc_Bits) {
       .state = PPG_Token_Initialized,
       .flags = 0,
       .action_state = 0,
       .action_flags = PPG_Action_Default
    };
    token->parent = NULL;
    token->children = NULL;
    token->n_allocated_children = 0;
    token->n_children = 0;
    token->action.callback.func = NULL;
    token->action.callback.user_data = NULL;
    token->layer = 0;
    
    return token;
}

PPG_Token ppg_token_set_action(  
                     PPG_Token token,
                     PPG_Action action)
{
   PPG_Token__ *token__ = (PPG_Token__ *)token;
   
   ppg_token_store_action(token__, action);
   
   return token;
}

PPG_Action ppg_token_get_action(PPG_Token token)
{
   PPG_Token__ *token__ = (PPG_Token__ *)token;
   
   return token__->action;
}

PPG_Token ppg_token_set_action_flags(
                           PPG_Token token,
                           PPG_Action_Flags_Type action_flags)
{
   PPG_Token__ *token__ = (PPG_Token__ *)token;
   
   token__->misc.action_flags = action_flags;
   
   return token;
}

PPG_Action_Flags_Type ppg_token_get_action_flags(PPG_Token token)
{
   PPG_Token__ *token__ = (PPG_Token__ *)token;
   
   return token__->misc.action_flags;
}

PPG_Token ppg_token_set_flags(
                           PPG_Token token,
                           PPG_Count flags)
{
   PPG_Token__ *token__ = (PPG_Token__ *)token;
   
   token__->misc.flags = flags;
   
   return token;
}

PPG_Count ppg_token_get_flags(PPG_Token token)
{
   PPG_Token__ *token__ = (PPG_Token__ *)token;
   
   return token__->misc.flags;
}

void ppg_token_list_all_active(void)
{
   PPG_LOG("T actv:\n");

   for(PPG_Count i = 0; i < PPG_GAT.n_tokens; ++i) {
      PPG_LOG("\t0x%" PRIXPTR "\n", (uintptr_t)PPG_GAT.tokens[i]);
   }
}