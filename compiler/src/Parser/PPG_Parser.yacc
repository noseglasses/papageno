%{

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

#include "Parser/PPG_Parser.hpp"
#include "Parser/PPG_Parser.lex.hpp"
#include "PPG_Compiler.hpp"

#include "ParserTree/PPG_Phrase.hpp"
#include "ParserTree/PPG_Pattern.hpp"
#include "ParserTree/PPG_Input.hpp"
#include "ParserTree/PPG_Token.hpp"
#include "ParserTree/PPG_Note.hpp"
#include "ParserTree/PPG_Chord.hpp"
#include "ParserTree/PPG_Cluster.hpp"
#include "ParserTree/PPG_Entity.hpp"
#include "ParserTree/PPG_Alias.hpp"
#include "ParserTree/PPG_NextEntity.hpp"

#include <iostream>
#include <string>
#include <fstream>

int yylex();
void yyerror(const char *s);

struct LocationRAII {
   LocationRAII(YYLTYPE *currentLocation__)
   {
      lastLOD_ = Papageno::Parser::currentLocation;
      Papageno::Parser::currentLocation 
         = Papageno::Parser::LocationOfDefinition(*currentLocation__);
   }
   ~LocationRAII() {
      Papageno::Parser::currentLocation = lastLOD_;
   }
   Papageno::Parser::LocationOfDefinition lastLOD_;
};

extern int yydebug;
extern int yylineno;
extern YYLTYPE yylloc;

typedef Papageno::Parser::Token ParserToken;

using namespace Papageno::ParserTree;

namespace Papageno {
namespace Parser {
extern std::ostringstream codeStream;
extern Token getCppCode();
}
}

%}

%start lines

/*%union {
   std::string id;
}*/

%token LAYER_KEYWORD SYMBOL_KEYWORD ARROW ACTION_KEYWORD INPUT_KEYWORD PHRASE_KEYWORD
%token ALIAS_KEYWORD LINE_END ID DEFINITION QUOTED_STRING

%locations

%%                   /* beginning of rules section */

lines:  /*  empty  */
        |
        line
        |      
        lines line
        ;
        
line:   LINE_END
        |
        phrase LINE_END
        {
           Pattern::finishPattern();
        }
        |
        PHRASE_KEYWORD ':' ID '=' phrase LINE_END
        {
           Phrase::storePhrase(ParserToken($3, @3));
        }
        |
        layer_def LINE_END
        |
        input_def LINE_END
        |
        action_def LINE_END
        |
        alias_def LINE_END
        |
        error LINE_END
        {
          yyerrok;
        }
        ;

input_list:
         ID
         {
            LocationRAII lr(&@1);
            Input::pushNext(ParserToken($1, @1));
         }
         |
         input_list ',' ID
         {
            LocationRAII lr(&@3);
            Input::pushNext(ParserToken($3, @3));
         }
         ;
         
phrase: 
        action_token
        |
        phrase ARROW action_token
        ;
        
action_token:
        rep_token
        |
        rep_token ':' action_list
        {
           Pattern::applyActions();
        }
        |
        '#' ID
        {
           // Lookup a phrase and copy its tokens to the current pattern
           //
           LocationRAII lr(&@2);
           Pattern::pushPhrase(ParserToken($2, @2));
        }
        ;
        
rep_token:
        token
        {
           (@1);
         }
        |
        token '*' ID
        {
           LocationRAII lr(&@3);
           Pattern::repeatLastToken(ParserToken($3, @3));
        }
        ;
       
token:  token__
        {
           Pattern::getMostRecentToken()->setLOD(@1);
           Pattern::getMostRecentToken()->setFlagCode(Papageno::Parser::getCppCode());
        }
        ;
           
token__:  note
        |
        cluster
        |
        chord
        |
        alpha_seq
        ;
        
note:   '|' input '|'
        {
            LocationRAII lr(&@2);
            Pattern::pushToken(std::make_shared<Note>("PPG_Note_Flags_A_N_D"));
        }
        |
        '|' input
        {
            LocationRAII lr(&@2);
            Pattern::pushToken(std::make_shared<Note>("PPG_Note_Flag_Match_Activation"));
        }
        |
        input '|'
        {
            LocationRAII lr(&@1);
            Pattern::pushToken(std::make_shared<Note>("PPG_Note_Flag_Match_Deactivation"));
        }
        ;
        
