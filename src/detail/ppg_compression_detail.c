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

int ppg_compression_check_symbol_registered(PPG_Compression_Context__ *ccontext,
                                            void *symbol)
{
   int s = 0;
   for(; s < ccontext->symbols_lookup.n_stored; ++s) {
      
//          printf("Checking symbol %s\n", ccontext->symbols_lookup.buffer[s].name);
//          printf("   Stored symbol %p\n", ccontext->symbols_lookup.buffer[s].address);
//          printf("   Checking symbol %p\n", symbol);
      
      if(ccontext->symbols_lookup.buffer[s].address == symbol) {
         break;
      }
   }
   
   if(!(s < ccontext->symbols_lookup.n_stored)) {
      s = -1;
   }
   
   return s;
}

void ppg_compression_context_register_symbol(void **symbol,
                                             PPG_Compression_Context__ *ccontext)
{
   PPG_ASSERT(symbol);
   
   if(!(*symbol)) {
      return;
   }
   
   PPG_ASSERT(ccontext->symbols);
   PPG_ASSERT(ccontext->n_symbols < ccontext->n_symbols_space);
   
   int s = ppg_compression_check_symbol_registered(ccontext, *symbol);
   
   if(s < 0) {
      PPG_LOG("Symbol %p unregistered\n", *symbol);
   }
   else {
   
      ccontext->symbols[ccontext->n_symbols] = symbol;
      
      ++ccontext->n_symbols;
   }
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
