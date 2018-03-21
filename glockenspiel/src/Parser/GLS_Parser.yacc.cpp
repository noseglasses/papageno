/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:339  */


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



#line 149 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "GLS_Parser.yacc.hpp".  */
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

/* Copy the second part of user declarations.  */

#line 229 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  52
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   169

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  86
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  149

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   270

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    34,     2,    20,    39,    32,    33,    38,
      35,    36,    21,    30,    19,    23,     2,    31,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    16,    37,
      28,    17,    29,     2,    18,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    26,     2,    27,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    24,    22,    25,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   104,   104,   106,   111,   117,   119,   125,   131,   136,
     141,   146,   151,   156,   161,   167,   173,   179,   186,   192,
     200,   202,   209,   211,   217,   227,   232,   239,   249,   251,
     253,   255,   258,   266,   274,   283,   289,   296,   304,   312,
     318,   320,   327,   335,   341,   349,   351,   358,   366,   374,
     382,   397,   412,   418,   421,   424,   427,   431,   437,   446,
     453,   454,   455,   456,   457,   458,   459,   460,   461,   462,
     463,   464,   465,   466,   467,   468,   469,   470,   471,   472,
     473,   474,   475,   476,   478,   483,   484
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LAYER_KEYWORD", "ARROW",
  "ACTION_KEYWORD", "INPUT_KEYWORD", "TYPE_KEYWORD", "PHRASE_KEYWORD",
  "ALIAS_KEYWORD", "INCLUDE_KEYWORD", "SETTING_KEYWORD", "DEFAULT_KEYWORD",
  "LINE_END", "ID", "QUOTED_STRING", "':'", "'='", "'@'", "','", "'#'",
  "'*'", "'|'", "'-'", "'{'", "'}'", "'['", "']'", "'<'", "'>'", "'+'",
  "'/'", "'%'", "'&'", "'!'", "'('", "')'", "';'", "'\\''", "'$'",
  "$accept", "lines", "line", "input_list", "phrase", "action_token",
  "rep_token", "token", "token__", "note", "input", "cluster", "chord",
  "aggregate", "flag_definition", "alpha_seq", "action_list_entry",
  "action_list", "layer_def", "input_type_def", "action_type_def",
  "input_def", "action_def", "alias_def", "action_parameters",
  "input_parameters", "typed_id", "parameters", "cpp_token", "cpp_code",
  "cpp_token_seq", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,    58,    61,    64,    44,
      35,    42,   124,    45,   123,   125,    91,    93,    60,    62,
      43,    47,    37,    38,    33,    40,    41,    59,    39,    36
};
# endif

#define YYPACT_NINF -13

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-13)))