input:  ID
        {
            Input::pushNext(ParserToken($1, @1));
        }
        |
        ID flag_definition
        {
            Input::pushNext(ParserToken($1, @1));
        }
        
cluster:
        '{' aggregate '}'
        {
            Pattern::pushToken(std::make_shared<Cluster>());
        }
        ;
        
chord:
        '[' aggregate ']'
        {
            Pattern::pushToken(std::make_shared<Chord>());
        }
        ;
        
aggregate:
        input_list flag_definition
        ;
        
flag_definition: /* allow empty */
        |
        ':' cpp_code;

alpha_seq:
        QUOTED_STRING
        {
            LocationRAII lr(&@1);
            std::string alphaSeqString($1.substr(1, $1.size() - 2));
            Pattern::addAlphaSequence(alphaSeqString);
        }
        ;
        
action_list_entry:
        ID
        {
            LocationRAII lr(&@1);
            Action::pushNext(ParserToken($1, @1));
        }
        |
        ID '@' ID
        {
            LocationRAII lr(&@1);
            Action::pushNext(ParserToken($3, @3), ParserToken($1, @1));
        }
        ;
        
action_list:
        action_list_entry
        |
        action_list ',' action_list_entry
        ;
        
layer_def:
        LAYER_KEYWORD ':' ID
        {
           Token::setCurrentLayer(ParserToken($3, @3));
        }
        ;
        
input_def:
        INPUT_KEYWORD ':' typed_id parameters
        {
           LocationRAII lr(&@$);
           Input::define(
               std::make_shared<Input>(
                  NextEntity::getNextId(),
                  NextEntity::getNextType(),
                  NextEntity::getNextParameters(),
                  NextEntity::getNextParametersDefined()
               )
           );
        }
        ;
        
action_def:
        ACTION_KEYWORD ':' typed_id action_parameters
        {
            LocationRAII lr(&@$);
            Action::define(
               std::make_shared<Action>(
                  NextEntity::getNextId(),
                  NextEntity::getNextType(),
                  NextEntity::getNextParameters(),
                  NextEntity::getNextParametersDefined()
               )
            );
        }
        ;
        
alias_def:
        ALIAS_KEYWORD ':' ID DEFINITION ID
        {
           Papageno::ParserTree::Alias::define($3, Papageno::Parser::Token($5, @$));
        }
        
action_parameters:
        // Allow empty
        |
        parameters
        ;
        
typed_id:
        ID
        {
            LocationRAII lr(&@1);
            NextEntity::setId(ParserToken($1, @1));
        }
        |
        ID '<' ID '>'
        {
            LocationRAII lr(&@1);
            NextEntity::setId(ParserToken($1, @1));
            NextEntity::setType(ParserToken($3, @3));
        }
        ;
        
parameters:
        DEFINITION cpp_code
        {
           LocationRAII lr(&@1);
           NextEntity::setParameters(Papageno::Parser::getCppCode());
        }
        ;
        
cpp_token:   '-' { Papageno::Parser::codeStream << $1; }
   |         '+' { Papageno::Parser::codeStream << $1; }
   |         '*' { Papageno::Parser::codeStream << $1; }
   |         '/' { Papageno::Parser::codeStream << $1; }
   |         '%' { Papageno::Parser::codeStream << $1; }
   |         '&' { Papageno::Parser::codeStream << $1; }
   |         '!' { Papageno::Parser::codeStream << $1; }
   |         '|' { Papageno::Parser::codeStream << $1; }
   |         '(' { Papageno::Parser::codeStream << $1; }
   |         ')' { Papageno::Parser::codeStream << $1; }
   |         '{' { Papageno::Parser::codeStream << $1; }
   |         '}' { Papageno::Parser::codeStream << $1; }
   |         '[' { Papageno::Parser::codeStream << $1; }
   |         ']' { Papageno::Parser::codeStream << $1; }
   |         '<' { Papageno::Parser::codeStream << $1; }
   |         '>' { Papageno::Parser::codeStream << $1; }
   |         '=' { Papageno::Parser::codeStream << $1; }
   |         '#' { Papageno::Parser::codeStream << $1; }
   |         ':' { Papageno::Parser::codeStream << $1; }
   |         ';' { Papageno::Parser::codeStream << $1; }
   |         ',' { Papageno::Parser::codeStream << $1; }
   |         '\'' { Papageno::Parser::codeStream << $1; }
   |         ID  { Papageno::Parser::codeStream << $1; }
   |         QUOTED_STRING { Papageno::Parser::codeStream << $1; }
   
