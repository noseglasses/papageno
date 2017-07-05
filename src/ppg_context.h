/* Copyright 2017 Florian Fleissner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PPG_CONTEXT_H
#define PPG_CONTEXT_H

/** @file */

/** @brief Creates a new papageno context
 * 
 * @returns The newly created context
 */
void* ppg_context_create(void);

/** @brief Destroys a papageno context
 * 
 * Make sure to unset a context before 
 * destroying it.
 * 
 * @param context The context to destroy
 */
void ppg_context_destroy(void *context);

/** @brief Sets a new current context
 * 
 * @param context The context to be activated
 * @returns The previously active context
 */
void* ppg_global_set_current_context(void *context);

/** @brief Retreives the current context
 * 
 * @returns The current context
 */
void* ppg_global_get_current_context(void);

#endif
