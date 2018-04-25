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

#include "Parser/GLS_Parser.hpp"
#include "Parser/GLS_Parser.yacc.hpp"
#include "CommandLine/GLS_CommandLine.hpp"
#include "Settings/GLS_Settings.hpp"
#include "Settings/GLS_Defaults.hpp"
#include "Generator/GLS_Global.hpp"
#include "ParserTree/GLS_Action.hpp"
#include "ParserTree/GLS_Pattern.hpp"
#include "ParserTree/GLS_Input.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char **argv)
{
   if(cmdline_parser(argc, argv, &Glockenspiel::commandLineArgs) != 0) {
      exit(1);
   }
   
   using namespace Glockenspiel;
   
   settings.init();
   defaults.init();
   
   try {
      for(int i = 0; i < Glockenspiel::commandLineArgs.source_filename_given; ++i) {
         Parser::generateTree(Glockenspiel::commandLineArgs.source_filename_arg[i]);
      }
      
      if(settings.join_duplicate_actions && settings.join_duplicate_entities) {
         ParserTree::Action::joinDuplicateEntries();
      }
      
      if(settings.join_duplicate_inputs && settings.join_duplicate_entities) {
         ParserTree::Input::joinDuplicateEntries();
      }
      
      if(settings.join_note_sequences) {
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
