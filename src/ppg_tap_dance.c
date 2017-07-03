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

PPG_Token ppg_tap_dance(	
							uint8_t layer,
							PPG_Input input,
							uint8_t default_action_flags,
							uint8_t n_tap_definitions,
							PPG_Tap_Definition tap_definitions[])
{
	PPG_PRINTF("Adding tap dance\n");
	
	ppg_init();
	
	int n_taps = 0;
	for (uint8_t i = 0; i < n_tap_definitions; i++) { 
		
		if(tap_definitions[i].tap_count > n_taps) {
			n_taps = tap_definitions[i].tap_count;
		}
	}
	
	if(n_taps == 0) { return NULL; }
		
	PPG_Token tokens[n_taps];
	
	for (int i = 0; i < n_taps; i++) {
		
		PPG_Token__ *new_note = (PPG_Token__*)ppg_create_note(input);
			new_note->action.flags = default_action_flags;
		
		tokens[i] = new_note;
	}
	
	for (uint8_t i = 0; i < n_tap_definitions; i++) {
		
		ppg_token_store_action(
					tokens[tap_definitions[i].tap_count - 1], 
					tap_definitions[i].action);
	}
			
	PPG_Token__ *leafToken 
		= ppg_pattern_from_list(layer, n_taps, tokens);
	
	return leafToken;
}