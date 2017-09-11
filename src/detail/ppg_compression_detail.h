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

#ifndef PPG_COMPRESSION_DETAIL_HH
#define PPG_COMPRESSION_DETAIL_HH

#include "ppg_compression.h"

#include <stddef.h>
#include <stdint.h>

typedef struct {
   void *address;
   char *name;
} PPG_Compression_Symbol;

typedef struct {
   PPG_Compression_Symbol *buffer;
   size_t n_allocated;
   size_t n_stored;
} PPG_Compression_Symbol_Buffer;

typedef struct {
   
   PPG_Compression_Symbol_Buffer symbols_lookup;
   
   char *target_storage;
   size_t storage_size;
   
   void ***symbols;
   size_t n_symbols;
   size_t n_symbols_space;
   
} PPG_Compression_Context__;

typedef struct {
   uint8_t token_type_id;
   size_t offset;
} PPG_Compression_VPtr_Info;

void ppg_compression_context_register_symbol(void **symbol,
                                             PPG_Compression_Context__ *ccontext);

void ppg_compression_context_register_vptr(void **vptr,
                                          PPG_Compression_Context__ *ccontext);
#endif
