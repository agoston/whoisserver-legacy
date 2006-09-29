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
#define yyparse defaultparse
#define yylex   defaultlex
#define yyerror defaulterror
#define yylval  defaultlval
#define yychar  defaultchar
#define yydebug defaultdebug
#define yynerrs defaultnerrs


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
     KEYW_TO = 278,
     KEYW_ACTION = 279,
     KEYW_NETWORKS = 280,
     KEYW_EXCEPT = 281,
     TKN_PREF = 282,
     TKN_MED = 283,
     TKN_DPA = 284,
     TKN_ASPATH = 285,
     TKN_COMMUNITY = 286,
     TKN_NEXT_HOP = 287,
     TKN_COST = 288,
     TKN_COMM_NO = 289,
     KEYW_IGP_COST = 290,
     KEYW_SELF = 291,
     KEYW_PREPEND = 292,
     KEYW_APPEND = 293,
     KEYW_DELETE = 294,
     KEYW_CONTAINS = 295,
     KEYW_AT = 296,
     KEYW_INTERNET = 297,
     KEYW_NO_EXPORT = 298,
     KEYW_NO_ADVERTISE = 299,
     TKN_INT = 300
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
#define KEYW_TO 278
#define KEYW_ACTION 279
#define KEYW_NETWORKS 280
#define KEYW_EXCEPT 281
#define TKN_PREF 282
#define TKN_MED 283
#define TKN_DPA 284
#define TKN_ASPATH 285
#define TKN_COMMUNITY 286
#define TKN_NEXT_HOP 287
#define TKN_COST 288
#define TKN_COMM_NO 289
#define KEYW_IGP_COST 290
#define KEYW_SELF 291
#define KEYW_PREPEND 292
#define KEYW_APPEND 293
#define KEYW_DELETE 294
#define KEYW_CONTAINS 295
#define KEYW_AT 296
#define KEYW_INTERNET 297
#define KEYW_NO_EXPORT 298
#define KEYW_NO_ADVERTISE 299
#define TKN_INT 300




/* Copy the first part of user declarations.  */
#line 1 "default.y"

/*
  filename: default.y

  description:
    Defines the grammar for an RPSL default attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Defines tokens for the associated lexer, default.l.
*/

/******************
  Copyright (c) 2002                              RIPE NCC

  All Rights Reserved

  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation, and that the name of the author not be
  used in advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
  AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
  DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  ***************************************/



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
#line 38 "default.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 216 "default.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 56 "default.y"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int yyerror(const char *s);
void syntax_error(char *fmt, ...);



