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
#define yyparse exportparse
#define yylex   exportlex
#define yyerror exporterror
#define yylval  exportlval
#define yychar  exportchar
#define yydebug exportdebug
#define yynerrs exportnerrs


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
     KEYW_TO = 277,
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
     KEYW_ANNOUNCE = 302,
     TKN_INT = 303,
     TKN_DNS = 304
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
#define KEYW_TO 277
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
#define KEYW_ANNOUNCE 302
#define TKN_INT 303
#define TKN_DNS 304




/* Copy the first part of user declarations.  */
#line 1 "export.y"

/*
  filename: export.y

  description:
    Defines the grammar for an RPSL dexport attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Defines tokens for the associated lexer, export.l.
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
#line 16 "export.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 202 "export.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 36 "export.y"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int yyerror(const char *s);
void syntax_error(char *fmt, ...);



/* Line 214 of yacc.c.  */
#line 223 "export.tab.c"

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
#define YYLAST   229

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
     279,   281,   285,   287,   289,   292,   294,   296,   298,   300,
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
      75,    50,    -1,    76,    47,   103,    -1,    22,    77,    78,
      -1,    76,    22,    77,    78,    -1,    79,    83,    84,    -1,
      21,    -1,    -1,    23,    90,    -1,    79,     3,    80,    -1,
      80,    -1,    80,     4,    81,    -1,    80,    24,    81,    -1,
      81,    -1,    53,    79,    54,    -1,    82,    -1,    14,    -1,
      16,    -1,    -1,    85,    -1,    -1,    39,    85,    -1,    85,
       3,    86,    -1,    86,    -1,    86,     4,    87,    -1,    86,
      24,    87,    -1,    87,    -1,    53,    85,    54,    -1,    88,
      -1,    19,    -1,    89,    -1,    20,    -1,    49,    -1,    89,
      55,    49,    -1,    91,    50,    -1,    90,    91,    50,    -1,
      92,    -1,    93,    -1,    94,    -1,    95,    -1,    97,    -1,
     101,    -1,   102,    -1,    25,     7,    48,    -1,    26,     7,
      48,    -1,    26,     7,    33,    -1,    27,     7,    48,    -1,
      28,    55,    35,    53,    96,    54,    -1,    14,    -1,    96,
      56,    14,    -1,    29,     7,    98,    -1,    29,     8,    98,
      -1,    29,    55,    36,    53,    99,    54,    -1,    29,    55,
      37,    53,    99,    54,    -1,    29,    55,    38,    53,    99,
      54,    -1,    29,    53,    99,    54,    -1,    29,     9,    98,
      -1,    51,    99,    52,    -1,   100,    -1,    99,    56,   100,
      -1,    40,    -1,    41,    -1,    42,    -1,    48,    -1,    32,
      -1,    30,     7,    19,    -1,    30,     7,    34,    -1,    31,
       7,    48,    -1,   103,     3,   104,    -1,   103,   104,    -1,
     104,    -1,   104,     4,   105,    -1,   105,    -1,     5,   105,
      -1,    53,   103,    54,    -1,   106,    -1,    10,    -1,    91,
      -1,    57,   112,    58,    -1,    13,    -1,   107,    -1,   108,
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
  "TKN_PRFXV4", "TKN_PRFXV4RNG", "TKN_IPV4", "TKN_RTRSNAME",
  "TKN_PRNGNAME", "KEYW_TO", "KEYW_ACTION", "KEYW_EXCEPT", "TKN_PREF",
  "TKN_MED", "TKN_DPA", "TKN_ASPATH", "TKN_COMMUNITY", "TKN_NEXT_HOP",
  "TKN_COST", "TKN_COMM_NO", "KEYW_IGP_COST", "KEYW_SELF", "KEYW_PREPEND",
  "KEYW_APPEND", "KEYW_DELETE", "KEYW_CONTAINS", "KEYW_AT",
  "KEYW_INTERNET", "KEYW_NO_EXPORT", "KEYW_NO_ADVERTISE", "KEYW_PROTOCOL",
  "TKN_PROTOCOL", "KEYW_INTO", "KEYW_REFINE", "KEYW_ANNOUNCE", "TKN_INT",
  "TKN_DNS", "';'", "'{'", "'}'", "'('", "')'", "'.'", "','", "'<'", "'>'",
  "'|'", "'*'", "'?'", "'+'", "'^'", "'$'", "'['", "']'", "'-'", "$accept",
  "export", "opt_protocol_from", "opt_protocol_into", "export_expr",
  "export_term", "export_factor_list", "export_factor",
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
       1,     3,     1,     1,     2,     1,     1,     1,     1,     1,
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
       0,     0,     0,    16,     0,    42,    44,    45,     0,    33,
      32,    36,    39,    41,    43,     0,     0,    12,     0,    13,
      10,     0,     9,    20,     0,    89,    97,    92,    96,    99,
      98,     0,     0,     0,     0,     0,     0,     0,   101,     0,
       0,    90,    49,    50,    51,    52,    53,    54,    55,    15,
      83,    85,    88,    93,    95,    27,    21,     0,    22,     0,
       0,    18,     0,     0,     0,     0,    24,    25,    14,    17,
      86,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   105,   106,     0,   102,   103,     0,   121,   120,
     122,     0,   123,   116,   117,   126,     0,   108,   110,   115,
     119,     0,    82,     0,    94,     0,    47,    40,    34,    35,
      37,    38,    46,    56,    58,    57,    59,     0,     0,    63,
      64,    69,    77,    73,    74,    75,    76,     0,    71,     0,
       0,     0,    78,    79,    80,   100,     0,    87,     0,   126,
       0,    91,     0,   109,   114,   111,   112,   113,    81,    84,
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
      43,    44,    86,    71,    72,    73,    74,    75,   198,    76,
     149,   157,   158,    77,    78,    79,    80,    81,    82,    83,
      84,   114,   115,   116,   126,   127,   128,   129,   130,   170
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -127
static const short int yypact[] =
{
     -13,    -3,    94,    80,  -127,  -127,    84,     9,  -127,    -4,
     129,  -127,     2,    92,    -2,  -127,  -127,  -127,    99,   148,
      77,    28,  -127,  -127,    21,   112,     9,     9,  -127,    -4,
     106,     1,   139,  -127,    99,  -127,  -127,  -127,    90,   147,
     155,   120,  -127,  -127,   132,    99,    99,  -127,   138,  -127,
    -127,    92,  -127,   148,   106,  -127,  -127,  -127,  -127,  -127,
    -127,   182,   183,   184,   137,    74,   186,   187,    67,   106,
      40,  -127,  -127,  -127,  -127,  -127,  -127,  -127,  -127,    61,
     191,  -127,  -127,  -127,   190,  -127,   139,   149,    28,    24,
      90,  -127,    90,    90,    90,   151,  -127,  -127,  -127,  -127,
    -127,   150,   -19,   153,   162,   152,   152,   152,   108,   117,
     104,   154,  -127,  -127,   156,   157,  -127,     8,  -127,  -127,
    -127,    40,  -127,  -127,  -127,   141,    43,    40,    38,  -127,
    -127,   106,   191,   106,  -127,   159,  -127,  -127,   155,   120,
    -127,  -127,  -127,  -127,  -127,  -127,  -127,   158,   108,  -127,
    -127,  -127,  -127,  -127,  -127,  -127,  -127,    91,  -127,   161,
     163,   164,  -127,  -127,  -127,  -127,    67,  -127,    87,  -127,
      -9,  -127,    40,    38,  -127,  -127,  -127,  -127,   191,  -127,
    -127,   192,   -12,  -127,   108,   108,   108,   108,  -127,  -127,
      -8,  -127,   140,  -127,  -127,  -127,    40,  -127,   118,  -127,
    -127,   119,   122,   123,  -127,   196,  -127,   198,  -127,  -127,
    -127,  -127,  -127
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -127,  -127,  -127,  -127,   134,  -127,  -127,    18,  -127,   176,
     165,   197,   185,   135,  -127,  -127,  -127,   -22,   128,    89,
    -127,  -127,  -127,   -23,  -127,  -127,  -127,  -127,  -127,  -127,
      78,   -79,    37,  -127,  -127,   160,   -64,   -54,  -127,  -127,
    -127,  -127,  -127,    56,   102,    52,  -126,  -127,  -127,    57
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
     100,   173,   191,   191,    34,   192,   192,   193,   193,    87,
      15,   131,    16,    54,   144,   132,    89,    17,    55,    56,
      29,    57,    58,    59,    60,    13,    26,    92,    25,   145,
       1,     9,    45,    61,    62,    63,    64,    65,    66,    67,
     199,     4,    48,     9,   184,    30,   194,   194,    27,    18,
     174,   118,    46,   132,   119,    85,   120,   195,   204,    68,
      10,    69,   167,   135,   131,    70,    54,   178,   138,   182,
     173,    55,    56,    47,    57,    58,    59,    60,   137,   179,
      34,   105,   106,   107,   112,   113,    61,    62,    63,    64,
      65,    66,    67,   121,     5,   122,    35,    36,   175,   176,
     177,   171,   172,   123,   124,   125,   201,   202,   203,    35,
      36,    54,    68,    15,    69,    16,    55,    56,    70,    57,
      58,    59,    60,   162,    93,     6,    37,   108,     8,   109,
      38,    61,    62,    63,    64,    65,    66,    67,   163,    37,
     152,   189,    28,    38,    94,   183,   172,   184,   153,   154,
     155,     9,    18,   159,   160,   161,   156,    68,    92,    69,
      50,    52,    49,    70,    61,    62,    63,    64,    65,    66,
      67,    32,   206,   208,   207,   184,   209,   210,   184,   184,
      96,    97,   140,   141,   150,   151,    90,    95,    98,   101,
     102,   103,   104,   110,   111,   133,   134,   147,   143,   136,
     142,   146,   164,   148,   169,    53,   197,   205,   165,   180,
     211,   181,   212,   166,   185,    31,   186,   187,    99,    88,
     139,   200,   188,   168,   196,     0,   190,     0,     0,   117
};

