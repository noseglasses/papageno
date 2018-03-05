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

#include "Generator/PPG_Global.hpp"

namespace Papageno {
namespace Generator {
   
void generateFileHeader(std::ostream &out) {
   
   out <<
"#include \"detail/ppg_context_detail.h\"\n"
"#include \"detail/ppg_note_detail.h\"\n"
"#include \"detail/ppg_chord_detail.h\"\n"
"#include \"detail/ppg_cluster_detail.h\"\n"
"\n";
}
   
void generateGlobalActionInformation(std::ostream &out)
{
   auto actionsByType = Action::getActionsByType();
   
   for(const auto &abtEntry: actionsByType) {
      const auto &tag = abtEntry.first;
      
      out << 
"#ifndef PPG_ACTION_MAP_" << tag << "\n";
      out <<
"#define PPG_ACTION_MAP_" << tag << "(...) __VA_ARGS__\n";
      out <<
"#endif\n"
"\n";
   
      out <<
"#ifndef PPG_ACTION_INITIALIZATION_" << tag << "\n";
      out << 
"#define PPG_ACTION_INITIALIZATION_" << tag << "(...) __VA_ARGS__\n";
      out <<
"#endif\n"
"\n";

      out <<
"#define PPG_ACTIONS___" << tag << "(OP) \\\n";

      for(const auto &actionPtr: abtEntry.second) {
         out <<
"   OP(" << actionPtr->getId() << ")\\\n"
      }
      
      out <<
"\n"
"\n";
   }
}

void generateGlobalInputInformation(std::ostream &out)
{
   auto inputsByType = Input::getInputsByType();
   
   for(const auto &abtEntry: inputsByType) {
      const auto &tag = abtEntry.first;
      
      out << 
"#ifndef PPG_ACTION_MAP_" << tag << "\n";
      out <<
"#define PPG_ACTION_MAP_" << tag << "(...) __VA_ARGS__\n";
      out <<
"#endif\n"
"\n";
   
      out <<
"#ifndef PPG_ACTION_INITIALIZATION_" << tag << "\n";
      out << 
"#define PPG_ACTION_INITIALIZATION_" << tag << "(...) __VA_ARGS__\n";
      out <<
"#endif\n"
"\n";

      out <<
"#define PPG_ACTIONS___" << tag << "(OP) \\\n";

      for(const auto &inputPtr: abtEntry.second) {
         out <<
"   OP(" << inputPtr->getId() << ")\\\n"
      }
      
      out <<
"\n"
"\n";
   }
}

static void outputInformationOfDefinition(std::ostream &out, const ParserTree::Node &node)
{
   out <<
"// Line " << node.getSourceLine() << ", col " << node.getSourceColumn() << "\n";
   out <<
"//\n";
}

void outputToken(std::ostream &out, const Token &token)
{
   for(const auto &childTokenPtr: token.getChildren()) {
      outputToken(out, *childTokenPtr);
   }
   token.generateCCode(out);
}

void generateGlobalContext(std::ostream &out)
{
   // Output all actions
   //
   for(const auto &actionEntry: Action::getActions()) {
      
      const auto &action = *actionEntry.second;
      
      outputInformationOfDefinition(out, action);
      
      out <<
"PPG_Action " << action.getId() << " = PPG_ACTION_INITIALIZATION_" << action.getType()
   << "(" << action.getParameters() << ");\n\n";
   }
   
   // Output all inputs
   //
   for(const auto &inputEntry: Input::getInputs()) {
      
      const auto &input = *inputEntry.second;
      
      outputInformationOfDefinition(out, input);
      
      out <<
"PPG_Input " << input.getId() << " = PPG_INPUT_INITIALIZATION_" << input.getType()
   << "(" << input.getParameters() << ");\n\n";
   }
   
   auto root = Pattern::getTreeRoot();
   
   // Recursively output token tree
   //
   outputToken(*root);
   
   out <<
"PPG_Context context = (PPG_Context) {\n"
"   .pattern_root = &" << root->getId() << "\n"
"};\n\n";
}
   
void generateInitializationFunction(std::ostream &out)
{
   out <<
"void papageno_initialize_context()\n"
"{\n";
   out <<
"   ppg_global_initialize_context_static_tree(&context);\n"
"   ppg_context = &context;\n"
"}\n\n";
}

void generateGlobal(const std::string &outputFile)
{
   std::ofstream out(outputFile);
   
   generateFileHeader(outputFile);
   
   // Generate global action information
   generateGlobalActionInformation(outputFile);
   
   generateGlobalInputInformation(outputFile);
   
   generateGlobalContext(outputFile);
   
   generateInitializationFunction(outputFile);
}

} // namespace ParserTree
} // namespace Papageno
