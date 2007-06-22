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
#define yyparse components_r6parse
#define yylex   components_r6lex
#define yyerror components_r6error
#define yylval  components_r6lval
#define yychar  components_r6char
#define yydebug components_r6debug
#define yynerrs components_r6nerrs


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
     OP_MS_V6 = 262,
     OP_EQUAL = 263,
     OP_APPEND = 264,
     OP_COMPARE = 265,
     KEYW_ANY = 266,
     KEYW_PEERAS = 267,
     ASPATH_POSTFIX = 268,
     TKN_FLTRNAME = 269,
     TKN_ASNO = 270,
     TKN_RSNAME = 271,
     TKN_ASNAME = 272,
     TKN_PRFXV4 = 273,
     TKN_PRFXV4RNG = 274,
     TKN_PRFXV6 = 275,
     TKN_PRFXV6DC = 276,
     TKN_PRFXV6RNG = 277,
     TKN_PRFXV6DCRNG = 278,
     KEYW_ATOMIC = 279,
     KEYW_PROTOCOL = 280,
     TKN_PROTOCOL = 281,
     KEYW_IGP_COST = 282,
     KEYW_SELF = 283,
     KEYW_PREPEND = 284,
     KEYW_APPEND = 285,
     KEYW_DELETE = 286,
     KEYW_CONTAINS = 287,
     KEYW_INTERNET = 288,
     KEYW_NO_EXPORT = 289,
     KEYW_NO_ADVERTISE = 290,
     TKN_PREF = 291,
     TKN_MED = 292,
     TKN_DPA = 293,
     TKN_ASPATH = 294,
     TKN_COMMUNITY = 295,
     TKN_NEXT_HOP = 296,
     TKN_COST = 297,
     TKN_COMM_NO = 298,
     TKN_IPV4 = 299,
     TKN_IPV6 = 300,
     TKN_IPV6DC = 301,
     TKN_INT = 302
   };
#endif
#define OP_OR 258
#define OP_AND 259
#define OP_NOT 260
#define OP_MS 261
#define OP_MS_V6 262
#define OP_EQUAL 263
#define OP_APPEND 264
#define OP_COMPARE 265
#define KEYW_ANY 266
#define KEYW_PEERAS 267
#define ASPATH_POSTFIX 268
#define TKN_FLTRNAME 269
#define TKN_ASNO 270
#define TKN_RSNAME 271
#define TKN_ASNAME 272
#define TKN_PRFXV4 273
#define TKN_PRFXV4RNG 274
#define TKN_PRFXV6 275
#define TKN_PRFXV6DC 276
#define TKN_PRFXV6RNG 277
#define TKN_PRFXV6DCRNG 278
#define KEYW_ATOMIC 279
#define KEYW_PROTOCOL 280
#define TKN_PROTOCOL 281
#define KEYW_IGP_COST 282
#define KEYW_SELF 283
#define KEYW_PREPEND 284
#define KEYW_APPEND 285
#define KEYW_DELETE 286
#define KEYW_CONTAINS 287
#define KEYW_INTERNET 288
#define KEYW_NO_EXPORT 289
#define KEYW_NO_ADVERTISE 290
#define TKN_PREF 291
#define TKN_MED 292
#define TKN_DPA 293
#define TKN_ASPATH 294
#define TKN_COMMUNITY 295
#define TKN_NEXT_HOP 296
#define TKN_COST 297
#define TKN_COMM_NO 298
#define TKN_IPV4 299
#define TKN_IPV6 300
#define TKN_IPV6DC 301
#define TKN_INT 302




/* Copy the first part of user declarations.  */
#line 1 "components_r6.y"

/*
  filename: components_r6.y

  description:
    Defines the grammar for an RPSLng route6 component attribute. It is
    derived from components.y.

  notes:
    Defines tokens for the associated lexer, components_r6.l.
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
#line 14 "components_r6.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 196 "components_r6.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 33 "components_r6.y"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int yyerror(const char *s);

/* defines for unit testing */
#ifdef UNIT_TEST
int main();
#endif /* UNIT_TEST */


