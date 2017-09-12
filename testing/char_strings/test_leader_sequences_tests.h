// PPG_PATTERN_PRINT_TREE

PPG_CS_PROCESS_STRING(  "A B C c b a FOO oof",
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Foo)
                        )
);

PPG_CS_PROCESS_STRING(  "A B C c b a BAR bar",
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Bar)
                        )
);

PPG_CS_PROCESS_STRING(  "A B C c b a KLAATUBARADANIKTO klaatubaradanikto",
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Klaatubaradan)
                        )
);

PPG_CS_PROCESS_STRING(  "FOO oof",
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Foo)
                        )
);

PPG_CS_PROCESS_STRING(  "BAR bar",
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Bar)
                        )
);

PPG_CS_PROCESS_STRING(  "KLAATUBARADAN klaatubaradan",
                        PPG_CS_EXPECT_EMPTY_FLUSH
                        PPG_CS_EXPECT_NO_EXCEPTIONS
                        PPG_CS_EXPECT_ACTION_SERIES(
                           PPG_CS_A(Klaatubaradan)
                        )
);