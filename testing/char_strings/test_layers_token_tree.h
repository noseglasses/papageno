ppg_pattern(
   ppg_cs_layer_0, /* Layer id */
   PPG_TOKENS(
      PPG_CS_N('a'),
      PPG_CS_N('b'),
      ppg_token_set_action(
         PPG_CS_N('c'),
         PPG_CS_ACTION(Action_1)
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
         PPG_CS_ACTION(Action_2)
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
         PPG_CS_ACTION(Action_3)
      )
   )
);

ppg_pattern(
   ppg_cs_layer_1, /* Layer id */
   PPG_TOKENS(
      PPG_CS_N('a'),
      PPG_CS_N('b'),
      ppg_token_set_action(
         PPG_CS_N('c'),
         PPG_CS_ACTION(Action_4)
      )
   )
); 

// Empty override
//
ppg_pattern(
   ppg_cs_layer_2, /* Layer id */
   PPG_TOKENS(
      PPG_CS_N('a'),
      PPG_CS_N('b'),
      PPG_CS_N('c')
   )
);