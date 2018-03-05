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

#include <string>

namespace Papageno {
namespace ParserTree {
   
class Entity
{
   public:
      
      static void setNextParameters(const std::string &parameters) {
         parameters_ = parameters;
      }
      
      static std::string getNextParameters() {
         std::string tmp;
         tmp.swap(parameters_);
         return tmp;
      }
      
      static void setNextType(const std::string &nextType) {
         nextType_ = nextType;
      }
      
      static std::string getNextType() {
         std::string tmp;
         tmp.swap(nextType_);
         return tmp;
      }
      
      static void setNextId(const std::string &nextId) {
         nextId_ = nextId;
      }
      
      static std::string getNextId() {
         std::string tmp;
         tmp.swap(nextId_);
         return tmp;
      }
   
   protected:
      
      static std::string   parameters_;
      
      static std::string   nextType_;
      static std::string   nextId_;
};

      
} // namespace ParserTree
} // namespace Papageno