/* Line 214 of yacc.c.  */
#line 237 "default.tab.c"

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
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   205

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  64
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  44
/* YYNRULES -- Number of rules. */
#define YYNRULES  118
/* YYNRULES -- Number of states. */
#define YYNSTATES  188

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   300

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    60,     2,     2,     2,
      46,    47,    56,    58,    50,    63,    48,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    49,
      53,     2,    54,    57,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    61,     2,    62,    59,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    51,    55,    52,     2,     2,     2,     2,
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
      45
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     8,    12,    14,    15,    18,    19,    22,
      26,    28,    32,    36,    38,    42,    44,    46,    48,    49,
      51,    52,    55,    59,    61,    65,    69,    71,    75,    77,
      79,    81,    83,    85,    89,    92,    96,    98,   100,   102,
     104,   106,   108,   110,   114,   118,   122,   126,   133,   135,
     139,   143,   147,   154,   161,   168,   173,   177,   181,   183,
     187,   189,   191,   193,   195,   197,   201,   205,   209,   213,
     216,   218,   222,   224,   227,   231,   233,   235,   239,   241,
     243,   245,   248,   250,   252,   254,   256,   258,   262,   263,
     265,   267,   271,   273,   275,   279,   281,   284,   286,   289,
     292,   295,   298,   300,   302,   304,   308,   310,   312,   314,
     316,   318,   322,   327,   328,   331,   334,   337,   342
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      65,     0,    -1,    23,    66,    67,    68,    -1,    69,    73,
      74,    -1,    22,    -1,    -1,    24,    80,    -1,    -1,    25,
      93,    -1,    69,     3,    70,    -1,    70,    -1,    70,     4,
      71,    -1,    70,    26,    71,    -1,    71,    -1,    46,    69,
      47,    -1,    72,    -1,    14,    -1,    16,    -1,    -1,    75,
      -1,    -1,    41,    75,    -1,    75,     3,    76,    -1,    76,
      -1,    76,     4,    77,    -1,    76,    26,    77,    -1,    77,
      -1,    46,    75,    47,    -1,    78,    -1,    19,    -1,    79,
      -1,    21,    -1,    20,    -1,    79,    48,    20,    -1,    81,
      49,    -1,    80,    81,    49,    -1,    82,    -1,    83,    -1,
      84,    -1,    85,    -1,    87,    -1,    91,    -1,    92,    -1,
      27,     7,    45,    -1,    28,     7,    45,    -1,    28,     7,
      35,    -1,    29,     7,    45,    -1,    30,    48,    37,    46,
      86,    47,    -1,    14,    -1,    86,    50,    14,    -1,    31,
       7,    88,    -1,    31,     8,    88,    -1,    31,    48,    38,
      46,    89,    47,    -1,    31,    48,    39,    46,    89,    47,
      -1,    31,    48,    40,    46,    89,    47,    -1,    31,    46,
      89,    47,    -1,    31,     9,    88,    -1,    51,    89,    52,
      -1,    90,    -1,    89,    50,    90,    -1,    42,    -1,    43,
      -1,    44,    -1,    45,    -1,    34,    -1,    32,     7,    19,
      -1,    32,     7,    36,    -1,    33,     7,    45,    -1,    93,
       3,    94,    -1,    93,    94,    -1,    94,    -1,    94,     4,
      95,    -1,    95,    -1,     5,    95,    -1,    46,    93,    47,
      -1,    96,    -1,    10,    -1,    53,   102,    54,    -1,    81,
      -1,    13,    -1,    97,    -1,    98,     6,    -1,    98,    -1,
      14,    -1,    11,    -1,    16,    -1,    15,    -1,    51,    99,
      52,    -1,    -1,   100,    -1,   101,    -1,   100,    50,   101,
      -1,    17,    -1,    18,    -1,   102,    55,   103,    -1,   103,
      -1,   103,   104,    -1,   104,    -1,   104,    56,    -1,   104,
      57,    -1,   104,    58,    -1,   104,    12,    -1,   105,    -1,
      59,    -1,    60,    -1,    46,   102,    47,    -1,   106,    -1,
      14,    -1,    11,    -1,    16,    -1,    48,    -1,    61,   107,
      62,    -1,    61,    59,   107,    62,    -1,    -1,   107,    14,
      -1,   107,    11,    -1,   107,    48,    -1,   107,    14,    63,
      14,    -1,   107,    16,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    68,    68,    71,    72,    75,    76,    79,    80,    83,
      84,    87,    88,    89,    92,    93,    96,    97,   100,   101,
     104,   105,   108,   109,   112,   113,   114,   117,   118,   121,
     122,   127,   130,   131,   134,   135,   138,   139,   140,   141,
     142,   143,   144,   147,   158,   167,   170,   181,   184,   185,
     188,   189,   190,   191,   192,   193,   194,   197,   200,   201,
     204,   205,   206,   207,   217,   220,   221,   224,   235,   236,
     237,   240,   241,   244,   245,   246,   249,   250,   251,   252,
     253,   256,   257,   260,   261,   262,   263,   264,   267,   268,
     271,   272,   275,   276,   279,   280,   283,   284,   287,   288,
     289,   290,   291,   294,   295,   296,   297,   300,   301,   302,
     303,   304,   305,   308,   309,   310,   311,   312,   313
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
  "TKN_PRNGNAME", "KEYW_TO", "KEYW_ACTION", "KEYW_NETWORKS", "KEYW_EXCEPT",
  "TKN_PREF", "TKN_MED", "TKN_DPA", "TKN_ASPATH", "TKN_COMMUNITY",
  "TKN_NEXT_HOP", "TKN_COST", "TKN_COMM_NO", "KEYW_IGP_COST", "KEYW_SELF",
  "KEYW_PREPEND", "KEYW_APPEND", "KEYW_DELETE", "KEYW_CONTAINS", "KEYW_AT",
  "KEYW_INTERNET", "KEYW_NO_EXPORT", "KEYW_NO_ADVERTISE", "TKN_INT", "'('",
  "')'", "'.'", "';'", "','", "'{'", "'}'", "'<'", "'>'", "'|'", "'*'",
  "'?'", "'+'", "'^'", "'$'", "'['", "']'", "'-'", "$accept",
  "default_rule", "peering", "opt_action", "opt_default_filter", "as_expr",
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
     295,   296,   297,   298,   299,   300,    40,    41,    46,    59,
      44,   123,   125,    60,    62,   124,    42,    63,    43,    94,
      36,    91,    93,    45
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    64,    65,    66,    66,    67,    67,    68,    68,    69,
      69,    70,    70,    70,    71,    71,    72,    72,    73,    73,
      74,    74,    75,    75,    76,    76,    76,    77,    77,    78,
      78,    78,    79,    79,    80,    80,    81,    81,    81,    81,
      81,    81,    81,    82,    83,    83,    84,    85,    86,    86,
      87,    87,    87,    87,    87,    87,    87,    88,    89,    89,
      90,    90,    90,    90,    90,    91,    91,    92,    93,    93,
      93,    94,    94,    95,    95,    95,    96,    96,    96,    96,
      96,    97,    97,    98,    98,    98,    98,    98,    99,    99,
     100,   100,   101,   101,   102,   102,   103,   103,   104,   104,
     104,   104,   104,   105,   105,   105,   105,   106,   106,   106,
     106,   106,   106,   107,   107,   107,   107,   107,   107
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     4,     3,     1,     0,     2,     0,     2,     3,
       1,     3,     3,     1,     3,     1,     1,     1,     0,     1,
       0,     2,     3,     1,     3,     3,     1,     3,     1,     1,
       1,     1,     1,     3,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     6,     1,     3,
       3,     3,     6,     6,     6,     4,     3,     3,     1,     3,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     2,
       1,     3,     1,     2,     3,     1,     1,     3,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     3,     0,     1,
       1,     3,     1,     1,     3,     1,     2,     1,     2,     2,
       2,     2,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     3,     4,     0,     2,     2,     2,     4,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,    16,    17,     4,     0,     5,    18,    10,
      13,    15,     1,     0,     0,     7,     0,    29,    32,    31,
       0,    20,    19,    23,    26,    28,    30,     0,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     6,     0,    36,
      37,    38,    39,    40,    41,    42,     0,     2,     9,     0,
       0,     3,     0,     0,     0,     0,    11,    12,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      34,     0,    76,    84,    79,    83,    86,    85,     0,    88,
       0,    78,     8,    70,    72,    75,    80,    82,    27,    21,
      22,    24,    25,    33,    43,    45,    44,    46,     0,     0,
      50,    51,    56,    64,    60,    61,    62,    63,     0,    58,
       0,     0,     0,    65,    66,    67,    35,    73,     0,    92,
      93,     0,    89,    90,   108,   107,   109,     0,   110,   103,
     104,   113,     0,    95,    97,   102,   106,     0,    69,     0,
      81,     0,     0,    55,     0,     0,     0,     0,    74,    87,
       0,     0,   113,     0,    77,     0,    96,   101,    98,    99,
     100,    68,    71,    48,     0,    57,    59,     0,     0,     0,
      91,   105,     0,   115,   114,   118,   116,   111,    94,    47,
       0,    52,    53,    54,   112,     0,    49,   117
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     2,     7,    15,    47,     8,     9,    10,    11,    21,
      51,    22,    23,    24,    25,    26,    37,    81,    39,    40,
      41,    42,   164,    43,   100,   108,   109,    44,    45,    82,
      83,    84,    85,    86,    87,   121,   122,   123,   132,   133,
     134,   135,   136,   153
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -124
static const short int yypact[] =
{
      -3,   122,    26,  -124,  -124,  -124,     2,     7,   111,     8,
    -124,  -124,  -124,    19,   123,    24,     2,  -124,  -124,  -124,
      63,   101,    68,    10,  -124,  -124,    37,     2,     2,  -124,
     151,   154,   158,   119,    87,   168,   169,   123,   100,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,    88,  -124,     8,    30,
      63,  -124,    63,    63,    63,   157,  -124,  -124,   133,   -24,
     134,   143,   130,   130,   130,   103,    89,    44,   137,   135,
    -124,    88,  -124,  -124,  -124,  -124,  -124,  -124,    88,   105,
      65,  -124,    59,   179,  -124,  -124,  -124,   180,  -124,    68,
      10,  -124,  -124,  -124,  -124,  -124,  -124,  -124,   139,   103,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,   -12,  -124,
     141,   144,   145,  -124,  -124,  -124,  -124,  -124,    14,  -124,
    -124,   136,   146,  -124,  -124,  -124,  -124,    65,  -124,  -124,
    -124,   138,    52,    65,     1,  -124,  -124,    88,   179,    88,
    -124,   175,   114,  -124,   103,   103,   103,   103,  -124,  -124,
     105,   -32,  -124,    -9,  -124,    65,     1,  -124,  -124,  -124,
    -124,   179,  -124,  -124,    50,  -124,  -124,    93,   112,   113,
    -124,  -124,    -8,  -124,   129,  -124,  -124,  -124,    65,  -124,
     181,  -124,  -124,  -124,  -124,   184,  -124,  -124
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -124,  -124,  -124,  -124,  -124,   187,   178,   142,  -124,  -124,
    -124,    58,   147,   118,  -124,  -124,  -124,    -5,  -124,  -124,
    -124,  -124,  -124,  -124,   110,   -95,    56,  -124,  -124,   124,
     -81,   -71,  -124,  -124,  -124,  -124,  -124,    51,    76,    49,
    -123,  -124,  -124,    53
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
     117,   138,   173,   173,   142,   174,   174,   175,   175,    38,
     156,    95,    27,   157,    53,   171,     3,   137,     4,    71,
       1,    96,    16,   155,    72,    73,    12,    74,    75,    76,
      77,    14,    69,    52,    28,   143,    54,   138,   144,   176,
     176,    30,    31,    32,    33,    34,    35,    36,     6,    46,
     167,   168,   169,   177,   184,   156,   161,   158,   159,   160,
      78,   148,   137,   113,    71,    79,    29,    80,   162,    72,
      73,    52,    74,    75,    76,    77,   124,    88,    49,   125,
     114,   126,    17,    18,    19,    55,    30,    31,    32,    33,
      34,    35,    36,    71,    62,    63,    64,   179,    72,    73,
     180,    74,    75,    76,    77,    78,   154,   155,    89,    20,
      79,   127,    80,   128,    16,    30,    31,    32,    33,    34,
      35,    36,   119,   120,   129,   130,   131,   110,   111,   112,
      17,    18,    19,    65,    78,    66,     3,   103,     4,    79,
     181,    80,    50,   144,     5,   104,   105,   106,   107,    70,
      30,    31,    32,    33,    34,    35,    36,    20,    58,   182,
     183,    59,   144,   144,   144,    60,   165,    61,     6,    56,
      57,    91,    92,   101,   102,    67,    68,    93,    94,    97,
      98,    99,   115,   139,   116,   141,   140,   145,   149,   163,
     146,   147,   185,    13,    48,   186,   150,   152,   187,    90,
     166,   170,   118,   151,   178,   172
};

