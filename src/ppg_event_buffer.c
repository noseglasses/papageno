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

#include "ppg_event_buffer.h"
#include "detail/ppg_event_buffer_detail.h"
#include "detail/ppg_context_detail.h"
#include "ppg_debug.h"

typedef struct {
   PPG_Event_Processor_Fun fun;
   void *user_data;
} PPG_Event_Buffer_Visitor_Adaptor_Data;

static void ppg_event_buffer_visit_adaptor(PPG_Event_Queue_Entry *eqe,
                        void *user_data)
{
   PPG_Event_Buffer_Visitor_Adaptor_Data *adaptor_data
      = (PPG_Event_Buffer_Visitor_Adaptor_Data*)user_data;
      
   PPG_ASSERT(adaptor_data);
   PPG_ASSERT(adaptor_data->fun);
   
   adaptor_data->fun(&eqe->event, adaptor_data->user_data);
}

void ppg_event_buffer_iterate(
                        PPG_Event_Processor_Fun event_processor,
                        void *user_data)
{
   PPG_Event_Buffer_Visitor_Adaptor_Data adaptor_data =
      { 
         .fun = event_processor,
         .user_data = user_data
      };
      
   ppg_event_buffer_iterate2(
      (PPG_Event_Processor_Visitor)ppg_event_buffer_visit_adaptor,
      &adaptor_data
   );
}