#define YYTABLE_NINF -3

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       0,     1,     5,    -5,     3,    15,    18,    21,   -13,   -13,
      33,    32,    34,    34,    38,    38,    27,   -13,    12,   -13,
      42,    -4,   -13,   -13,   -13,   -13,   -13,    28,    71,    95,
      96,    98,   102,   -13,   103,   104,   105,   106,   105,   107,
     109,   110,   111,   112,   -13,   113,    84,   108,   -13,   -12,
      93,    99,   -13,   -13,    90,   -13,   117,   118,   -13,   -13,
     -13,   -13,   -13,   -13,   -13,   119,    97,   120,   121,   120,
     122,   123,   124,   127,   129,   130,   -13,   -13,   -13,   -13,
     132,   -13,   -13,   -13,   -13,   116,   -13,   125,   -13,   -13,
     133,   114,   -13,   -13,   -13,   -13,   -13,    90,   134,   114,
     136,   135,   -13,   -13,   137,   117,   126,    66,   -13,    25,
     -13,   141,   -13,   142,   -13,   -13,   -13,   -13,   -13,   -13,
     -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,
     -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,
     -13,   -13,    40,   -13,   -13,   143,   -13,   -13,   -13
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     5,    42,
       0,     0,     0,     0,     0,     0,     0,     3,     0,    20,
      22,    25,    27,    28,    29,    30,    31,     0,     0,     0,
       0,     0,     0,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    24,    35,     0,     0,    18,    40,
       0,     0,     1,     4,     0,     6,     0,     0,     8,     9,
      10,    11,    12,    13,    47,     0,    57,    53,     0,    55,
       0,     0,     0,     0,     0,     0,    36,    32,    33,    34,
       0,    39,    37,    38,    21,    43,    45,    23,    26,    49,
       0,     0,    51,    54,    48,    50,    56,     0,     0,     0,
       0,     0,    41,    19,     0,     0,     0,     0,    59,     0,
      52,     0,    15,     0,    44,    46,    58,    82,    83,    78,
      76,    80,    77,    62,    67,    60,    70,    71,    72,    73,
      74,    75,    61,    63,    64,    65,    66,    68,    69,    79,
      81,    85,     0,     7,    14,     0,    84,    86,    16
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -13,   -13,   144,   -13,    39,   115,   -13,   -13,   -13,   -13,
     145,   -13,   -13,   146,    89,   -13,    45,   -13,   -13,   -13,
     -13,   -13,   -13,   -13,   -13,   -13,   128,    94,    17,    63,
     -13
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    16,    17,    49,    18,    19,    20,    21,    22,    23,
      46,    24,    25,    50,    76,    26,    86,    87,    27,    28,
      29,    30,    31,    32,    92,    95,    67,    93,   141,   108,
     142
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      -2,     1,    35,     2,    75,     3,     4,    80,     5,     6,
      37,    36,     7,     8,    33,     9,    54,    57,    10,    38,
      11,    34,    12,    13,    14,    55,    15,    52,     1,    54,
       2,    39,     3,     4,    40,     5,     6,    41,   143,     7,
       8,    58,     9,    42,    43,    10,    44,    11,    45,    12,
      13,    14,    48,    15,   117,   118,   119,   120,    56,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   146,
     117,   118,   119,   120,    59,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,     9,    77,    78,    60,    61,
      11,    62,    12,    13,    14,    63,    15,    64,    82,    66,
      65,    70,    68,    71,    72,    90,    83,    73,    74,    75,
      79,    85,    88,    89,   104,    94,   109,    91,    81,    97,
      98,    99,   100,   101,   105,   102,   103,   106,   110,   112,
     115,   114,   113,   107,   144,   116,   148,   145,    47,   147,
      53,    51,   111,    96,     0,     0,    69,     0,     0,    84
};

static const yytype_int16 yycheck[] =
{
       0,     1,     7,     3,    16,     5,     6,    19,     8,     9,
       7,    16,    12,    13,    13,    15,     4,    21,    18,    16,
      20,    16,    22,    23,    24,    13,    26,     0,     1,     4,
       3,    16,     5,     6,    16,     8,     9,    16,    13,    12,
      13,    13,    15,    10,    11,    18,    14,    20,    14,    22,
      23,    24,    14,    26,    14,    15,    16,    17,    16,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      14,    15,    16,    17,    13,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    15,    22,    23,    13,    13,
      20,    13,    22,    23,    24,    13,    26,    14,    25,    14,
      16,    14,    16,    14,    14,    28,    27,    16,    16,    16,
      22,    14,    14,    14,    18,    14,    97,    17,    49,    17,
      17,    17,    15,    14,    19,    15,    14,    14,    14,    13,
     105,    14,    17,    39,    13,    29,    13,    15,    13,   142,
      16,    15,    99,    69,    -1,    -1,    38,    -1,    -1,    54
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     5,     6,     8,     9,    12,    13,    15,
      18,    20,    22,    23,    24,    26,    41,    42,    44,    45,
      46,    47,    48,    49,    51,    52,    55,    58,    59,    60,
      61,    62,    63,    13,    16,     7,    16,     7,    16,    16,
      16,    16,    10,    11,    14,    14,    50,    50,    14,    43,
      53,    53,     0,    42,     4,    13,    16,    21,    13,    13,
      13,    13,    13,    13,    14,    16,    14,    66,    16,    66,
      14,    14,    14,    16,    16,    16,    54,    22,    23,    22,
      19,    54,    25,    27,    45,    14,    56,    57,    14,    14,
      28,    17,    64,    67,    14,    65,    67,    17,    17,    17,
      15,    14,    15,    14,    18,    19,    14,    39,    69,    44,
      14,    69,    13,    17,    14,    56,    29,    14,    15,    16,
      17,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    68,    70,    13,    13,    15,    39,    68,    13
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    40,    41,    41,    41,    42,    42,    42,    42,    42,
      42,    42,    42,    42,    42,    42,    42,    42,    43,    43,
      44,    44,    45,    45,    45,    46,    46,    47,    48,    48,
      48,    48,    49,    49,    49,    50,    50,    51,    52,    53,
      54,    54,    55,    56,    56,    57,    57,    58,    59,    60,
      61,    62,    63,    64,    64,    65,    65,    66,    66,    67,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    69,    70,    70
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     2,     1,     2,     6,     2,     2,
       2,     2,     2,     2,     6,     5,     7,     2,     1,     3,
       1,     3,     1,     3,     2,     1,     3,     1,     1,     1,
       1,     1,     3,     3,     3,     1,     2,     3,     3,     2,
       0,     2,     1,     1,     3,     1,     3,     3,     4,     4,
       4,     4,     5,     0,     1,     0,     1,     1,     4,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, scanner, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *scanner)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *scanner)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, void *scanner)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void *scanner)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 107 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
        }
