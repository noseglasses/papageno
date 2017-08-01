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

#ifndef PPG_TOKEN_DETAIL_H
#define PPG_TOKEN_DETAIL_H

#include "ppg_token.h"
#include "ppg_event.h"
#include "ppg_action.h"
#include "ppg_layer.h"
#include "ppg_settings.h"
#include "ppg_debug.h"

struct PPG_TokenStruct;

/** @returns Whether the event was considered
 */
typedef bool (*PPG_Token_Match_Event_Fun)(
                                          struct PPG_TokenStruct *token, 
                                          PPG_Event *event
                                        );

typedef void (*PPG_Token_Reset_Fun)(   struct PPG_TokenStruct *token);

typedef struct PPG_TokenStruct * (*PPG_Token_Destroy_Fun)(struct PPG_TokenStruct *token);

typedef bool (*PPG_Token_Equals_Fun)(struct PPG_TokenStruct *p1, struct PPG_TokenStruct *p2);

typedef PPG_Count (*PPG_Token_Precedence_Fun)(struct PPG_TokenStruct *token);

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
                           
   #if PPG_PRINT_SELF_ENABLED
   PPG_Token_Print_Self_Fun
                           print_self;
   #endif      
                           
   #if PPG_HAVE_DEBUGGING
   PPG_Token_Check_Initialized_Fun
                           check_initialized;
   #endif
} PPG_Token_Vtable;

#define PPG_CALL_VIRT_METHOD(THIS, METHOD, ...) \
   ((PPG_Token__*)THIS)->vtable->METHOD(THIS, ##__VA_ARGS__);
   
enum PPG_Action_State {
   PPG_Action_Disabled     = 0,
   PPG_Action_Enabled
};

enum {   PPG_Token_N_State_Bits = 3 };
enum {   PPG_Token_N_Action_Flags_Bits = 4 };
enum {   PPG_Token_N_Action_State_Bits = 1 };
enum {   PPG_Token_N_Flag_Bits = 16  - PPG_Token_N_State_Bits 
                                    - PPG_Token_N_Action_Flags_Bits
                                    - PPG_Token_N_Action_State_Bits};
                                    
typedef struct {
   PPG_Count state         : PPG_Token_N_State_Bits;
   PPG_Count flags         : PPG_Token_N_Flag_Bits;
   PPG_Count action_state  : PPG_Token_N_Action_State_Bits;
   PPG_Count action_flags  : PPG_Token_N_Action_Flags_Bits;
} PPG_Misc_Bits;

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
   PPG_Token_Invalid
};

// enum {   PPG_Action_Flags_Start_Bit_Id = 3 };
// enum {   PPG_Flags_Start_Bit_Id = PPG_Action_Flags_Start_Bit_Id + 1 };
// 
// enum {   PPG_N_Action_Flag_Bits = PPG_Flags_Start_Bit_Id - PPG_Action_Flags_Start_Bit_Id };
// 
// enum {   PPG_State_Mask = ((1 << PPG_N_Action_Flag_Bits) - 1) << PPG_Action_Flags_Start_Bit_Id };
// 
// enum {   PPG_Action_Flags_Mask = ~((1 << (PPG_Flags_Start_Bit_Id) - 1) };
// 
// enum {   PPG_Flags_Mask = ~((1 << PPG_Flags_Start_Bit_Id) - 1) };
// 
// enum {   PPG_Note_Flag_Match_Activation = 1 << PPG_Flags_Start_Bit_Id,
//          PPG_Note_Flag_Match_Deactivation = PPG_Note_Flag_Match_Activation << 1,
//          PPG_Token_Allow_Deactivation = PPG_Note_Flag_Match_Deactivation << 1
// };
// 
// inline PPG_Count ppg_token_get_state(PPG_Token__ *token) 
// {
//    return token->misc & PPG_State_Mask;
// }
// inline void ppg_token_set_state(PPG_Token__ *token, PPG_Count state)
// {
//    token->misc &= PPG_Flags_Mask;
//    token->misc |= state;
// }
// 
// inline PPG_Count ppg_token_get_flags(PPG_Token__ *token)
// {
//    return (token->misc & PPG_Flags_Mask) >> PPG_Flags_Start_Bit_Id;
// }
// 
// inline PPG_Count ppg_token_set_flags(PPG_Token__ *token, PPG_Count values, PPG_Count mask)
// {
//    // Clear bits in mask range
//    //
//    token->misc &= ~(mask << PPG_Flags_Start_Bit_Id); 
//    
//    token->misc |= ((values & mask) << PPG_Flags_Start_Bit_Id);
// }

void ppg_token_free_children(PPG_Token__ *token);

void ppg_token_store_action(PPG_Token__ *token, 
                                   PPG_Action action);

void ppg_token_reset_control_state(   PPG_Token__ *token);

PPG_Token__ *ppg_token_new(PPG_Token__ *token);

PPG_Token__* ppg_token_destroy(PPG_Token__ *token);

void ppg_token_free(PPG_Token__ *token);

void ppg_token_add_child(PPG_Token__ *token, PPG_Token__ *child);

PPG_Token__* ppg_token_get_equivalent_child(
                                          PPG_Token__ *parent_token,
                                          PPG_Token__ *sample);

#if PPG_PRINT_SELF_ENABLED
void ppg_token_print_self_start(PPG_Token__ *p, PPG_Count indent);
void ppg_token_print_self_end(PPG_Token__ *p, PPG_Count indent, bool recurse);
#endif

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

#endif
