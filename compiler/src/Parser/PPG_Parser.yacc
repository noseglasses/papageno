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
#include "Settings/PPG_Settings.hpp"

#include <iostream>
#include <string>
#include <fstream>

void yyerror(YYLTYPE *yylloc, yyscan_t scanner, const char *s);

struct LocationRAII {
   LocationRAII(YYLTYPE *currentLocation__)
   {
      lastLOD_ = Glockenspiel::Parser::currentLocation;
      Glockenspiel::Parser::currentLocation 
         = Glockenspiel::Parser::LocationOfDefinition(*currentLocation__);
   }
   ~LocationRAII() {
      Glockenspiel::Parser::currentLocation = lastLOD_;
   }
   Glockenspiel::Parser::LocationOfDefinition lastLOD_;
};

// extern int yydebug;
// extern YYLTYPE yylloc;

typedef Glockenspiel::Parser::Token ParserToken;

using namespace Glockenspiel::ParserTree;

namespace Glockenspiel {
namespace Parser {
extern std::ostringstream codeStream;
extern Token getCppCode();
extern void searchFileGenerateTree(const std::string &quotedInputFilename);

inline
std::string unquote(const std::string &s) {
   return s.substr(1, s.size() - 2);
}

}
}

%}

%start lines

%define api.value.type {double}
%define parse.error verbose
%define api.pure 
%lex-param {YYLTYPE *yylloc, void *scanner}
%parse-param {void *scanner}

/*%union {
   std::string id;
}*/

