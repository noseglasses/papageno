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

#include "Parser/PPG_Parser.hpp"

#include <string>
#include <ostream>
                   
#define REPORT_TOKEN(PARSER_TOKEN) \
   (PARSER_TOKEN).getLOD() << " \'" << (PARSER_TOKEN).getText() << '\''
   
#define THROW_TOKEN_ERROR(PARSER_TOKEN, ...) \
   THROW_ERROR(REPORT_TOKEN(PARSER_TOKEN) << ": " << __VA_ARGS__)
   
namespace Papageno {
namespace Parser {
   
struct LocationOfDefinition
{
   LocationOfDefinition(YYLTYPE location = (YYLTYPE){ .first_line = -1, .first_column = -1, .last_line = -1, .last_column = -1 },
                        const char *file = Papageno::Parser::currentFileParsed)
      :  file_(file),
         location_(location)
   {}
   
   operator bool() const {
      return location_.first_line != -1;
   }
   
   const char* file_;
   YYLTYPE location_;
};

class Token
{   
   public:
      
      Token() 
      {}
      
      Token(const std::string &text, YYLTYPE location = (YYLTYPE){ .first_line = -1, .first_column = -1, .last_line = -1, .last_column = -1 })
         :  text_(text),
            lod_(location)
      {}
      
      void setText(const std::string &text) { text_ = text; }
      const std::string &getText() const { return text_; }
      void setLOD(const LocationOfDefinition &lod) { lod_ = lod; }
      const LocationOfDefinition &getLOD() const { return lod_; }
      
   private:
   
      std::string             text_;
      LocationOfDefinition    lod_;
};

struct TokenCompare : public std::binary_function<const Token &, const Token &, bool>
{
    bool operator()(const Token &x, const Token &y) const
    {   
        return x.getText() < y.getText();
    }
};

} // namespace Parser
} // namespace Papageno

inline 
std::ostream &operator<<(std::ostream &out, const YYLTYPE &location)
{
   if(location.first_line == -1) { return out; }
   
   if(location.first_line != location.last_line) {   
      out   << '('
            << ((location).first_line) << ':'
            << (location).first_column 
            << " - "
            << ((location).last_line) << ':'
            << (location).last_column << ')';
   }
   else {      
      out   << '('
            << ((location).first_line) << ':'
            << (location).first_column 
            << "-"
            << (location).last_column << ')';
   }
         
   return out;
}

inline
std::ostream &operator<<(std::ostream &out, const Papageno::Parser::LocationOfDefinition &lod)
{
   if(lod.file_) {
      out << lod.file_ << ": ";
   }
   
   out << lod.location_;
   
   return out;
}

inline
std::ostream &operator<<(std::ostream &out, const Papageno::Parser::Token &token)
{
   out << '\'' << token.getText() << '\'';
   out << token.getLOD();
   
   return out;
}
