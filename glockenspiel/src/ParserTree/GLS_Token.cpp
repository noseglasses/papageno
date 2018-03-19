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

#include "ParserTree/GLS_Token.hpp"
#include "Generator/GLS_Prefix.hpp"

#define SP Glockenspiel::Generator::symbolsPrefix()
#define MP Glockenspiel::Generator::macroPrefix()

namespace Glockenspiel {
namespace ParserTree {
   
Parser::Token                    Token::curLayer_("0");

void 
   Token 
      ::generateCCode(std::ostream &out) const 
{
   this->generateDependencyCodeInternal(out);

   // Output children
   //
   if(!children_.empty()) {
      out << 
"PPG_Token__ *" << SP << this->getId().getText() << "_children[" << children_.size() << "] = {\n";

      for(int i = 0; i < children_.size(); ++i) {
         out <<
"   (PPG_Token__*)&" << SP << children_[i]->getId().getText();
         if(i < children_.size() - 1) {
            out << ",";
         }
         out << "\n";
      }
      out <<
"};\n\n";
   }
      
   this->outputCTokenDeclaration(out);
   
   out << " = {\n";

   this->generateCCodeInternal(out);
   
   out <<
"};\n\n";


   // TODO: Howto set parent ptrs?
}

void  
   Token 
      ::outputCTokenDeclaration(std::ostream &out) const 
{
   out << this->getTokenType() << " " << SP << this->getId().getText();
}
      
void 
   Token 
      ::generateCCodeInternal(std::ostream &out) const 
{
   out <<
"      .vtable = " << this->getVTableId() << ",\n";

   if(parent_) {
      out <<
"      .parent = (PPG_Token__*)&" << SP << parent_->getId().getText() << ",\n";
   }
   else {
      out <<
"      .parent = NULL,\n";
   }
   
   if(!children_.empty()) {
      out <<
"      .children = " << SP << this->getId().getText() << "_children,\n" <<
"      .n_allocated_children = sizeof(" << SP << this->getId().getText() << "_children)/sizeof(PPG_Token__*),\n" <<
"      .n_children = sizeof(" << SP << this->getId().getText() << "_children)/sizeof(PPG_Token__*),\n";
   }
   else {
      out <<
"      .children = NULL,\n"
"      .n_allocated_children = 0,\n"
"      .n_children = 0,\n";
   }
   
   if(!action_.getText().empty()) {
      const auto &actionPtr = Action::lookup(action_.getText());
      out <<
"      .action = " << MP << "GLS_ACTION_INITIALIZE_GLOBAL___" << actionPtr->getType().getText() << "("
      << actionPtr->getId().getText();
      if(actionPtr->getParametersDefined()) {
         out << ", " << actionPtr->getParameters().getText();
      }
      out << "), // " 
         << actionPtr->getId().getText() << ": " << actionPtr->getLOD() << "\n";
   }
   else {
      
      out <<
"      .action = { \n"
"         .callback = (PPG_Action_Callback) {\n"
"            .func = NULL,\n"
"            .user_data = NULL\n"
"         }\n"
"      },\n";
   }         
   out <<
"      .misc = (PPG_Misc_Bits) {\n"
"         .state = PPG_Token_Initialized,\n"
"         .flags = 0";
   const auto &flags = this->getFlags().tokenFlags_.string_;
   if(!flags.empty()) {
      out << flags;
   }
   out << ",\n";
"         .action_state = 0,\n";

   out <<
"         .action_flags = PPG_Action_Default";
   const auto &factionFlags = this->getFlags().actionFlags_.string_;
   if(!factionFlags.empty()) {
      out << factionFlags;
   }
   out << "\n"
"      },\n";
   out <<
"      .layer = " << this->layer_.getText() << "\n";
}
      
}
}
