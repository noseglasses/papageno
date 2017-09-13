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

#include "detail/ppg_token_vtable_detail.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_note_detail.h"
#include "detail/ppg_chord_detail.h"
#include "detail/ppg_cluster_detail.h"

uintptr_t ppg_token_vtable_id_from_ptr(void *vtable_ptr)
{
   if(vtable_ptr == &ppg_token_vtable) {
      return 1;
   }
   if(vtable_ptr == &ppg_note_vtable) {
      return 2;
   }
   if(vtable_ptr == &ppg_chord_vtable) {
      return 3;
   }
   if(vtable_ptr == &ppg_cluster_vtable) {
      return 4;
   }
   
   return 0;
}

void *ppg_token_vtable_ptr_from_id(uintptr_t id)
{
   switch(id) {
      case 1:
         return &ppg_token_vtable;
         break;
      case 2:
         return &ppg_note_vtable;
         break;
      case 3:
         return &ppg_chord_vtable;
         break;
      case 4:
         return &ppg_cluster_vtable;
         break;
   }
   
   return NULL;
}
