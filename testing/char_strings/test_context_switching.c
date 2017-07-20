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

#if !PPG_DISABLE_CONTEXT_SWITCHING
   
enum {
   ppg_cs_layer_0 = 0
};

// To check context switching, we register the same chord with
// different actions on two contexts.

PPG_CS_START_TEST

   PPG_CS_REGISTER_ACTION(Chord_1)
   PPG_CS_REGISTER_ACTION(Chord_2)
   
   ppg_chord(
      ppg_cs_layer_0,
      PPG_CS_ACTION(Chord_1),
      PPG_INPUTS(
         PPG_CS_CHAR('a'),
         PPG_CS_CHAR('b'),
         PPG_CS_CHAR('c')
      )
   );
   
   // Compile the search tree of the first context
   //
   ppg_cs_compile();
   
   void* context_2 = ppg_context_create();
   void* context_1 = ppg_global_set_current_context(context_2);
   
   // Now the second context is active. First we prepare/customize it.
   //
   PPG_CS_PREPARE_CONTEXT
   
   ppg_chord(
      ppg_cs_layer_0,
      PPG_CS_ACTION(Chord_2),
      PPG_INPUTS(
         PPG_CS_CHAR('a'),
         PPG_CS_CHAR('b'),
         PPG_CS_CHAR('c')
      )
   );
   
   // Compile the search tree of the second context
   //
   ppg_cs_compile();
   
   //***********************************************
   // Check context 1
   //***********************************************
   
   ppg_global_set_current_context(context_1);
   
   PPG_CS_PROCESS_STRING(  "A B C c b a",
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Chord_1)
                           )
   );
   
   //***********************************************
   // Check context 2
   //***********************************************
   
   ppg_global_set_current_context(context_2);
   
   PPG_CS_PROCESS_STRING(  "A B C c b a",
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Chord_2)
                           )
   );
   
   //***********************************************
   // Check context 1
   //***********************************************
   
   ppg_global_set_current_context(context_1);
   
   PPG_CS_PROCESS_STRING(  "A B C c b a",
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Chord_1)
                           )
   );
   

   //***********************************************
   // Cleanup
   //***********************************************
   
   // Context 2 is currently inactive, so we can safely destroy it
   //
   ppg_context_destroy(context_2); 
   
   // Note: Context 1 is still active and will be destroyed during 
   //       an automatic call to ppg_global_finalize
   //       at the end of the test
   
PPG_CS_END_TEST

#endif
