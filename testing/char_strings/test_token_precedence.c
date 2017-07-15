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
   
enum {
   ppg_cs_layer_0 = 0,
   ppg_cs_layer_1
};

PPG_CS_START_TEST

   PPG_CS_REGISTER_ACTION(Single_Note_Line_1)
   PPG_CS_REGISTER_ACTION(Chord_1)
   PPG_CS_REGISTER_ACTION(Cluster_1)
   
   PPG_CS_REGISTER_ACTION(Chord_2)
   PPG_CS_REGISTER_ACTION(Cluster_2)
   
   PPG_CS_REGISTER_ACTION(Chord_3)
   PPG_CS_REGISTER_ACTION(Cluster_3)
      
   PPG_CS_REGISTER_ACTION(Single_Note_Line_4)
   PPG_CS_REGISTER_ACTION(Cluster_4)
   
   ppg_single_note_line(
      ppg_cs_layer_0,
      PPG_CS_ACTION(Single_Note_Line_1),
      PPG_INPUTS(
         PPG_CS_CHAR('a'),
         PPG_CS_CHAR('b'),
         PPG_CS_CHAR('c')
      )
   );
   
   ppg_chord(
      ppg_cs_layer_0,
      PPG_CS_ACTION(Chord_1),
      PPG_INPUTS(
         PPG_CS_CHAR('a'),
         PPG_CS_CHAR('b'),
         PPG_CS_CHAR('c')
      )
   );
   
   ppg_cluster(
      ppg_cs_layer_0,
      PPG_CS_ACTION(Cluster_1),
      PPG_INPUTS(
         PPG_CS_CHAR('a'),
         PPG_CS_CHAR('b'),
         PPG_CS_CHAR('c')
      )
   );
   
   //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   
   ppg_chord(
      ppg_cs_layer_0,
      PPG_CS_ACTION(Chord_2),
      PPG_INPUTS(
         PPG_CS_CHAR('x'),
         PPG_CS_CHAR('y'),
         PPG_CS_CHAR('z')
      )
   );
   
   ppg_cluster(
      ppg_cs_layer_0,
      PPG_CS_ACTION(Cluster_2),
      PPG_INPUTS(
         PPG_CS_CHAR('x'),
         PPG_CS_CHAR('y'),
         PPG_CS_CHAR('z')
      )
   ); 
   
   //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   
   ppg_single_note_line(
      ppg_cs_layer_0,
      PPG_CS_ACTION(Single_Note_Line_4),
      PPG_INPUTS(
         PPG_CS_CHAR('d'),
         PPG_CS_CHAR('e'),
         PPG_CS_CHAR('f')
      )
   );
   
   ppg_cluster(
      ppg_cs_layer_0,
      PPG_CS_ACTION(Cluster_4),
      PPG_INPUTS(
         PPG_CS_CHAR('d'),
         PPG_CS_CHAR('e'),
         PPG_CS_CHAR('f')
      )
   ); 
   
   //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   
   ppg_chord(
      ppg_cs_layer_0,
      PPG_CS_ACTION(Chord_3),
      PPG_INPUTS(
         PPG_CS_CHAR('m'),
         PPG_CS_CHAR('n'),
         PPG_CS_CHAR('o')
      )
   );
      
   ppg_cluster(
      ppg_cs_layer_1,
      PPG_CS_ACTION(Cluster_3),
      PPG_INPUTS(
         PPG_CS_CHAR('m'),
         PPG_CS_CHAR('n'),
         PPG_CS_CHAR('o')
      )
   ); 

   ppg_cs_compile();
   
   //***********************************************
   // Check precedence of single note lines against
   // chords
   //***********************************************
   
   PPG_CS_PROCESS_STRING(  "A B C c b a", 
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Single_Note_Line_1)
                           )
   );
   
   //***********************************************
   // Check precedence of single note lines against
   // clusters
   //***********************************************
      
   PPG_CS_PROCESS_STRING(  "D E F e f d", 
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Single_Note_Line_4)
                           )
   );
      
   //***********************************************
   // Check precedence of chords against
   // clusters
   //***********************************************
      
   PPG_CS_PROCESS_STRING(  "X Y Z y x z", 
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Chord_2)
                           )
   ); 
   
   //***********************************************
   // Check precedence of clusters against
   // chords if the cluster is associated with a higher layer
   //***********************************************
   
   ppg_global_set_layer(ppg_cs_layer_1);
   
   // Layer switches abort processing. Assert that aborting
   // works correctly.
   //
   PPG_CS_CHECK_NO_PROCESS(
                           PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_EA)
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_ACTIONS
   )
   
   // Allow to release keys and then repress them 
   //
   PPG_CS_PROCESS_STRING(  "M N O m n o", 
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Cluster_3)
                           )
   );
   
PPG_CS_END_TEST
