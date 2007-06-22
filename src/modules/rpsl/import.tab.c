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
#define yyparse importparse
#define yylex   importlex
#define yyerror importerror
#define yylval  importlval
#define yychar  importchar
#define yydebug importdebug
#define yynerrs importnerrs


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
     TKN_DNS = 275,
     TKN_RTRSNAME = 276,
     TKN_PRNGNAME = 277,
     KEYW_ACTION = 278,
     KEYW_EXCEPT = 279,
     TKN_PREF = 280,
     TKN_MED = 281,
     TKN_DPA = 282,
     TKN_ASPATH = 283,
     TKN_COMMUNITY = 284,
     TKN_NEXT_HOP = 285,
     TKN_COST = 286,
     TKN_COMM_NO = 287,
     KEYW_IGP_COST = 288,
     KEYW_SELF = 289,
     KEYW_PREPEND = 290,
     KEYW_APPEND = 291,
     KEYW_DELETE = 292,
     KEYW_CONTAINS = 293,
     KEYW_AT = 294,
     KEYW_INTERNET = 295,
     KEYW_NO_EXPORT = 296,
     KEYW_NO_ADVERTISE = 297,
     KEYW_PROTOCOL = 298,
     TKN_PROTOCOL = 299,
     KEYW_INTO = 300,
     KEYW_REFINE = 301,
     KEYW_ACCEPT = 302,
     KEYW_FROM = 303,
     TKN_INT = 304
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
#define TKN_DNS 275
#define TKN_RTRSNAME 276
#define TKN_PRNGNAME 277
#define KEYW_ACTION 278
#define KEYW_EXCEPT 279
#define TKN_PREF 280
#define TKN_MED 281
#define TKN_DPA 282
#define TKN_ASPATH 283
#define TKN_COMMUNITY 284
#define TKN_NEXT_HOP 285
#define TKN_COST 286
#define TKN_COMM_NO 287
#define KEYW_IGP_COST 288
#define KEYW_SELF 289
#define KEYW_PREPEND 290
#define KEYW_APPEND 291
#define KEYW_DELETE 292
#define KEYW_CONTAINS 293
#define KEYW_AT 294
#define KEYW_INTERNET 295
#define KEYW_NO_EXPORT 296
#define KEYW_NO_ADVERTISE 297
#define KEYW_PROTOCOL 298
#define TKN_PROTOCOL 299
#define KEYW_INTO 300
#define KEYW_REFINE 301
#define KEYW_ACCEPT 302
#define KEYW_FROM 303
#define TKN_INT 304




/* Copy the first part of user declarations.  */
#line 1 "import.y"

