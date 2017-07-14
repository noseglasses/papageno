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

#include "ppg_bitfield.h"

void ppg_bitfield_clear(PPG_Bitfield *bitfield)
{
	PPG_Count max_id = bitfield->n_bits/8 + 1;
	
	for(PPG_Count i = 0; i < max_id; ++i) {
		bitfield->bitarray[i] = 0;
	}
}

bool ppg_bitfield_get_bit(PPG_Bitfield *bitfield, 
							  PPG_Count pos)
{
	PPG_Count byte = pos/8;
	PPG_Count bit = pos%8;
	
	return bitfield->bitarray[byte] & (1 << bit);
}

void ppg_bitfield_set_bit(PPG_Bitfield *bitfield, 
							  PPG_Count pos, 
							  bool state)
{
	PPG_Count byte = pos/8;
	PPG_Count bit = pos%8;
	
	if(state) {
		
		// Set the specific bit
		//
		bitfield->bitarray[byte] |= (1 << bit);
	}
	else {
		
		// Clear the specific bit
		//
		bitfield->bitarray[byte] &= ~(1 << bit);
	}
}
