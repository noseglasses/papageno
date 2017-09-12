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

#ifdef PPG_CS_COMPRESSED_CODE_FILE
#include PPG_CS_COMPRESSED_CODE_FILE
#endif
   
enum {
   ppg_cs_layer_0 = 0
};

PPG_CS_START_TEST
   
   #ifdef PPG_CS_COMPRESS_CONTEXT
   
   PPG_LOGGING_SET_ENABLED(false)
   
   // This assignment serves just to silence an unused variable
   // warning
   //
   automatically_reset_testing_system = false;
   (void)automatically_reset_testing_system;
   PPG_CS_INIT_COMPRESSION(ccontext)
   #endif
   
   #include "test_node_lines_actions.h"

//    PPG_CS_REGISTER_ACTION(Pattern_1)
//    PPG_CS_REGISTER_ACTION(Pattern_2)
//    PPG_CS_REGISTER_ACTION(Pattern_3)
//    PPG_CS_REGISTER_ACTION(Single_Note_Line_1)
//    PPG_CS_REGISTER_ACTION(3_Taps)
//    PPG_CS_REGISTER_ACTION(5_Taps)
   
   #ifdef PPG_CS_READ_COMPRESSED_CONTEXT
   PPG_INITIALIZE_CONTEXT_test
   PPG_LOGGING_SET_ENABLED(true)
   #else
   
   #include "test_node_lines_token_tree.h"
//    ppg_pattern(
//       ppg_cs_layer_0, /* Layer id */
//       PPG_TOKENS(
//          PPG_CS_N('a'),
//          PPG_CS_N('b'),
//          ppg_token_set_action(
//             PPG_CS_N('c'),
//             PPG_CS_ACTION(Pattern_1)
//          )
//       )
//    );
//    
//    ppg_pattern(
//       ppg_cs_layer_0, /* Layer id */
//       PPG_TOKENS(
//          PPG_CS_N('a'),
//          PPG_CS_N('a'),
//          ppg_token_set_action(
//             PPG_CS_N('c'),
//             PPG_CS_ACTION(Pattern_2)
//          )
//       )
//    );
//    
//    ppg_pattern(
//       ppg_cs_layer_0, /* Layer id */
//       PPG_TOKENS(
//          PPG_CS_N('a'),
//          PPG_CS_N('b'),
//          ppg_token_set_action(
//             PPG_CS_N('d'),
//             PPG_CS_ACTION(Pattern_3)
//          )
//       )
//    );
//    
//    ppg_single_note_line(
//       ppg_cs_layer_0,
//       PPG_CS_ACTION(Single_Note_Line_1),
//       PPG_INPUTS(
//          PPG_CS_CHAR('r'),
//          PPG_CS_CHAR('n'),
//          PPG_CS_CHAR('m')
//       )
//    );
//    
//    ppg_tap_dance(
//       ppg_cs_layer_0,
//       PPG_CS_CHAR('a'), /* The tap key */
//       PPG_TAP_DEFINITIONS(
//          PPG_TAP(3, 
//             PPG_CS_ACTION(3_Taps)
//          ),
//          PPG_TAP(5, 
//             PPG_CS_ACTION(5_Taps)
//          )
//       )
//    );
   
   ppg_cs_compile();
   #endif // PPG_CS_READ_COMPRESSED_CONTEXT
   
   #ifdef PPG_CS_COMPRESS_CONTEXT
   ppg_compression_run(ccontext, "test");
   #endif
   
   #ifndef PPG_CS_SUPPRESS_TESTS
   
   #include "test_node_lines_tests.h"
//    PPG_CS_PROCESS_ON_OFF(  "a b c", 
//                            PPG_CS_EXPECT_EMPTY_FLUSH
//                            PPG_CS_EXPECT_NO_EXCEPTIONS
//                            PPG_CS_EXPECT_ACTION_SERIES(
//                               PPG_CS_A(Pattern_1)
//                            )
//    );
//    
//    //PPG_PATTERN_PRINT_TREE
//    
//    PPG_CS_PROCESS_ON_OFF(  "a a c", 
//                            PPG_CS_EXPECT_EMPTY_FLUSH
//                            PPG_CS_EXPECT_NO_EXCEPTIONS
//                            PPG_CS_EXPECT_ACTION_SERIES(
//                               PPG_CS_A(Pattern_2)
//                            )
//    );
//    
//    PPG_CS_PROCESS_ON_OFF(  "a b d", 
//                            PPG_CS_EXPECT_EMPTY_FLUSH
//                            PPG_CS_EXPECT_NO_EXCEPTIONS
//                            PPG_CS_EXPECT_ACTION_SERIES(
//                               PPG_CS_A(Pattern_3)
//                            )
//    );
//    
//    PPG_CS_PROCESS_ON_OFF(  "r n m", 
//                            PPG_CS_EXPECT_EMPTY_FLUSH
//                            PPG_CS_EXPECT_NO_EXCEPTIONS
//                            PPG_CS_EXPECT_ACTION_SERIES(
//                               PPG_CS_A(Single_Note_Line_1)
//                            )
//    );
//    
//    PPG_CS_PROCESS_ON_OFF(  "a a a |", 
//                            PPG_CS_EXPECT_EMPTY_FLUSH
//                            PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_ET)
//                            PPG_CS_EXPECT_ACTION_SERIES(
//                               PPG_CS_A(3_Taps)
//                            )
//    );
//    
//    PPG_PATTERN_PRINT_TREE
//    
//    PPG_CS_PROCESS_ON_OFF(  "a a a a |", 
//                            PPG_CS_EXPECT_EMPTY_FLUSH
//                            PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_ET)
//                            PPG_CS_EXPECT_ACTION_SERIES(
//                               PPG_CS_A(3_Taps)
//                            )
//    );
//    
//    PPG_CS_PROCESS_ON_OFF(  "a a a a a |",  
//                            PPG_CS_EXPECT_EMPTY_FLUSH
//                            PPG_CS_EXPECT_NO_EXCEPTIONS
//                            PPG_CS_EXPECT_ACTION_SERIES(
//                               PPG_CS_A(5_Taps)
//                            )
//    );
   
   #endif // ifndef PPG_CS_SUPPRESS_TESTS
   
PPG_CS_END_TEST
