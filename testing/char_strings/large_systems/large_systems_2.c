/* Copyright 2017 noseglasses <shinynoseglasses@gmail.com>
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

#include <math.h>

#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>
#include <boost/preprocessor/repetition/for.hpp>
#include <boost/preprocessor/tuple/elem.hpp> 

#include <stdio.h>
   
enum {
   ppg_cs_layer_0 = 0
};

int main(int argc, char **argv)
{
#undef PAPAGENO_TEST__TREE_DEPTH
#define PAPAGENO_TEST__TREE_DEPTH 1
#include "test_large_systems_2_aux.h"
   
#undef PAPAGENO_TEST__TREE_DEPTH
#define PAPAGENO_TEST__TREE_DEPTH 2
#include "test_large_systems_2_aux.h"
   
#undef PAPAGENO_TEST__TREE_DEPTH
#define PAPAGENO_TEST__TREE_DEPTH 3
#include "test_large_systems_2_aux.h"
   
#undef PAPAGENO_TEST__TREE_DEPTH
#define PAPAGENO_TEST__TREE_DEPTH 4
#include "test_large_systems_2_aux.h"
   
#undef PAPAGENO_TEST__TREE_DEPTH
#define PAPAGENO_TEST__TREE_DEPTH 5
#include "test_large_systems_2_aux.h"
   
#undef PAPAGENO_TEST__TREE_DEPTH
#define PAPAGENO_TEST__TREE_DEPTH 6
#include "test_large_systems_2_aux.h"
   
#undef PAPAGENO_TEST__TREE_DEPTH
#define PAPAGENO_TEST__TREE_DEPTH 7
#include "test_large_systems_2_aux.h"
   
#undef PAPAGENO_TEST__TREE_DEPTH
#define PAPAGENO_TEST__TREE_DEPTH 8
#include "test_large_systems_2_aux.h"
 
   return 0;
}
