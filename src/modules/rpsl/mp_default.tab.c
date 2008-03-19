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
#define yyparse mp_defaultparse
#define yylex   mp_defaultlex
#define yyerror mp_defaulterror
#define yylval  mp_defaultlval
#define yychar  mp_defaultchar
#define yydebug mp_defaultdebug
#define yynerrs mp_defaultnerrs


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
     KEYW_NETWORKS = 285,
     KEYW_EXCEPT = 286,
     KEYW_AFI = 287,
     KEYW_AFI_VALUE_V4 = 288,
     KEYW_AFI_VALUE_V6 = 289,
     KEYW_AFI_VALUE_ANY = 290,
     TKN_PREF = 291,
     TKN_MED = 292,
     TKN_DPA = 293,
     TKN_ASPATH = 294,
     TKN_COMMUNITY = 295,
     TKN_NEXT_HOP = 296,
     TKN_COST = 297,
     TKN_COMM_NO = 298,
     KEYW_IGP_COST = 299,
     KEYW_SELF = 300,
     KEYW_PREPEND = 301,
     KEYW_APPEND = 302,
     KEYW_DELETE = 303,
     KEYW_CONTAINS = 304,
     KEYW_AT = 305,
     KEYW_INTERNET = 306,
     KEYW_NO_EXPORT = 307,
     KEYW_NO_ADVERTISE = 308,
     TKN_INT = 309,
     TKN_DNS = 310
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
#define KEYW_NETWORKS 285
#define KEYW_EXCEPT 286
#define KEYW_AFI 287
#define KEYW_AFI_VALUE_V4 288
#define KEYW_AFI_VALUE_V6 289
#define KEYW_AFI_VALUE_ANY 290
#define TKN_PREF 291
#define TKN_MED 292
#define TKN_DPA 293
#define TKN_ASPATH 294
#define TKN_COMMUNITY 295
#define TKN_NEXT_HOP 296
#define TKN_COST 297
#define TKN_COMM_NO 298
#define KEYW_IGP_COST 299
#define KEYW_SELF 300
#define KEYW_PREPEND 301
#define KEYW_APPEND 302
#define KEYW_DELETE 303
#define KEYW_CONTAINS 304
#define KEYW_AT 305
#define KEYW_INTERNET 306
#define KEYW_NO_EXPORT 307
#define KEYW_NO_ADVERTISE 308
#define TKN_INT 309
#define TKN_DNS 310




/* Copy the first part of user declarations.  */
#line 1 "mp_default.y"

