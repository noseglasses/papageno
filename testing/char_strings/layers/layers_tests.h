PPG_CS_PROCESS_ON_OFF(  "a b c", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Action_1)
                        )
);

PPG_CS_PROCESS_ON_OFF(  "a a c", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Action_2)
                        )
);

PPG_CS_PROCESS_ON_OFF(  "a b d", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Action_3)
                        )
);

// Layer switches abort processing. Assert that aborting
// works correctly.
//
// Note: Here no abortion should take place as no processing
//       is on the way.
//
automatically_reset_testing_system = false;

PPG_CS_PROCESS_ON_OFF(  "a b", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_NO_ACTIONS
);

ppg_global_set_layer(ppg_cs_layer_1);

PPG_CS_CHECK_NO_PROCESS(
                        PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_EA)
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_ACTIONS
)
   
PPG_CS_PROCESS_ON_OFF(  "a b c", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Action_4)
                        )
);
   
ppg_global_set_layer(ppg_cs_layer_2);
      
// Note: Here no abortion should take place as no processing
//       is on the way.
//
PPG_CS_CHECK_NO_PROCESS(
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_ACTIONS
)

PPG_CS_PROCESS_ON_OFF(  "a b c", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_NO_ACTIONS
);

PPG_CS_PROCESS_ON_OFF(  "a a c", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Action_2)
                        )
);