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
#define yyparse componentsparse
#define yylex   componentslex
#define yyerror componentserror
#define yylval  componentslval
#define yychar  componentschar
#define yydebug componentsdebug
#define yynerrs componentsnerrs


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
     KEYW_ATOMIC = 274,
     KEYW_PROTOCOL = 275,
     TKN_PROTOCOL = 276,
     KEYW_IGP_COST = 277,
     KEYW_SELF = 278,
     KEYW_PREPEND = 279,
     KEYW_APPEND = 280,
     KEYW_DELETE = 281,
     KEYW_CONTAINS = 282,
     KEYW_INTERNET = 283,
     KEYW_NO_EXPORT = 284,
     KEYW_NO_ADVERTISE = 285,
     TKN_PREF = 286,
     TKN_MED = 287,
     TKN_DPA = 288,
     TKN_ASPATH = 289,
     TKN_COMMUNITY = 290,
     TKN_NEXT_HOP = 291,
     TKN_COST = 292,
     TKN_COMM_NO = 293,
     TKN_IPV4 = 294,
     TKN_INT = 295
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
#define KEYW_ATOMIC 274
#define KEYW_PROTOCOL 275
#define TKN_PROTOCOL 276
#define KEYW_IGP_COST 277
#define KEYW_SELF 278
#define KEYW_PREPEND 279
#define KEYW_APPEND 280
#define KEYW_DELETE 281
#define KEYW_CONTAINS 282
#define KEYW_INTERNET 283
#define KEYW_NO_EXPORT 284
#define KEYW_NO_ADVERTISE 285
#define TKN_PREF 286
#define TKN_MED 287
#define TKN_DPA 288
#define TKN_ASPATH 289
#define TKN_COMMUNITY 290
#define TKN_NEXT_HOP 291
#define TKN_COST 292
#define TKN_COMM_NO 293
#define TKN_IPV4 294
#define TKN_INT 295




/* Copy the first part of user declarations.  */
#line 1 "components.y"

/*
  filename: components.y

  description:
    Defines the grammar for an RPSL component attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Defines tokens for the associated lexer, components.l.
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
#line 16 "components.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 184 "components.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 34 "components.y"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int yyerror(const char *s);

/* defines for unit testing */
#ifdef UNIT_TEST
int main();
#endif /* UNIT_TEST */


