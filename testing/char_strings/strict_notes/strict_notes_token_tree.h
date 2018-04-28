ppg_pattern(
   ppg_cs_layer_0, /* Layer id */
   PPG_TOKENS(
      PPG_CS_N_A('A'),
      PPG_CS_N_D('a'),
      PPG_CS_N_A('B'),
      PPG_CS_N_A('C'),
      PPG_CS_N_D('b'),
      ppg_token_set_action(
         PPG_CS_N_D('c'),
         PPG_CS_ACTION(Pattern)
      )
   )
);