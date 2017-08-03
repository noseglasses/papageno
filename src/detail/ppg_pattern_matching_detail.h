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

#ifndef PPG_PATTERN_MATCHING_DETAIL_H
#define PPG_PATTERN_MATCHING_DETAIL_H

#include <stdbool.h>

// Returns true if a match occurred
//
bool ppg_pattern_matching_run(void);

// Returns true if a match occurred
//
bool ppg_pattern_matching_process_remaining_branch_options(void);

PPG_Token__ * ppg_branch_find_root(
                        PPG_Token__ *cur_token,
                        PPG_Token__ *end_token);
#endif
