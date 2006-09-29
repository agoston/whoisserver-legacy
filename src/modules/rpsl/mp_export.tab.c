/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse mp_exportparse
#define yylex   mp_exportlex
#define yyerror mp_exporterror
#define yylval  mp_exportlval
#define yychar  mp_exportchar
#define yydebug mp_exportdebug
#define yynerrs mp_exportnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OP_OR = 258,
     OP_AND = 259,
     OP_NOT = 260,
     OP_MS = 261,
     OP_EQUAL = 262,
     OP_APPEND = 263,
     OP_COMPARE = 264,
     KEYW_ANY = 265,
     KEYW_PEERAS = 266,
     ASPATH_POSTFIX = 267,
     TKN_FLTRNAME = 268,
     TKN_ASNO = 269,
     TKN_RSNAME = 270,
     TKN_ASNAME = 271,
     TKN_PRFXV4 = 272,
     TKN_PRFXV4RNG = 273,
     TKN_IPV4 = 274,
     TKN_RTRSNAME = 275,
     TKN_PRNGNAME = 276,
     TKN_IPV6 = 277,
     TKN_IPV6DC = 278,
     TKN_PRFXV6 = 279,
     TKN_PRFXV6DC = 280,
     TKN_PRFXV6RNG = 281,
     TKN_PRFXV6DCRNG = 282,
     KEYW_TO = 283,
     KEYW_ACTION = 284,
     KEYW_EXCEPT = 285,
     KEYW_AFI = 286,
     KEYW_AFI_VALUE_V4 = 287,
     KEYW_AFI_VALUE_V6 = 288,
     KEYW_AFI_VALUE_ANY = 289,
     TKN_PREF = 290,
     TKN_MED = 291,
     TKN_DPA = 292,
     TKN_ASPATH = 293,
     TKN_COMMUNITY = 294,
     TKN_NEXT_HOP = 295,
     TKN_COST = 296,
     TKN_COMM_NO = 297,
     KEYW_IGP_COST = 298,
     KEYW_SELF = 299,
     KEYW_PREPEND = 300,
     KEYW_APPEND = 301,
     KEYW_DELETE = 302,
     KEYW_CONTAINS = 303,
     KEYW_AT = 304,
     KEYW_INTERNET = 305,
     KEYW_NO_EXPORT = 306,
     KEYW_NO_ADVERTISE = 307,
     KEYW_PROTOCOL = 308,
     TKN_PROTOCOL = 309,
     KEYW_INTO = 310,
     KEYW_REFINE = 311,
     KEYW_ANNOUNCE = 312,
     TKN_INT = 313,
     TKN_DNS = 314
   };
#endif
#define OP_OR 258
#define OP_AND 259
#define OP_NOT 260
#define OP_MS 261
#define OP_EQUAL 262
#define OP_APPEND 263
#define OP_COMPARE 264
#define KEYW_ANY 265
#define KEYW_PEERAS 266
#define ASPATH_POSTFIX 267
#define TKN_FLTRNAME 268
#define TKN_ASNO 269
#define TKN_RSNAME 270
#define TKN_ASNAME 271
#define TKN_PRFXV4 272
#define TKN_PRFXV4RNG 273
#define TKN_IPV4 274
#define TKN_RTRSNAME 275
#define TKN_PRNGNAME 276
#define TKN_IPV6 277
#define TKN_IPV6DC 278
#define TKN_PRFXV6 279
#define TKN_PRFXV6DC 280
#define TKN_PRFXV6RNG 281
#define TKN_PRFXV6DCRNG 282
#define KEYW_TO 283
#define KEYW_ACTION 284
#define KEYW_EXCEPT 285
#define KEYW_AFI 286
#define KEYW_AFI_VALUE_V4 287
#define KEYW_AFI_VALUE_V6 288
#define KEYW_AFI_VALUE_ANY 289
#define TKN_PREF 290
#define TKN_MED 291
#define TKN_DPA 292
#define TKN_ASPATH 293
#define TKN_COMMUNITY 294
#define TKN_NEXT_HOP 295
#define TKN_COST 296
#define TKN_COMM_NO 297
#define KEYW_IGP_COST 298
#define KEYW_SELF 299
#define KEYW_PREPEND 300
#define KEYW_APPEND 301
#define KEYW_DELETE 302
#define KEYW_CONTAINS 303
#define KEYW_AT 304
#define KEYW_INTERNET 305
#define KEYW_NO_EXPORT 306
#define KEYW_NO_ADVERTISE 307
#define KEYW_PROTOCOL 308
#define TKN_PROTOCOL 309
#define KEYW_INTO 310
#define KEYW_REFINE 311
#define KEYW_ANNOUNCE 312
#define TKN_INT 313
#define TKN_DNS 314




