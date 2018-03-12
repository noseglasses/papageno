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

#pragma once

#include "ParserTree/PPG_Aggregate.hpp"

namespace Papageno {
namespace ParserTree {
  
class Cluster : public Aggregate
{
   public:
   
      virtual std::string getNodeType() const override { return "Cluster"; }
      
      virtual std::shared_ptr<Token> clone() const override {
         return std::make_shared<Cluster>(*this);
      }
      
   protected:
      
      virtual std::string getVTableId() const override { return "&ppg_cluster_vtable"; }
      virtual std::string getTokenType() const override { return "PPG_Cluster"; }
      
      virtual std::string getActionPath() const override { return "aggregate.super.action"; }
      
      virtual std::string getEntitiesPath() const override { return "aggregate.inputs"; }
      
      virtual void generateCCodeInternal(std::ostream &out) const override;
      virtual void generateDependencyCodeInternal(std::ostream &out) const override;
};

} // namespace ParserTree
} // namespace Papageno
