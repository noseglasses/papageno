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

	PPG_CS_REGISTER_ACTION(Fallback_Action)
	PPG_CS_REGISTER_ACTION(Actual_Action)
	
	ppg_pattern(
		ppg_cs_layer_0, /* Layer id */
		PPG_TOKENS(
			PPG_CS_N('a'),
			ppg_token_set_action_flags(
				ppg_token_set_action(
					PPG_CS_N('b'),
					PPG_CS_ACTION(Fallback_Action)
				),
				PPG_Action_Fallback
			),
			ppg_token_set_action_flags(
				ppg_token_set_action(
					PPG_CS_N('c'),
					PPG_CS_ACTION(Actual_Action)
				),
				PPG_Action_Fallback
			)
		)
	);
	
	ppg_cs_compile();
	
	PPG_CS_PROCESS_ON_OFF(	"a b c", 
									PPG_CS_EXPECT_EMPTY_FLUSH
									PPG_CS_EXPECT_NO_EXCEPTIONS
									PPG_CS_EXPECT_ACTION_SERIES(
										PPG_CS_A(Fallback_Action),
										PPG_CS_A(Actual_Action)
									)
	);
	
	PPG_CS_PROCESS_ON_OFF(	"a b |", 
									PPG_CS_EXPECT_EMPTY_FLUSH
									PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_ET)
									PPG_CS_EXPECT_ACTION_SERIES(
										PPG_CS_A(Fallback_Action)
									)
	);
	
PPG_CS_END_TEST
