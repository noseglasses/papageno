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

#ifndef PPG_LEADER_SEQUENCES_H
#define PPG_LEADER_SEQUENCES_H

#include "ppg_action.h"
#include "ppg_input.h"
#include "ppg_token.h"

#include <stdint.h>

#define PPG_MAX_SEQUENCE_LENGTH 16

/** @brief A function that provides strings that
 *         are converted into leader sequences.
 */
typedef void (*PPG_Leader_String_Retreival_Callback)(
                              uint8_t sequence_id,
                              char *buffer, 
                              uint8_t max_chars);

/** @brief A function that provides actions that
 *         can be associated with leader sequences.
 */
typedef PPG_Action (*PPG_Leader_Action_Retreival_Callback)(
                                       uint8_t sequence_id);

/** @brief A function that maps alphabetic characters to input ids.
 */
typedef PPG_Input_Id (*PPG_Leader_Character_To_Input_Callback)(char c);

/** @brief A collection of auxiliary functions that aid
 *         leader sequence generation.
 */
typedef struct {
   PPG_Leader_String_Retreival_Callback retreive_string;
      ///< Leader sequence character string retreival function
   
   PPG_Leader_Action_Retreival_Callback retreive_action;
      ///< Leader sequence action retreival function
   
   PPG_Leader_Character_To_Input_Callback input_from_char;
      ///< Character to input id mapping
   
} PPG_Leader_Functions;

/** @brief This function generates a set of note lines based on
 *       proviede character sequences that are mapped to input id sequences.
 * 
 * @param layer The layer, the sequences will be associated with
 * @param leader_token The token that is used as a start for sequences. If
 *              the provided value is NULL, no leaeder will be used.
 * @param n_sequences The number of sequences to generate. For each sequence the retreival functions provided in parameter functions must be able to deliver appropriate items.
 * @param allow_fallback If this parameter is true, the generated sequences are generated in a way that any number or non-ambiguous events leads to a match on timeout. If allow_fallback is false, pattern matches only result if a whole sequence is matched by the supplied events.
 * 
 * @note The tokens that are passed through parameter leader_token must result from the generation of a note line, note, chord or cluster and must have been registered at the pattern matching system.
 */
void ppg_alphabetic_leader_sequences(
            PPG_Layer layer,
            PPG_Token leader_token,
            uint8_t n_sequences,
            PPG_Leader_Functions functions,
            bool allow_fallback);

#endif