/*
  filename: mp_default.y

  description:
    Defines the grammar for an RPSLng mp-default attribute. Derived
    from default.y.

  notes:
    Defines tokens for the associated lexer, mp_default.l.
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
#line 14 "mp_default.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 212 "mp_default.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 34 "mp_default.y"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

int yyerror(const char *s);
void syntax_error(char *fmt, ...);
#undef mp_defaulterror
#undef yyerror


void yyerror (const char* msg);
int
mp_defaulterror (const char *s)
{
    yyerror(s);
    return 0;
}



/* Line 214 of yacc.c.  */
#line 245 "mp_default.tab.c"

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
#define YYFINAL  17
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   227

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  45
/* YYNRULES -- Number of rules. */
#define YYNRULES  133
/* YYNRULES -- Number of states. */
#define YYNSTATES  209

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   310

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    70,     2,     2,     2,
      57,    58,    66,    68,    56,    73,    59,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    60,
      63,     2,    64,    67,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    71,     2,    72,    69,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    61,    65,    62,     2,     2,     2,     2,
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
      55
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,    10,    15,    19,    23,    27,    29,    31,
      33,    37,    39,    40,    43,    44,    47,    51,    53,    57,
      61,    63,    67,    69,    71,    73,    74,    76,    77,    80,
      84,    86,    90,    94,    96,   100,   102,   104,   106,   108,
     110,   112,   114,   118,   121,   125,   127,   129,   131,   133,
     135,   137,   139,   143,   147,   151,   155,   162,   164,   168,
     172,   176,   183,   190,   197,   202,   206,   210,   212,   216,
     218,   220,   222,   224,   226,   230,   234,   238,   242,   246,
     250,   253,   255,   259,   261,   264,   268,   270,   272,   276,
     278,   280,   282,   285,   287,   289,   291,   293,   295,   299,
     300,   302,   304,   308,   310,   312,   314,   316,   318,   320,
     324,   326,   329,   331,   334,   337,   340,   343,   345,   347,
     349,   353,   355,   357,   359,   361,   363,   367,   372,   373,
     376,   379,   382,   387
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      75,     0,    -1,    32,    76,    28,    77,    78,    79,    -1,
      28,    77,    78,    79,    -1,    76,    56,    33,    -1,    76,
      56,    34,    -1,    76,    56,    35,    -1,    33,    -1,    34,
      -1,    35,    -1,    80,    84,    85,    -1,    21,    -1,    -1,
      29,    91,    -1,    -1,    30,   104,    -1,    80,     3,    81,
      -1,    81,    -1,    81,     4,    82,    -1,    81,    31,    82,
      -1,    82,    -1,    57,    80,    58,    -1,    83,    -1,    14,
      -1,    16,    -1,    -1,    86,    -1,    -1,    50,    86,    -1,
      86,     3,    87,    -1,    87,    -1,    87,     4,    88,    -1,
      87,    31,    88,    -1,    88,    -1,    57,    86,    58,    -1,
      89,    -1,    19,    -1,    22,    -1,    23,    -1,    90,    -1,
      20,    -1,    55,    -1,    90,    59,    55,    -1,    92,    60,
      -1,    91,    92,    60,    -1,    93,    -1,    94,    -1,    95,
      -1,    96,    -1,    98,    -1,   102,    -1,   103,    -1,    36,
       7,    54,    -1,    37,     7,    54,    -1,    37,     7,    44,
      -1,    38,     7,    54,    -1,    39,    59,    46,    57,    97,
      58,    -1,    14,    -1,    97,    56,    14,    -1,    40,     7,
      99,    -1,    40,     8,    99,    -1,    40,    59,    47,    57,
     100,    58,    -1,    40,    59,    48,    57,   100,    58,    -1,
      40,    59,    49,    57,   100,    58,    -1,    40,    57,   100,
      58,    -1,    40,     9,    99,    -1,    61,   100,    62,    -1,
     101,    -1,   100,    56,   101,    -1,    51,    -1,    52,    -1,
      53,    -1,    54,    -1,    43,    -1,    41,     7,    19,    -1,
      41,     7,    22,    -1,    41,     7,    23,    -1,    41,     7,
      45,    -1,    42,     7,    54,    -1,   104,     3,   105,    -1,
     104,   105,    -1,   105,    -1,   105,     4,   106,    -1,   106,
      -1,     5,   106,    -1,    57,   104,    58,    -1,   107,    -1,
      10,    -1,    63,   113,    64,    -1,    92,    -1,    13,    -1,
     108,    -1,   109,     6,    -1,   109,    -1,    14,    -1,    11,
      -1,    16,    -1,    15,    -1,    61,   110,    62,    -1,    -1,
     111,    -1,   112,    -1,   111,    56,   112,    -1,    17,    -1,
      24,    -1,    25,    -1,    18,    -1,    26,    -1,    27,    -1,
     113,    65,   114,    -1,   114,    -1,   114,   115,    -1,   115,
      -1,   115,    66,    -1,   115,    67,    -1,   115,    68,    -1,
     115,    12,    -1,   116,    -1,    69,    -1,    70,    -1,    57,
     113,    58,    -1,   117,    -1,    14,    -1,    11,    -1,    16,
      -1,    59,    -1,    71,   118,    72,    -1,    71,    69,   118,
      72,    -1,    -1,   118,    14,    -1,   118,    11,    -1,   118,
      59,    -1,   118,    14,    73,    14,    -1,   118,    16,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    58,    58,    59,    62,    63,    64,    65,    66,    67,
      70,    71,    74,    75,    78,    79,    82,    83,    86,    87,
      88,    91,    92,    95,    96,    99,   100,   103,   104,   107,
     108,   111,   112,   113,   116,   117,   120,   121,   122,   123,
     128,   131,   132,   135,   136,   139,   140,   141,   142,   143,
     144,   145,   148,   159,   168,   171,   182,   185,   186,   189,
     190,   191,   192,   193,   194,   195,   198,   201,   202,   205,
     206,   207,   208,   218,   221,   222,   223,   224,   227,   238,
     239,   240,   243,   244,   247,   248,   249,   252,   253,   254,
     255,   256,   259,   260,   263,   264,   265,   266,   267,   270,
     271,   274,   275,   278,   279,   280,   281,   282,   283,   286,
     287,   290,   291,   294,   295,   296,   297,   298,   301,   302,
     303,   304,   307,   308,   309,   310,   311,   312,   315,   316,
     317,   318,   319,   320
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
  "KEYW_NETWORKS", "KEYW_EXCEPT", "KEYW_AFI", "KEYW_AFI_VALUE_V4",
  "KEYW_AFI_VALUE_V6", "KEYW_AFI_VALUE_ANY", "TKN_PREF", "TKN_MED",
  "TKN_DPA", "TKN_ASPATH", "TKN_COMMUNITY", "TKN_NEXT_HOP", "TKN_COST",
  "TKN_COMM_NO", "KEYW_IGP_COST", "KEYW_SELF", "KEYW_PREPEND",
  "KEYW_APPEND", "KEYW_DELETE", "KEYW_CONTAINS", "KEYW_AT",
  "KEYW_INTERNET", "KEYW_NO_EXPORT", "KEYW_NO_ADVERTISE", "TKN_INT",
  "TKN_DNS", "','", "'('", "')'", "'.'", "';'", "'{'", "'}'", "'<'", "'>'",
  "'|'", "'*'", "'?'", "'+'", "'^'", "'$'", "'['", "']'", "'-'", "$accept",
  "mp_default_rule", "afi_list", "peering", "opt_action",
  "opt_default_filter", "as_expr", "as_expr_term", "as_expr_factor",
  "as_expr_operand", "opt_router_expr", "opt_router_expr_with_at",
  "router_expr", "router_expr_term", "router_expr_factor",
  "router_expr_operand", "domain_name", "action", "rp_attribute", "pref",
  "med", "dpa", "aspath", "asno_list", "community", "community_list",
  "community_elm_list", "community_elm", "next_hop", "cost", "filter",
  "filter_term", "filter_factor", "filter_operand", "filter_prefix",
  "filter_prefix_operand", "opt_filter_prefix_list", "filter_prefix_list",
  "filter_prefix_list_prefix", "filter_aspath", "filter_aspath_term",
  "filter_aspath_closure", "filter_aspath_factor", "filter_aspath_no",
  "filter_aspath_range", 0
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
     305,   306,   307,   308,   309,   310,    44,    40,    41,    46,
      59,   123,   125,    60,    62,   124,    42,    63,    43,    94,
      36,    91,    93,    45
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    74,    75,    75,    76,    76,    76,    76,    76,    76,
      77,    77,    78,    78,    79,    79,    80,    80,    81,    81,
      81,    82,    82,    83,    83,    84,    84,    85,    85,    86,
      86,    87,    87,    87,    88,    88,    89,    89,    89,    89,
      89,    90,    90,    91,    91,    92,    92,    92,    92,    92,
      92,    92,    93,    94,    94,    95,    96,    97,    97,    98,
      98,    98,    98,    98,    98,    98,    99,   100,   100,   101,
     101,   101,   101,   101,   102,   102,   102,   102,   103,   104,
     104,   104,   105,   105,   106,   106,   106,   107,   107,   107,
     107,   107,   108,   108,   109,   109,   109,   109,   109,   110,
     110,   111,   111,   112,   112,   112,   112,   112,   112,   113,
     113,   114,   114,   115,   115,   115,   115,   115,   116,   116,
     116,   116,   117,   117,   117,   117,   117,   117,   118,   118,
     118,   118,   118,   118
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     6,     4,     3,     3,     3,     1,     1,     1,
       3,     1,     0,     2,     0,     2,     3,     1,     3,     3,
       1,     3,     1,     1,     1,     0,     1,     0,     2,     3,
       1,     3,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     3,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     6,     1,     3,     3,
       3,     6,     6,     6,     4,     3,     3,     1,     3,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       2,     1,     3,     1,     2,     3,     1,     1,     3,     1,
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
       0,     0,     0,     0,    23,    24,    11,     0,    12,    25,
      17,    20,    22,     7,     8,     9,     0,     1,     0,     0,
      14,     0,    36,    40,    37,    38,    41,     0,    27,    26,
      30,    33,    35,    39,     0,     0,     0,     0,    21,     0,
       0,     0,     0,     0,     0,     0,    13,     0,    45,    46,
      47,    48,    49,    50,    51,     0,     3,    16,     0,     0,
      10,     0,     0,     0,     0,    18,    19,    12,     4,     5,
       6,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,     0,    87,    95,    90,    94,    97,
      96,     0,    99,     0,    89,    15,    81,    83,    86,    91,
      93,    34,    28,    29,    31,    32,    42,    14,    52,    54,
      53,    55,     0,     0,    59,    60,    65,    73,    69,    70,
      71,    72,     0,    67,     0,     0,     0,    74,    75,    76,
      77,    78,    44,    84,     0,   103,   106,   104,   105,   107,
     108,     0,   100,   101,   123,   122,   124,     0,   125,   118,
     119,   128,     0,   110,   112,   117,   121,     0,    80,     0,
      92,     2,     0,     0,     0,    64,     0,     0,     0,    85,
      98,     0,     0,   128,     0,    88,     0,   111,   116,   113,
     114,   115,    79,    82,    57,     0,    66,    68,     0,     0,
       0,   102,   120,     0,   130,   129,   133,   131,   126,   109,
       0,    56,    61,    62,    63,   127,     0,    58,   132
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     3,    16,     8,    20,    56,     9,    10,    11,    12,
      28,    60,    29,    30,    31,    32,    33,    46,    94,    48,
      49,    50,    51,   185,    52,   114,   122,   123,    53,    54,
      95,    96,    97,    98,    99,   100,   141,   142,   143,   152,
     153,   154,   155,   156,   174
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -127
static const short int yypact[] =
{
      91,    79,   135,     2,  -127,  -127,  -127,    -3,    -6,   102,
      70,  -127,  -127,  -127,  -127,  -127,    22,  -127,     9,   140,
       6,    -3,  -127,  -127,  -127,  -127,  -127,   107,   -11,    46,
      80,  -127,  -127,    29,    -3,    -3,    79,   150,  -127,   113,
     121,   144,    96,    99,   153,   164,   140,   136,  -127,  -127,
    -127,  -127,  -127,  -127,  -127,     5,  -127,    70,    11,   107,
    -127,   107,   107,   107,   142,  -127,  -127,    -6,  -127,  -127,
    -127,   141,   -16,   145,   152,   143,   143,   143,    95,   139,
     122,   146,   147,  -127,     5,  -127,  -127,  -127,  -127,  -127,
    -127,     5,   148,    83,  -127,    76,   197,  -127,  -127,  -127,
     196,  -127,    46,    80,  -127,  -127,  -127,     6,  -127,  -127,
    -127,  -127,   149,    95,  -127,  -127,  -127,  -127,  -127,  -127,
    -127,  -127,    27,  -127,   151,   154,   155,  -127,  -127,  -127,
    -127,  -127,  -127,  -127,    19,  -127,  -127,  -127,  -127,  -127,
    -127,   156,   157,  -127,  -127,  -127,  -127,    83,  -127,  -127,
    -127,   158,    45,    83,    36,  -127,  -127,     5,   197,     5,
    -127,  -127,   189,   -31,    95,  -127,    95,    95,    95,  -127,
    -127,   148,    85,  -127,    -8,  -127,    83,    36,  -127,  -127,
    -127,  -127,   197,  -127,  -127,    40,  -127,  -127,   105,   133,
     134,  -127,  -127,    -7,  -127,   132,  -127,  -127,  -127,    83,
     195,  -127,  -127,  -127,  -127,  -127,   200,  -127,  -127
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -127,  -127,  -127,   174,   159,   108,   209,   198,    97,  -127,
    -127,  -127,   -22,   160,    72,  -127,  -127,  -127,     7,  -127,
    -127,  -127,  -127,  -127,  -127,   117,   -96,    53,  -127,  -127,
     129,   -94,   -84,  -127,  -127,  -127,  -127,  -127,    51,    77,
      47,  -126,  -127,  -127,    52
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
     133,   158,    17,   194,   194,    58,   195,   195,   196,   196,
      84,     4,    21,     5,    61,    85,    86,   163,    87,    88,
      89,    90,   157,    19,    84,   164,    47,   177,   109,    85,
      86,   186,    87,    88,    89,    90,    55,   102,   110,    59,
     158,    39,    40,    41,    42,    43,    44,    45,   178,    61,
      36,   197,   197,    82,     7,    39,    40,    41,    42,    43,
      44,    45,    91,   182,   198,   205,    92,    38,    93,   101,
     188,   189,   190,   177,    34,   183,    91,   169,    37,   157,
      92,    84,    93,   164,    62,   165,    85,    86,    64,    87,
      88,    89,    90,     4,   144,     5,   200,   145,   201,   146,
       6,    35,   179,   180,   181,    21,    75,    76,    77,   175,
     176,    63,    39,    40,    41,    42,    43,    44,    45,     1,
      71,    22,    23,     2,    24,    25,    22,    23,    72,    24,
      25,    65,    66,    91,   104,   105,     7,    92,   117,    93,
     147,   127,   148,   192,   128,   129,   118,   119,   120,   121,
     176,    73,   149,   150,   151,    74,    78,    26,    79,    27,
      80,   164,    26,   202,    27,   135,   136,   130,    13,    14,
      15,    81,   137,   138,   139,   140,    39,    40,    41,    42,
      43,    44,    45,    68,    69,    70,   124,   125,   126,   164,
     164,   203,   204,   115,   116,   108,    83,   106,   112,   111,
     131,   159,   160,   184,   113,   206,   162,   132,   166,   207,
      67,   167,   168,   171,   208,   161,    18,   187,   170,    57,
     134,   103,   191,   199,   172,   193,   107,   173
};

