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

#include "Parser/GLS_Parser.hpp"
#include "Parser/GLS_Parser.lex.hpp"
#include "GLS_Compiler.hpp"

#include "ParserTree/GLS_Phrase.hpp"
#include "ParserTree/GLS_Pattern.hpp"
#include "ParserTree/GLS_Input.hpp"
#include "ParserTree/GLS_Token.hpp"
#include "ParserTree/GLS_Note.hpp"
#include "ParserTree/GLS_Chord.hpp"
#include "ParserTree/GLS_Cluster.hpp"
#include "ParserTree/GLS_Entity.hpp"
#include "ParserTree/GLS_Alias.hpp"
#include "ParserTree/GLS_NextEntity.hpp"
#include "Settings/GLS_Settings.hpp"
#include "Settings/GLS_Defaults.hpp"

#include <iostream>
#include <string>
#include <fstream>

// Fix build problems with XCode clang
//
#if defined(__clang__) && defined(__USE_FILE_OFFSET64)
#define fseeko fseeko64
#define ftello ftello64
#endif

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

extern int yydebug;

typedef Glockenspiel::Parser::Token ParserToken;

using namespace Glockenspiel;
using namespace Glockenspiel::ParserTree;
using namespace Glockenspiel::Parser;

namespace Glockenspiel {
namespace Parser {
extern std::ostringstream codeStream;
extern Token getCppCode();
extern void searchFileGenerateTree(const std::string &quotedInputFilename);
extern std::string flagsString;

inline
std::string unquote(const std::string &s) {
   return s.substr(1, s.size() - 2);
}

}
}

#define MARK_LOCATION(LOC) \
           LocationRAII lr(&(LOC));


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

%token LAYER_KEYWORD ARROW ACTION_KEYWORD INPUT_KEYWORD TYPE_KEYWORD PHRASE_KEYWORD
%token ALIAS_KEYWORD INCLUDE_KEYWORD SETTING_KEYWORD DEFAULT_KEYWORD
%token LINE_END ID QUOTED_STRING

%locations

%%                   /* beginning of rules section */

lines:  /*  empty  */
        |
        line
        {
           MARK_LOCATION(@$)
        }
        |      
        lines line
        {
           MARK_LOCATION(@$)
        }
        ;
        
line:   LINE_END
        |
        phrase LINE_END
        {
           MARK_LOCATION(@$)
           Pattern::finishPattern();
        }
        |
        PHRASE_KEYWORD ':' ID '=' phrase LINE_END
        {
           MARK_LOCATION(@$)
           Phrase::storePhrase(ParserToken($3, @3));
        }
        |
        layer_def LINE_END
        {
           MARK_LOCATION(@$)
        }
        |
        input_type_def LINE_END
        {
           MARK_LOCATION(@$)
        }
        |
        action_type_def LINE_END
        {
           MARK_LOCATION(@$)
        }
        |
        input_def LINE_END
        {
           MARK_LOCATION(@$)
        }
        |
        action_def LINE_END
        {
           MARK_LOCATION(@$)
        }
        |
        alias_def LINE_END
        {
           MARK_LOCATION(@$)
        }
        |
        DEFAULT_KEYWORD ':' ID '=' cpp_code LINE_END
        {
           MARK_LOCATION(@$)
           defaults.set($3, Glockenspiel::Parser::getCppCode().getText());
        }
        |
        '@' INCLUDE_KEYWORD ':' QUOTED_STRING LINE_END
        {
           MARK_LOCATION(@$)
           Glockenspiel::Parser::searchFileGenerateTree(Glockenspiel::Parser::unquote($4));
        }
        |
        '@' SETTING_KEYWORD ':' ID '=' QUOTED_STRING LINE_END
        {
           MARK_LOCATION(@$)
           Glockenspiel::settings.set($4, Glockenspiel::Parser::unquote($6));
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
            MARK_LOCATION(@$)
            Input::pushNext(ParserToken($1, @1));
         }
         |
         input_list ',' ID
         {
            MARK_LOCATION(@$)
            Input::pushNext(ParserToken($3, @3));
         }
         ;
         
phrase: 
        action_token
        |
        phrase ARROW action_token
        {
            MARK_LOCATION(@$)
        }
        ;
        
action_token:
        rep_token
        |
        rep_token ':' action_list
        {
           MARK_LOCATION(@$)
           Pattern::applyActions();
        }
        ;
        
rep_token:
        token
        {
           MARK_LOCATION(@$)
        }
        |
        token '*' ID
        {
           MARK_LOCATION(@$)
           Pattern::repeatLastToken(ParserToken($3, @3));
        }
        ;
       
token:  token__
        {
           MARK_LOCATION(@$)
           Pattern::getMostRecentToken()->setLOD(@1);
           
           Pattern::getMostRecentToken()->setFlagsString(Glockenspiel::Parser::flagsString);
           Glockenspiel::Parser::flagsString.clear();
        }
        ;
           
