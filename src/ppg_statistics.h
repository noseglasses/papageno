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

#ifndef PPG_STATISTICS_H
#define PPG_STATISTICS_H

#include "ppg_settings.h"

#if PPG_HAVE_STATISTICS

typedef struct {
   
   uint32_t n_nodes_visited;
   uint32_t n_token_checks;
   uint32_t n_furcations;
   uint32_t n_reversions;
   
} PPG_Statistics;

/** @brief Retreives global statistics
 * 
 * @param stat A pointer to an existing statistics data set where to store
 *             a copy of the current global statistics.
 */
void ppg_statistics_get(PPG_Statistics *stat);

/** @brief Clears a statistics data set
 * 
 * @param stat The statistics to clear. Pass NULL to clear global statistics.
 */
void ppg_statistics_clear(PPG_Statistics *stat);

#endif

#endif
