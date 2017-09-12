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

#include "string.h"

enum {
   ppg_cs_layer_0 = 0
};

#define PPG_CS_INPUT_FROM_CHAR(CHAR) \
   (CHAR - 'a')

#define PPG_CS_ALPH_CASE(LOWER_CASE_QUOTED, UPPER_CASE_QUOTED, ID) \
   case LOWER_CASE_QUOTED: \
   case UPPER_CASE_QUOTED: \
      return LOWER_CASE_QUOTED;
   
static PPG_Input_Id ppg_cs_input_from_alphabetic_character(char c) {   
   
   switch(c) {
      PPG_CS_ALPH_CASE('a', 'A', A)
      PPG_CS_ALPH_CASE('b', 'B', B)
      PPG_CS_ALPH_CASE('c', 'C', C)
      PPG_CS_ALPH_CASE('d', 'D', D)
      PPG_CS_ALPH_CASE('e', 'E', E)
      PPG_CS_ALPH_CASE('f', 'F', F)
      PPG_CS_ALPH_CASE('g', 'G', G)
      PPG_CS_ALPH_CASE('h', 'H', H)
      PPG_CS_ALPH_CASE('i', 'I', I)
      PPG_CS_ALPH_CASE('j', 'J', J)
      PPG_CS_ALPH_CASE('k', 'K', K)
      PPG_CS_ALPH_CASE('l', 'L', L)
      PPG_CS_ALPH_CASE('m', 'M', M)
      PPG_CS_ALPH_CASE('n', 'N', N)
      PPG_CS_ALPH_CASE('o', 'O', O)
      PPG_CS_ALPH_CASE('p', 'P', P)
      PPG_CS_ALPH_CASE('q', 'Q', Q)
      PPG_CS_ALPH_CASE('r', 'R', R)
      PPG_CS_ALPH_CASE('s', 'S', S)
      PPG_CS_ALPH_CASE('t', 'T', T)
      PPG_CS_ALPH_CASE('u', 'U', U)
      PPG_CS_ALPH_CASE('v', 'V', V)
      PPG_CS_ALPH_CASE('w', 'W', W)
      PPG_CS_ALPH_CASE('x', 'X', X)
      PPG_CS_ALPH_CASE('y', 'Y', Y)
      PPG_CS_ALPH_CASE('z', 'Z', Z)
   }
   
   return 0;
}

static void ppg_cs_retreive_string(uint8_t sequence_id,
                              char *buffer, 
                              uint8_t max_chars)
{
   switch(sequence_id) {
      case 0:
         strcpy(buffer, "foo");
         break;
      case 1:
         strcpy(buffer, "bar");
         break;
      case 2:
         strcpy(buffer, "Klaatubaradan");
         break;
   }
}

static PPG_Action ppg_cs_retreive_action(uint8_t sequence_id)
{
   return (PPG_Action) {
      .callback = (PPG_Action_Callback) {
         .func = (PPG_Action_Callback_Fun)ppg_cs_process_action,
         
         // The first three actions are reserved
         //
         .user_data = (void*)(uintptr_t)(sequence_id + 3)
      }
   };
}

PPG_CS_START_TEST

   PPG_CS_REGISTER_ACTION(Foo)
   PPG_CS_REGISTER_ACTION(Bar)
   PPG_CS_REGISTER_ACTION(Klaatubaradan)
   
   PPG_Token leader_token
      = 
   ppg_token_set_flags(
      ppg_chord(
         ppg_cs_layer_0,
         PPG_ACTION_NOOP,
         PPG_INPUTS(
            PPG_CS_CHAR('a'),
            PPG_CS_CHAR('b'),
            PPG_CS_CHAR('c')
         )
      ),
      
      // Without the following the chord would consume all consequent
      // releases and re-pressing of keys involved
      //
      PPG_Chord_Flags_Disallow_Input_Deactivation
   );
   
   ppg_alphabetic_leader_sequences(
      ppg_cs_layer_0, // layer
      leader_token,  // The leader input, used zero if no leader key is 
                     // necessary
      4,
      (PPG_Leader_Functions) {
         .retreive_string = ppg_cs_retreive_string,
         .retreive_action = ppg_cs_retreive_action,
         .input_from_char = ppg_cs_input_from_alphabetic_character
      },
      true // Allow fallback, i.e. only require input until a sequence
           // can be uniquely identified
   );
   
   ppg_alphabetic_leader_sequences(
      ppg_cs_layer_0, // layer
      NULL,  // The leader input, used zero if no leader key is 
                     // necessary
      4,
      (PPG_Leader_Functions) {
         .retreive_string = ppg_cs_retreive_string,
         .retreive_action = ppg_cs_retreive_action,
         .input_from_char = ppg_cs_input_from_alphabetic_character
      },
      true // Allow fallback, i.e. only require input until a sequence
           // can be uniquely identified
   );
   
   ppg_cs_compile();
   
   PPG_CS_PROCESS_STRING(  "A B C c b a FOO oof",
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Foo)
                           )
   );
   
   PPG_CS_PROCESS_STRING(  "A B C c b a BAR bar",
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Bar)
                           )
   );
   
   PPG_CS_PROCESS_STRING(  "A B C c b a KLAATUBARADANIKTO klaatubaradanikto",
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Klaatubaradan)
                           )
   );
   
   PPG_CS_PROCESS_STRING(  "FOO oof",
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Foo)
                           )
   );
   
   PPG_CS_PROCESS_STRING(  "BAR bar",
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Bar)
                           )
   );
   
   PPG_CS_PROCESS_STRING(  "KLAATUBARADANIKTO klaatubaradanikto",
                           PPG_CS_EXPECT_EMPTY_FLUSH
                           PPG_CS_EXPECT_NO_EXCEPTIONS
                           PPG_CS_EXPECT_ACTION_SERIES(
                              PPG_CS_A(Klaatubaradan)
                           )
   );
   
PPG_CS_END_TEST