static const short int yycheck[] =
{
      54,   127,    11,    11,     3,    14,    14,    16,    16,    32,
      14,     3,    16,     5,    33,    79,    38,    21,    10,    11,
      22,    13,    14,    15,    16,     7,    24,     3,    10,    48,
      43,    22,     4,    25,    26,    27,    28,    29,    30,    31,
      52,    44,    24,    22,    56,    47,    55,    55,    46,    53,
      12,    11,    24,   117,    14,    54,    16,    66,    66,    51,
      51,    53,    54,    86,     3,    57,     5,   131,    90,   148,
     196,    10,    11,    52,    13,    14,    15,    16,    54,   133,
       3,     7,     8,     9,    17,    18,    25,    26,    27,    28,
      29,    30,    31,    53,     0,    55,    19,    20,    60,    61,
      62,    58,    59,    63,    64,    65,   185,   186,   187,    19,
      20,     5,    51,    14,    53,    16,    10,    11,    57,    13,
      14,    15,    16,    19,     4,    45,    49,    53,    44,    55,
      53,    25,    26,    27,    28,    29,    30,    31,    34,    49,
      32,    54,    50,    53,    24,    54,    59,    56,    40,    41,
      42,    22,    53,    36,    37,    38,    48,    51,     3,    53,
      26,    27,    50,    57,    25,    26,    27,    28,    29,    30,
      31,    23,    54,    54,    56,    56,    54,    54,    56,    56,
      45,    46,    93,    94,   106,   107,    39,    55,    50,     7,
       7,     7,    55,     7,     7,     4,     6,    35,    48,    50,
      49,    48,    48,    51,    63,    29,    14,    67,    52,    50,
      14,    53,    14,    56,    53,    18,    53,    53,    53,    34,
      92,   184,   166,   121,   172,    -1,   169,    -1,    -1,    69
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    43,    69,    70,    44,     0,    45,    71,    44,    22,
      51,    72,    73,    75,    76,    14,    16,    21,    53,    77,
      79,    80,    81,    82,    74,    75,    24,    46,    50,    22,
      47,    79,    23,    78,     3,    19,    20,    49,    53,    83,
      85,    86,    87,    88,    89,     4,    24,    52,    75,    50,
      72,    75,    72,    77,     5,    10,    11,    13,    14,    15,
      16,    25,    26,    27,    28,    29,    30,    31,    51,    53,
      57,    91,    92,    93,    94,    95,    97,   101,   102,   103,
     104,   105,   106,   107,   108,    54,    90,    91,    80,    85,
      39,    84,     3,     4,    24,    55,    81,    81,    50,    78,
     105,     7,     7,     7,    55,     7,     8,     9,    53,    55,
       7,     7,    17,    18,   109,   110,   111,   103,    11,    14,
      16,    53,    55,    63,    64,    65,   112,   113,   114,   115,
     116,     3,   104,     4,     6,    91,    50,    54,    85,    86,
      87,    87,    49,    48,    33,    48,    48,    35,    51,    98,
      98,    98,    32,    40,    41,    42,    48,    99,   100,    36,
      37,    38,    19,    34,    48,    52,    56,    54,   112,    63,
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
#line 127 "export.y"
    {
    if (strlen(yyvsp[0].sval) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", yyvsp[0].sval);
    }
;}
    break;

  case 56:
#line 152 "export.y"
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
#line 163 "export.y"
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
#line 175 "export.y"
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
#line 212 "export.y"
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
#line 229 "export.y"
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
#line 1406 "export.tab.c"

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


#line 321 "export.y"


#undef exporterror
#undef yyerror

int
exporterror (const char *s)
{
    yyerror(s);
    return 0;
}