static const unsigned char yycheck[] =
{
      71,    82,    11,    11,    99,    14,    14,    16,    16,    14,
     133,    35,     4,    12,     4,    47,    14,     3,    16,     5,
      23,    45,     3,    55,    10,    11,     0,    13,    14,    15,
      16,    24,    37,     3,    26,    47,    26,   118,    50,    48,
      48,    27,    28,    29,    30,    31,    32,    33,    46,    25,
     145,   146,   147,    62,    62,   178,   137,    56,    57,    58,
      46,    47,     3,    19,     5,    51,    47,    53,   139,    10,
      11,     3,    13,    14,    15,    16,    11,    47,    20,    14,
      36,    16,    19,    20,    21,    48,    27,    28,    29,    30,
      31,    32,    33,     5,     7,     8,     9,    47,    10,    11,
      50,    13,    14,    15,    16,    46,    54,    55,    50,    46,
      51,    46,    53,    48,     3,    27,    28,    29,    30,    31,
      32,    33,    17,    18,    59,    60,    61,    38,    39,    40,
      19,    20,    21,    46,    46,    48,    14,    34,    16,    51,
      47,    53,    41,    50,    22,    42,    43,    44,    45,    49,
      27,    28,    29,    30,    31,    32,    33,    46,     7,    47,
      47,     7,    50,    50,    50,     7,    52,    48,    46,    27,
      28,    53,    54,    63,    64,     7,     7,    20,    45,    45,
      37,    51,    45,     4,    49,    46,     6,    46,    52,    14,
      46,    46,    63,     6,    16,    14,    50,    59,    14,    52,
     144,   150,    78,   127,   155,   152
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    23,    65,    14,    16,    22,    46,    66,    69,    70,
      71,    72,     0,    69,    24,    67,     3,    19,    20,    21,
      46,    73,    75,    76,    77,    78,    79,     4,    26,    47,
      27,    28,    29,    30,    31,    32,    33,    80,    81,    82,
      83,    84,    85,    87,    91,    92,    25,    68,    70,    75,
      41,    74,     3,     4,    26,    48,    71,    71,     7,     7,
       7,    48,     7,     8,     9,    46,    48,     7,     7,    81,
      49,     5,    10,    11,    13,    14,    15,    16,    46,    51,
      53,    81,    93,    94,    95,    96,    97,    98,    47,    75,
      76,    77,    77,    20,    45,    35,    45,    45,    37,    51,
      88,    88,    88,    34,    42,    43,    44,    45,    89,    90,
      38,    39,    40,    19,    36,    45,    49,    95,    93,    17,
      18,    99,   100,   101,    11,    14,    16,    46,    48,    59,
      60,    61,   102,   103,   104,   105,   106,     3,    94,     4,
       6,    46,    89,    47,    50,    46,    46,    46,    47,    52,
      50,   102,    59,   107,    54,    55,   104,    12,    56,    57,
      58,    94,    95,    14,    86,    52,    90,    89,    89,    89,
     101,    47,   107,    11,    14,    16,    48,    62,   103,    47,
      50,    47,    47,    47,    62,    63,    14,    14
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
        case 30:
#line 122 "default.y"
    {
    if (strlen(yyvsp[0].sval) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", yyvsp[0].sval);
    }
;}
    break;

  case 43:
#line 147 "default.y"
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

  case 44:
#line 158 "default.y"
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

  case 46:
#line 170 "default.y"
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

  case 63:
#line 207 "default.y"
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

  case 67:
#line 224 "default.y"
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
#line 1392 "default.tab.c"

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


#line 316 "default.y"


#undef defaulterror
#undef yyerror

int
defaulterror (const char *s)
{
    yyerror(s);
    return 0;
}


