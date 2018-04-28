/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_HOME_FLO_INPARTIK_DEVELOPMENT_KALEIDOSCOPE_KALEIDOSCOPE_PAPAGENO_HARDWARE_KEYBOARDIO_AVR_LIBRARIES_KALEIDOSCOPE_PAPAGENO_3RD_PARTY_PAPAGENO_GLOCKENSPIEL_SRC_PARSER_GLS_PARSER_YACC_HPP_INCLUDED
# define YY_YY_HOME_FLO_INPARTIK_DEVELOPMENT_KALEIDOSCOPE_KALEIDOSCOPE_PAPAGENO_HARDWARE_KEYBOARDIO_AVR_LIBRARIES_KALEIDOSCOPE_PAPAGENO_3RD_PARTY_PAPAGENO_GLOCKENSPIEL_SRC_PARSER_GLS_PARSER_YACC_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LAYER_KEYWORD = 258,
    ARROW = 259,
    ACTION_KEYWORD = 260,
    INPUT_KEYWORD = 261,
    TYPE_KEYWORD = 262,
    PHRASE_KEYWORD = 263,
    ALIAS_KEYWORD = 264,
    INCLUDE_KEYWORD = 265,
    SETTING_KEYWORD = 266,
    DEFAULT_KEYWORD = 267,
    LINE_END = 268,
    ID = 269,
    QUOTED_STRING = 270
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef double YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int yyparse (void *scanner);

#endif /* !YY_YY_HOME_FLO_INPARTIK_DEVELOPMENT_KALEIDOSCOPE_KALEIDOSCOPE_PAPAGENO_HARDWARE_KEYBOARDIO_AVR_LIBRARIES_KALEIDOSCOPE_PAPAGENO_3RD_PARTY_PAPAGENO_GLOCKENSPIEL_SRC_PARSER_GLS_PARSER_YACC_HPP_INCLUDED  */
