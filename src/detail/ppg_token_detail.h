/* Copyright 2017 noseglasses <shinynoseglasses@gmail.com>
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

#ifndef PPG_TOKEN_DETAIL_H
#define PPG_TOKEN_DETAIL_H

#include "ppg_token.h"
#include "ppg_event.h"
#include "ppg_action.h"
#include "ppg_layer.h"
#include "ppg_settings.h"
#include "ppg_debug.h"
#include "detail/ppg_compression_detail.h"

struct PPG_TokenStruct;

/** @returns Whether the event was considered
 */
typedef bool (*PPG_Token_Match_Event_Fun)(
                                          struct PPG_TokenStruct *token, 
                                          PPG_Event *event,
                                          bool modify_only_if_consuming
                                        );

typedef void (*PPG_Token_Reset_Fun)(   struct PPG_TokenStruct *token);

typedef struct PPG_TokenStruct * (*PPG_Token_Destroy_Fun)(struct PPG_TokenStruct *token);

typedef bool (*PPG_Token_Equals_Fun)(struct PPG_TokenStruct *p1, struct PPG_TokenStruct *p2);

typedef PPG_Count (*PPG_Token_Precedence_Fun)(struct PPG_TokenStruct *token);

typedef size_t (*PPG_Token_Dynamic_Size_Requirement_Fun)(struct PPG_TokenStruct *p);

typedef char *(*PPG_Token_Placement_Clone_Fun)(struct PPG_TokenStruct *p,
                                             char *target_buffer
);

typedef void (*PPG_Token_Register_Pointers_For_Compression)(
                                             struct PPG_TokenStruct *p,
                                             PPG_Compression_Context__ *ccontext
);

typedef void (*PPG_Token_Addresses_To_Relative)(
                                             struct PPG_TokenStruct *p,
                                             void *begin_of_buffer
);

typedef void (*PPG_Token_Addresses_To_Absolute)(
                                             struct PPG_TokenStruct *p,
                                             void *begin_of_buffer
);

#if PPG_PRINT_SELF_ENABLED
typedef void (*PPG_Token_Print_Self_Fun)(struct PPG_TokenStruct *p, PPG_Count indent, bool recurse);
#endif

#if PPG_HAVE_DEBUGGING
typedef bool (*PPG_Token_Check_Initialized_Fun)(struct PPG_TokenStruct *p);
#endif

typedef struct {
                           
   PPG_Token_Match_Event_Fun 
                           match_event;
                           
   PPG_Token_Reset_Fun
                           reset;
                           
   PPG_Token_Destroy_Fun   
                           destroy;
         
   PPG_Token_Equals_Fun
                           equals;
                           
   PPG_Token_Precedence_Fun
                           token_precedence;
                           
   PPG_Token_Dynamic_Size_Requirement_Fun
                           dynamic_size;
                           
   PPG_Token_Placement_Clone_Fun
                           placement_clone;
                           
   PPG_Token_Register_Pointers_For_Compression
                           register_ptrs_for_compression;
                           
   PPG_Token_Addresses_To_Relative   
                           addresses_to_relative;
                           
   PPG_Token_Addresses_To_Absolute   
                           addresses_to_absolute;
                           
   #if PPG_PRINT_SELF_ENABLED
   PPG_Token_Print_Self_Fun
                           print_self;
   #endif      
                           
   #if PPG_HAVE_DEBUGGING
   PPG_Token_Check_Initialized_Fun
                           check_initialized;
   #endif
} PPG_Token_Vtable;

extern PPG_Token_Vtable ppg_token_vtable;

