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

#include <math.h>

#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>
#include <boost/preprocessor/repetition/for.hpp>
#include <boost/preprocessor/tuple/elem.hpp> 

#include <stdio.h>
   
enum {
   ppg_cs_layer_0 = 0
};

PPG_CS_START_TEST

   // This test might take too long
   // especially using valgrind. Thus we
   // disable timeout to be on the safe side.
   //
   ppg_global_set_timeout_enabled(false);

	const int tree_depth = PAPAGENO_TEST__TREE_DEPTH;
   (void)tree_depth;
   
	const int n_chars = PAPAGENO_TEST__N_CHARS;
   (void)n_chars;

   PPG_CS_REGISTER_ACTION(Pattern)
   
	PPG_Token tokens[tree_depth];
   
   #define GLUE_AUX(S1, S2) S1##S2
   #define GLUE(S1, S2) GLUE_AUX(S1, S2)
   
   #define PRED(r, state) \
   BOOST_PP_NOT_EQUAL( \
      BOOST_PP_TUPLE_ELEM(2, 0, state), \
      BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2, 1, state)) \
   ) \
   /**/

   #define OP(r, state) \
      ( \
         BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2, 0, state)), \
         BOOST_PP_TUPLE_ELEM(2, 1, state) \
      ) \
      /**/
      
   #define COUNTER_VAR(state) GLUE(l_,BOOST_PP_TUPLE_ELEM(2, 0, state))

   #define LOOP_START(r, state) \
__NL__   for(int COUNTER_VAR(state) = 0; \
__NL__               COUNTER_VAR(state) < n_chars; ++COUNTER_VAR(state)) { \
__NL__      \
__NL__      tokens[BOOST_PP_TUPLE_ELEM(2, 0, state) - 1] \
__NL__           = PPG_CS_N(('a' + COUNTER_VAR(state)));
     
   #define LOOP_END(r, state) }
   
   BOOST_PP_FOR((1, PAPAGENO_TEST__TREE_DEPTH), PRED, OP, LOOP_START)
   
      ppg_token_set_action(
         ppg_pattern(
            ppg_cs_layer_0, /* Layer id */
            tree_depth,
            tokens
         ),
         PPG_CS_ACTION(Pattern)
      );

   BOOST_PP_FOR((1, PAPAGENO_TEST__TREE_DEPTH), PRED, OP, LOOP_END)
   
   ppg_cs_compile();
   
   const int n_attempts = 10;
   
   char test_string[PAPAGENO_TEST__TREE_DEPTH + 1];
   char expect_flush[2*PAPAGENO_TEST__TREE_DEPTH + 1];
   
   for(int i = 0; i < PAPAGENO_TEST__TREE_DEPTH - 1; ++i) {
      
      char the_char = 'a' + PAPAGENO_TEST__N_CHARS - 1;
      
      test_string[i] = the_char;
      
      expect_flush[2*i] = toupper(the_char);
      expect_flush[2*i + 1] = tolower(the_char);
   }
   
   const int last = PAPAGENO_TEST__TREE_DEPTH - 1;
   
   #if PAPAGENO_TEST__FAIL
   test_string[last] = 'z';
   expect_flush[2*last] = toupper('z');
   expect_flush[2*last + 1] = tolower('z');
   expect_flush[2*PAPAGENO_TEST__TREE_DEPTH] = '\0';
   #else
   test_string[last] = 'a' + PAPAGENO_TEST__N_CHARS - 1;
   expect_flush[0] = '\0';
   #endif
   
   test_string[PAPAGENO_TEST__TREE_DEPTH] = '\0';
   
   for(int i = 0; i < n_attempts; ++i) {

      #if PAPAGENO_TEST__FAIL
      PPG_CS_PROCESS_ON_OFF(  test_string, 
                              PPG_CS_EXPECT_FLUSH(expect_flush)
                              PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_EMF)
                              PPG_CS_EXPECT_NO_ACTIONS
      );
      #else
      PPG_CS_PROCESS_ON_OFF(  test_string, 
                              PPG_CS_EXPECT_FLUSH(expect_flush)
                              PPG_CS_EXPECT_NO_EXCEPTIONS
                              PPG_CS_EXPECT_ACTION_SERIES(PPG_CS_A(Pattern))
      );
      #endif
   }
   
   #if PPG_HAVE_STATISTICS
   PPG_Statistics stat;
   ppg_statistics_get(&stat);
   
   long unsigned n_nodes = (long unsigned)(
      (pow(n_chars, tree_depth + 1) - n_chars)/(n_chars - 1)
                                 );
   
   printf("Searched for \'%s\'\n", test_string);
   
   printf("# nodes in tree: %lu\n", n_nodes);
   
   printf("# attempts: %u\n", n_attempts);
   
   printf("# nodes visited: %u, avg. %f\n",
                        stat.n_nodes_visited,
                        (double)stat.n_nodes_visited/n_attempts);
   
   printf("# tokens checked: %u, avg. %f\n", 
                        stat.n_token_checks,
                        (double)stat.n_token_checks/n_attempts);
   
   printf("# furcations met: %u, avg. %f\n",
                        stat.n_furcations,
                        (double)stat.n_furcations/n_attempts);
   
   printf("# branch reversions: %u, avg. %f\n",
                        stat.n_reversions,
                        (double)stat.n_reversions/n_attempts);
   #endif
   
PPG_CS_END_TEST
