/* Copyright 2017 Florian Fleissner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "papageno_char_strings.h"
	
enum {
	ppg_cs_layer_0 = 0
};

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
	
	ppg_chord(
		ppg_cs_layer_0,
		PPG_CS_ACTION(Chord_2),
		PPG_INPUTS(
			PPG_CS_CHAR('a'),
			PPG_CS_CHAR('b'),
			PPG_CS_CHAR('d')
		)
	);
	
	ppg_cs_compile();
	
	//***********************************************
	// Check chord 1
	//***********************************************
	
	PPG_CS_PROCESS_STRING("A B C c b a", Chord_1);
	
	PPG_PATTERN_PRINT_TREE
	
	// Allow to release keys and then repress them 
	//
	PPG_CS_PROCESS_STRING("A a A B C c b a", Chord_1);
	
	PPG_PATTERN_PRINT_TREE
	
	PPG_CS_PROCESS_STRING("A B b C B c b a", Chord_1);
	
	PPG_PATTERN_PRINT_TREE
	
	// Do not allow keys being released
	//
	PPG_CS_PROCESS_STRING("A B b C c a |", Exception_Timeout);
	
	PPG_PATTERN_PRINT_TREE
	
	
	//***********************************************
	// Check chord 2
	//***********************************************
	
	PPG_CS_PROCESS_STRING("A B D d b a", Chord_2);
	
	// Allow to release keys and then repress them 
	//
	PPG_CS_PROCESS_STRING("A a A B D d b a", Chord_2);
	PPG_CS_PROCESS_STRING("A B b D B c b a", Chord_2);
	
	// Do not allow keys being released
	//
	PPG_CS_PROCESS_STRING("A B b D d a |", Exception_Timeout);
	
// 	PPG_CS_PROCESS_ON_OFF("a a c", Pattern_2);
	
// 	PPG_PATTERN_PRINT_TREE
	
PPG_CS_END_TEST