/*
  filename: import.y

  description:
    Defines the grammar for an RPSL import attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Defines tokens for the associated lexer, import.l.
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
#line 16 "import.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 202 "import.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 36 "import.y"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int yyerror(const char *s);
void syntax_error(char *fmt, ...);



/* Line 214 of yacc.c.  */
#line 223 "import.tab.c"

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
#define YYLAST   228

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  68
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  50
/* YYNRULES -- Number of rules. */
#define YYNRULES  131
/* YYNRULES -- Number of states. */
#define YYNSTATES  213

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   304

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    64,     2,     2,     2,
      53,    54,    60,    62,    56,    67,    55,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    50,
      57,     2,    58,    61,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    65,     2,    66,    63,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    51,    59,    52,     2,     2,     2,     2,
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
      45,    46,    47,    48,    49
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     7,    11,    12,    15,    16,    19,    21,
      25,    29,    32,    36,    39,    43,    47,    51,    56,    60,
      62,    63,    66,    70,    72,    76,    80,    82,    86,    88,
      90,    92,    93,    95,    96,    99,   103,   105,   109,   113,
     115,   119,   121,   123,   125,   127,   129,   133,   136,   140,
     142,   144,   146,   148,   150,   152,   154,   158,   162,   166,
     170,   177,   179,   183,   187,   191,   198,   205,   212,   217,
     221,   225,   227,   231,   233,   235,   237,   239,   241,   245,
     249,   253,   257,   260,   262,   266,   268,   271,   275,   277,
     279,   283,   285,   287,   289,   292,   294,   296,   298,   300,
     302,   306,   307,   309,   311,   315,   317,   319,   323,   325,
     328,   330,   333,   336,   339,   342,   344,   346,   348,   352,
     354,   356,   358,   360,   362,   366,   371,   372,   375,   378,
     381,   386
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      69,     0,    -1,    70,    71,    72,    -1,    70,    71,    75,
      -1,    -1,    43,    44,    -1,    -1,    45,    44,    -1,    73,
      -1,    73,    46,    72,    -1,    73,    24,    72,    -1,    75,
      50,    -1,    51,    74,    52,    -1,    75,    50,    -1,    74,
      75,    50,    -1,    76,    47,   103,    -1,    48,    77,    78,
      -1,    76,    48,    77,    78,    -1,    79,    83,    84,    -1,
      22,    -1,    -1,    23,    90,    -1,    79,     3,    80,    -1,
      80,    -1,    80,     4,    81,    -1,    80,    24,    81,    -1,
      81,    -1,    53,    79,    54,    -1,    82,    -1,    14,    -1,
      16,    -1,    -1,    85,    -1,    -1,    39,    85,    -1,    85,
       3,    86,    -1,    86,    -1,    86,     4,    87,    -1,    86,
      24,    87,    -1,    87,    -1,    53,    85,    54,    -1,    88,
      -1,    19,    -1,    89,    -1,    21,    -1,    20,    -1,    89,
      55,    20,    -1,    91,    50,    -1,    90,    91,    50,    -1,
      92,    -1,    93,    -1,    94,    -1,    95,    -1,    97,    -1,
     101,    -1,   102,    -1,    25,     7,    49,    -1,    26,     7,
      49,    -1,    26,     7,    33,    -1,    27,     7,    49,    -1,
      28,    55,    35,    53,    96,    54,    -1,    14,    -1,    96,
      56,    14,    -1,    29,     7,    98,    -1,    29,     8,    98,
      -1,    29,    55,    36,    53,    99,    54,    -1,    29,    55,
      37,    53,    99,    54,    -1,    29,    55,    38,    53,    99,
      54,    -1,    29,    53,    99,    54,    -1,    29,     9,    98,
      -1,    51,    99,    52,    -1,   100,    -1,    99,    56,   100,
      -1,    40,    -1,    41,    -1,    42,    -1,    49,    -1,    32,
      -1,    30,     7,    19,    -1,    30,     7,    34,    -1,    31,
       7,    49,    -1,   103,     3,   104,    -1,   103,   104,    -1,
     104,    -1,   104,     4,   105,    -1,   105,    -1,     5,   105,
      -1,    53,   103,    54,    -1,   106,    -1,    10,    -1,    57,
     112,    58,    -1,    91,    -1,    13,    -1,   107,    -1,   108,
       6,    -1,   108,    -1,    14,    -1,    11,    -1,    16,    -1,
      15,    -1,    51,   109,    52,    -1,    -1,   110,    -1,   111,
      -1,   110,    56,   111,    -1,    17,    -1,    18,    -1,   112,
      59,   113,    -1,   113,    -1,   113,   114,    -1,   114,    -1,
     114,    60,    -1,   114,    61,    -1,   114,    62,    -1,   114,
      12,    -1,   115,    -1,    63,    -1,    64,    -1,    53,   112,
      54,    -1,   116,    -1,    14,    -1,    11,    -1,    16,    -1,
      55,    -1,    65,   117,    66,    -1,    65,    63,   117,    66,
      -1,    -1,   117,    14,    -1,   117,    11,    -1,   117,    55,
      -1,   117,    14,    67,    14,    -1,   117,    16,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    48,    48,    49,    52,    53,    56,    57,    60,    61,
      62,    65,    66,    69,    70,    73,    76,    77,    80,    81,
      84,    85,    88,    89,    92,    93,    94,    97,    98,   101,
     102,   105,   106,   109,   110,   113,   114,   117,   118,   119,
     122,   123,   126,   127,   132,   135,   136,   139,   140,   143,
     144,   145,   146,   147,   148,   149,   152,   163,   172,   175,
     186,   189,   190,   193,   194,   195,   196,   197,   198,   199,
     202,   205,   206,   209,   210,   211,   212,   222,   225,   226,
     229,   240,   241,   242,   245,   246,   249,   250,   251,   254,
     255,   256,   257,   258,   261,   262,   265,   266,   267,   268,
     269,   272,   273,   276,   277,   280,   281,   284,   285,   288,
     289,   292,   293,   294,   295,   296,   299,   300,   301,   302,
     305,   306,   307,   308,   309,   310,   313,   314,   315,   316,
     317,   318
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
  "TKN_PRFXV4", "TKN_PRFXV4RNG", "TKN_IPV4", "TKN_DNS", "TKN_RTRSNAME",
  "TKN_PRNGNAME", "KEYW_ACTION", "KEYW_EXCEPT", "TKN_PREF", "TKN_MED",
  "TKN_DPA", "TKN_ASPATH", "TKN_COMMUNITY", "TKN_NEXT_HOP", "TKN_COST",
  "TKN_COMM_NO", "KEYW_IGP_COST", "KEYW_SELF", "KEYW_PREPEND",
  "KEYW_APPEND", "KEYW_DELETE", "KEYW_CONTAINS", "KEYW_AT",
  "KEYW_INTERNET", "KEYW_NO_EXPORT", "KEYW_NO_ADVERTISE", "KEYW_PROTOCOL",
  "TKN_PROTOCOL", "KEYW_INTO", "KEYW_REFINE", "KEYW_ACCEPT", "KEYW_FROM",
  "TKN_INT", "';'", "'{'", "'}'", "'('", "')'", "'.'", "','", "'<'", "'>'",
  "'|'", "'*'", "'?'", "'+'", "'^'", "'$'", "'['", "']'", "'-'", "$accept",
  "import_attribute", "opt_protocol_from", "opt_protocol_into",
  "import_expr", "import_term", "import_factor_list", "import_factor",
  "import_peering_action_list", "peering", "opt_action", "as_expr",
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
      59,   123,   125,    40,    41,    46,    44,    60,    62,   124,
      42,    63,    43,    94,    36,    91,    93,    45
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    68,    69,    69,    70,    70,    71,    71,    72,    72,
      72,    73,    73,    74,    74,    75,    76,    76,    77,    77,
      78,    78,    79,    79,    80,    80,    80,    81,    81,    82,
      82,    83,    83,    84,    84,    85,    85,    86,    86,    86,
      87,    87,    88,    88,    88,    89,    89,    90,    90,    91,
      91,    91,    91,    91,    91,    91,    92,    93,    93,    94,
      95,    96,    96,    97,    97,    97,    97,    97,    97,    97,
      98,    99,    99,   100,   100,   100,   100,   100,   101,   101,
     102,   103,   103,   103,   104,   104,   105,   105,   105,   106,
     106,   106,   106,   106,   107,   107,   108,   108,   108,   108,
     108,   109,   109,   110,   110,   111,   111,   112,   112,   113,
     113,   114,   114,   114,   114,   114,   115,   115,   115,   115,
     116,   116,   116,   116,   116,   116,   117,   117,   117,   117,
     117,   117
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     3,     3,     0,     2,     0,     2,     1,     3,
       3,     2,     3,     2,     3,     3,     3,     4,     3,     1,
       0,     2,     3,     1,     3,     3,     1,     3,     1,     1,
       1,     0,     1,     0,     2,     3,     1,     3,     3,     1,
       3,     1,     1,     1,     1,     1,     3,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       6,     1,     3,     3,     3,     6,     6,     6,     4,     3,
       3,     1,     3,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     2,     1,     3,     1,     2,     3,     1,     1,
       3,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       3,     0,     1,     1,     3,     1,     1,     3,     1,     2,
       1,     2,     2,     2,     2,     1,     1,     1,     3,     1,
       1,     1,     1,     1,     3,     4,     0,     2,     2,     2,
       4,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       4,     0,     0,     6,     5,     1,     0,     0,     7,     0,
       0,     2,     8,     3,     0,    29,    30,    19,     0,    20,
      31,    23,    26,    28,     0,     0,     0,     0,    11,     0,
       0,     0,     0,    16,     0,    42,    45,    44,     0,    33,
      32,    36,    39,    41,    43,     0,     0,    12,     0,    13,
      10,     0,     9,     0,    89,    97,    92,    96,    99,    98,
       0,     0,     0,     0,     0,     0,     0,   101,     0,     0,
      91,    49,    50,    51,    52,    53,    54,    55,    15,    83,
      85,    88,    93,    95,    20,    27,    21,     0,    22,     0,
       0,    18,     0,     0,     0,     0,    24,    25,    14,    86,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   105,   106,     0,   102,   103,     0,   121,   120,   122,
       0,   123,   116,   117,   126,     0,   108,   110,   115,   119,
       0,    82,     0,    94,    17,     0,    47,    40,    34,    35,
      37,    38,    46,    56,    58,    57,    59,     0,     0,    63,
      64,    69,    77,    73,    74,    75,    76,     0,    71,     0,
       0,     0,    78,    79,    80,   100,     0,    87,     0,   126,
       0,    90,     0,   109,   114,   111,   112,   113,    81,    84,
      48,     0,     0,    68,     0,     0,     0,     0,   104,   118,
       0,   128,   127,   131,   129,   124,   107,    61,     0,    70,
      72,     0,     0,     0,   125,     0,    60,     0,    65,    66,
      67,   130,    62
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     2,     3,     7,    11,    12,    24,    51,    14,    19,
      33,    20,    21,    22,    23,    39,    91,    40,    41,    42,
      43,    44,    86,    70,    71,    72,    73,    74,   198,    75,
     149,   157,   158,    76,    77,    78,    79,    80,    81,    82,
      83,   113,   114,   115,   125,   126,   127,   128,   129,   170
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -133
static const short int yypact[] =
{
     -26,    -1,    74,     7,  -133,  -133,    78,    86,  -133,    -2,
     107,  -133,     2,   126,   -16,  -133,  -133,  -133,    85,   105,
      66,    26,  -133,  -133,    -7,   134,    86,    86,  -133,   116,
      -2,     6,   133,  -133,    85,  -133,  -133,  -133,    83,   148,
     136,    87,  -133,  -133,   110,    85,    85,  -133,   138,  -133,
    -133,   126,  -133,   116,  -133,  -133,  -133,  -133,  -133,  -133,
     182,   183,   184,   137,    20,   186,   187,    96,   116,    52,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,    67,   191,
    -133,  -133,  -133,   190,   105,  -133,   133,   147,    26,    22,
      83,  -133,    83,    83,    83,   178,  -133,  -133,  -133,  -133,
     150,    51,   151,   166,   152,   152,   152,   108,   115,    91,
     153,  -133,  -133,   154,   149,  -133,     8,  -133,  -133,  -133,
      52,  -133,  -133,  -133,   141,   120,    52,    28,  -133,  -133,
     116,   191,   116,  -133,  -133,   157,  -133,  -133,   136,    87,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,   155,   108,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,    79,  -133,   156,
     158,   159,  -133,  -133,  -133,  -133,    96,  -133,   -10,  -133,
      -9,  -133,    52,    28,  -133,  -133,  -133,  -133,   191,  -133,
    -133,   196,    56,  -133,   108,   108,   108,   108,  -133,  -133,
      -8,  -133,   146,  -133,  -133,  -133,    52,  -133,   100,  -133,
    -133,   112,   118,   121,  -133,   200,  -133,   201,  -133,  -133,
    -133,  -133,  -133
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -133,  -133,  -133,  -133,   144,  -133,  -133,    99,  -133,   188,
     132,   199,   185,   135,  -133,  -133,  -133,   -23,   128,    89,
    -133,  -133,  -133,   -22,  -133,  -133,  -133,  -133,  -133,  -133,
      80,  -132,    37,  -133,  -133,   160,   -74,   -53,  -133,  -133,
    -133,  -133,  -133,    57,   102,    53,  -125,  -133,  -133,    55
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      99,   173,   191,   191,   131,   192,   192,   193,   193,    34,
      87,   130,    15,    53,    16,    89,   182,     1,    54,    55,
      17,    56,    57,    58,    59,    92,    26,   104,   105,   106,
      45,    29,    30,    60,    61,    62,    63,    64,    65,    66,
     174,     9,   131,     4,   189,    47,   194,   194,    27,   172,
      46,    18,     6,   201,   202,   203,   178,   195,   204,    67,
      85,    68,   167,   117,   135,    69,   118,   138,   119,    34,
     130,   173,    53,   107,     5,   108,   137,    54,    55,   179,
      56,    57,    58,    59,   144,    35,    36,    37,   175,   176,
     177,    93,    60,    61,    62,    63,    64,    65,    66,    15,
     145,    16,    35,    36,    37,   120,    13,   121,   199,    25,
     162,    94,   184,   111,   112,   122,   123,   124,    67,    38,
      68,    53,     8,    48,    69,   163,    54,    55,    32,    56,
      57,    58,    59,   183,     9,   184,    38,    10,    18,    92,
     152,    60,    61,    62,    63,    64,    65,    66,   153,   154,
     155,   159,   160,   161,   206,     9,   207,   156,    60,    61,
      62,    63,    64,    65,    66,    95,   208,    67,   184,    68,
      50,    52,   209,    69,   184,   210,    28,   184,   171,   172,
      96,    97,   140,   141,    49,   150,   151,    90,    98,   100,
     101,   102,   103,   109,   110,   132,   133,   136,   142,   143,
     146,   147,   164,   148,   169,   166,   165,   180,   181,   185,
     197,   186,   187,   205,   211,   212,   134,    31,    84,    88,
     139,   200,   168,   188,   190,   196,     0,     0,   116
};

