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

#ifndef PPG_GLOBAL_DETAIL_H
#define PPG_GLOBAL_DETAIL_H

#include "ppg_signals.h"
#include "ppg_global.h"
#include "detail/ppg_token_detail.h"

#include <stdbool.h>

bool ppg_recurse_and_process_actions(PPG_Token__ *cur_token);

void ppg_delete_stored_events(void);

void ppg_reset_pattern_matching_engine(void);

#endif