#line 1519 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 112 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
        }
#line 1527 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 120 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Pattern::finishPattern();
        }
#line 1536 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 126 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Phrase::storePhrase(ParserToken((yyvsp[-3]), (yylsp[-3])));
        }
#line 1545 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 132 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
        }
#line 1553 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 137 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
        }
#line 1561 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 142 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
        }
#line 1569 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 147 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
        }
#line 1577 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 152 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
        }
#line 1585 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 157 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
        }
#line 1593 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 162 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           defaults.set((yyvsp[-3]), Glockenspiel::Parser::getCppCode().getText());
        }
#line 1602 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 168 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Glockenspiel::Parser::searchFileGenerateTree(Glockenspiel::Parser::unquote((yyvsp[-1])));
        }
#line 1611 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 174 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Glockenspiel::settings.set((yyvsp[-3]), Glockenspiel::Parser::unquote((yyvsp[-1])));
        }
#line 1620 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 180 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
          yyerrok;
        }
#line 1628 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 187 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
            MARK_LOCATION((yyloc))
            Input::pushNext(ParserToken((yyvsp[0]), (yylsp[0])));
         }
#line 1637 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 193 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
            MARK_LOCATION((yyloc))
            Input::pushNext(ParserToken((yyvsp[0]), (yylsp[0])));
         }
#line 1646 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 203 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
            MARK_LOCATION((yyloc))
        }
#line 1654 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 212 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Pattern::applyActions();
        }
#line 1663 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 218 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           // Lookup a phrase and copy its tokens to the current pattern
           //
           MARK_LOCATION((yyloc))
           Pattern::pushPhrase(ParserToken((yyvsp[0]), (yylsp[0])));
        }
#line 1674 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 228 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
        }
#line 1682 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 233 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Pattern::repeatLastToken(ParserToken((yyvsp[0]), (yylsp[0])));
        }
#line 1691 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 240 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Pattern::getMostRecentToken()->setLOD((yylsp[0]));
           
           Pattern::getMostRecentToken()->setFlagsString(Glockenspiel::Parser::flagsString);
           Glockenspiel::Parser::flagsString.clear();
        }
#line 1703 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 259 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
            MARK_LOCATION((yyloc))
            auto newNote = std::make_shared<Note>();
            newNote->getFlags().tokenFlags_.set("PPG_Note_Flags_A_N_D");
            Pattern::pushToken(newNote);
        }
#line 1714 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 267 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
            MARK_LOCATION((yyloc))
            auto newNote = std::make_shared<Note>();
            newNote->getFlags().tokenFlags_.set("PPG_Note_Flag_Match_Activation");
            Pattern::pushToken(newNote);
        }
