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

#ifndef PPG_PATTERN_DETAIL_H
#define PPG_PATTERN_DETAIL_H

#include "ppg_token.h"
#include "ppg_layer.h"
#include "detail/ppg_token_detail.h"

PPG_Token ppg_pattern_from_list(   
                                    PPG_Token__ *parent_token,
                                    PPG_Layer layer,
                                    PPG_Count n_tokens,
                                    PPG_Token__ *tokens[]);

PPG_Count ppg_pattern_tree_depth(void);

#endif