/* Line 214 of yacc.c.  */
#line 220 "components_r6.tab.c"

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
#define YYLAST   176

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  65
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  31
/* YYNRULES -- Number of rules. */
#define YYNRULES  94
/* YYNRULES -- Number of states. */
#define YYNSTATES  148

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   302

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    60,     2,     2,     2,
      48,    49,    56,    58,    54,    64,    61,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      50,     2,    51,    57,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    62,     2,    63,    59,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    52,    55,    53,     2,     2,     2,     2,
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
      45,    46,    47
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,     7,     9,    10,    12,    17,    21,
      24,    26,    30,    32,    35,    39,    41,    43,    47,    49,
      51,    53,    56,    59,    61,    63,    65,    67,    69,    73,
      74,    76,    78,    82,    84,    86,    88,    90,    94,    96,
      99,   101,   104,   107,   110,   113,   115,   117,   119,   123,
     125,   127,   129,   131,   133,   137,   142,   143,   146,   149,
     152,   157,   160,   162,   164,   166,   168,   170,   172,   174,
     178,   182,   186,   190,   197,   199,   203,   207,   211,   218,
     225,   232,   237,   241,   245,   247,   251,   253,   255,   257,
     259,   261,   265,   269,   273
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      66,     0,    -1,    67,    68,    -1,    -1,    24,    -1,    -1,
      69,    -1,    68,    25,    26,    69,    -1,    69,     3,    70,
      -1,    69,    70,    -1,    70,    -1,    70,     4,    71,    -1,
      71,    -1,     5,    71,    -1,    48,    69,    49,    -1,    72,
      -1,    11,    -1,    50,    78,    51,    -1,    84,    -1,    14,
      -1,    73,    -1,    74,     6,    -1,    74,     7,    -1,    74,
      -1,    15,    -1,    12,    -1,    17,    -1,    16,    -1,    52,
      75,    53,    -1,    -1,    76,    -1,    77,    -1,    76,    54,
      77,    -1,    20,    -1,    21,    -1,    22,    -1,    23,    -1,
      78,    55,    79,    -1,    79,    -1,    79,    80,    -1,    80,
      -1,    80,    56,    -1,    80,    57,    -1,    80,    58,    -1,
      80,    13,    -1,    81,    -1,    59,    -1,    60,    -1,    48,
      78,    49,    -1,    82,    -1,    15,    -1,    12,    -1,    17,
      -1,    61,    -1,    62,    83,    63,    -1,    62,    59,    83,
      63,    -1,    -1,    83,    15,    -1,    83,    12,    -1,    83,
      61,    -1,    83,    15,    64,    15,    -1,    83,    17,    -1,
      85,    -1,    86,    -1,    87,    -1,    88,    -1,    90,    -1,
      94,    -1,    95,    -1,    36,     8,    47,    -1,    37,     8,
      47,    -1,    37,     8,    27,    -1,    38,     8,    47,    -1,
      39,    61,    29,    48,    89,    49,    -1,    15,    -1,    89,
      54,    15,    -1,    40,     8,    91,    -1,    40,     9,    91,
      -1,    40,    61,    30,    48,    92,    49,    -1,    40,    61,
      31,    48,    92,    49,    -1,    40,    61,    32,    48,    92,
      49,    -1,    40,    48,    92,    49,    -1,    40,    10,    91,
      -1,    52,    92,    53,    -1,    93,    -1,    92,    54,    93,
      -1,    33,    -1,    34,    -1,    35,    -1,    47,    -1,    43,
      -1,    41,     8,    45,    -1,    41,     8,    46,    -1,    41,
       8,    28,    -1,    42,     8,    47,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    48,    48,    51,    52,    55,    56,    57,    60,    61,
      62,    65,    66,    69,    70,    71,    74,    75,    76,    77,
      78,    81,    82,    83,    86,    87,    88,    89,    90,    93,
      94,    97,    98,   101,   102,   103,   104,   107,   108,   111,
     112,   115,   116,   117,   118,   119,   122,   123,   124,   125,
     128,   129,   130,   131,   132,   133,   136,   137,   138,   139,
     140,   141,   144,   145,   146,   147,   148,   149,   150,   153,
     164,   173,   176,   187,   190,   191,   194,   195,   196,   197,
     198,   199,   200,   203,   206,   207,   210,   211,   212,   213,
     223,   226,   227,   228,   231
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OP_OR", "OP_AND", "OP_NOT", "OP_MS",
  "OP_MS_V6", "OP_EQUAL", "OP_APPEND", "OP_COMPARE", "KEYW_ANY",
  "KEYW_PEERAS", "ASPATH_POSTFIX", "TKN_FLTRNAME", "TKN_ASNO",
  "TKN_RSNAME", "TKN_ASNAME", "TKN_PRFXV4", "TKN_PRFXV4RNG", "TKN_PRFXV6",
  "TKN_PRFXV6DC", "TKN_PRFXV6RNG", "TKN_PRFXV6DCRNG", "KEYW_ATOMIC",
  "KEYW_PROTOCOL", "TKN_PROTOCOL", "KEYW_IGP_COST", "KEYW_SELF",
  "KEYW_PREPEND", "KEYW_APPEND", "KEYW_DELETE", "KEYW_CONTAINS",
  "KEYW_INTERNET", "KEYW_NO_EXPORT", "KEYW_NO_ADVERTISE", "TKN_PREF",
  "TKN_MED", "TKN_DPA", "TKN_ASPATH", "TKN_COMMUNITY", "TKN_NEXT_HOP",
  "TKN_COST", "TKN_COMM_NO", "TKN_IPV4", "TKN_IPV6", "TKN_IPV6DC",
  "TKN_INT", "'('", "')'", "'<'", "'>'", "'{'", "'}'", "','", "'|'", "'*'",
  "'?'", "'+'", "'^'", "'$'", "'.'", "'['", "']'", "'-'", "$accept",
  "components_r6", "opt_atomic", "v6_components_list", "v6_filter",
  "v6_filter_term", "v6_filter_factor", "v6_filter_operand",
  "v6_filter_prefix", "v6_filter_prefix_operand",
  "v6_opt_filter_prefix_list", "v6_filter_prefix_list",
  "v6_filter_prefix_list_prefix", "filter_aspath", "filter_aspath_term",
  "filter_aspath_closure", "filter_aspath_factor", "filter_aspath_no",
  "filter_aspath_range", "v6_rp_attribute", "pref", "med", "dpa", "aspath",
  "asno_list", "community", "community_list", "community_elm_list",
  "community_elm", "v6_next_hop", "cost", 0
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
     295,   296,   297,   298,   299,   300,   301,   302,    40,    41,
      60,    62,   123,   125,    44,   124,    42,    63,    43,    94,
      36,    46,    91,    93,    45
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    65,    66,    67,    67,    68,    68,    68,    69,    69,
      69,    70,    70,    71,    71,    71,    72,    72,    72,    72,
      72,    73,    73,    73,    74,    74,    74,    74,    74,    75,
      75,    76,    76,    77,    77,    77,    77,    78,    78,    79,
      79,    80,    80,    80,    80,    80,    81,    81,    81,    81,
      82,    82,    82,    82,    82,    82,    83,    83,    83,    83,
      83,    83,    84,    84,    84,    84,    84,    84,    84,    85,
      86,    86,    87,    88,    89,    89,    90,    90,    90,    90,
      90,    90,    90,    91,    92,    92,    93,    93,    93,    93,
      93,    94,    94,    94,    95
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     0,     1,     0,     1,     4,     3,     2,
       1,     3,     1,     2,     3,     1,     1,     3,     1,     1,
       1,     2,     2,     1,     1,     1,     1,     1,     3,     0,
       1,     1,     3,     1,     1,     1,     1,     3,     1,     2,
       1,     2,     2,     2,     2,     1,     1,     1,     3,     1,
       1,     1,     1,     1,     3,     4,     0,     2,     2,     2,
       4,     2,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     6,     1,     3,     3,     3,     6,     6,
       6,     4,     3,     3,     1,     3,     1,     1,     1,     1,
       1,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       3,     4,     0,     5,     1,     0,    16,    25,    19,    24,
      27,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     2,     6,    10,    12,    15,    20,    23,    18,
      62,    63,    64,    65,    66,    67,    68,    13,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      51,    50,    52,     0,    46,    47,    53,    56,     0,    38,
      40,    45,    49,    33,    34,    35,    36,     0,    30,    31,
       0,     0,     9,     0,    21,    22,    69,    71,    70,    72,
       0,     0,    76,    77,    82,    86,    87,    88,    90,    89,
       0,    84,     0,     0,     0,    93,    91,    92,    94,    14,
       0,    56,     0,    17,     0,    39,    44,    41,    42,    43,
      28,     0,     0,     8,    11,     0,     0,    81,     0,     0,
       0,     0,    48,     0,    58,    57,    61,    59,    54,    37,
      32,     7,    74,     0,    83,    85,     0,     0,     0,    55,
       0,    73,     0,    78,    79,    80,    60,    75
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     2,     3,    22,    23,    24,    25,    26,    27,    28,
      67,    68,    69,    58,    59,    60,    61,    62,   102,    29,
      30,    31,    32,    33,   133,    34,    82,    90,    91,    35,
      36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -58
