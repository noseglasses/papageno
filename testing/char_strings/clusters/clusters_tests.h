//***********************************************
// Check cluster 1
//***********************************************

PPG_CS_PROCESS_STRING(  "A B C c b a", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Cluster_1)
                        )
);

PPG_PATTERN_PRINT_TREE

// Allow to release keys and then repress them 
//
PPG_CS_PROCESS_STRING(  "A a A B C c b a", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Cluster_1)
                        )
);

PPG_PATTERN_PRINT_TREE

PPG_CS_PROCESS_STRING(  "A B b C B c b a", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Cluster_1)
                        )
);

PPG_PATTERN_PRINT_TREE

// Do not allow keys being released
//
PPG_CS_PROCESS_STRING(  "A B b C c a |", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Cluster_1)
                        )
);

PPG_PATTERN_PRINT_TREE

// Check for match fails
//
PPG_CS_PROCESS_STRING(  "A B E C c a |", 
                        PPG_CS_EXPECT_FLUSH("ABECca")
                        PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_EMF | PPG_CS_ET)
                        PPG_CS_EXPECT_NO_ACTIONS
);

PPG_PATTERN_PRINT_TREE

//***********************************************
// Check cluster 2
//***********************************************

PPG_CS_PROCESS_STRING(  "A B D d b a", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Cluster_2)
                        )
);

// Allow to release keys and then repress them 
//
PPG_CS_PROCESS_STRING(  "A a A B D d b a",
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Cluster_2)
                        )
);

PPG_CS_PROCESS_STRING(  "A B b D B d b a",
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Cluster_2)
                        )
);

// Assert that it is possible to release cluster chords
// and still gain a match
//
PPG_CS_PROCESS_STRING(  "A B b D d a |", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Cluster_2)
                        )
);
//    PPG_PATTERN_PRINT_TREE