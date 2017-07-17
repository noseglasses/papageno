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
   ppg_cs_layer_0 = 0
};

PPG_CS_START_TEST

	const int tree_depth = 4;
	const int n_chars = 3;

   PPG_CS_REGISTER_ACTION(Pattern)
	
	PPG_Token tokens[tree_depth];
	
	for(int l1 = 0; l1 < n_chars; ++l1) {
	for(int l2 = 0; l2 < n_chars; ++l2) {
	for(int l3 = 0; l3 < n_chars; ++l3) {
	for(int l4 = 0; l4 < n_chars; ++l4) {
		tokens[0] = PPG_CS_N(('a' + l1));
		tokens[1] = PPG_CS_N(('a' + l2));
		tokens[2] = PPG_CS_N(('a' + l3));
		tokens[3] = PPG_CS_N(('a' + l4));
   
      ppg_token_set_action(
         ppg_pattern(
            ppg_cs_layer_0, /* Layer id */
            tree_depth,
            tokens
         ),
         PPG_CS_ACTION(Pattern)
      );
   }
   }
   }
   }
   
   ppg_cs_compile();
   
   const int n_attempts = 1000;
   
   for(int i = 0; i < n_attempts; ++i) {

      PPG_CS_PROCESS_ON_OFF(  "a b c z", 
                              PPG_CS_EXPECT_FLUSH("AaBbCcZz")
                              PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_EMF)
                              PPG_CS_EXPECT_NO_ACTIONS
      );
   }
   
PPG_CS_END_TEST
