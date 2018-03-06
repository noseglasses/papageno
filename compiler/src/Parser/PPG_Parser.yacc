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

#include "ParserTree/PPG_Phrase.hpp"
#include "ParserTree/PPG_Pattern.hpp"
#include "ParserTree/PPG_Input.hpp"
#include "ParserTree/PPG_Token.hpp"
#include "ParserTree/PPG_Note.hpp"
#include "ParserTree/PPG_Chord.hpp"
#include "ParserTree/PPG_Cluster.hpp"
#include "ParserTree/PPG_Entity.hpp"

#include <iostream>
#include <string>

int yylex();
void yyerror(const char *s);

struct LocationRAII {
   LocationRAII(YYLTYPE *currentLocation__)
   {
      Papageno::Parser::currentLocation = currentLocation__;
   }
   ~LocationRAII() {
      Papageno::Parser::currentLocation = nullptr;
   }
};

extern int yydebug;

typedef Papageno::Parser::Token ParserToken;

using namespace Papageno::ParserTree;

extern "C" {
   int yywrap (void );
}

%}

%start lines

%union {
   char id[MAX_ID_LENGTH]; 
}

%token LAYER_KEYWORD SYMBOL_KEYWORD ARROW ACTION_KEYWORD INPUT_KEYWORD PHRASE_KEYWORD
%token LINE_END
%token<id> ID RAW_CODE

%%                   /* beginning of rules section */

lines:  /*  empty  */
        |
        LINE_END
        |
        line
        |      
        lines line
        ;
        
line:
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
        error LINE_END
        {
          yyerrok;
        }
        ;

input_list:
         ID
         {
            LocationRAII lr(&@1);
            Input::pushNextInput(ParserToken($1, @1));
         }
         |
         input_list ',' ID
         {
            LocationRAII lr(&@3);
            Input::pushNextInput(ParserToken($3, @3));
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
           Pattern::getMostRecentToken()->setLocation(@1);
        }
           
token__:  note
        |
        cluster
        |
        chord
        ;
        
note:   '|' ID '|'
        {
            LocationRAII lr(&@2);
            Input::pushNextInput(ParserToken($2, @2));
            Pattern::pushToken(std::make_shared<Note>("PPG_Note_Flags_A_N_D"));
        }
        |
        '|' ID
        {
            LocationRAII lr(&@2);
            Input::pushNextInput(ParserToken($2, @2));
            Pattern::pushToken(std::make_shared<Note>("PPG_Note_Flag_Match_Activation"));
        }
        |
        ID '|'
        {
            LocationRAII lr(&@1);
            Input::pushNextInput(ParserToken($1, @1));
            Pattern::pushToken(std::make_shared<Note>("PPG_Note_Flag_Match_Deactivation"));
        }
        ;
        
cluster:
        '{' input_list '}'
        {
            Pattern::pushToken(std::make_shared<Cluster>());
        }
        ;
        
chord:
        '[' input_list ']'
        {
            Pattern::pushToken(std::make_shared<Chord>());
        }
        ;
        
action_def:
        ID
        {
            LocationRAII lr(&@1);
            Action::pushNextAction(ParserToken($1, @1));
        }
        |
        ID '@' ID
        {
            LocationRAII lr(&@1);
            Action::pushNextAction(ParserToken($3, @3), ParserToken($1, @1));
        }
        ;
        
action_list:
        action_def
        |
        action_list ',' action_def
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
           Input::define(
               std::make_shared<Input>(
                  Entity::getNextId(),
                  Entity::getNextType(),
                  Entity::getNextParameters()
               )
           );
        }
        ;
        
action_def:
        ACTION_KEYWORD ':' typed_id parameters
        {
            Action::define(
               std::make_shared<Action>(
                  Entity::getNextId(),
                  Entity::getNextType(),
                  Entity::getNextParameters()
               )
            );
        }
        ;
        
typed_id:
        ID
        {
            LocationRAII lr(&@1);
            Entity::setNextId(ParserToken($1, @1));
        }
        |
        ID '<' ID '>'
        {
            LocationRAII lr(&@1);
            Entity::setNextId(ParserToken($1, @1));
            Entity::setNextType(ParserToken($3, @3));
        }
        ;
        
parameters:
        /* possibly empty */
        |
        RAW_CODE
        {
           LocationRAII lr(&@1);
           Entity::setNextParameters(ParserToken($1, @1));
        }
        ;
%%

namespace Papageno {
namespace Parser {

YYLTYPE *currentLocation = nullptr;

int lineOffset = 0;

static void process_definitions(const char *line)
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

void generateTree(std::istream &input) 
{
   yydebug = 1;
   
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
         std::cout << "line " << curLine << " in ppg\n";
         inPPG = true;
         wasInPPG = true;
         lineOffset = curLine + 1;
         continue;
      }
      
      if(!inPPG) {
         if(wasInPPG) { break; }
         
         continue;
      }
      
      if(line.find(PPG_END_TOKEN) != std::string::npos) {
         std::cout << "line " << curLine << ": end of ppg\n";
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
      
      std::cout << "Processing line \'" << line << "\'\n";
      buffer << line;
   }   
   
   if(!wasInPPG) {
      std::cerr << "No papageno definitions encountered\n";
      exit(EXIT_FAILURE);
   }
   
   process_definitions(buffer.str().c_str());
}
   
} // namespace ParserTree
} // namespace Papageno