static const unsigned char yycheck[] =
{
      84,    95,     0,    11,    11,    27,    14,    14,    16,    16,
       5,    14,     3,    16,     3,    10,    11,   113,    13,    14,
      15,    16,     3,    29,     5,    56,    19,   153,    44,    10,
      11,    62,    13,    14,    15,    16,    30,    59,    54,    50,
     134,    36,    37,    38,    39,    40,    41,    42,    12,     3,
      28,    59,    59,    46,    57,    36,    37,    38,    39,    40,
      41,    42,    57,   157,    72,    72,    61,    58,    63,    58,
     166,   167,   168,   199,     4,   159,    57,    58,    56,     3,
      61,     5,    63,    56,     4,    58,    10,    11,    59,    13,
      14,    15,    16,    14,    11,    16,    56,    14,    58,    16,
      21,    31,    66,    67,    68,     3,     7,     8,     9,    64,
      65,    31,    36,    37,    38,    39,    40,    41,    42,    28,
       7,    19,    20,    32,    22,    23,    19,    20,     7,    22,
      23,    34,    35,    57,    62,    63,    57,    61,    43,    63,
      57,    19,    59,    58,    22,    23,    51,    52,    53,    54,
      65,     7,    69,    70,    71,    59,    57,    55,    59,    57,
       7,    56,    55,    58,    57,    17,    18,    45,    33,    34,
      35,     7,    24,    25,    26,    27,    36,    37,    38,    39,
      40,    41,    42,    33,    34,    35,    47,    48,    49,    56,
      56,    58,    58,    76,    77,    54,    60,    55,    46,    54,
      54,     4,     6,    14,    61,    73,    57,    60,    57,    14,
      36,    57,    57,    56,    14,   107,     7,   164,    62,    21,
      91,    61,   171,   176,   147,   173,    67,    69
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    28,    32,    75,    14,    16,    21,    57,    77,    80,
      81,    82,    83,    33,    34,    35,    76,     0,    80,    29,
      78,     3,    19,    20,    22,    23,    55,    57,    84,    86,
      87,    88,    89,    90,     4,    31,    28,    56,    58,    36,
      37,    38,    39,    40,    41,    42,    91,    92,    93,    94,
      95,    96,    98,   102,   103,    30,    79,    81,    86,    50,
      85,     3,     4,    31,    59,    82,    82,    77,    33,    34,
      35,     7,     7,     7,    59,     7,     8,     9,    57,    59,
       7,     7,    92,    60,     5,    10,    11,    13,    14,    15,
      16,    57,    61,    63,    92,   104,   105,   106,   107,   108,
     109,    58,    86,    87,    88,    88,    55,    78,    54,    44,
      54,    54,    46,    61,    99,    99,    99,    43,    51,    52,
      53,    54,   100,   101,    47,    48,    49,    19,    22,    23,
      45,    54,    60,   106,   104,    17,    18,    24,    25,    26,
      27,   110,   111,   112,    11,    14,    16,    57,    59,    69,
      70,    71,   113,   114,   115,   116,   117,     3,   105,     4,
       6,    79,    57,   100,    56,    58,    57,    57,    57,    58,
      62,    56,   113,    69,   118,    64,    65,   115,    12,    66,
      67,    68,   105,   106,    14,    97,    62,   101,   100,   100,
     100,   112,    58,   118,    11,    14,    16,    59,    72,   114,
      56,    58,    58,    58,    58,    72,    73,    14,    14
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
        case 39:
#line 123 "mp_default.y"
    {
    if (strlen(yyvsp[0].sval) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", yyvsp[0].sval);
    }
;}
    break;

  case 52:
#line 148 "mp_default.y"
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

  case 53:
#line 159 "mp_default.y"
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

  case 55:
#line 171 "mp_default.y"
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

  case 72:
#line 208 "mp_default.y"
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

  case 78:
#line 227 "mp_default.y"
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
#line 1428 "mp_default.tab.c"

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


#line 323 "mp_default.y"



