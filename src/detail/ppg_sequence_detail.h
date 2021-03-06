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

#ifndef PPG_SEQUENCE_DETAIL_H   
#define PPG_SEQUENCE_DETAIL_H

#include "detail/ppg_token_detail.h"
#include "detail/ppg_aggregate_detail.h"

typedef struct {
   PPG_Aggregate aggregate;
   PPG_Count next_member;
} PPG_Sequence;

extern PPG_Token_Vtable ppg_sequence_vtable;

#endif