static const short int yycheck[] =
{
      53,   126,    11,    11,    78,    14,    14,    16,    16,     3,
      32,     3,    14,     5,    16,    38,   148,    43,    10,    11,
      22,    13,    14,    15,    16,     3,    24,     7,     8,     9,
       4,    47,    48,    25,    26,    27,    28,    29,    30,    31,
      12,    48,   116,    44,    54,    52,    55,    55,    46,    59,
      24,    53,    45,   185,   186,   187,   130,    66,    66,    51,
      54,    53,    54,    11,    86,    57,    14,    90,    16,     3,
       3,   196,     5,    53,     0,    55,    54,    10,    11,   132,
      13,    14,    15,    16,    33,    19,    20,    21,    60,    61,
      62,     4,    25,    26,    27,    28,    29,    30,    31,    14,
      49,    16,    19,    20,    21,    53,     7,    55,    52,    10,
      19,    24,    56,    17,    18,    63,    64,    65,    51,    53,
      53,     5,    44,    24,    57,    34,    10,    11,    23,    13,
      14,    15,    16,    54,    48,    56,    53,    51,    53,     3,
      32,    25,    26,    27,    28,    29,    30,    31,    40,    41,
      42,    36,    37,    38,    54,    48,    56,    49,    25,    26,
      27,    28,    29,    30,    31,    55,    54,    51,    56,    53,
      26,    27,    54,    57,    56,    54,    50,    56,    58,    59,
      45,    46,    93,    94,    50,   105,   106,    39,    50,     7,
       7,     7,    55,     7,     7,     4,     6,    50,    20,    49,
      49,    35,    49,    51,    63,    56,    52,    50,    53,    53,
      14,    53,    53,    67,    14,    14,    84,    18,    30,    34,
      92,   184,   120,   166,   169,   172,    -1,    -1,    68
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    43,    69,    70,    44,     0,    45,    71,    44,    48,
      51,    72,    73,    75,    76,    14,    16,    22,    53,    77,
      79,    80,    81,    82,    74,    75,    24,    46,    50,    47,
      48,    79,    23,    78,     3,    19,    20,    21,    53,    83,
      85,    86,    87,    88,    89,     4,    24,    52,    75,    50,
      72,    75,    72,     5,    10,    11,    13,    14,    15,    16,
      25,    26,    27,    28,    29,    30,    31,    51,    53,    57,
      91,    92,    93,    94,    95,    97,   101,   102,   103,   104,
     105,   106,   107,   108,    77,    54,    90,    91,    80,    85,
      39,    84,     3,     4,    24,    55,    81,    81,    50,   105,
       7,     7,     7,    55,     7,     8,     9,    53,    55,     7,
       7,    17,    18,   109,   110,   111,   103,    11,    14,    16,
      53,    55,    63,    64,    65,   112,   113,   114,   115,   116,
       3,   104,     4,     6,    78,    91,    50,    54,    85,    86,
      87,    87,    20,    49,    33,    49,    49,    35,    51,    98,
      98,    98,    32,    40,    41,    42,    49,    99,   100,    36,
      37,    38,    19,    34,    49,    52,    56,    54,   112,    63,
     117,    58,    59,   114,    12,    60,    61,    62,   104,   105,
      50,    53,    99,    54,    56,    53,    53,    53,   111,    54,
     117,    11,    14,    16,    55,    66,   113,    14,    96,    52,
     100,    99,    99,    99,    66,    67,    54,    56,    54,    54,
      54,    14,    14
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
        case 43:
#line 127 "import.y"
    {
    if (strlen(yyvsp[0].sval) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", yyvsp[0].sval);
    }
;}
    break;

  case 56:
#line 152 "import.y"
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

  case 57:
#line 163 "import.y"
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

  case 59:
#line 175 "import.y"
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

  case 76:
#line 212 "import.y"
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

  case 80:
#line 229 "import.y"
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
#line 1406 "import.tab.c"

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


#line 321 "import.y"


#undef importerror
#undef yyerror

int
importerror (const char *s)
{
    yyerror(s);
    return 0;
}


