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
#include "ParserTree/PPG_Cluster.hpp"

namespace Papageno {
namespace ParserTree {
   
void 
   Cluster
      ::generateCCodeInternal(std::ostream &out) const
{ 
   out <<
"      .aggregate = {\n";

   this->Aggregate::generateCCodeInternal(out);
   out <<
"      },\n"
"      .member_active_lasting = {\n"
"         .n_bits = " << this->children_.size() << ",\n" <<
"         .bitarray = " << this->getId().getText() << "_member_active_lasting\n" <<
"      },\n"
"      .n_lasting = 0\n";
}

void 
   Cluster
      ::generateDependencyCodeInternal(std::ostream &out) const
{ 
   std::size_t n_bits = inputs_.size();
   
   out <<
"PPG_Bitfield_Storage_Type " << this->getId().getText() << "_member_active_lasting[(PPG_NUM_BITS_LEFT(" << n_bits << ") != 0) ? (PPG_NUM_BYTES(" << n_bits << ") + 1) : PPG_NUM_BYTES(" << n_bits << ")]\n"
"   = { 0 };\n"
"\n";

   this->Aggregate::generateDependencyCodeInternal(out);
}

} // namespace ParserTree
} // namespace Papageno