/* Line 214 of yacc.c.  */
#line 208 "components.tab.c"

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
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   185

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  58
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  31
/* YYNRULES -- Number of rules. */
#define YYNRULES  90
/* YYNRULES -- Number of states. */
#define YYNSTATES  144

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   295

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    53,     2,     2,     2,
      41,    42,    49,    51,    47,    57,    54,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      43,     2,    44,    50,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    55,     2,    56,    52,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    45,    48,    46,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,     7,     9,    10,    12,    17,    21,
      24,    26,    30,    32,    35,    39,    41,    43,    47,    49,
      51,    53,    56,    58,    60,    62,    64,    66,    70,    71,
      73,    75,    79,    81,    83,    87,    89,    92,    94,    97,
     100,   103,   106,   108,   110,   112,   116,   118,   120,   122,
     124,   126,   130,   135,   136,   139,   142,   145,   150,   153,
     155,   157,   159,   161,   163,   165,   167,   171,   175,   179,
     183,   190,   192,   196,   200,   204,   211,   218,   225,   230,
     234,   238,   240,   244,   246,   248,   250,   252,   254,   258,
     262
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      59,     0,    -1,    60,    61,    -1,    -1,    19,    -1,    -1,
      62,    -1,    61,    20,    21,    62,    -1,    62,     3,    63,
      -1,    62,    63,    -1,    63,    -1,    63,     4,    64,    -1,
      64,    -1,     5,    64,    -1,    41,    62,    42,    -1,    65,
      -1,    10,    -1,    43,    71,    44,    -1,    77,    -1,    13,
      -1,    66,    -1,    67,     6,    -1,    67,    -1,    14,    -1,
      11,    -1,    16,    -1,    15,    -1,    45,    68,    46,    -1,
      -1,    69,    -1,    70,    -1,    69,    47,    70,    -1,    17,
      -1,    18,    -1,    71,    48,    72,    -1,    72,    -1,    72,
      73,    -1,    73,    -1,    73,    49,    -1,    73,    50,    -1,
      73,    51,    -1,    73,    12,    -1,    74,    -1,    52,    -1,
      53,    -1,    41,    71,    42,    -1,    75,    -1,    14,    -1,
      11,    -1,    16,    -1,    54,    -1,    55,    76,    56,    -1,
      55,    52,    76,    56,    -1,    -1,    76,    14,    -1,    76,
      11,    -1,    76,    54,    -1,    76,    14,    57,    14,    -1,
      76,    16,    -1,    78,    -1,    79,    -1,    80,    -1,    81,
      -1,    83,    -1,    87,    -1,    88,    -1,    31,     7,    40,
      -1,    32,     7,    40,    -1,    32,     7,    22,    -1,    33,
       7,    40,    -1,    34,    54,    24,    41,    82,    42,    -1,
      14,    -1,    82,    47,    14,    -1,    35,     7,    84,    -1,
      35,     8,    84,    -1,    35,    54,    25,    41,    85,    42,
      -1,    35,    54,    26,    41,    85,    42,    -1,    35,    54,
      27,    41,    85,    42,    -1,    35,    41,    85,    42,    -1,
      35,     9,    84,    -1,    45,    85,    46,    -1,    86,    -1,
      85,    47,    86,    -1,    28,    -1,    29,    -1,    30,    -1,
      40,    -1,    38,    -1,    36,     7,    39,    -1,    36,     7,
      23,    -1,    37,     7,    40,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    49,    49,    52,    53,    56,    57,    58,    61,    62,
      63,    66,    67,    70,    71,    72,    75,    76,    77,    78,
      79,    82,    83,    86,    87,    88,    89,    90,    93,    94,
      97,    98,   101,   102,   105,   106,   109,   110,   113,   114,
     115,   116,   117,   120,   121,   122,   123,   126,   127,   128,
     129,   130,   131,   134,   135,   136,   137,   138,   139,   142,
     143,   144,   145,   146,   147,   148,   151,   162,   171,   174,
     185,   188,   189,   192,   193,   194,   195,   196,   197,   198,
     201,   204,   205,   208,   209,   210,   211,   221,   224,   225,
     228
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
  "TKN_PRFXV4", "TKN_PRFXV4RNG", "KEYW_ATOMIC", "KEYW_PROTOCOL",
  "TKN_PROTOCOL", "KEYW_IGP_COST", "KEYW_SELF", "KEYW_PREPEND",
  "KEYW_APPEND", "KEYW_DELETE", "KEYW_CONTAINS", "KEYW_INTERNET",
  "KEYW_NO_EXPORT", "KEYW_NO_ADVERTISE", "TKN_PREF", "TKN_MED", "TKN_DPA",
  "TKN_ASPATH", "TKN_COMMUNITY", "TKN_NEXT_HOP", "TKN_COST", "TKN_COMM_NO",
  "TKN_IPV4", "TKN_INT", "'('", "')'", "'<'", "'>'", "'{'", "'}'", "','",
  "'|'", "'*'", "'?'", "'+'", "'^'", "'$'", "'.'", "'['", "']'", "'-'",
  "$accept", "components", "opt_atomic", "components_list", "filter",
  "filter_term", "filter_factor", "filter_operand", "filter_prefix",
  "filter_prefix_operand", "opt_filter_prefix_list", "filter_prefix_list",
  "filter_prefix_list_prefix", "filter_aspath", "filter_aspath_term",
  "filter_aspath_closure", "filter_aspath_factor", "filter_aspath_no",
  "filter_aspath_range", "rp_attribute", "pref", "med", "dpa", "aspath",
  "asno_list", "community", "community_list", "community_elm_list",
  "community_elm", "next_hop", "cost", 0
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
     295,    40,    41,    60,    62,   123,   125,    44,   124,    42,
      63,    43,    94,    36,    46,    91,    93,    45
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    58,    59,    60,    60,    61,    61,    61,    62,    62,
      62,    63,    63,    64,    64,    64,    65,    65,    65,    65,
      65,    66,    66,    67,    67,    67,    67,    67,    68,    68,
      69,    69,    70,    70,    71,    71,    72,    72,    73,    73,
      73,    73,    73,    74,    74,    74,    74,    75,    75,    75,
      75,    75,    75,    76,    76,    76,    76,    76,    76,    77,
      77,    77,    77,    77,    77,    77,    78,    79,    79,    80,
      81,    82,    82,    83,    83,    83,    83,    83,    83,    83,
      84,    85,    85,    86,    86,    86,    86,    86,    87,    87,
      88
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     0,     1,     0,     1,     4,     3,     2,
       1,     3,     1,     2,     3,     1,     1,     3,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     3,     0,     1,
       1,     3,     1,     1,     3,     1,     2,     1,     2,     2,
       2,     2,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     3,     4,     0,     2,     2,     2,     4,     2,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       6,     1,     3,     3,     3,     6,     6,     6,     4,     3,
       3,     1,     3,     1,     1,     1,     1,     1,     3,     3,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       3,     4,     0,     5,     1,     0,    16,    24,    19,    23,
      26,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    28,     2,     6,    10,    12,    15,    20,    22,    18,
      59,    60,    61,    62,    63,    64,    65,    13,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,    47,    49,     0,    43,    44,    50,    53,     0,    35,
      37,    42,    46,    32,    33,     0,    29,    30,     0,     0,
       9,     0,    21,    66,    68,    67,    69,     0,     0,    73,
      74,    79,    83,    84,    85,    87,    86,     0,    81,     0,
       0,     0,    89,    88,    90,    14,     0,    53,     0,    17,
       0,    36,    41,    38,    39,    40,    27,     0,     0,     8,
      11,     0,     0,    78,     0,     0,     0,     0,    45,     0,
      55,    54,    58,    56,    51,    34,    31,     7,    71,     0,
      80,    82,     0,     0,     0,    52,     0,    70,     0,    75,
      76,    77,    57,    72
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     2,     3,    22,    23,    24,    25,    26,    27,    28,
      65,    66,    67,    58,    59,    60,    61,    62,    98,    29,
      30,    31,    32,    33,   129,    34,    79,    87,    88,    35,
      36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -67