static const yysigned_char yypact[] =
{
      -3,   -58,    19,   124,   -58,   124,   -58,   -58,   -58,   -58,
     -58,   -58,    15,    23,    31,   -34,    -4,    33,    35,   124,
       5,    67,    30,   107,    21,   -58,   -58,   -58,    45,   -58,
     -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,    12,   -19,
      14,    34,    26,    26,    26,     3,    66,   -13,    22,    65,
     -58,   -58,   -58,     5,   -58,   -58,   -58,    36,   -42,     5,
      17,   -58,   -58,   -58,   -58,   -58,   -58,    58,    55,   -58,
      90,   124,    21,   124,   -58,   -58,   -58,   -58,   -58,   -58,
      72,     3,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,
     -20,   -58,    77,    78,    79,   -58,   -58,   -58,   -58,   -58,
     -31,   -58,    -5,   -58,     5,    17,   -58,   -58,   -58,   -58,
     -58,    67,   124,    21,   -58,   113,    39,   -58,     3,     3,
       3,     3,   -58,    -1,   -58,    68,   -58,   -58,   -58,     5,
     -58,   107,   -58,   -14,   -58,   -58,    -7,     0,    37,   -58,
     115,   -58,   116,   -58,   -58,   -58,   -58,   -58
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -58,   -58,   -58,   -58,   -18,   -23,    -2,   -58,   -58,   -58,
     -58,   -58,    40,    80,    38,   -57,   -58,   -58,    49,   -58,
     -58,   -58,   -58,   -58,   -58,   -58,    56,   -36,    16,   -58,
     -58
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      72,    49,   105,    37,    42,    43,    44,   124,    77,   103,
     125,   124,   126,   104,   125,    95,   126,    50,   122,     4,
      51,     1,    52,    38,   104,    73,    72,    41,    78,   117,
     106,    39,    96,    97,   118,   141,    85,    86,    87,    40,
     142,    47,   143,    48,    45,   116,    88,   118,   113,   144,
      89,    74,    75,    53,   118,    70,   127,    46,   128,    76,
     127,    79,   139,    80,    54,    55,    56,    57,    71,    98,
       5,   114,   105,   107,   108,   109,     6,     7,    81,     8,
       9,    10,    11,   136,   137,   138,   145,    63,    64,    65,
      66,   118,   134,   118,   131,   101,    92,    93,    94,    83,
      84,    12,    13,    14,    15,    16,    17,    18,    72,   111,
      71,   110,     5,    19,    99,    20,   112,    21,     6,     7,
     115,     8,     9,    10,    11,   119,   120,   121,   132,     5,
     146,   147,   140,   100,   135,     6,     7,     0,     8,     9,
      10,    11,   129,    12,    13,    14,    15,    16,    17,    18,
     123,   130,     0,     0,     0,    19,     0,    20,     0,    21,
      12,    13,    14,    15,    16,    17,    18,     0,     0,     0,
       0,     0,    19,     0,    20,     0,    21
};

