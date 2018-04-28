PPG_CS_PROCESS_ON_OFF(  "a b c", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Action)
                        )
);

PPG_CS_PROCESS_ON_OFF(  "a z c |", 
                        PPG_CS_EXPECT_FLUSH("AazCc") 
                        PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_EA | PPG_CS_EMF)
                        PPG_CS_EXPECT_NO_ACTIONS
);