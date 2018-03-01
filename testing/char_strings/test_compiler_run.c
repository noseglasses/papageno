/* Copyright 2017 noseglasses <shinynoseglasses@gmail.com>
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

#include "/home/itm/fleissne/Scratch/Projects/papageno/testing/char_strings/default_includes.h"
   
#include "/home/itm/fleissne/Scratch/Projects/papageno/testing/char_strings/default_layers.h"

#define PPG_INPUT_MAP_(NAME, CHAR) PPG_CS_CHAR(CHAR)
#define PPG_ACTION_MAP_(NAME) PPG_CS_ACTION(NAME)
#define PPG_ACTION_INITIALIZATION_(NAME) PPG_CS_REGISTER_ACTION(NAME)

#include "test_compiler_run_output.c"

PPG_CS_START_TEST
   
   /*
   papageno_start
   
   action: C1
   action: C2
   
   input: a === 'a'
   input: b === 'b'
   input: c === 'c'
   input: d === 'd'
   
   {a, b, c} : C1
   {a, b, d} : C2
   
   papageno_end
   */
      
   generate_tree(ccontext);
   compress_tree(ccontext);
   
PPG_CS_END_TEST
