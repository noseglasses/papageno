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

#ifndef PPG_PATTERN_H
#define PPG_PATTERN_H

/** @file */

#include "ppg_token.h"

/** @brief Defines a pattern that consists of tokens
 * 
 * @note Use setter functions that operate on tokens to change attributes of the tokens
 *       that make up the pattern. This is e.g. required to assign an action to a token,
 * 		or to modify fall through behavior.
 * 
 * @param layer The layer the pattern is associated with
 * @param n_tokens The number of tokens combined to form the pattern
 * @param tokens The tokens that make up the pattern
 * @returns The constructed token
 */
PPG_Token ppg_pattern(		
							PPG_Layer layer,
							PPG_Count n_tokens,
							PPG_Token tokens[]);

#ifdef PAPAGENO_PRINT_SELF_ENABLED

/** @brief Recursively prints the current pattern search tree
 */ 
void ppg_pattern_print_tree(void);

#define PPG_PATTERN_PRINT_TREE ppg_pattern_print_tree();
#else
#define PPG_PATTERN_PRINT_TREE
#endif

#endif
