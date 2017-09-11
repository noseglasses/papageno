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

#include "detail/ppg_compression_detail.h"
#include "ppg_debug.h"

#include "assert.h"

void ppg_compression_context_register_symbol(void **symbol,
                                             PPG_Compression_Context__ *ccontext)
{
   assert(symbol);
   
   if(!(*symbol)) {
      return;
   }
   
   PPG_ASSERT(ccontext->symbols);
   PPG_ASSERT(ccontext->n_symbols < ccontext->n_symbols_space);
   
   ccontext->symbols[ccontext->n_symbols] = symbol;
   
   ++ccontext->n_symbols;
}

// void ppg_compression_context_register_vptr(void **vptr,
//                                           PPG_Compression_Context__ *ccontext)
// {
//    PPG_ASSERT(ccontext->vptrs);
//    PPG_ASSERT(ccontext->n_vptrs < ccontext->n_vptrs_space);
//    
//    ccontext->vptrs[ccontext->n_vptrs] = vptr;
//    
//    ++ccontext->n_vptrs;
// }
