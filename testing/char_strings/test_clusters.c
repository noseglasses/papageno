/* Copyright 2017 Florian Fleissner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "papageno_char_strings.h"
	
enum {
	ppg_cs_layer_0 = 0
};

PPG_CS_START_TEST

	PPG_CS_REGISTER_ACTION(Cluster_1)
	PPG_CS_REGISTER_ACTION(Cluster_2)
	
	ppg_cluster(
		ppg_cs_layer_0,
		PPG_CS_ACTION(Cluster_1),
		PPG_INPUTS(
			PPG_CS_CHAR('a'),
			PPG_CS_CHAR('b'),
			PPG_CS_CHAR('c')
		)
	);
	
	ppg_cluster(
		ppg_cs_layer_0,
		PPG_CS_ACTION(Cluster_2),
		PPG_INPUTS(
			PPG_CS_CHAR('a'),
			PPG_CS_CHAR('b'),
			PPG_CS_CHAR('d')
		)
	);
	
	ppg_cs_compile();
	
	//***********************************************
	// Check cluster 1
	//***********************************************
	
	PPG_CS_PROCESS_STRING(	"A B C c b a", 
									PPG_CS_EXPECT_EMPTY_FLUSH
									PPG_CS_EXPECT_NO_EXCEPTIONS
									PPG_CS_EXPECT_ACTION_SERIES(
										PPG_CS_A(Cluster_1)
									)
	);
	
	PPG_PATTERN_PRINT_TREE
	
	// Allow to release keys and then repress them 
	//
	PPG_CS_PROCESS_STRING(	"A a A B C c b a", 
									PPG_CS_EXPECT_EMPTY_FLUSH
									PPG_CS_EXPECT_NO_EXCEPTIONS
									PPG_CS_EXPECT_ACTION_SERIES(
										PPG_CS_A(Cluster_1)
									)
	);
	
	PPG_PATTERN_PRINT_TREE
	
	PPG_CS_PROCESS_STRING(	"A B b C B c b a", 
									PPG_CS_EXPECT_EMPTY_FLUSH
									PPG_CS_EXPECT_NO_EXCEPTIONS
									PPG_CS_EXPECT_ACTION_SERIES(
										PPG_CS_A(Cluster_1)
									)
	);
	
	PPG_PATTERN_PRINT_TREE
	
	// Do not allow keys being released
	//
	PPG_CS_PROCESS_STRING(	"A B b C c a |", 
									PPG_CS_EXPECT_EMPTY_FLUSH
									PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_ET)
									PPG_CS_EXPECT_ACTION_SERIES(
										PPG_CS_A(Cluster_1)
									)
	);
	
	PPG_PATTERN_PRINT_TREE
	
	// Check for match fails
	//
	PPG_CS_PROCESS_STRING(	"A B E C c a |", 
									PPG_CS_EXPECT_FLUSH("ABECca")
									PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_EMF | PPG_CS_ET)
									PPG_CS_EXPECT_NO_ACTIONS
	);
	
	PPG_PATTERN_PRINT_TREE
	
	//***********************************************
	// Check cluster 2
	//***********************************************
	
	PPG_CS_PROCESS_STRING(	"A B D d b a", 
									PPG_CS_EXPECT_EMPTY_FLUSH
									PPG_CS_EXPECT_NO_EXCEPTIONS
									PPG_CS_EXPECT_ACTION_SERIES(
										PPG_CS_A(Cluster_2)
									)
	);
	
	// Allow to release keys and then repress them 
	//
	PPG_CS_PROCESS_STRING(	"A a A B D d b a",
									PPG_CS_EXPECT_EMPTY_FLUSH
									PPG_CS_EXPECT_NO_EXCEPTIONS
									PPG_CS_EXPECT_ACTION_SERIES(
										PPG_CS_A(Cluster_2)
									)
	);
	
	PPG_CS_PROCESS_STRING(	"A B b D B d b a",
									PPG_CS_EXPECT_EMPTY_FLUSH
									PPG_CS_EXPECT_NO_EXCEPTIONS
									PPG_CS_EXPECT_ACTION_SERIES(
										PPG_CS_A(Cluster_2)
									)
	);
	
	// Do not allow keys being released
	//
	PPG_CS_PROCESS_STRING(	"A B b d D d a |", 
									PPG_CS_EXPECT_EMPTY_FLUSH
									PPG_CS_EXPECT_EXCEPTIONS(PPG_CS_ET)
									PPG_CS_EXPECT_ACTION_SERIES(
										PPG_CS_A(Cluster_2)
									)
	);
// 	PPG_PATTERN_PRINT_TREE
	
PPG_CS_END_TEST
