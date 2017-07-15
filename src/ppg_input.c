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

#include "ppg_input.h"
#include "detail/ppg_event_buffer_detail.h"
#include "detail/ppg_context_detail.h"
#include "ppg_debug.h"
#include "ppg_bitfield.h"

void ppg_input_list_all_active(void)
{
   PPG_LOG("I actv:\n");
   
   for(PPG_Count i = 0; i < ppg_context->active_inputs.n_bits; ++i) {
      if(ppg_bitfield_get_bit(&ppg_context->active_inputs, i)) {
         PPG_LOG("\t%d\n", i);
      }
   }
}
