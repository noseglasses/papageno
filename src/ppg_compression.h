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

#ifndef PPG_COMPRESSION_H
#define PPG_COMPRESSION_H

#include "stddef.h"

typedef void *  PPG_Compression_Context;

PPG_Compression_Context ppg_compression_init(void);

void ppg_compression_finalize(PPG_Compression_Context ccontext);

void ppg_compression_register_symbol(
                         PPG_Compression_Context ccontext,
                         void *symbol,
                         char *symbol_name);

#define PPG_COMPRESSION_STRINGIZE(S) #S

#define PPG_COMPRESSION_REGISTER_SYMBOL(CONTEXT, S) \
 \
         ppg_compression_register_symbol( \
                  CONTEXT, \
                  &S, \
                  PPG_COMPRESSION_STRINGIZE(S));
         
void ppg_compression_run(  PPG_Compression_Context ccontext,
                           char *name_tag);

void ppg_compression_setup_context(   void *context, 
                                        void *aux_array, 
                                        size_t aux_array_size);

#endif
