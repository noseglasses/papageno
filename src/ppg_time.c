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

#include "ppg_time.h"
#include "ppg_global.h"
#include "detail/ppg_context_detail.h"

static void ppg_default_time(PPG_Time *time)
{
   *time = 0;
}

static void ppg_default_time_difference(PPG_Time time1, 
                                        PPG_Time time2, 
                                        PPG_Time *delta)
{
   *delta = 0;
}

PPG_Time_Comparison_Result_Type ppg_default_time_comparison(
                        PPG_Time time1,
                        PPG_Time time2)
{
   return 0;
}

void ppg_time_manager_init(PPG_Time_Manager *time_manager)
{
   time_manager->time = ppg_default_time;
   time_manager->time_difference = ppg_default_time_difference;
   time_manager->compare_times = ppg_default_time_comparison;
}

PPG_Time_Manager ppg_global_set_time_manager(PPG_Time_Manager time_manager)
{
   PPG_Time_Manager old_time_manager = ppg_context->time_manager;
   
   ppg_context->time_manager = time_manager;
   
   return old_time_manager;
}

PPG_Time_Manager ppg_global_get_time_manager(void)
{
   return ppg_context->time_manager;
}