token__:  note
        |
        cluster
        |
        chord
        |
        alpha_seq
        |
        phrase_lookup
        ;
        
phrase_lookup:
        '#' ID
        {
           // Lookup a phrase and copy its tokens to the current pattern
           //
           MARK_LOCATION(@$)
           Pattern::pushPhrase(ParserToken($2, @2));
        }
        ;
        
note:   '|' input '|'
        {
            MARK_LOCATION(@$)
            auto newNote = std::make_shared<Note>();
            newNote->getFlags().tokenFlags_.set("PPG_Note_Flags_A_N_D");
            Pattern::pushToken(newNote);
        }
        |
        '|' input '-'
        {
            MARK_LOCATION(@$)
            auto newNote = std::make_shared<Note>();
            newNote->getFlags().tokenFlags_.set("PPG_Note_Flag_Match_Activation");
            Pattern::pushToken(newNote);
        }
        |
        '-' input '|'
        {
            MARK_LOCATION(@$)
            auto newNote = std::make_shared<Note>();
            newNote->getFlags().tokenFlags_.set("PPG_Note_Flag_Match_Deactivation");
            Pattern::pushToken(newNote);
        }
        ;
        
input:  ID
        {
            MARK_LOCATION(@$)
            Input::pushNext(ParserToken($1, @1));
        }
        |
        ID flag_definition
        {
            MARK_LOCATION(@$)
            Input::pushNext(ParserToken($1, @1));
        }
        
cluster:
        '{' aggregate '}'
        {
            MARK_LOCATION(@$)
            Pattern::pushToken(std::make_shared<Cluster>());
        }
        ;
        
chord:
        '[' aggregate ']'
        {
            MARK_LOCATION(@$)
            Pattern::pushToken(std::make_shared<Chord>());
        }
        ;
        
aggregate:
        input_list flag_definition
        {
           MARK_LOCATION(@$)
        }           
        ;
        
flag_definition: /* allow empty */
        |
        ':' QUOTED_STRING
        {
           MARK_LOCATION(@$)
           Glockenspiel::Parser::flagsString = Glockenspiel::Parser::unquote($2);
        }

alpha_seq:
        QUOTED_STRING
        {
           MARK_LOCATION(@$)
            Pattern::addAlphaSequence(Glockenspiel::Parser::unquote($1));
        }
        ;
        
action_list_entry:
        ID
        {
           MARK_LOCATION(@$)
            Action::pushNext(ParserToken($1, @1));
        }
        |
        ID '@' ID
        {
           MARK_LOCATION(@$)
            Action::pushNext(ParserToken($3, @3), ParserToken($1, @1));
        }
        ;
        
action_list:
        action_list_entry
        |
        action_list ',' action_list_entry
        {
           MARK_LOCATION(@$)
        }
        ;
        
layer_def:
        LAYER_KEYWORD ':' ID
        {
           MARK_LOCATION(@$)
           ParserTree::Token::setCurrentLayer(ParserToken($3, @3));
        }
        ;
        
input_type_def:
        INPUT_KEYWORD TYPE_KEYWORD ':' ID
        {
           MARK_LOCATION(@$)
           Input::defineType($4);
        }
        ;
        
action_type_def:
        ACTION_KEYWORD TYPE_KEYWORD ':' ID
        {
           MARK_LOCATION(@$)
           Action::defineType($4);
        }
        ;
        
input_def:
        INPUT_KEYWORD ':' typed_id input_parameters
        {
           MARK_LOCATION(@$)
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
            MARK_LOCATION(@$)
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
           MARK_LOCATION(@$)
           Alias::define($3, Glockenspiel::Parser::Token($5, @$));
        }
        
action_parameters:
        // Allow empty
        |
        parameters
        ;
        
input_parameters:
        // Allow empty
        |
        parameters
        ;
        
typed_id:
        ID
        {
           MARK_LOCATION(@$)
            NextEntity::setId(ParserToken($1, @1));
        }
        |
        ID '<' ID '>'
        {
           MARK_LOCATION(@$)
            NextEntity::setId(ParserToken($1, @1));
            NextEntity::setType(ParserToken($3, @3));
        }
        ;
        