/* Copy the first part of user declarations.  */
#line 1 "mp_export.y"

/*
  filename: mp_export.y

  description:
    Defines the grammar for an RPSLng mp-export attribute. Derived
    from export.y. 

  notes:
    Defines tokens for the associated lexer, mp_export.l.
*/


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 14 "mp_export.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 220 "mp_export.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 36 "mp_export.y"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void syntax_error(char *fmt, ...);



/* Line 214 of yacc.c.  */
#line 240 "mp_export.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   271

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  78
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  53
/* YYNRULES -- Number of rules. */
#define YYNRULES  153
/* YYNRULES -- Number of states. */
#define YYNSTATES  241

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   314

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    74,     2,     2,     2,
      64,    65,    70,    72,    61,    77,    66,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    60,
      67,     2,    68,    71,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    75,     2,    76,    73,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    62,    69,    63,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     7,    11,    16,    17,    20,    21,    24,
      26,    30,    32,    36,    40,    43,    47,    51,    55,    59,
      61,    63,    65,    67,    70,    74,    76,    80,    83,    87,
      91,    95,   100,   104,   106,   107,   110,   114,   116,   120,
     124,   126,   130,   132,   134,   136,   137,   139,   140,   143,
     147,   149,   153,   157,   159,   163,   165,   167,   169,   171,
     173,   175,   177,   181,   184,   188,   190,   192,   194,   196,
     198,   200,   202,   206,   210,   214,   218,   225,   227,   231,
     235,   239,   246,   253,   260,   265,   269,   273,   275,   279,
     281,   283,   285,   287,   289,   293,   297,   301,   305,   309,
     313,   316,   318,   322,   324,   327,   331,   333,   335,   337,
     341,   343,   345,   348,   350,   352,   354,   356,   358,   362,
     363,   365,   367,   371,   373,   375,   377,   379,   381,   383,
     387,   389,   392,   394,   397,   400,   403,   406,   408,   410,
     412,   416,   418,   420,   422,   424,   426,   430,   435,   436,
     439,   442,   445,   450
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
      79,     0,    -1,    80,    81,    82,    -1,    80,    81,    86,
      -1,    80,    81,    86,    60,    -1,    -1,    53,    54,    -1,
      -1,    55,    54,    -1,    83,    -1,    31,    84,    83,    -1,
      85,    -1,    85,    56,    82,    -1,    85,    30,    82,    -1,
      84,    61,    -1,    84,    61,    32,    -1,    84,    61,    33,
      -1,    84,    61,    34,    -1,    84,    61,    10,    -1,    32,
      -1,    33,    -1,    34,    -1,    10,    -1,    88,    60,    -1,
      62,    87,    63,    -1,    88,    -1,    31,    84,    88,    -1,
      88,    60,    -1,    87,    88,    60,    -1,    89,    57,   116,
      -1,    28,    90,    91,    -1,    89,    28,    90,    91,    -1,
      92,    96,    97,    -1,    21,    -1,    -1,    29,   103,    -1,
      92,     3,    93,    -1,    93,    -1,    93,     4,    94,    -1,
      93,    30,    94,    -1,    94,    -1,    64,    92,    65,    -1,
      95,    -1,    14,    -1,    16,    -1,    -1,    98,    -1,    -1,
      49,    98,    -1,    98,     3,    99,    -1,    99,    -1,    99,
       4,   100,    -1,    99,    30,   100,    -1,   100,    -1,    64,
      98,    65,    -1,   101,    -1,    19,    -1,    22,    -1,    23,
      -1,   102,    -1,    20,    -1,    59,    -1,   102,    66,    59,
      -1,   104,    60,    -1,   103,   104,    60,    -1,   105,    -1,
     106,    -1,   107,    -1,   108,    -1,   110,    -1,   114,    -1,
     115,    -1,    35,     7,    58,    -1,    36,     7,    58,    -1,
      36,     7,    43,    -1,    37,     7,    58,    -1,    38,    66,
      45,    64,   109,    65,    -1,    14,    -1,   109,    61,    14,
      -1,    39,     7,   111,    -1,    39,     8,   111,    -1,    39,
      66,    46,    64,   112,    65,    -1,    39,    66,    47,    64,
     112,    65,    -1,    39,    66,    48,    64,   112,    65,    -1,
      39,    64,   112,    65,    -1,    39,     9,   111,    -1,    62,
     112,    63,    -1,   113,    -1,   112,    61,   113,    -1,    50,
      -1,    51,    -1,    52,    -1,    58,    -1,    42,    -1,    40,
       7,    19,    -1,    40,     7,    22,    -1,    40,     7,    23,
      -1,    40,     7,    44,    -1,    41,     7,    58,    -1,   116,
       3,   117,    -1,   116,   117,    -1,   117,    -1,   117,     4,
     118,    -1,   118,    -1,     5,   118,    -1,    64,   116,    65,
      -1,   119,    -1,    10,    -1,   104,    -1,    67,   125,    68,
      -1,    13,    -1,   120,    -1,   121,     6,    -1,   121,    -1,
      14,    -1,    11,    -1,    16,    -1,    15,    -1,    62,   122,
      63,    -1,    -1,   123,    -1,   124,    -1,   123,    61,   124,
      -1,    17,    -1,    24,    -1,    25,    -1,    18,    -1,    26,
      -1,    27,    -1,   125,    69,   126,    -1,   126,    -1,   126,
     127,    -1,   127,    -1,   127,    70,    -1,   127,    71,    -1,
     127,    72,    -1,   127,    12,    -1,   128,    -1,    73,    -1,
      74,    -1,    64,   125,    65,    -1,   129,    -1,    14,    -1,
      11,    -1,    16,    -1,    66,    -1,    75,   130,    76,    -1,
      75,    73,   130,    76,    -1,    -1,   130,    14,    -1,   130,
      11,    -1,   130,    66,    -1,   130,    14,    77,    14,    -1,
     130,    16,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    47,    47,    48,    49,    52,    53,    56,    57,    60,
      61,    64,    65,    66,    67,    70,    71,    72,    73,    74,
      75,    76,    77,    80,    81,    84,    85,    88,    89,    92,
      95,    96,    99,   100,   103,   104,   107,   108,   111,   112,
     113,   116,   117,   120,   121,   124,   125,   128,   129,   132,
     133,   136,   137,   138,   141,   142,   145,   146,   147,   148,
     153,   156,   157,   160,   161,   164,   165,   166,   167,   168,
     169,   170,   173,   184,   193,   196,   207,   210,   211,   214,
     215,   216,   217,   218,   219,   220,   223,   226,   227,   230,
     231,   232,   233,   243,   246,   247,   248,   249,   252,   263,
     264,   265,   268,   269,   272,   273,   274,   277,   278,   279,
     280,   281,   284,   285,   288,   289,   290,   291,   292,   295,
     296,   299,   300,   303,   304,   305,   306,   307,   308,   311,
     312,   315,   316,   319,   320,   321,   322,   323,   326,   327,
     328,   329,   332,   333,   334,   335,   336,   337,   340,   341,
     342,   343,   344,   345
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OP_OR", "OP_AND", "OP_NOT", "OP_MS",
  "OP_EQUAL", "OP_APPEND", "OP_COMPARE", "KEYW_ANY", "KEYW_PEERAS",
  "ASPATH_POSTFIX", "TKN_FLTRNAME", "TKN_ASNO", "TKN_RSNAME", "TKN_ASNAME",
  "TKN_PRFXV4", "TKN_PRFXV4RNG", "TKN_IPV4", "TKN_RTRSNAME",
  "TKN_PRNGNAME", "TKN_IPV6", "TKN_IPV6DC", "TKN_PRFXV6", "TKN_PRFXV6DC",
  "TKN_PRFXV6RNG", "TKN_PRFXV6DCRNG", "KEYW_TO", "KEYW_ACTION",
  "KEYW_EXCEPT", "KEYW_AFI", "KEYW_AFI_VALUE_V4", "KEYW_AFI_VALUE_V6",
  "KEYW_AFI_VALUE_ANY", "TKN_PREF", "TKN_MED", "TKN_DPA", "TKN_ASPATH",
  "TKN_COMMUNITY", "TKN_NEXT_HOP", "TKN_COST", "TKN_COMM_NO",
  "KEYW_IGP_COST", "KEYW_SELF", "KEYW_PREPEND", "KEYW_APPEND",
  "KEYW_DELETE", "KEYW_CONTAINS", "KEYW_AT", "KEYW_INTERNET",
  "KEYW_NO_EXPORT", "KEYW_NO_ADVERTISE", "KEYW_PROTOCOL", "TKN_PROTOCOL",
  "KEYW_INTO", "KEYW_REFINE", "KEYW_ANNOUNCE", "TKN_INT", "TKN_DNS", "';'",
  "','", "'{'", "'}'", "'('", "')'", "'.'", "'<'", "'>'", "'|'", "'*'",
  "'?'", "'+'", "'^'", "'$'", "'['", "']'", "'-'", "$accept",
  "mp_export_attribute", "opt_protocol_from", "opt_protocol_into",
  "afi_export_expr", "export_expr", "afi_list", "export_term",
  "afi_export_factor", "export_factor_list", "export_factor",
  "export_peering_action_list", "peering", "opt_action", "as_expr",
  "as_expr_term", "as_expr_factor", "as_expr_operand", "opt_router_expr",
  "opt_router_expr_with_at", "router_expr", "router_expr_term",
  "router_expr_factor", "router_expr_operand", "domain_name", "action",
  "rp_attribute", "pref", "med", "dpa", "aspath", "asno_list", "community",
  "community_list", "community_elm_list", "community_elm", "next_hop",
  "cost", "filter", "filter_term", "filter_factor", "filter_operand",
  "filter_prefix", "filter_prefix_operand", "opt_filter_prefix_list",
  "filter_prefix_list", "filter_prefix_list_prefix", "filter_aspath",
  "filter_aspath_term", "filter_aspath_closure", "filter_aspath_factor",
  "filter_aspath_no", "filter_aspath_range", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
      59,    44,   123,   125,    40,    41,    46,    60,    62,   124,
      42,    63,    43,    94,    36,    91,    93,    45
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    78,    79,    79,    79,    80,    80,    81,    81,    82,
      82,    83,    83,    83,    83,    84,    84,    84,    84,    84,
      84,    84,    84,    85,    85,    86,    86,    87,    87,    88,
      89,    89,    90,    90,    91,    91,    92,    92,    93,    93,
      93,    94,    94,    95,    95,    96,    96,    97,    97,    98,
      98,    99,    99,    99,   100,   100,   101,   101,   101,   101,
     101,   102,   102,   103,   103,   104,   104,   104,   104,   104,
     104,   104,   105,   106,   106,   107,   108,   109,   109,   110,
     110,   110,   110,   110,   110,   110,   111,   112,   112,   113,
     113,   113,   113,   113,   114,   114,   114,   114,   115,   116,
     116,   116,   117,   117,   118,   118,   118,   119,   119,   119,
     119,   119,   120,   120,   121,   121,   121,   121,   121,   122,
     122,   123,   123,   124,   124,   124,   124,   124,   124,   125,
     125,   126,   126,   127,   127,   127,   127,   127,   128,   128,
     128,   128,   129,   129,   129,   129,   129,   129,   130,   130,
     130,   130,   130,   130
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     3,     3,     4,     0,     2,     0,     2,     1,
       3,     1,     3,     3,     2,     3,     3,     3,     3,     1,
       1,     1,     1,     2,     3,     1,     3,     2,     3,     3,
       3,     4,     3,     1,     0,     2,     3,     1,     3,     3,
       1,     3,     1,     1,     1,     0,     1,     0,     2,     3,
       1,     3,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     3,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     6,     1,     3,     3,
       3,     6,     6,     6,     4,     3,     3,     1,     3,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       2,     1,     3,     1,     2,     3,     1,     1,     1,     3,
       1,     1,     2,     1,     1,     1,     1,     1,     3,     0,
       1,     1,     3,     1,     1,     1,     1,     1,     1,     3,
       1,     2,     1,     2,     2,     2,     2,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     3,     4,     0,     2,
       2,     2,     4,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       5,     0,     0,     7,     6,     1,     0,     0,     8,    22,
       0,     0,    19,    20,    21,     0,     2,     9,     0,    11,
       3,    25,     0,    43,    44,    33,     0,    34,    45,    37,
      40,    42,     0,     0,     0,    14,     0,     0,     4,    23,
       0,     0,     0,     0,    30,     0,    56,    60,    57,    58,
      61,     0,    47,    46,    50,    53,    55,    59,     0,     0,
       0,    10,    26,    24,     0,    27,    18,    15,    16,    17,
       0,    13,     0,    12,    34,     0,   107,   115,   110,   114,
     117,   116,     0,     0,     0,     0,     0,     0,     0,   119,
       0,     0,   108,    65,    66,    67,    68,    69,    70,    71,
      29,   101,   103,   106,   111,   113,    41,    35,     0,    36,
       0,     0,    32,     0,     0,     0,     0,    38,    39,    28,
       0,    31,   104,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   123,   126,   124,   125,   127,   128,
       0,   120,   121,     0,   143,   142,   144,     0,   145,   138,
     139,   148,     0,   130,   132,   137,   141,     0,   100,     0,
     112,     0,    63,    54,    48,    49,    51,    52,    62,    72,
      74,    73,    75,     0,     0,    79,    80,    85,    93,    89,
      90,    91,    92,     0,    87,     0,     0,     0,    94,    95,
      96,    97,    98,   118,     0,   105,     0,   148,     0,   109,
       0,   131,   136,   133,   134,   135,    99,   102,    64,     0,
       0,     0,    84,     0,     0,     0,   122,   140,     0,   150,
     149,   153,   151,   146,   129,    77,     0,    86,    88,     0,
       0,     0,   147,     0,     0,    76,    81,    82,    83,   152,
      78
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     2,     3,     7,    16,    17,    18,    19,    20,    33,
      72,    22,    27,    44,    28,    29,    30,    31,    52,   112,
      53,    54,    55,    56,    57,   107,    92,    93,    94,    95,
      96,   226,    97,   175,   183,   184,    98,    99,   100,   101,
     102,   103,   104,   105,   140,   141,   142,   152,   153,   154,
     155,   156,   198
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -152
static const short int yypact[] =
{
     -22,   -12,    22,    -1,  -152,  -152,    30,   109,  -152,  -152,
     130,   154,  -152,  -152,  -152,    28,  -152,  -152,    33,   -19,
      58,    93,   -14,  -152,  -152,  -152,    18,    83,   108,    67,
    -152,  -152,   122,   -10,   113,   175,   129,   129,  -152,  -152,
     130,    85,     2,   187,  -152,    18,  -152,  -152,  -152,  -152,
    -152,   146,    96,   176,    79,  -152,  -152,   116,    18,    18,
     175,  -152,    93,  -152,   132,  -152,  -152,  -152,  -152,  -152,
     154,  -152,    93,  -152,    83,    85,  -152,  -152,  -152,  -152,
    -152,  -152,   226,   227,   228,   170,    72,   230,   231,   194,
      85,    41,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
      25,   235,  -152,  -152,  -152,   234,  -152,   187,   181,    67,
      13,   146,  -152,   146,   146,   146,   183,  -152,  -152,  -152,
     122,  -152,  -152,   185,    50,   186,   200,   184,   184,   184,
     148,   167,   158,   189,  -152,  -152,  -152,  -152,  -152,  -152,
     188,   191,  -152,    10,  -152,  -152,  -152,    41,  -152,  -152,
    -152,   177,    90,    41,    32,  -152,  -152,    85,   235,    85,
    -152,   193,  -152,  -152,   176,    79,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,   190,   148,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,    45,  -152,   192,   195,   196,  -152,  -152,
    -152,  -152,  -152,  -152,   194,  -152,   -36,  -152,    -8,  -152,
      41,    32,  -152,  -152,  -152,  -152,   235,  -152,  -152,   241,
     -44,   148,  -152,   148,   148,   148,  -152,  -152,    -7,  -152,
     171,  -152,  -152,  -152,    41,  -152,    52,  -152,  -152,    68,
     128,   136,  -152,   243,   244,  -152,  -152,  -152,  -152,  -152,
    -152
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -152,  -152,  -152,  -152,   180,   -32,    16,  -152,  -152,  -152,
     163,  -152,   209,   197,   236,   216,   145,  -152,  -152,  -152,
     -41,   150,   115,  -152,  -152,  -152,   -31,  -152,  -152,  -152,
    -152,  -152,  -152,   103,   -39,    53,  -152,  -152,   178,    -9,
     -74,  -152,  -152,  -152,  -152,  -152,    71,   119,    69,  -151,
    -152,  -152,    70
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      61,   122,   201,   219,   219,    45,   220,   220,   221,   221,
     110,    36,   108,   157,    40,    75,   113,   211,    10,   227,
      76,    77,     5,    78,    79,    80,    81,    32,   157,   217,
      75,     1,    23,   200,    24,    76,    77,    37,    78,    79,
      80,    81,     4,    41,   202,    82,    83,    84,    85,    86,
      87,    88,   144,    63,     6,   145,    10,   146,   222,   222,
      82,    83,    84,    85,    86,    87,    88,   106,   223,   232,
     164,    58,    89,   201,    90,   195,   161,    91,   163,   127,
     128,   129,    26,   114,     8,   207,   120,    89,    61,    90,
      75,   158,    91,   170,    35,    76,    77,    59,    78,    79,
      80,    81,   203,   204,   205,   147,   211,   148,   171,   115,
     212,    45,    43,   234,   149,   150,   151,   235,    38,     9,
      82,    83,    84,    85,    86,    87,    88,    46,    47,   211,
      48,    49,     9,   236,   158,   210,   130,    10,   131,     9,
      11,    12,    13,    14,    23,   111,    24,    89,   206,    90,
      10,    25,    91,    39,    12,    13,    14,    10,   199,   200,
      70,    12,    13,    14,     9,    46,    47,    50,    48,    49,
      21,    15,    51,    65,   229,   230,   231,   188,    34,   113,
     189,   190,   116,    60,    15,    66,    12,    13,    14,   211,
     178,    15,   119,   237,    26,    62,    64,   211,   179,   180,
     181,   238,   191,   117,   118,    50,   182,    67,    68,    69,
      51,   134,   135,   185,   186,   187,    71,    73,   136,   137,
     138,   139,    82,    83,    84,    85,    86,    87,    88,   166,
     167,   176,   177,   123,   124,   125,   126,   132,   133,   159,
     160,   162,   168,   169,   172,   173,   174,   192,   233,    74,
     197,   193,   194,   208,   209,   225,   213,   239,   240,   214,
     215,   109,    42,   165,   228,   216,   196,   218,   143,   224,
       0,   121
};

