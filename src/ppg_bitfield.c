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

#include "ppg_bitfield.h"
#include "detail/ppg_malloc_detail.h"

#include <string.h>
#include <stdlib.h>

void ppg_bitfield_init(PPG_Bitfield *bitfield)
{
   bitfield->n_bits = 0;
   bitfield->bitarray = NULL;
}

PPG_Count ppg_bitfield_get_num_cells_from_bits(uint8_t n_bits)
{
   PPG_Count cells = n_bits/(8*sizeof(PPG_Bitfield_Storage_Type));
   PPG_Count bits = n_bits%(8*sizeof(PPG_Bitfield_Storage_Type));
      
   if(bits != 0) {
      cells += 1;
   }
   
   return cells;
}

PPG_Count ppg_bitfield_get_num_cells(PPG_Bitfield *bitfield)
{
   return ppg_bitfield_get_num_cells_from_bits(bitfield->n_bits);
}

void ppg_bitfield_clear(PPG_Bitfield *bitfield)
{
   if(!bitfield->bitarray) { return; }
  
   PPG_Count cells = ppg_bitfield_get_num_cells_from_bits(bitfield->n_bits);
   
   memset(bitfield->bitarray, 0, cells);
}

bool ppg_bitfield_get_bit(PPG_Bitfield *bitfield, 
                       PPG_Count pos)
{
   PPG_Count cell = pos/(8*sizeof(PPG_Bitfield_Storage_Type));
   PPG_Count bit = pos%(8*sizeof(PPG_Bitfield_Storage_Type));
   
   return bitfield->bitarray[cell] & (1 << bit);
}

void ppg_bitfield_set_bit(PPG_Bitfield *bitfield, 
                       PPG_Count pos, 
                       bool state)
{
   PPG_Count cell = pos/(8*sizeof(PPG_Bitfield_Storage_Type));
   PPG_Count bit = pos%(8*sizeof(PPG_Bitfield_Storage_Type));
   
   if(state) {
      
      // Set the specific bit
      //
      bitfield->bitarray[cell] |= (PPG_Bitfield_Storage_Type)(1 << bit);
   }
   else {
      
      // Clear the specific bit
      //
      bitfield->bitarray[cell] &= (PPG_Bitfield_Storage_Type)~(1 << bit);
   }
}

void ppg_bitfield_resize(PPG_Bitfield *bitfield, 
                         uint8_t n_bits, 
                         bool keep_content)
{
   if(bitfield->n_bits != n_bits) {
            
      PPG_Count cells = ppg_bitfield_get_num_cells_from_bits(n_bits);
      
      if(bitfield->bitarray) {
         
         uint8_t *new_bitarray
            = (uint8_t*)PPG_MALLOC(cells*sizeof(PPG_Bitfield_Storage_Type));
            
         // Copy the content of the previous bitarray
         //
         if(keep_content) {
               
            PPG_Count old_cells 
               = ppg_bitfield_get_num_cells_from_bits(bitfield->n_bits);
               
            memcpy(new_bitarray, bitfield->bitarray, old_cells);
         }
         
         free(bitfield->bitarray);
         
         bitfield->bitarray = new_bitarray;
      }
      else {
         bitfield->bitarray 
            = (uint8_t*)PPG_MALLOC(cells*sizeof(PPG_Bitfield_Storage_Type));
         
         for(size_t cell_id = 0; cell_id < cells; ++cell_id) {
            bitfield->bitarray[cell_id] = 0;
         }
      }
      
      bitfield->n_bits = n_bits;
   }
}

void ppg_bitfield_copy(PPG_Bitfield *source, PPG_Bitfield *target)
{
   if(source->bitarray == NULL) {
      ppg_bitfield_destroy(target);
      return;
   }
      
   if(target->n_bits != source->n_bits) {
      
      ppg_bitfield_resize(target, source->n_bits, 
         false /* no need to keep content as it is overwritten */);
   }

   PPG_Count cells = ppg_bitfield_get_num_cells_from_bits(source->n_bits);
   
   memcpy(target->bitarray, source->bitarray, cells);
}

void ppg_bitfield_destroy(PPG_Bitfield *bitfield)
{
   if(bitfield->bitarray) {
      free(bitfield->bitarray);
      bitfield->bitarray = NULL;
      bitfield->n_bits = 0;
   }
}
