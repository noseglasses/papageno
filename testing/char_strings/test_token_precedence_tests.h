//***********************************************
// Check precedence of chords against single note lines 
//***********************************************

PPG_CS_PROCESS_STRING(  "A B C c b a", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Chord_1)
                        )
);

//***********************************************
// Check precedence of single note lines against
// clusters
//***********************************************
   
PPG_CS_PROCESS_STRING(  "D E F e f d", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Single_Note_Line_4)
                        )
);
   
//***********************************************
// Check precedence of chords against
// clusters
//***********************************************
   
PPG_CS_PROCESS_STRING(  "X Y Z y x z", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Chord_2)
                        )
); 

//***********************************************
// Check precedence of clusters against
// chords if the cluster is associated with a higher layer
//***********************************************

ppg_global_set_layer(ppg_cs_layer_1);

// Allow to release keys and then repress them 
//
PPG_CS_PROCESS_STRING(  "M N O m n o", 
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Cluster_3)
                        )
);