static const short int yycheck[] =
{
      23,    19,    59,     5,     8,     9,    10,    12,    27,    51,
      15,    12,    17,    55,    15,    28,    17,    12,    49,     0,
      15,    24,    17,     8,    55,     4,    49,    61,    47,    49,
      13,     8,    45,    46,    54,    49,    33,    34,    35,     8,
      54,     8,    49,     8,    48,    81,    43,    54,    71,    49,
      47,     6,     7,    48,    54,    25,    61,    61,    63,    47,
      61,    47,    63,    29,    59,    60,    61,    62,     3,    47,
       5,    73,   129,    56,    57,    58,    11,    12,    52,    14,
      15,    16,    17,   119,   120,   121,    49,    20,    21,    22,
      23,    54,    53,    54,   112,    59,    30,    31,    32,    43,
      44,    36,    37,    38,    39,    40,    41,    42,   131,    54,
       3,    53,     5,    48,    49,    50,    26,    52,    11,    12,
      48,    14,    15,    16,    17,    48,    48,    48,    15,     5,
      15,    15,    64,    53,   118,    11,    12,    -1,    14,    15,
      16,    17,   104,    36,    37,    38,    39,    40,    41,    42,
     101,   111,    -1,    -1,    -1,    48,    -1,    50,    -1,    52,
      36,    37,    38,    39,    40,    41,    42,    -1,    -1,    -1,
      -1,    -1,    48,    -1,    50,    -1,    52
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    24,    66,    67,     0,     5,    11,    12,    14,    15,
      16,    17,    36,    37,    38,    39,    40,    41,    42,    48,
      50,    52,    68,    69,    70,    71,    72,    73,    74,    84,
      85,    86,    87,    88,    90,    94,    95,    71,     8,     8,
       8,    61,     8,     9,    10,    48,    61,     8,     8,    69,
      12,    15,    17,    48,    59,    60,    61,    62,    78,    79,
      80,    81,    82,    20,    21,    22,    23,    75,    76,    77,
      25,     3,    70,     4,     6,     7,    47,    27,    47,    47,
      29,    52,    91,    91,    91,    33,    34,    35,    43,    47,
      92,    93,    30,    31,    32,    28,    45,    46,    47,    49,
      78,    59,    83,    51,    55,    80,    13,    56,    57,    58,
      53,    54,    26,    70,    71,    48,    92,    49,    54,    48,
      48,    48,    49,    83,    12,    15,    17,    61,    63,    79,
      77,    69,    15,    89,    53,    93,    92,    92,    92,    63,
      64,    49,    54,    49,    49,    49,    15,    15
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
        case 69:
#line 153 "components_r6.y"
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

  case 70:
#line 164 "components_r6.y"
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

  case 72:
#line 176 "components_r6.y"
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

  case 89:
#line 213 "components_r6.y"
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

  case 94:
#line 231 "components_r6.y"
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
#line 1330 "components_r6.tab.c"

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


#line 242 "components_r6.y"


/* define a unit test that simply returns "yes" or "no" for filter input */
#ifndef UNIT_TEST

#undef components_r6error
#undef yyerror

int
components_r6error (const char *s)
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

