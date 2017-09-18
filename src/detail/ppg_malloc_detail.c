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

#include "detail/ppg_malloc_detail.h"

#include "string.h"

#if PPG_HAVE_ASSERTIONS

void *ppg_safe_malloc(size_t n_bytes,
                      char *file,
                      unsigned long line)
{
    void* p = malloc(n_bytes);
    if (!p)
    {
        PPG_ERROR("[%s:%lu]Out of memory(%lu bytes)\n",
                file, line, (unsigned long)n_bytes);
         abort();
    }
    return p;
}

#else

void *ppg_safe_malloc(size_t n_bytes)
{
    void* p = malloc(n_bytes);
    if (!p)
    {
        PPG_ERROR("Out of memory(%lu bytes)\n",
                (unsigned long)n_bytes);
         abort();
    }
    return p;
}

#endif
