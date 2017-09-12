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