#line 1725 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 275 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
            MARK_LOCATION((yyloc))
            auto newNote = std::make_shared<Note>();
            newNote->getFlags().tokenFlags_.set("PPG_Note_Flag_Match_Deactivation");
            Pattern::pushToken(newNote);
        }
#line 1736 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 284 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
            MARK_LOCATION((yyloc))
            Input::pushNext(ParserToken((yyvsp[0]), (yylsp[0])));
        }
#line 1745 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 290 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
            MARK_LOCATION((yyloc))
            Input::pushNext(ParserToken((yyvsp[-1]), (yylsp[-1])));
        }
#line 1754 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 297 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
            MARK_LOCATION((yyloc))
            Pattern::pushToken(std::make_shared<Cluster>());
        }
#line 1763 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 305 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
            MARK_LOCATION((yyloc))
            Pattern::pushToken(std::make_shared<Chord>());
        }
#line 1772 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 313 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
        }
#line 1780 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 321 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Glockenspiel::Parser::flagsString = Glockenspiel::Parser::unquote((yyvsp[0]));
        }
#line 1789 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 328 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
            Pattern::addAlphaSequence(Glockenspiel::Parser::unquote((yyvsp[0])));
        }
#line 1798 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 336 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
            Action::pushNext(ParserToken((yyvsp[0]), (yylsp[0])));
        }
#line 1807 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 342 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
            Action::pushNext(ParserToken((yyvsp[0]), (yylsp[0])), ParserToken((yyvsp[-2]), (yylsp[-2])));
        }
#line 1816 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 352 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
        }
#line 1824 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 359 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           ParserTree::Token::setCurrentLayer(ParserToken((yyvsp[0]), (yylsp[0])));
        }
#line 1833 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 367 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Input::defineType((yyvsp[0]));
        }
#line 1842 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 375 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Action::defineType((yyvsp[0]));
        }
#line 1851 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 383 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Input::define(
               std::make_shared<Input>(
                  NextEntity::getNextId(),
                  NextEntity::getNextType(),
                  NextEntity::getNextParameters(),
                  NextEntity::getNextParametersDefined()
               )
           );
        }
#line 1867 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 398 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
            MARK_LOCATION((yyloc))
            Action::define(
               std::make_shared<Action>(
                  NextEntity::getNextId(),
                  NextEntity::getNextType(),
                  NextEntity::getNextParameters(),
                  NextEntity::getNextParametersDefined()
               )
            );
        }
#line 1883 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 413 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           Alias::define((yyvsp[-2]), Glockenspiel::Parser::Token((yyvsp[0]), (yyloc)));
        }
#line 1892 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 432 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
            NextEntity::setId(ParserToken((yyvsp[0]), (yylsp[0])));
        }
#line 1901 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 438 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
            NextEntity::setId(ParserToken((yyvsp[-3]), (yylsp[-3])));
            NextEntity::setType(ParserToken((yyvsp[-1]), (yylsp[-1])));
        }
#line 1911 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 447 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
           NextEntity::setParameters(Glockenspiel::Parser::getCppCode());
        }
#line 1920 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 453 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1926 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 454 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1932 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 455 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1938 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 456 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1944 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 457 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1950 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 458 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1956 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 459 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1962 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 460 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1968 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 461 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1974 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 462 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1980 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 463 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1986 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 464 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1992 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 465 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 1998 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 466 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 2004 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 467 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 2010 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 468 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 2016 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 469 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 2022 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 470 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 2028 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 471 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 2034 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 472 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 2040 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 473 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 2046 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 474 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 2052 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 475 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 2058 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 476 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    { Glockenspiel::Parser::codeStream << (yyvsp[0]); }
#line 2064 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 479 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
         }
#line 2072 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 485 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1646  */
    {
           MARK_LOCATION((yyloc))
         }
#line 2080 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
    break;


#line 2084 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 488 "/home/flo/Inpartik/Development/Kaleidoscope/Kaleidoscope-Papageno/hardware/keyboardio/avr/libraries/Kaleidoscope-Papageno/3rd_party/Papageno/glockenspiel/src/Parser/GLS_Parser.yacc" /* yacc.c:1906  */


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
      exit(EXIT_FAILURE);
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
