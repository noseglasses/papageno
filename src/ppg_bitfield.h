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

#ifndef PPG_BITFIELD_H
#define PPG_BITFIELD_H

typedef struct {
	
	// Use a bitarray to store input state
	//
	uint8_t bitarray[PPG_MAX_INPUTS/8];
	
	uint8_t n_inputs;
	
} PPG_Bitfield;

void ppg_bitfield_clear(void);

bool ppg_bitfield_get_bit(PPG_Bitfield *bitfield, 
									PPG_Count pos);

void ppg_bitfield_set_bit(PPG_Bitfield *bitfield, 
									PPG_Count pos, 
									bool state);

#endif
