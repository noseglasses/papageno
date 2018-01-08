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

#ifndef PPG_BITFIELD_H
#define PPG_BITFIELD_H

#include "ppg_settings.h"

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t PPG_Bitfield_Storage_Type;

/** @brief A bitfield data type that can efficiently store boolean variables
 */
typedef struct {
   
   // Use a bitarray to store input state
   //
   PPG_Bitfield_Storage_Type *bitarray; ///< The actual storage
   
   uint8_t n_bits; ///< The number of bits that are considered as mutable
   
} PPG_Bitfield;

/** @brief Initialize a bitfield
 * 
 * In initialized state, the bitfield is empty and no memory is dynamically
 * allocated
 * 
 * @param bitfield The target bitfield
 */
void ppg_bitfield_init(PPG_Bitfield *bitfield);

/** @brief Resizes a bitfield
 * 
 * @param bitfield The target bitfield
 * @param n_bits The new number of bits that can be stored
 * @param keep_content It this is true, the old content (if old size was non-zero) is copied during resizing
 */
void ppg_bitfield_resize(PPG_Bitfield *bitfield, 
                         uint8_t n_bits, 
                         bool keep_content);

/** @brief Retrieves the number of cells (bytes) used to store the bits
 * 
 * @param bitfield The bitfield to check
 */
PPG_Count ppg_bitfield_get_num_cells(PPG_Bitfield *bitfield);
PPG_Count ppg_bitfield_get_num_cells_from_bits(uint8_t n_bits);

/** @brief Sets all bits to zero
 * 
 * 
 * @param bitfield The target bitfield
 */
void ppg_bitfield_clear(PPG_Bitfield *bitfield);

/** @brief Retreives the value of an individual bit
 * 
 * @param bitfield The target bitfield
 * @param pos The position of the bit
 */
bool ppg_bitfield_get_bit(PPG_Bitfield *bitfield, 
                           PPG_Count pos);

/** @brief Sets a bit to a specific state
 * 
 * @param bitfield The target bitfield
 * @param pos The position of the bit
 * @param state The new bit state
 */
void ppg_bitfield_set_bit(PPG_Bitfield *bitfield, 
                           PPG_Count pos, 
                           bool state);

/** @brief Copies a bitfield 
 * 
 * After the copy operation both bitfields have exactly the same state.
 * 
 * @param source The source bitfield
 * @param target The target bitfield
 */
void ppg_bitfield_copy(PPG_Bitfield *source, PPG_Bitfield *target);

/** @brief Frees all dynamically allocated resources and restores initialized state
 * 
 * @param bitfield The target bitfield
 */
void ppg_bitfield_destroy(PPG_Bitfield *bitfield);

#endif
