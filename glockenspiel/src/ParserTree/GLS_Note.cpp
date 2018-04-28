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

#include "ParserTree/GLS_Note.hpp"
#include "Generator/GLS_Prefix.hpp"

#define SP Glockenspiel::Generator::symbolsPrefix()
#define MP Glockenspiel::Generator::macroPrefix()
      
namespace Glockenspiel {
namespace ParserTree {
   
void 
   Note 
      ::generateCCodeInternal(std::ostream &out) const
{
   
   const auto &input = this->getInputPtr();
   out <<
"   __GLS_DI__(super) {\n";

   this->Token::generateCCodeInternal(out);
   out <<
"   },\n"
"   __GLS_DI__(input) " << MP << "GLS_INPUT_INITIALIZE_GLOBAL___" << input->getType().getText()
<< "(" 
<< input->getId().getText() << ", "
<< input->getParameters().getText() << ")\n";
}

} // namespace ParserTree
} // namespace Glockenspiel
