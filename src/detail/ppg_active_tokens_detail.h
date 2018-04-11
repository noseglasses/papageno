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
#ifndef PPG_ACTIVE_TOKENS_DETAIL_HPP
#define PPG_ACTIVE_TOKENS_DETAIL_HPP

#include "detail/ppg_token_detail.h"
#include "ppg_settings.h"

#define PPG_GAT ppg_context->active_tokens

typedef struct {
   PPG_Token__ **tokens;
   PPG_Count   n_tokens;
   PPG_Count   max_tokens;
} PPG_Active_Tokens;

void ppg_active_tokens_resize(PPG_Active_Tokens *active_tokens,
                              PPG_Count new_size);

void ppg_active_tokens_restore(PPG_Active_Tokens *active_tokens);

void ppg_active_tokens_init(PPG_Active_Tokens *active_tokens);

void ppg_active_tokens_free(PPG_Active_Tokens *active_tokens);

void ppg_active_tokens_update(void);

bool ppg_active_tokens_check_consumption(PPG_Event *event);

void ppg_active_tokens_repeat_actions(void);

#endif
