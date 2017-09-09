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

#include "papageno_char_strings.h"

#include "detail/ppg_context_detail.h"
#include "detail/ppg_compression_detail.h"
   
enum {
   ppg_cs_layer_0 = 0
};

int ppg_cs_test_value = 13;

void ppg_cs_test_callback(bool activation, void *user_data)
{
   int *val = (int*)user_data;
   
   *val = 17;
}

#include "ppg_compressed_context.h"

PPG_CS_START_TEST

   // This assignment serves just to silence an unused variable
   // warning
   //
   automatically_reset_testing_system = false;
   (void)automatically_reset_testing_system;

   PPG_Compression_Context ccontext
               = ppg_compression_init();
               
   PPG_CS_INIT_COMPRESSION(ccontext)
               
   PPG_COMPRESSION_REGISTER_SYMBOL(ccontext, ppg_cs_test_value)
   PPG_COMPRESSION_REGISTER_SYMBOL(ccontext, ppg_cs_test_callback)

   PPG_CS_REGISTER_ACTION(Pattern_2)
   PPG_CS_REGISTER_ACTION(Pattern_3)
   PPG_CS_REGISTER_ACTION(Single_Note_Line_1)
   PPG_CS_REGISTER_ACTION(3_Taps)
   PPG_CS_REGISTER_ACTION(5_Taps)

   ppg_pattern(
      ppg_cs_layer_0, /* Layer id */
      PPG_TOKENS(
         PPG_CS_N('a'),
         PPG_CS_N('b'),
         ppg_token_set_action(
            PPG_CS_N('c'),
            PPG_CS_ACTION_CALLBACK(
               ppg_cs_test_callback,
               (void*)&ppg_cs_test_value
            )
         )
      )
   );
   
   ppg_pattern(
      ppg_cs_layer_0, /* Layer id */
      PPG_TOKENS(
         PPG_CS_N('a'),
         PPG_CS_N('a'),
         ppg_token_set_action(
            PPG_CS_N('c'),
            PPG_CS_ACTION(Pattern_2)
         )
      )
   );
   
   ppg_pattern(
      ppg_cs_layer_0, /* Layer id */
      PPG_TOKENS(
         PPG_CS_N('a'),
         PPG_CS_N('b'),
         ppg_token_set_action(
            PPG_CS_N('d'),
            PPG_CS_ACTION(Pattern_3)
         )
      )
   );
   
   ppg_single_note_line(
      ppg_cs_layer_0,
      PPG_CS_ACTION(Single_Note_Line_1),
      PPG_INPUTS(
         PPG_CS_CHAR('r'),
         PPG_CS_CHAR('n'),
         PPG_CS_CHAR('m')
      )
   );
   
   ppg_tap_dance(
      ppg_cs_layer_0,
      PPG_CS_CHAR('a'), /* The tap key */
      PPG_TAP_DEFINITIONS(
         PPG_TAP(3, 
            PPG_CS_ACTION(3_Taps)
         ),
         PPG_TAP(5, 
            PPG_CS_ACTION(5_Taps)
         )
      )
   );
   
   ppg_cs_compile();
   
   PPG_CS_PRINT_TREE
   
   ppg_compression_run(ccontext, "test");
   
//    PPG_CS_PRINT_TREE
   
   PPG_Compression_Context__ *ccontext__ = (PPG_Compression_Context__ *)ccontext;
//    PPG_Context *compression_context = (PPG_Context*)ccontext__->target_storage;
   
   printf("sizeof target storage: %lu\n", ccontext__->storage_size);
   printf("sizeof test_context: %lu\n", sizeof(test_context));
//    PPG_PRINT_TOKEN_RECURSE(compression_context->pattern_root)
   
   PPG_INITIALIZE_CONTEXT_test
   
   PPG_PRINT_TOKEN_RECURSE(((PPG_Context*)test_context)->pattern_root)
   
//    ppg_compression_finalize(ccontext);
   
PPG_CS_END_TEST
