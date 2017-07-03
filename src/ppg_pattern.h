/* Copyright 2017 Florian Fleissner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PPG_PATTERN_H
#define PPG_PATTERN_H

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
							uint8_t layer,
							uint8_t n_tokens,
							PPG_Token tokens[]);

#endif