cpp_code: '$' cpp_token_seq '$'

cpp_token_seq:    cpp_token
   |         cpp_token_seq cpp_token
%%

void yyerror(const char *s)
{
  THROW_ERROR("Parser error: " << s);
}

namespace Papageno {
namespace Parser {

std::ostringstream codeStream;

LocationOfDefinition currentLocation;

std::vector<std::string> filesParsed;
std::vector<std::string> codeParsed;

const char *currentFileParsed = nullptr;

Token getCppCode()
{
   Token cppCode(codeStream.str());
   codeStream.clear();
   codeStream.str("");
   return cppCode;
}

static void processDefinitions(const char *line)
{
   // add the second NULL terminator
   int len = strlen(line);
   char *temp = (char*)malloc(len + 2);
   strcpy( temp, line );
   temp[ len + 1 ] = 0; // The first NULL terminator is added by strcpy

   YY_BUFFER_STATE my_string_buffer = yy_scan_string(temp); 
   yy_switch_to_buffer( my_string_buffer ); // switch flex to the buffer we just created
   yyparse(); 
   yy_delete_buffer(my_string_buffer );
   free(temp);
}

#define PPG_START_TOKEN "papageno_start"
#define PPG_END_TOKEN "papageno_end"

#define DEBUG_OUTPUT(...) \
   if(ai.debug_flag) { \
      std::cout << __VA_ARGS__; \
   }

static void generateTree(std::istream &input) 
{
   if(ai.debug_flag) {
      yydebug = 1;
   }
   
   #define LINE_SIZE 4096
   
   std::string line;
   
   std::ostringstream buffer;
   
   long curLine = 0;
   
   bool inPPG = false;
   bool wasInPPG = false;
   
   while(std::getline(input, line)) {
   
      ++curLine;
      
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
           
      if(line.find(PPG_START_TOKEN) != std::string::npos) {
         DEBUG_OUTPUT("Start tag \'" PPG_START_TOKEN "\' detected in line " << curLine << "\n")
         inPPG = true;
         wasInPPG = true;
         yylineno = curLine + 1;
         continue;
      }
      
      if(!inPPG) {
         if(wasInPPG) { break; }
         
         continue;
      }
      
      if(line.find(PPG_END_TOKEN) != std::string::npos) {
         DEBUG_OUTPUT("End tag \'" PPG_END_TOKEN "\' detected in line " << curLine << "\n")
         break;
      }
      
      if(!inPPG) {
         continue;
      }
      
      // Remove comments
      //
      for(auto it = line.begin(); *it != '\0'; ++it) {
         if(*it == '%') {
            *it = '\n';
            ++it;
            *it = '\0';
            break;
         }
      }
      
      DEBUG_OUTPUT("Processing line \'" << line << "\'\n")
      buffer << line << "\n";
   }   
   
   if(!wasInPPG) {
      std::cerr << "No papageno definitions encountered\n";
      exit(EXIT_FAILURE);
   }
   
   codeParsed.push_back(buffer.str());
   
   processDefinitions(codeParsed.back().c_str());
}
   
void generateTree(const char *inputFilename)
{
   filesParsed.push_back(inputFilename);
   currentFileParsed = filesParsed.back().c_str();
   
   std::ifstream inFile(inputFilename);
   
   generateTree(inFile);
}

} // namespace ParserTree
} // namespace Papageno
