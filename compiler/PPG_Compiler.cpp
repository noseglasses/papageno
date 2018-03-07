/* Copyright 2018 noseglasses <shinynoseglasses@gmail.com>
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

#include "Parser/PPG_Parser.hpp"
#include "Parser/PPG_Parser.yacc.hpp"
#include "CommandLine/PPG_CommandLine.hpp"
#include "Generator/PPG_Global.hpp"

#include <iostream>
#include <stdexcept>

struct gengetopt_args_info ai;

int main(int argc, char **argv)
{
   if(cmdline_parser(argc, argv, &ai) != 0) {
      exit(1);
   }
   
   try {
      for(int i = 0; i < ai.source_filename_given; ++i) {
         Papageno::Parser::generateTree(ai.source_filename_arg[i]);
      }
      
      Papageno::Generator::generateGlobal(ai.output_filename_arg);
   }
   catch(const std::runtime_error &e) {
      std::cerr << e.what() <<std::endl;
      return 1;
   }
   
   return 0;
}
