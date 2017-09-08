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

#include "ppg_cluster.h"
#include "ppg_debug.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_aggregate_detail.h"
#include "detail/ppg_pattern_detail.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_token_precedence_detail.h"

typedef struct {
   PPG_Aggregate aggregate;
   PPG_Bitfield member_active_lasting;
   PPG_Count n_lasting;
} PPG_Cluster;

static bool ppg_cluster_match_event(   
                                 PPG_Cluster *cluster,
                                 PPG_Event *event,
                                 bool modify_only_if_consuming)
{
   bool input_part_of_cluster = false;
   
   PPG_ASSERT(cluster->aggregate.n_members != 0);
   
   /* Check it the input is part of the current cluster 
    */
   for(PPG_Count i = 0; i < cluster->aggregate.n_members; ++i) {
      
      if(cluster->aggregate.inputs[i] == event->input) {
         
         input_part_of_cluster = true;
         
         if(event->flags & PPG_Event_Active) {
            if(!ppg_bitfield_get_bit(&cluster->aggregate.member_active, i)) {
               ppg_bitfield_set_bit(&cluster->aggregate.member_active, i, true);
               ++cluster->aggregate.n_inputs_active;
            }
            
            if(!ppg_bitfield_get_bit(&cluster->member_active_lasting, i)) {
               ppg_bitfield_set_bit(&cluster->member_active_lasting, i, true);
               ++cluster->n_lasting;
            }
         }
         else {
            
            if(cluster->aggregate.super.misc.flags & PPG_Cluster_Flags_Disallow_Input_Deactivation) {
               if(!modify_only_if_consuming) {
                  cluster->aggregate.super.misc.state = PPG_Token_Invalid;
               }
               return false;
            }
            
            /* Note: If input deactivation is allowed, we do not care for 
             * released inputs here. Every cluster member must be 
             * pressed only once
            */
            if(ppg_bitfield_get_bit(&cluster->aggregate.member_active, i)) {
               
               ppg_bitfield_set_bit(&cluster->aggregate.member_active, i, false);
               --cluster->aggregate.n_inputs_active;
            }
            else {
               return false;
            }

            break;
         }
      }
   }
   
#if PPG_HAVE_LOGGING
   for(PPG_Count i = 0; i < cluster->aggregate.n_members; ++i) {
      PPG_LOG("%d: 0x%d = %d\n", 
              i, 
              cluster->aggregate.inputs[i],
              ppg_bitfield_get_bit(&cluster->aggregate.member_active, i)
      );
   }
   PPG_LOG("Lasting\n"); 
   for(PPG_Count i = 0; i < cluster->aggregate.n_members; ++i) {
      PPG_LOG("%d: 0x%d = %d\n", 
              i, 
              cluster->aggregate.inputs[i],
              ppg_bitfield_get_bit(&cluster->member_active_lasting, i)
      );
   }
#endif
   
   if(!input_part_of_cluster) {
      if(event->flags & PPG_Event_Active) {
         
         if(!modify_only_if_consuming) {
            cluster->aggregate.super.misc.state = PPG_Token_Invalid;
         }
      }
      
      // Clusters ignore unmatching deactivation events
      
      return false;
   }
   
   cluster->aggregate.super.misc.state = PPG_Token_Activation_In_Progress;
   
   if(   (cluster->aggregate.n_inputs_active == 0)
      && (cluster->aggregate.super.misc.flags & PPG_Aggregate_All_Active)
   ) {
      
      if(cluster->aggregate.super.misc.flags & PPG_Aggregate_All_Active) {
         
         if(cluster->aggregate.super.misc.flags & PPG_Token_Flags_Pedantic) {
            /* Cluster matches
            */
            cluster->aggregate.super.misc.state = PPG_Token_Matches;
         }
         else {
            cluster->aggregate.super.misc.state = PPG_Token_Finalized;
         }
      }
   }
   else if(cluster->n_lasting == cluster->aggregate.n_members) {
      
      cluster->aggregate.super.misc.flags |= PPG_Aggregate_All_Active;
      
      /* Cluster matches
       */
      cluster->aggregate.super.misc.state = PPG_Token_Matches;
//       PPG_LOG("O");
   }
   else {
      if(cluster->aggregate.super.misc.flags & PPG_Aggregate_All_Active) {
         cluster->aggregate.super.misc.state = PPG_Token_Deactivation_In_Progress;
      }
   }
   
   return true;
}

static PPG_Count ppg_cluster_token_precedence(PPG_Token__ *token)
{
   return PPG_Token_Precedence_Cluster;
}

static size_t ppg_cluster_dynamic_member_size(PPG_Token *token)
{
   return   sizeof(PPG_Cluster)
         +  ppg_aggregate_dynamic_member_size((PPG_Aggregate *)token);
}