static const short int yypact[] =
{
       2,   -67,    34,   140,   -67,   140,   -67,   -67,   -67,   -67,
     -67,   -67,    31,    38,    41,     3,    -2,    57,    65,   140,
       6,     1,    24,   103,    63,   -67,   -67,   -67,    82,   -67,
     -67,   -67,   -67,   -67,   -67,   -67,   -67,   -67,    58,   -13,
      59,    76,    62,    62,    62,    49,    55,   -15,    69,    60,
     -67,   -67,   -67,     6,   -67,   -67,   -67,    68,    18,     6,
      -8,   -67,   -67,   -67,   -67,    64,    74,   -67,    90,   140,
      63,   140,   -67,   -67,   -67,   -67,   -67,    71,    49,   -67,
     -67,   -67,   -67,   -67,   -67,   -67,   -67,   -17,   -67,    81,
      83,    84,   -67,   -67,   -67,   -67,   -19,   -67,    -1,   -67,
       6,    -8,   -67,   -67,   -67,   -67,   -67,     1,   140,    63,
     -67,   101,    37,   -67,    49,    49,    49,    49,   -67,     0,
     -67,    66,   -67,   -67,   -67,     6,   -67,   103,   -67,   -14,
     -67,   -67,   -11,   -10,    -7,   -67,   112,   -67,   113,   -67,
     -67,   -67,   -67,   -67
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -67,   -67,   -67,   -67,   -18,   -23,    -3,   -67,   -67,   -67,
     -67,   -67,    21,    77,    29,   -56,   -67,   -67,    35,   -67,
     -67,   -67,   -67,   -67,   -67,   -67,    42,   -66,    17,   -67,
     -67
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      70,    49,    37,   101,   102,    42,    43,    44,    92,    74,
     120,   120,   112,   121,   121,   122,   122,    50,    63,    64,
      51,     1,    52,   118,    93,   113,    70,    75,   137,   100,
     114,   139,   140,   138,     4,   141,   114,   114,    38,    45,
     114,   103,   104,   105,    68,    39,   109,    53,    40,   132,
     133,   134,    46,   123,   123,   124,   135,    41,    54,    55,
      56,    57,    99,    69,    47,     5,   100,    71,   110,   101,
       6,     7,    48,     8,     9,    10,    11,    82,    83,    84,
      89,    90,    91,   130,   114,    80,    81,    85,    72,    86,
     127,    12,    13,    14,    15,    16,    17,    18,    73,    76,
      77,    19,    95,    20,    70,    21,    69,    78,     5,    94,
     106,   108,   111,     6,     7,   128,     8,     9,    10,    11,
      97,   107,   115,   136,   116,   117,   142,   143,   126,   125,
      96,   131,   119,     0,    12,    13,    14,    15,    16,    17,
      18,     0,     0,     0,    19,     5,    20,     0,    21,     0,
       6,     7,     0,     8,     9,    10,    11,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    12,    13,    14,    15,    16,    17,    18,     0,     0,
       0,    19,     0,    20,     0,    21
};

