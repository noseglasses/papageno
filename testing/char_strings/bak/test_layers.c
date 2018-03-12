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

#include "papageno_char_strings.h"
   
enum {
   ppg_cs_layer_0 = 0,
   ppg_cs_layer_1,
   ppg_cs_layer_2
};

PPG_CS_START_TEST

   PPG_CS_REGISTER_ACTION(Action_1)
   PPG_CS_REGISTER_ACTION(Action_2)
   PPG_CS_REGISTER_ACTION(Action_3)
   PPG_CS_REGISTER_ACTION(Action_4)
   
   ppg_pattern(
      ppg_cs_layer_0, /* Layer id */
      PPG_TOKENS(
         PPG_CS_N('a'),
         PPG_CS_N('b'),
         ppg_token_set_action(
            PPG_CS_N('c'),
            PPG_CS_ACTION(Action_1)
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
            PPG_CS_ACTION(Action_2)
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
            PPG_CS_ACTION(Action_3)
         )
      )
   );
   
   ppg_pattern(
      ppg_cs_layer_1, /* Layer id */
      PPG_TOKENS(
         PPG_CS_N('a'),
         PPG_CS_N('b'),
         ppg_token_set_action(
            PPG_CS_N('c'),
            PPG_CS_ACTION(Action_4)
         )
      )
   ); 
   
   // Empty override
   //
   ppg_pattern(
      ppg_cs_layer_2, /* Layer id */
      PPG_TOKENS(
         PPG_CS_N('a'),
         PPG_CS_N('b'),
         PPG_CS_N('c')
      )
   );
   
   ppg_cs_compile();
   
   PPG_CS_PROCESS_ON_OFF(  "a b c", 
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Action_1)
                           )
   );
   
   PPG_CS_PROCESS_ON_OFF(  "a a c", 
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Action_2)
                           )
   );
   
   PPG_CS_PROCESS_ON_OFF(  "a b d", 
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Action_3)
                           )
   );
   
   // Layer switches abort processing. Assert that aborting
   // works correctly.
   //
   // Note: Here no abortion should take place as no processing
   //       is on the way.
   //
   automatically_reset_testing_system = false;
   
   PPG_CS_PROCESS_ON_OFF(  "a b", 
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_NO_ACTIONS
   );
   
   ppg_global_set_layer(ppg_cs_layer_1);
   
   PPG_CS_CHECK_NO_PROCESS(
                           PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_EA)
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_ACTIONS
   )
      
   PPG_CS_PROCESS_ON_OFF(  "a b c", 
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Action_4)
                           )
   );
      
   ppg_global_set_layer(ppg_cs_layer_2);
         
   // Note: Here no abortion should take place as no processing
   //       is on the way.
   //
   PPG_CS_CHECK_NO_PROCESS(
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_ACTIONS
   )
   
   PPG_CS_PROCESS_ON_OFF(  "a b c", 
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_NO_ACTIONS
   );
   
   PPG_CS_PROCESS_ON_OFF(  "a a c", 
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Action_2)
                           )
   );
   
PPG_CS_END_TEST
