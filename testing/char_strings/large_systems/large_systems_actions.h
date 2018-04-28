// This test might take too long
// especially using valgrind. Thus we
// disable timeout to be on the safe side.
//
ppg_global_set_timeout_enabled(false);

const int tree_depth = PAPAGENO_TEST__TREE_DEPTH;
(void)tree_depth;

const int n_chars = PAPAGENO_TEST__N_CHARS;
(void)n_chars;

PPG_CS_REGISTER_ACTION(Pattern)