%token LAYER_KEYWORD SYMBOL_KEYWORD ARROW ACTION_KEYWORD INPUT_KEYWORD PHRASE_KEYWORD
%token ALIAS_KEYWORD INCLUDE_KEYWORD SETTING_KEYWORD LINE_END ID QUOTED_STRING

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
        '@' INCLUDE_KEYWORD ':' QUOTED_STRING LINE_END
        {
           LocationRAII lr(&@$);
           Glockenspiel::Parser::searchFileGenerateTree(Glockenspiel::Parser::unquote($4));
        }
        |
        '@' SETTING_KEYWORD ':' ID '=' QUOTED_STRING LINE_END
        {
           LocationRAII lr(&@$);
           Glockenspiel::Settings::set($4, Glockenspiel::Parser::unquote($6));
        }
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
           Pattern::getMostRecentToken()->setFlagCode(Glockenspiel::Parser::getCppCode());
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
            Pattern::addAlphaSequence(Glockenspiel::Parser::unquote($1));
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
        ALIAS_KEYWORD ':' ID '=' ID
        {
           Glockenspiel::ParserTree::Alias::define($3, Glockenspiel::Parser::Token($5, @$));
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
        '=' cpp_code
        {
           LocationRAII lr(&@1);
           NextEntity::setParameters(Glockenspiel::Parser::getCppCode());
        }
        ;
        
cpp_token:   '-' { Glockenspiel::Parser::codeStream << $1; }
   |         '+' { Glockenspiel::Parser::codeStream << $1; }
   |         '*' { Glockenspiel::Parser::codeStream << $1; }
   |         '/' { Glockenspiel::Parser::codeStream << $1; }
   |         '%' { Glockenspiel::Parser::codeStream << $1; }
   |         '&' { Glockenspiel::Parser::codeStream << $1; }
   |         '!' { Glockenspiel::Parser::codeStream << $1; }
   |         '|' { Glockenspiel::Parser::codeStream << $1; }
   |         '(' { Glockenspiel::Parser::codeStream << $1; }
   |         ')' { Glockenspiel::Parser::codeStream << $1; }
   |         '{' { Glockenspiel::Parser::codeStream << $1; }
   |         '}' { Glockenspiel::Parser::codeStream << $1; }
   |         '[' { Glockenspiel::Parser::codeStream << $1; }
   |         ']' { Glockenspiel::Parser::codeStream << $1; }
   |         '<' { Glockenspiel::Parser::codeStream << $1; }
   |         '>' { Glockenspiel::Parser::codeStream << $1; }
   |         '=' { Glockenspiel::Parser::codeStream << $1; }
   |         '#' { Glockenspiel::Parser::codeStream << $1; }
   |         ':' { Glockenspiel::Parser::codeStream << $1; }
   |         ';' { Glockenspiel::Parser::codeStream << $1; }
   |         ',' { Glockenspiel::Parser::codeStream << $1; }
   |         '\'' { Glockenspiel::Parser::codeStream << $1; }
   |         ID  { Glockenspiel::Parser::codeStream << $1; }
   |         QUOTED_STRING { Glockenspiel::Parser::codeStream << $1; }
   
cpp_code: '$' cpp_token_seq '$'

cpp_token_seq:    cpp_token
   |         cpp_token_seq cpp_token
%%

void yyerror(YYLTYPE *yylloc, yyscan_t scanner, const char *s)
{
  THROW_ERROR("Parser error: " << s);
}

namespace Glockenspiel {
namespace Parser {

struct CurrentFileRAII {
   CurrentFileRAII(const char *currentFileParsed__)
   {
      lastFileParsed_ = Glockenspiel::Parser::currentFileParsed;
      
      Glockenspiel::Parser::currentFileParsed 
         = currentFileParsed__;
   }
   ~CurrentFileRAII() {
      Glockenspiel::Parser::currentFileParsed = lastFileParsed_;
   }
   const char *lastFileParsed_;
};

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

static void processDefinitions(const char *line, int startLine)
{
   // add the second NULL terminator
/*   int len = strlen(line);
   char *temp = (char*)malloc(len + 2);
   strcpy( temp, line );
   temp[ len + 1 ] = 0; // The first NULL terminator is added by strcpy*/
   
   yyscan_t scanner;

   yylex_init(&scanner);
   
   YY_BUFFER_STATE my_string_buffer = yy_scan_string(line, scanner); 
   //yy_switch_to_buffer( my_string_buffer ); // switch flex to the buffer we just created
   
   yy_switch_to_buffer(my_string_buffer, scanner);
   my_string_buffer->yy_bs_lineno = startLine;
   yyparse(scanner); 
   
   yy_delete_buffer(my_string_buffer, scanner);
/*    free(temp); */
   
   yylex_destroy(scanner);
}

#define PPG_START_TOKEN "papageno_start"
#define PPG_END_TOKEN "papageno_end"

#define DEBUG_OUTPUT(...) \
   if(Glockenspiel::commandLineArgs.debug_flag) { \
      std::cout << __VA_ARGS__; \
   }

static void generateTree(std::istream &input) 
{
   if(Glockenspiel::commandLineArgs.debug_flag) {
      yydebug = 1;
   }
   
   #define LINE_SIZE 4096
   
   std::string line;
   
   std::ostringstream buffer;
   
   long curLine = 0;
   long startLine = 0;
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
         startLine = curLine + 1;
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
      std::size_t commentStart = 0;
      for(auto it = line.begin(); *it != '\0'; ++it) {
         if(*it == '%') {
            line.resize(commentStart + 1);
            line[commentStart] = '\n';
            break;
         }
         ++commentStart;
      }
      
      DEBUG_OUTPUT("Processing line \'" << line << "\'\n")
      buffer << line << "\n";
   }   
   
   if(!wasInPPG) {
      std::cerr << "No papageno definitions encountered\n";
      exit(EXIT_FAILURE);
   }
   
   codeParsed.push_back(buffer.str());
   
   processDefinitions(codeParsed.back().c_str(), startLine);
}

inline bool exists(const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

   
void generateTree(const char *inputFilename)
{
   filesParsed.push_back(inputFilename);
   
   CurrentFileRAII cfr(filesParsed.back().c_str());
   
   std::ifstream inFile(inputFilename);
   
   generateTree(inFile);
}

void searchFileGenerateTree(const std::string &inputFilename)
{   
   // Search in current directory
   if(exists(inputFilename)) {
      generateTree(inputFilename.c_str());
      return;
   }
   else {
      for(int i = 0; i < Glockenspiel::commandLineArgs.include_directory_given; ++i) {
         std::string filenameFull 
            = std::string(Glockenspiel::commandLineArgs.include_directory_arg[i]) + "/" + inputFilename;
           
         if(exists(filenameFull)) {
            generateTree(filenameFull.c_str());
            return;
         }
      }
   }
   
   THROW_ERROR("Unable to include file \'" << inputFilename << "\'");
}

} // namespace ParserTree
} // namespace Glockenspiel