static char *ppg_cluster_placement_clone(PPG_Token__ *token, char *buffer)
{
   PPG_Cluster *cluster = (PPG_Cluster *)token;
   
   *((PPG_Cluster *)buffer) = *cluster;
   
   PPG_Token__ *clone = (PPG_Token__ *)buffer;
   
//    printf("Replacing children pointer %p with %p\n", clone->children, (PPG_Token__ **)(buffer + sizeof(PPG_Cluster)));
   
   clone->children = (PPG_Token__ **)(buffer + sizeof(PPG_Cluster));
   
   return ppg_aggregate_copy_dynamic_members(token, buffer + sizeof(PPG_Cluster));
}

#if PPG_PRINT_SELF_ENABLED
static void ppg_cluster_print_self(PPG_Cluster *c, PPG_Count indent, bool recurse)
{
   PPG_I PPG_LOG("<*** clstr (0x%" PRIXPTR ") ***>\n", (uintptr_t)c);
   ppg_token_print_self_start((PPG_Token__*)c, indent);
   PPG_I PPG_LOG("\tn mbr: %d\n", c->aggregate.n_members);
   PPG_I PPG_LOG("\tn I actv: %d\n", c->aggregate.n_inputs_active);
   PPG_I PPG_LOG("\tn I lastg.: %d\n", c->n_lasting);
   
   for(PPG_Count i = 0; i < c->aggregate.n_members; ++i) {
      PPG_LOG("\t\tI: 0x%d, actv: %d\n", 
             c->aggregate.inputs[i], 
                 ppg_bitfield_get_bit(&c->aggregate.member_active, i));
   }
    
   for(PPG_Count i = 0; i < c->aggregate.n_members; ++i) {
      PPG_LOG("\t\tI: 0x%d, lasting actv: %d\n", 
             c->aggregate.inputs[i], 
                 ppg_bitfield_get_bit(&c->member_active_lasting, i));
   }
   
   ppg_token_print_self_end((PPG_Token__*)c, indent, recurse);
}
#endif

static PPG_Cluster *ppg_cluster_destroy(PPG_Cluster *cluster) {
   
   ppg_bitfield_destroy(&cluster->member_active_lasting);
   
   ppg_aggregate_destroy(&cluster->aggregate);
   
   return cluster;
}

static void ppg_cluster_reset(PPG_Cluster *cluster) 
{
   ppg_aggregate_reset(&cluster->aggregate);

   for(PPG_Count i = 0; i < cluster->aggregate.n_members; ++i) {
      ppg_bitfield_set_bit(&cluster->member_active_lasting,
                           i,
                           false);
   }
   
   cluster->n_lasting = 0;
}

PPG_Token_Vtable ppg_cluster_vtable =
{
   .match_event 
      = (PPG_Token_Match_Event_Fun) ppg_cluster_match_event,
   .reset 
      = (PPG_Token_Reset_Fun) ppg_cluster_reset,
   .destroy 
      = (PPG_Token_Destroy_Fun) ppg_cluster_destroy,
   .equals
      = (PPG_Token_Equals_Fun) ppg_aggregates_equal,
   .token_precedence
      = (PPG_Token_Precedence_Fun)ppg_cluster_token_precedence,
   .dynamic_size
      = (PPG_Token_Dynamic_Size_Requirement_Fun)ppg_cluster_dynamic_member_size,
   .placement_clone
      = (PPG_Token_Placement_Clone_Fun)ppg_cluster_placement_clone,
   .register_ptrs_for_compression
      = (PPG_Token_Register_Pointers_For_Compression)ppg_token_register_pointers_for_compression
      
   #if PPG_PRINT_SELF_ENABLED
   ,
   .print_self
      = (PPG_Token_Print_Self_Fun) ppg_cluster_print_self
   #endif
     
   #if PPG_HAVE_DEBUGGING
   ,
   .check_initialized
      = (PPG_Token_Check_Initialized_Fun)ppg_aggregate_check_initialized
   #endif
};

PPG_Cluster *ppg_cluster_alloc(void) {
    return (PPG_Cluster*)calloc(1, sizeof(PPG_Cluster));
}
   
PPG_Token ppg_cluster_create(
                        PPG_Count n_inputs,
                           PPG_Input_Id inputs[])
{
   PPG_Cluster *cluster = ppg_cluster_alloc();
   
   ppg_aggregate_new(cluster);
    
   cluster->aggregate.super.vtable = &ppg_cluster_vtable;
   
   ppg_bitfield_init(&cluster->member_active_lasting);
   ppg_bitfield_resize(&cluster->member_active_lasting,
                       n_inputs,
                       false /*drop old bits*/);
   
   cluster->n_lasting = 0;
   
   return ppg_global_initialize_aggregate(&cluster->aggregate, n_inputs, inputs);
}

PPG_Token ppg_cluster(     
                     PPG_Layer layer, 
                     PPG_Action action, 
                     PPG_Count n_inputs,
                     PPG_Input_Id inputs[])
{     
//    PPG_LOG("Adding cluster\n");
   
   PPG_Token__ *token = 
      (PPG_Token__ *)ppg_cluster_create(n_inputs, inputs);
      
   token->action = action;
   
   PPG_Token__ *tokens[1] = { token };
   
   PPG_Token__ *leaf_token 
      = ppg_pattern_from_list(NULL, layer, 1, tokens);
      
   return leaf_token;
}
