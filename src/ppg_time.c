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

#include "ppg_time.h"
#include "ppg_global.h"
#include "detail/ppg_context_detail.h"

PPG_Time_Fun ppg_set_time_function(PPG_Time_Fun fun)
{
	PPG_Time_Fun old_fun = fun;
	
	ppg_context->time = fun;
	
	return old_fun;
}

PPG_Time_Difference_Fun ppg_set_time_difference_function(PPG_Time_Difference_Fun fun)
{
	PPG_Time_Difference_Fun old_fun = ppg_context->time_difference;
	
	ppg_context->time_difference = fun;
	
	return old_fun;
}

PPG_Time_Comparison_Fun ppg_set_time_comparison_function(PPG_Time_Comparison_Fun fun)
{
	PPG_Time_Comparison_Fun old_fun = ppg_context->time_comparison;
	
	ppg_context->time_comparison = fun;
	
	return old_fun;
}