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
#include "Settings/PPG_Settings.hpp"
#include "Generator/PPG_Global.hpp"
#include "ParserTree/PPG_Action.hpp"
#include "ParserTree/PPG_Pattern.hpp"
#include "ParserTree/PPG_Input.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char **argv)
{
   if(cmdline_parser(argc, argv, &Glockenspiel::commandLineArgs) != 0) {
      exit(1);
   }
   
   using namespace Glockenspiel;
   
   Settings::init();
   
   try {
      for(int i = 0; i < Glockenspiel::commandLineArgs.source_filename_given; ++i) {
         Parser::generateTree(Glockenspiel::commandLineArgs.source_filename_arg[i]);
      }
      
      if(Settings::join_duplicate_actions || Settings::join_duplicate_entities) {
         ParserTree::Action::joinDuplicateEntries();
      }
      
      if(Settings::join_duplicate_inputs || Settings::join_duplicate_entities) {
         ParserTree::Input::joinDuplicateEntries();
      }
      
      if(Settings::join_note_sequences) {
         ParserTree::Pattern::performSequenceReplacement();
      }
      
      Generator::generateGlobal(Glockenspiel::commandLineArgs.output_filename_arg);
   }
   catch(const std::runtime_error &e) {
      std::cerr << e.what() <<std::endl;
      return 1;
   }
   
   return 0;
}