#define PPG_CALL_VIRT_METHOD(THIS, METHOD, ...) \
   ((PPG_Token__*)THIS)->vtable->METHOD(THIS, ##__VA_ARGS__);
   
enum PPG_Action_State {
   PPG_Action_Disabled     = 0,
   PPG_Action_Enabled,
   PPG_Action_Activation_Triggered,
   PPG_Action_Deactivation_Triggered
};

enum {   PPG_Token_N_State_Bits = 3 };
enum {   PPG_Token_N_Action_Flags_Bits = 4 };
enum {   PPG_Token_N_Action_State_Bits = 2 };
enum {   PPG_Token_N_Flag_Bits = 16  - PPG_Token_N_State_Bits 
                                    - PPG_Token_N_Action_Flags_Bits
                                    - PPG_Token_N_Action_State_Bits};
                                    
typedef struct {
   unsigned int state         : PPG_Token_N_State_Bits;
   unsigned int flags         : PPG_Token_N_Flag_Bits;
   unsigned int action_state  : PPG_Token_N_Action_State_Bits;
   unsigned int action_flags  : PPG_Token_N_Action_Flags_Bits;
} PPG_Misc_Bits;

enum {
   PPG_Token_Flags_Empty = 0, ///< Initial value
   PPG_Token_Flags_Done = 1
};

typedef struct PPG_TokenStruct {

   PPG_Token_Vtable *vtable;
   
   struct PPG_TokenStruct *parent;
   
   struct PPG_TokenStruct **children;
   
   PPG_Count n_allocated_children;
   PPG_Count n_children;
   
   PPG_Action action;
   
   PPG_Misc_Bits misc;
   
   PPG_Layer layer;
    
} PPG_Token__;

enum PPG_Processing_State {
   PPG_Token_Initialized = 0,
   PPG_Token_Activation_In_Progress,
   PPG_Token_Matches,
   PPG_Token_Deactivation_In_Progress,
   PPG_Token_Finalized,
   PPG_Token_Invalid
};

void ppg_token_free_children(PPG_Token__ *token);

void ppg_token_store_action(PPG_Token__ *token, 
                                   PPG_Action action);

void ppg_token_reset_control_state(   PPG_Token__ *token);

PPG_Token__ *ppg_token_alloc(void);

PPG_Token__ *ppg_token_new(PPG_Token__ *token);

PPG_Token__* ppg_token_destroy(PPG_Token__ *token);

void ppg_token_free(PPG_Token__ *token);

void ppg_token_add_child(PPG_Token__ *token, PPG_Token__ *child);

PPG_Token__* ppg_token_get_equivalent_child(
                                          PPG_Token__ *parent_token,
                                          PPG_Token__ *sample);

size_t ppg_token_dynamic_member_size(PPG_Token__ *token);

char *ppg_token_copy_dynamic_members(PPG_Token__ *token, char *buffer);

char *ppg_token_placement_clone(PPG_Token__ *token, char *buffer);

void ppg_token_register_pointers_for_compression(
                                             PPG_Token__ *token,
                                             PPG_Compression_Context__ *ccontext);

void ppg_token_addresses_to_relative(  PPG_Token__ *token,
                                       void *begin_of_buffer);

void ppg_token_addresses_to_absolute(  PPG_Token__ *token,
                                       void *begin_of_buffer);

#if PPG_PRINT_SELF_ENABLED
void ppg_token_print_self_start(PPG_Token__ *p, PPG_Count indent);
void ppg_token_print_self_end(PPG_Token__ *p, PPG_Count indent, bool recurse);
#endif


typedef void (*PPG_Token_Tree_Visitor)(PPG_Token__ *token, void *user_data);

void ppg_token_traverse_tree(PPG_Token__ *token,
                             PPG_Token_Tree_Visitor pre_children_visitor,
                             PPG_Token_Tree_Visitor post_children_visitor,
                             void *user_data);

#if PPG_HAVE_DEBUGGING

// Use this method directly to start recursion
//
bool ppg_token_check_initialized(PPG_Token__ *token);

bool ppg_token_recurse_check_initialized(PPG_Token__ *token);

#define PPG_ASSERT_WARN(...) \
   if(!(__VA_ARGS__)) { \
      PPG_LOG("Assertion failed\n"); \
      PPG_LOG("   token: 0x%" PRIXPTR "\n", (uintptr_t)token); \
      PPG_LOG("   assertion: %s\n", #__VA_ARGS__); \
      PPG_LOG("   location: %s:%d\n", __FILE__, __LINE__); \
      assertion_failed = true; \
   }
#endif

#if PPG_PRINT_SELF_ENABLED
#define PPG_PRINT_TOKEN(TOKEN) \
    PPG_CALL_VIRT_METHOD(TOKEN, print_self, 0, false /* do not recurse */);
#define PPG_PRINT_TOKEN_RECURSE(TOKEN) \
    PPG_CALL_VIRT_METHOD(TOKEN, print_self, 0, true /* do not recurse */);
#else
#define PPG_PRINT_TOKEN(TOKEN)
#define PPG_PRINT_TOKEN_RECURSE(TOKEN)
#endif

#endif
