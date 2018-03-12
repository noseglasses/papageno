PPG_CS_PROCESS_STRING(  "A a B C b c",  
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Pattern)
                        )
);

PPG_CS_PROCESS_STRING(  "A B a C b c",  
                        PPG_CS_EXPECT_FLUSH("ABaCbc")
                        PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_EMF)
                        PPG_CS_EXPECT_NO_ACTIONS
);


PPG_CS_PROCESS_ON_OFF(  "a b c |",  
                        PPG_CS_EXPECT_FLUSH("AaBbCc")
                        PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_EMF)
                        PPG_CS_EXPECT_NO_ACTIONS
);