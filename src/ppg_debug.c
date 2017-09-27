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

#include "ppg_debug.h"
#include "detail/ppg_context_detail.h"

#if PPG_HAVE_LOGGING
bool ppg_logging_set_enabled(bool state)
{
   bool old_state = ppg_context->properties.logging_enabled;
   ppg_context->properties.logging_enabled = state;
   return old_state;
}

bool ppg_logging_get_enabled(void)
{
   return !ppg_context || ppg_context->properties.logging_enabled;
}
#endif
