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

#define REPORT_LOCATION(LOCATION) \
                   '(' \
                   << (Papageno::Parser::lineOffset + (LOCATION).first_line) << ':' \
                   << (LOCATION).first_column << ")" \
                      "...(" \
                   << (Papageno::Parser::lineOffset + (LOCATION).last_line) << ':' \
                   << (LOCATION).last_column << ")"
                   
#define REPORT_TOKEN(PARSER_TOKEN) \
   REPORT_LOCATION((PARSER_TOKEN).getLocation()) << " \'" << (PARSER_TOKEN).getText() << '\''
   
#define THROW_TOKEN_ERROR(PARSER_TOKEN, ...) \
   THROW_ERROR(REPORT_TOKEN(PARSER_TOKEN) << ": " << __VA_ARGS__)
   
namespace Papageno {
namespace Parser {

class Token {
   
   public:
      
      Token() 
         :  text_(nullptr),
            location_{(YYLTYPE){ .first_line = -1, .first_column = -1, .last_line = -1, .last_column = -1 }}
      {}
      
      Token(const char *text, YYLTYPE location)
         :  text_(text),
            location_(location)
      {}
      
      void setText(const std::string &text) { text_ = text; }
      const std::string &getText() const { return text_; }
      const YYLTYPE &getLocation() const { return location_; }
      
   private:
   
      std::string text_;
      YYLTYPE location_;
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
std::ostream &operator<<(std::ostream &out, const Papageno::Parser::Token &token)
{
   if(token.getLocation().first_line >= 0) {
       out << REPORT_LOCATION(token.getLocation()) << " ";
   }
   
   out << '\'' << token.getText() << '\'';
   
   return out;
}
