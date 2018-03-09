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

#define PPG_CS_NO_COMPRESSION
#define PPG_CS_NO_AUTOMATIC_CONTEXT_HANDLING

#include "papageno_char_strings.h"

#include "default_includes.h"
   
#include "default_layers.h"

#include "test_compiler_run_output.c"

PPG_CS_START_TEST
   
   /*
   papageno_start
   
   action: Cluster_1
   action: Cluster_2
   
   input: a := 'a'
   input: b := 'b'
   input: c := 'c'
   input: d := 'd'
   
   {a, b, c} : Cluster_1
   {a, b, d} : Cluster_2
   
   papageno_end
   */
   
   PPG_LOCAL_INITIALIZATION
   
   papageno_initialize_context();
   
   PPG_CS_PREPARE_CONTEXT
   
   #ifndef PPG_CS_SUPPRESS_TESTS
   
   #include "test_clusters_tests.h"
   
   #endif // ifndef PPG_CS_SUPPRESS_TESTS
   
PPG_CS_END_TEST
