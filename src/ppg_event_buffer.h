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

#ifndef PPG_EVENT_BUFFER_H
#define PPG_EVENT_BUFFER_H

#include "ppg_event.h"

/** @brief Call this function to actively flush any input events that are in cache.
 * 
 * This function is called internally when melodies complete, on abort and on timeout.
 * 
 * @param event_processor A custom input processor callback. If NULL the default processor registered by
 *                ppg_global_set_default_event_processor is used.
 * @param user_data Optional user data is passed on to the input processor callback
 * 
 * @returns The number of events that were processed.
 */
uint8_t ppg_event_buffer_iterate(
                        PPG_Event_Processor_Fun event_processor,
                        void *user_data);

#endif
