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

#ifndef PPG_MALLOC_DETAIL_H
#define PPG_MALLOC_DETAIL_H

#include "ppg_debug.h"

#if PPG_HAVE_ASSERTIONS

void *ppg_safe_malloc(size_t n_bytes,
                      char *file,
                      unsigned long line);

#define PPG_MALLOC(N_BYTES) \
   ppg_safe_malloc(N_BYTES, __FILE__, __LINE__)
   
#else

void *ppg_safe_malloc(size_t n_bytes);

#define PPG_MALLOC(N_BYTES) \
   ppg_safe_malloc(N_BYTES)
   
#endif

#endif