static const short int yycheck[] =
{
      32,    75,   153,    11,    11,     3,    14,    14,    16,    16,
      51,    30,    43,     3,    28,     5,     3,    61,    28,    63,
      10,    11,     0,    13,    14,    15,    16,    11,     3,    65,
       5,    53,    14,    69,    16,    10,    11,    56,    13,    14,
      15,    16,    54,    57,    12,    35,    36,    37,    38,    39,
      40,    41,    11,    63,    55,    14,    28,    16,    66,    66,
      35,    36,    37,    38,    39,    40,    41,    65,    76,    76,
     111,     4,    62,   224,    64,    65,   107,    67,    65,     7,
       8,     9,    64,     4,    54,   159,    70,    62,   120,    64,
       5,   100,    67,    43,    61,    10,    11,    30,    13,    14,
      15,    16,    70,    71,    72,    64,    61,    66,    58,    30,
      65,     3,    29,    61,    73,    74,    75,    65,    60,    10,
      35,    36,    37,    38,    39,    40,    41,    19,    20,    61,
      22,    23,    10,    65,   143,   174,    64,    28,    66,    10,
      31,    32,    33,    34,    14,    49,    16,    62,   157,    64,
      28,    21,    67,    60,    32,    33,    34,    28,    68,    69,
      31,    32,    33,    34,    10,    19,    20,    59,    22,    23,
       7,    62,    64,    60,   213,   214,   215,    19,    15,     3,
      22,    23,    66,    61,    62,    10,    32,    33,    34,    61,
      42,    62,    60,    65,    64,    32,    33,    61,    50,    51,
      52,    65,    44,    58,    59,    59,    58,    32,    33,    34,
      64,    17,    18,    46,    47,    48,    36,    37,    24,    25,
      26,    27,    35,    36,    37,    38,    39,    40,    41,   114,
     115,   128,   129,     7,     7,     7,    66,     7,     7,     4,
       6,    60,    59,    58,    58,    45,    62,    58,    77,    40,
      73,    63,    61,    60,    64,    14,    64,    14,    14,    64,
      64,    45,    26,   113,   211,   194,   147,   197,    90,   200,
      -1,    74
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    53,    79,    80,    54,     0,    55,    81,    54,    10,
      28,    31,    32,    33,    34,    62,    82,    83,    84,    85,
      86,    88,    89,    14,    16,    21,    64,    90,    92,    93,
      94,    95,    84,    87,    88,    61,    30,    56,    60,    60,
      28,    57,    92,    29,    91,     3,    19,    20,    22,    23,
      59,    64,    96,    98,    99,   100,   101,   102,     4,    30,
      61,    83,    88,    63,    88,    60,    10,    32,    33,    34,
      31,    82,    88,    82,    90,     5,    10,    11,    13,    14,
      15,    16,    35,    36,    37,    38,    39,    40,    41,    62,
      64,    67,   104,   105,   106,   107,   108,   110,   114,   115,
     116,   117,   118,   119,   120,   121,    65,   103,   104,    93,
      98,    49,    97,     3,     4,    30,    66,    94,    94,    60,
      84,    91,   118,     7,     7,     7,    66,     7,     8,     9,
      64,    66,     7,     7,    17,    18,    24,    25,    26,    27,
     122,   123,   124,   116,    11,    14,    16,    64,    66,    73,
      74,    75,   125,   126,   127,   128,   129,     3,   117,     4,
       6,   104,    60,    65,    98,    99,   100,   100,    59,    58,
      43,    58,    58,    45,    62,   111,   111,   111,    42,    50,
      51,    52,    58,   112,   113,    46,    47,    48,    19,    22,
      23,    44,    58,    63,    61,    65,   125,    73,   130,    68,
      69,   127,    12,    70,    71,    72,   117,   118,    60,    64,
     112,    61,    65,    64,    64,    64,   124,    65,   130,    11,
      14,    16,    66,    76,   126,    14,   109,    63,   113,   112,
     112,   112,    76,    77,    61,    65,    65,    65,    65,    14,
      14
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 59:
#line 148 "mp_export.y"
    {
    if (strlen(yyvsp[0].sval) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", yyvsp[0].sval);
    }
;}
    break;

  case 72:
#line 173 "mp_export.y"
    {
      long int val;
      char *s, *p;
      p = yyvsp[0].sval;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("pref value \"%s\" is not between 0 and 65535", p);
      }
;}
    break;

  case 73:
