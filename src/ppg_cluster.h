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

#ifndef PPG_CLUSTER_H
#define PPG_CLUSTER_H

#include "ppg_token.h"
#include "ppg_input.h"
#include "ppg_layer.h"

/** @file */

/** @brief Flags that configure cluster behavior
  *
  * Use the functions ppg_token_set_flags and
  * ppg_token_get_flags to manipulated cluster flags.
 */
enum PPG_Cluster_Flags {
   PPG_Cluster_Flags_Disallow_Input_Deactivation = (1 << 3) ///< Set this flag to gain 
            // a cluster that does not tolerate deactivation of inputs before all
            // inputs are activated. If this condition is violated, the
            // cluster is considered as invalid
};

/** @brief Defines a stand alone note cluster. 
 * 
 * All members must be activated/pressed at 
 * least once for   the cluster to be considered as match.
 * 
 * @param layer The layer the pattern is associated with
 * @param action The action that is supposed to be carried out if the pattern matches
 * @param n_inputs The number of inputs
 * @param inputs A pointer to an array of input definitions.
 * @returns The constructed token
 */
PPG_Token ppg_cluster(      
                     PPG_Layer layer, 
                     PPG_Action action,
                     PPG_Count n_inputs, 
                     PPG_Input_Id inputs[]);

/** @brief Generates a cluster token.
 *
 * Use this function to generate tokens that are passed to the ppg_pattern function
 * to generate complex patterns. Every member of the cluster must have been
 * activated at least once for the cluster to be considered as match.
 * 
 * @note Clusters that are generated by this function must be passed to ppg_pattern
 *       to be effective
 * @note Use setter functions that operate on tokens to change attributes of the generated token 
 * 
 * @param n_inputs The number of inputs that are associated with the cluster 
 * @param inputs An array of input ids that represent the notes of the cluster
 * @returns The constructed token
 */
PPG_Token ppg_cluster_create(
                     PPG_Count n_inputs,
                     PPG_Input_Id inputs[]);

/** @brief Auxiliary macro to create a cluster based on a set of input specifications
 * 
 * @param ... A list of inputs
 */
#define PPG_CLUSTER_CREATE(...) \
   ppg_cluster_create(PPG_INPUTS(__VA_ARGS__))

#endif
