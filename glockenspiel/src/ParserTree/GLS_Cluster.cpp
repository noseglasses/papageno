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
#include "ParserTree/GLS_Cluster.hpp"
#include "Generator/GLS_Prefix.hpp"

namespace Glockenspiel {
namespace ParserTree {
   
#define SP Glockenspiel::Generator::symbolsPrefix()
#define MP Glockenspiel::Generator::macroPrefix()
   
void 
   Cluster
      ::generateCCodeInternal(std::ostream &out) const
{ 
   out <<
"   __GLS_DI__(aggregate) {\n";

   this->Aggregate::generateCCodeInternal(out);
   out <<
"   },\n"
"   __GLS_DI__(member_active_lasting) {\n"
"      __GLS_DI__(bitarray) " << this->getId().getText() << "_member_active_lasting,\n" <<
"      __GLS_DI__(n_bits) " << this->inputs_.size() << "\n" <<
"   },\n"
"   __GLS_DI__(n_lasting) 0\n";
}

void 
   Cluster
      ::generateDependencyCodeInternal(std::ostream &out) const
{ 
   std::size_t n_bits = inputs_.size();
   
   out <<
"PPG_Bitfield_Storage_Type " << this->getId().getText() << "_member_active_lasting[(GLS_NUM_BITS_LEFT(" << n_bits << ") != 0) ? (GLS_NUM_BYTES(" << n_bits << ") + 1) : GLS_NUM_BYTES(" << n_bits << ")]\n"
"   = { 0 };\n"
"\n";

   this->Aggregate::generateDependencyCodeInternal(out);
}

} // namespace ParserTree
} // namespace Glockenspiel
