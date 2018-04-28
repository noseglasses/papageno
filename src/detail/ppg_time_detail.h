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

#ifndef PPG_TIME_DETAIL_H
#define PPG_TIME_DETAIL_H

#include "ppg_time.h"

void ppg_default_time(PPG_Time *time);
void ppg_default_time_difference(PPG_Time time1, 
                                        PPG_Time time2, 
                                        PPG_Time *delta);

PPG_Time_Comparison_Result_Type ppg_default_time_comparison(
                        PPG_Time time1,
                        PPG_Time time2);

#endif
