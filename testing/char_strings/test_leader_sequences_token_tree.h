PPG_Token leader_token
   = 
ppg_token_set_flags(
   ppg_chord(
      ppg_cs_layer_0,
      PPG_ACTION_NOOP,
      PPG_INPUTS(
         PPG_CS_CHAR('a'),
         PPG_CS_CHAR('b'),
         PPG_CS_CHAR('c')
      )
   ),
   
   // Without the following the chord would consume all consequent
   // releases and re-pressing of keys involved
   //
   PPG_Chord_Flags_Disallow_Input_Deactivation
);

ppg_alphabetic_leader_sequences(
   ppg_cs_layer_0, // layer
   leader_token,  // The leader input, used zero if no leader key is 
                  // necessary
   3,
   (PPG_Leader_Functions) {
      .retreive_string = ppg_cs_retreive_string,
      .retreive_action = ppg_cs_retreive_action,
      .input_from_char = ppg_cs_input_from_alphabetic_character
   },
   true // Allow fallback, i.e. only require input until a sequence
         // can be uniquely identified
);

ppg_alphabetic_leader_sequences(
   ppg_cs_layer_0, // layer
   NULL,  // The leader input, used zero if no leader key is 
                  // necessary
   3,
   (PPG_Leader_Functions) {
      .retreive_string = ppg_cs_retreive_string,
      .retreive_action = ppg_cs_retreive_action,
      .input_from_char = ppg_cs_input_from_alphabetic_character
   },
   true // Allow fallback, i.e. only require input until a sequence
         // can be uniquely identified
);