static const yysigned_char yycheck[] =
{
      23,    19,     5,    59,    12,     7,     8,     9,    23,    22,
      11,    11,    78,    14,    14,    16,    16,    11,    17,    18,
      14,    19,    16,    42,    39,    42,    49,    40,    42,    48,
      47,    42,    42,    47,     0,    42,    47,    47,     7,    41,
      47,    49,    50,    51,    20,     7,    69,    41,     7,   115,
     116,   117,    54,    54,    54,    56,    56,    54,    52,    53,
      54,    55,    44,     3,     7,     5,    48,     4,    71,   125,
      10,    11,     7,    13,    14,    15,    16,    28,    29,    30,
      25,    26,    27,    46,    47,    43,    44,    38,     6,    40,
     108,    31,    32,    33,    34,    35,    36,    37,    40,    40,
      24,    41,    42,    43,   127,    45,     3,    45,     5,    40,
      46,    21,    41,    10,    11,    14,    13,    14,    15,    16,
      52,    47,    41,    57,    41,    41,    14,    14,   107,   100,
      53,   114,    97,    -1,    31,    32,    33,    34,    35,    36,
      37,    -1,    -1,    -1,    41,     5,    43,    -1,    45,    -1,
      10,    11,    -1,    13,    14,    15,    16,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    32,    33,    34,    35,    36,    37,    -1,    -1,
      -1,    41,    -1,    43,    -1,    45
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    19,    59,    60,     0,     5,    10,    11,    13,    14,
      15,    16,    31,    32,    33,    34,    35,    36,    37,    41,
      43,    45,    61,    62,    63,    64,    65,    66,    67,    77,
      78,    79,    80,    81,    83,    87,    88,    64,     7,     7,
       7,    54,     7,     8,     9,    41,    54,     7,     7,    62,
      11,    14,    16,    41,    52,    53,    54,    55,    71,    72,
      73,    74,    75,    17,    18,    68,    69,    70,    20,     3,
      63,     4,     6,    40,    22,    40,    40,    24,    45,    84,
      84,    84,    28,    29,    30,    38,    40,    85,    86,    25,
      26,    27,    23,    39,    40,    42,    71,    52,    76,    44,
      48,    73,    12,    49,    50,    51,    46,    47,    21,    63,
      64,    41,    85,    42,    47,    41,    41,    41,    42,    76,
      11,    14,    16,    54,    56,    72,    70,    62,    14,    82,
      46,    86,    85,    85,    85,    56,    57,    42,    47,    42,
      42,    42,    14,    14
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
        case 66:
#line 151 "components.y"
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

  case 67:
#line 162 "components.y"
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

  case 69:
#line 174 "components.y"
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

  case 86:
#line 211 "components.y"
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

  case 90:
#line 228 "components.y"
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
#line 1314 "components.tab.c"

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


#line 239 "components.y"


/* define a unit test that simply returns "yes" or "no" for filter input */
#ifndef UNIT_TEST

#undef componentserror
#undef yyerror

int
componentserror (const char *s)
{
    yyerror(s);
    return 0;
}
#else /* UNIT_TEST */

int parse_error_count;

/* parse error */
void 
yyerror(const char *s)
{
    fputs(s, stderr);
    putc('\n', stderr);
    parse_error_count++;
}

/* syntax error */
void 
syntax_error(const char *fmt, ...)
{
    va_list args;
    char buf[1024];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    yyerror(buf);
}

int
yywrap()
{
    return 1;
}

/* taken from _lex & yacc_ p.157, by Levine, Mason, & Brown (corrected) */
char *myinputptr;
char *myinputlim;

#undef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))

void
yy_input(char *buf, int *result, int max_size)
{
    int n = min(max_size, myinputlim - myinputptr);

    if (n > 0) {
        memcpy(buf, myinputptr, n);
        myinputptr += n;
    }
    *result = n;
}

/* program entry point */
int 
main()
{
    char buf[4096];

    setbuf(stdout, NULL);
    for (;;) {
        printf("filter> ");
        if (gets(buf) == NULL) {
            printf("\n");
            return(0);
        }
        parse_error_count = 0;
        myinputptr = buf;
        myinputlim = buf + strlen(buf);
        filterparse();
        printf("%d parse errors\n", parse_error_count);
    }
}
#endif /* UNIT_TEST */

