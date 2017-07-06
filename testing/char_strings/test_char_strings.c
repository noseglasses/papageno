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

#include <unistd.h>

#define PPG_CS_N(CHAR) \
	ppg_note_create(PPG_CS_CHAR(CHAR))
	
enum {
	ppg_cs_layer_0 = 0,
	ppg_cs_layer_1,
	ppg_cs_layer_2
};

int main(int argc, char **argv)
{
	PPG_CS_INIT
	
	ppg_global_set_abort_trigger(PPG_CS_CHAR('z'));
	
	int timeout_ms = 200;
	
	ppg_cs_set_timeout_ms(timeout_ms);
	
	ppg_pattern(
		ppg_cs_layer_0, /* Layer id */
		PPG_TOKENS(
			PPG_CS_N('a'),
			PPG_CS_N('b'),
			ppg_token_set_action(
				PPG_CS_N('c'),
				PPG_CS_ACTION_STRING("pattern_1")
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
				PPG_CS_ACTION_STRING("pattern_2")
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
				PPG_CS_ACTION_STRING("pattern_3")
			)
		)
	);
	
	ppg_single_note_line(
		ppg_cs_layer_0,
		PPG_CS_ACTION_STRING("single note line 1"),
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
				PPG_CS_ACTION_STRING("three taps")
			),
			PPG_TAP(5, 
				PPG_CS_ACTION_STRING("five taps")
			)
		)
	);
	
	ppg_global_compile();
	
	ppg_cs_process_events_from_string("AaBbCc");
	ppg_cs_process_events_from_string("AaAaCc");
	ppg_cs_process_events_from_string("AaBbDd");
	
	ppg_cs_process_events_from_string("RrNnMm");
	
	ppg_cs_process_events_from_string("AaAaAa");
	
	usleep(2*timeout_ms);
   ppg_timeout_check();
	
	ppg_cs_process_events_from_string("AaAaAaAa");
	
	usleep(2*timeout_ms);
   ppg_timeout_check();	
	
	ppg_cs_process_events_from_string("AaAaAaAaAa");
	
	usleep(2*timeout_ms);
   ppg_timeout_check();	
	
	return 0;
}
