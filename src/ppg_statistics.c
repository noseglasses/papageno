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

#include "ppg_statistics.h"
#include "detail/ppg_context_detail.h"

#if PPG_HAVE_STATISTICS

void ppg_statistics_get(PPG_Statistics *stat)
{
   *stat = ppg_context->statistics;
}

void ppg_statistics_clear(PPG_Statistics *stat)
{
   #define PPG_STAT(S) stat->S
   
   if(!stat) {
      stat = &ppg_context->statistics;
   }
   
   PPG_STAT(n_nodes_visited) = 0;
   PPG_STAT(n_token_checks) = 0;
   PPG_STAT(n_furcations) = 0;
   PPG_STAT(n_reversions) = 0;
}

#endif // PPG_HAVE_STATISTICS
