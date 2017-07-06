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

#include "papageno.h"
#include "papageno_char_strings.h"
	
enum {
	ppg_cs_layer_0 = 0,
	ppg_cs_layer_1,
	ppg_cs_layer_2
};

enum {
	PPG_CS_R_Pattern_1 = 1,
	PPG_CS_R_Pattern_2,
	PPG_CS_R_Pattern_3,
	
	PPG_CS_R_Single_Note_Line_1,
	
	PPG_CS_R_3_Taps,
	PPG_CS_R_5_Taps
};	
	
PPG_CS_START_TEST
	
	ppg_pattern(
		ppg_cs_layer_0, /* Layer id */
		PPG_TOKENS(
			PPG_CS_N('a'),
			PPG_CS_N('b'),
			ppg_token_set_action(
				PPG_CS_N('c'),
				PPG_CS_ACTION(PPG_CS_R_Pattern_1)
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
				PPG_CS_ACTION(PPG_CS_R_Pattern_2)
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
				PPG_CS_ACTION(PPG_CS_R_Pattern_3)
			)
		)
	);
	
	ppg_single_note_line(
		ppg_cs_layer_0,
		PPG_CS_ACTION(PPG_CS_R_Single_Note_Line_1),
		PPG_INPUTS(
			PPG_CS_CHAR('r'),
			PPG_CS_CHAR('n'),
			PPG_CS_CHAR('m')
		)
	);
	
	ppg_tap_dance(
		ppg_cs_layer_0,
		PPG_CS_CHAR('a'), /* The tap key */
		PPG_Action_Fall_Back,
							/* Use PPG_Action_Fall_Back if you want fall back, 
								e.g. if something happens after three and five keypresses
								and you want to fall back to the three keypress action
								if only four keypresses arrived before timeout. */
		PPG_TAP_DEFINITIONS(
			PPG_TAP(3, 
				PPG_CS_ACTION(PPG_CS_R_3_Taps)
			),
			PPG_TAP(5, 
				PPG_CS_ACTION(PPG_CS_R_5_Taps)
			)
		)
	);
	
	ppg_cs_compile();
	
	PPG_CS_PROCESS_ON_OFF("a b c", PPG_CS_R_Pattern_1);
	PPG_CS_PROCESS_ON_OFF("a a c", PPG_CS_R_Pattern_2);
	PPG_CS_PROCESS_ON_OFF("a b d", PPG_CS_R_Pattern_3);
	
	PPG_CS_PROCESS_ON_OFF("r n m", PPG_CS_R_Single_Note_Line_1);
	
	PPG_CS_PROCESS_ON_OFF("a a a |", PPG_CS_R_3_Taps);
	
	PPG_CS_PROCESS_ON_OFF("a a a a |", PPG_CS_R_3_Taps);
	
	PPG_CS_PROCESS_ON_OFF("a a a a a |", PPG_CS_R_5_Taps);
	
PPG_CS_END_TEST