#line 184 "mp_export.y"
    {
      long int val;
      char *s, *p;
      p = yyvsp[0].sval;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("med value \"%s\" is not between 0 and 65535", p);
      }
;}
    break;

  case 75:
#line 196 "mp_export.y"
    {
      long int val;
      char *s, *p;
      p = yyvsp[0].sval;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("dpa value \"%s\" is not between 0 and 65535", p);
      }
;}
    break;

  case 92:
#line 233 "mp_export.y"
    {
      unsigned long int val;
      char *s, *p;
      p = yyvsp[0].sval;
      val = strtoul(p, &s, 10);
      if ((val < 1) || (val > 4294967295UL) || (*s != '\0')) {
          syntax_error("community element \"%s\" is not between 1 and 4294967295", 
                       p);
      }
;}
    break;

  case 98:
#line 252 "mp_export.y"
    {
      long int val;
      char *s, *p;
      p = yyvsp[0].sval;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("cost value \"%s\" is not between 0 and 65535", p);
      }
;}
    break;


    }

/* Line 1010 of yacc.c.  */
#line 1465 "mp_export.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
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

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 348 "mp_export.y"


#undef mp_exporterror
#undef yyerror

int yyerror(const char *s);
int
mp_exporterror (const char *s)
{
    yyerror(s);
    return 0;
}


