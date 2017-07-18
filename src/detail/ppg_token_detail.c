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
#include "ppg_action.h"
#include "ppg_action_flags.h"
#include "ppg_signals.h"
#include "detail/ppg_context_detail.h"

#include <stdlib.h>

void ppg_token_store_action(PPG_Token__ *token, 
                                   PPG_Action action)
{
   token->action = action; 
   
   PPG_LOG("A tk 0x%" PRIXPTR ": 0x%" PRIXPTR "\n",
              (uintptr_t)token, (uintptr_t)token->action.callback.user_data);
}

void ppg_token_reset(   PPG_Token__ *token)
{
   token->state = PPG_Token_Initialized;
}

/* Tokens have states. This method resets the states
 * after processing a pattern. On our way through the
 * pattern tree we only need to reset thoses tokens
 * that were candidates.
 */
void ppg_token_reset_children(PPG_Token__ *token)
{
   /* Reset all child tokens if there are any
    */
   for(PPG_Count i = 0; i < token->n_children; ++i) {
      PPG_CALL_VIRT_METHOD(token->children[i], reset);
   }
}

static void ppg_token_allocate_children(PPG_Token__ *a_This, PPG_Count n_children) {

    a_This->children 
      = (struct PPG_TokenStruct **)malloc(n_children*sizeof(struct PPG_TokenStruct*));
    a_This->n_allocated_children = n_children;
}

static void ppg_token_grow_children(PPG_Token__ *a_This) {

   if(a_This->n_allocated_children == 0) {
      
      ppg_token_allocate_children(a_This, 1);
   }
   else {
      PPG_Token__ **oldSucessors = a_This->children;
      
      ppg_token_allocate_children(a_This, 2*a_This->n_allocated_children);
         
      for(PPG_Count i = 0; i < a_This->n_children; ++i) {
         a_This->children[i] = oldSucessors[i];
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
                                          PPG_Token__ *sample) {
   
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

#ifdef PAPAGENO_PRINT_SELF_ENABLED

void ppg_token_print_self_start(PPG_Token__ *p, PPG_Count indent)
{
   PPG_I PPG_LOG("\tprnt: 0x%" PRIXPTR "\n", (uintptr_t)p->parent);
   PPG_I PPG_LOG("\ta.flgs: %d\n", p->action.flags);
   PPG_I PPG_LOG("\ta.u_f: 0x%" PRIXPTR "\n", (uintptr_t)p->action.callback.func);
   PPG_I PPG_LOG("\ta.u_d: 0x%" PRIXPTR "\n", (uintptr_t)p->action.callback.user_data);
   PPG_I PPG_LOG("\tst: %d\n", p->state);
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

static PPG_Token_Vtable ppg_token_vtable =
{
   .match_event 
      = NULL,
   .reset 
      = (PPG_Token_Reset_Fun) ppg_token_reset,
   .destroy 
      = (PPG_Token_Destroy_Fun) ppg_token_destroy,
   .equals
      = NULL
   
   #ifdef PAPAGENO_PRINT_SELF_ENABLED
   ,
   .print_self
      = (PPG_Token_Print_Self_Fun) ppg_token_print_self,
   #endif
};

PPG_Token__ *ppg_token_new(PPG_Token__ *token) {
   
    token->vtable = &ppg_token_vtable;
    
    token->parent = NULL;
    token->children = NULL;
    token->n_allocated_children = 0;
    token->n_children = 0;
    token->action.flags = PPG_Action_Default;
    token->action.callback.func = NULL;
    token->action.callback.user_data = NULL;
    token->state = PPG_Token_Initialized;
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
   
   token__->action.flags = action_flags;
   
   return token;
}

PPG_Action_Flags_Type ppg_token_get_action_flags(PPG_Token token)
{
   PPG_Token__ *token__ = (PPG_Token__ *)token;
   
   return token__->action.flags;
}
