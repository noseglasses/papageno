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

#include "detail/ppg_token_detail.h"
#include "detail/ppg_pattern_detail.h"
#include "detail/ppg_context_detail.h"
#include "ppg_debug.h"
#include "ppg_global.h"

PPG_Token ppg_pattern(     
                     PPG_Layer layer, 
                     PPG_Count n_tokens,
                     PPG_Token tokens[])
{ 
//    PPG_LOG("Adding pattern\n");
   
   ppg_global_init();
      
   return ppg_pattern_from_list(NULL, layer, n_tokens, (PPG_Token__ **)tokens);
}

#if PPG_PRINT_SELF_ENABLED
void ppg_pattern_print_tree(void)
{
   PPG_PRINT_TOKEN_RECURSE(ppg_context->pattern_root)
}
#endif
