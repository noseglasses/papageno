PPG_CS_PROCESS_ON_OFF(  "a b c", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Pattern_1)
                        )
);

//PPG_PATTERN_PRINT_TREE

PPG_CS_PROCESS_ON_OFF(  "a a c", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Pattern_2)
                        )
);

PPG_CS_PROCESS_ON_OFF(  "a b d", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Pattern_3)
                        )
);

PPG_CS_PROCESS_ON_OFF(  "r n m", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Single_Note_Line_1)
                        )
);

PPG_CS_PROCESS_ON_OFF(  "a a a |", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_ET)
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(3_Taps)
                        )
);

PPG_PATTERN_PRINT_TREE

PPG_CS_PROCESS_ON_OFF(  "a a a a |", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_ET)
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(3_Taps)
                        )
);

PPG_CS_PROCESS_ON_OFF(  "a a a a a |",  
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(5_Taps)
                        )
);