parameters:
        '=' cpp_code
        {
           MARK_LOCATION(@$)
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
   |         '.' { Glockenspiel::Parser::codeStream << $1;  }
   |         ',' { Glockenspiel::Parser::codeStream << $1; }
   |         '\'' { Glockenspiel::Parser::codeStream << $1; }
   |         ID  { Glockenspiel::Parser::codeStream << $1; }
   |         QUOTED_STRING { Glockenspiel::Parser::codeStream << $1; }
   
cpp_code: '$' cpp_token_seq '$'
         {
           MARK_LOCATION(@$)
         }

cpp_token_seq:    cpp_token
   |         cpp_token_seq cpp_token
         {
           MARK_LOCATION(@$)
         }
%%

void yyerror(YYLTYPE *yylloc, yyscan_t scanner, const char *s)
{
  MARK_LOCATION((*yylloc))
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

std::set<std::string> filesParsed;

std::vector<CodeLine> code;

std::string flagsString;

const char *currentFileParsed = nullptr;

Token getCppCode()
{
   Token cppCode(codeStream.str());
   codeStream.clear();
   codeStream.str("");
   return cppCode;
}

class FileScanner 
{
   public:
   
      FileScanner(const char *filename)
         :  filename_(filename)
      {
         yylex_init(&scanner_);
      }
      
      FileScanner(FileScanner &&other)
      {
         this->operator=(std::move(other));
      }
      
      FileScanner &operator=(FileScanner &&other) {
         scanner_ = other.scanner_;
         other.scanner_ = nullptr;
         filename_ = other.filename_;
         other.filename_ = nullptr;
         return *this;
      }
      
      ~FileScanner() {
         if(scanner_) {
            yylex_destroy(scanner_);
         }
      }
      
      void parseLine(const char *line, int lineNumber) {
         YY_BUFFER_STATE my_string_buffer = yy_scan_string(line, scanner_); 
         yy_switch_to_buffer(my_string_buffer, scanner_);
         my_string_buffer->yy_bs_lineno = lineNumber;
         yyparse(scanner_); 
/*          yy_delete_buffer(my_string_buffer, scanner_); */
      }

   private:
   
      yyscan_t scanner_;
      const char *filename_;
};

std::vector<FileScanner> fileScanners;

#define PPG_START_TOKEN "glockenspiel_begin"
#define PPG_END_TOKEN "glockenspiel_end"

#define DEBUG_OUTPUT(...) \
   if(Glockenspiel::commandLineArgs.debug_flag) { \
      std::cout << __VA_ARGS__; \
   }
   
struct DebugRAII {
   DebugRAII(bool state) { 
      oldState_ = yydebug;
      yydebug = state;
   }
   ~DebugRAII() {
      yydebug = oldState_;
   }
   bool oldState_;
};

static void generateTree(std::istream &input) 
{
   DebugRAII dr(Glockenspiel::commandLineArgs.debug_flag);
   
   std::string line;
   
   long curLine = 0;
   long lineStart = 0;
   bool inPPG = false;
   bool wasInPPGAtAll = false;
   
   std::string actualLine; 
   
   while(std::getline(input, line)) {
   
      ++curLine;
      
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
           
      if(line.find(PPG_START_TOKEN) != std::string::npos) {
         DEBUG_OUTPUT("Start tag \'" PPG_START_TOKEN "\' detected in line " << curLine << "\n")
         inPPG = true;
         wasInPPGAtAll = true;
         continue;
      }
      
      if(!inPPG) {
         
         continue;
      }
      
      if(line.find(PPG_END_TOKEN) != std::string::npos) {
         DEBUG_OUTPUT("End tag \'" PPG_END_TOKEN "\' detected in line " << curLine << "\n")
         inPPG = false;
         continue;
      }
      
      code.push_back((CodeLine){ curLine, currentFileParsed, line });
      
      // Remove comments
      //
      std::size_t commentStart = 0;
      for(auto it = line.begin(); *it != '\0'; ++it) {
         if(*it == '%') {
            line.resize(commentStart + 1);
            line.resize(commentStart);
            break;
         }
         ++commentStart;
      }
      
      if(actualLine.empty()) {
         lineStart = curLine;
      }
      
      if(line.back() == '\\') {
          actualLine += line + "\n";
          continue;
      }
      
      if(actualLine.empty()) {
         DEBUG_OUTPUT("****** Processing line \'" << line << "\'\n")
         fileScanners.back().parseLine((line + "\n").c_str(), lineStart);
      }
      else {
         actualLine += line;
         DEBUG_OUTPUT("****** Processing continued line \'" << actualLine << "\'\n")
         fileScanners.back().parseLine((actualLine + "\n").c_str(), lineStart);
         actualLine.clear();
      }
   }   
   
   if(!wasInPPGAtAll) {
      std::cerr << "No glockenspiel definitions encountered\n";
      exit(0);
      //exit(EXIT_FAILURE);
   }
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
   std::string filenameString(inputFilename);
   auto entity = filesParsed.insert(filenameString);
   
   fileScanners.emplace_back(FileScanner(entity.first->c_str()));
   
   CurrentFileRAII cfr(entity.first->c_str());
   
   std::ifstream inFile(inputFilename);
   
   generateTree(inFile);
   
   fileScanners.pop_back();
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
           
         std::cout << "Trying \'" << filenameFull << "\'" << std::endl;  
           
         if(exists(filenameFull)) {
            std::cout << "Found" << std::endl;
            generateTree(filenameFull.c_str());
            return;
         }
      }
   }
   
   THROW_ERROR("Unable to include file \'" << inputFilename << "\'");
}

} // namespace ParserTree
} // namespace Glockenspiel
