ppg_pattern(
   ppg_cs_layer_0, /* Layer id */
   PPG_TOKENS(
      PPG_CS_N('a'),
      PPG_CS_N('b'),
      ppg_token_set_action(
         PPG_CS_N('c'),
         PPG_CS_ACTION(Pattern_1)
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
         PPG_CS_ACTION(Pattern_2)
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
         PPG_CS_ACTION(Pattern_3)
      )
   )
);

ppg_single_note_line(
   ppg_cs_layer_0,
   PPG_CS_ACTION(Single_Note_Line_1),
   PPG_INPUTS(
      PPG_CS_CHAR('r'),
      PPG_CS_CHAR('n'),
      PPG_CS_CHAR('m')
   )
);

ppg_tap_dance(
   ppg_cs_layer_0,
   PPG_CS_CHAR('a'), /* The tap key */
   PPG_TAP_DEFINITIONS(
      PPG_TAP(3, 
         PPG_CS_ACTION(3_Taps)
      ),
      PPG_TAP(5, 
         PPG_CS_ACTION(5_Taps)
      )
   )
);