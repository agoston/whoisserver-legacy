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
#define yyparse injectparse
#define yylex   injectlex
#define yyerror injecterror
#define yylval  injectlval
#define yychar  injectchar
#define yydebug injectdebug
#define yynerrs injectnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OP_OR = 258,
     OP_EQUAL = 259,
     OP_APPEND = 260,
     OP_COMPARE = 261,
     OP_AND = 262,
     TKN_ASNO = 263,
     TKN_IPV4 = 264,
     KEYW_ACTION = 265,
     TKN_PREF = 266,
     TKN_MED = 267,
     TKN_DPA = 268,
     TKN_ASPATH = 269,
     TKN_COMMUNITY = 270,
     TKN_NEXT_HOP = 271,
     TKN_COST = 272,
     TKN_COMM_NO = 273,
     TKN_RTRSNAME = 274,
     TKN_PRFXV4 = 275,
     TKN_PRFXV4RNG = 276,
     KEYW_IGP_COST = 277,
     KEYW_SELF = 278,
     KEYW_PREPEND = 279,
     KEYW_APPEND = 280,
     KEYW_DELETE = 281,
     KEYW_CONTAINS = 282,
     KEYW_AT = 283,
     KEYW_EXCEPT = 284,
     KEYW_UPON = 285,
     KEYW_STATIC = 286,
     KEYW_HAVE_COMPONENTS = 287,
     KEYW_EXCLUDE = 288,
     KEYW_INTERNET = 289,
     KEYW_NO_EXPORT = 290,
     KEYW_NO_ADVERTISE = 291,
     KEYW_MASKLEN = 292,
     TKN_INT = 293,
     TKN_DNS = 294
   };
#endif
#define OP_OR 258
#define OP_EQUAL 259
#define OP_APPEND 260
#define OP_COMPARE 261
#define OP_AND 262
#define TKN_ASNO 263
#define TKN_IPV4 264
#define KEYW_ACTION 265
#define TKN_PREF 266
#define TKN_MED 267
#define TKN_DPA 268
#define TKN_ASPATH 269
#define TKN_COMMUNITY 270
#define TKN_NEXT_HOP 271
#define TKN_COST 272
#define TKN_COMM_NO 273
#define TKN_RTRSNAME 274
#define TKN_PRFXV4 275
#define TKN_PRFXV4RNG 276
#define KEYW_IGP_COST 277
#define KEYW_SELF 278
#define KEYW_PREPEND 279
#define KEYW_APPEND 280
#define KEYW_DELETE 281
#define KEYW_CONTAINS 282
#define KEYW_AT 283
#define KEYW_EXCEPT 284
#define KEYW_UPON 285
#define KEYW_STATIC 286
#define KEYW_HAVE_COMPONENTS 287
#define KEYW_EXCLUDE 288
#define KEYW_INTERNET 289
#define KEYW_NO_EXPORT 290
#define KEYW_NO_ADVERTISE 291
#define KEYW_MASKLEN 292
#define TKN_INT 293
#define TKN_DNS 294




/* Copy the first part of user declarations.  */
#line 1 "inject.y"

/*
  filename: inject.y

  description:
    Defines the grammar for an RPSL inject attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Defines tokens for the associated lexer, inject.l.
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
#line 16 "inject.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 182 "inject.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 33 "inject.y"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int yyerror(const char *s);
void syntax_error(char *fmt, ...);



/* Line 214 of yacc.c.  */
#line 203 "inject.tab.c"

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
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   102

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  30
/* YYNRULES -- Number of rules. */
#define YYNRULES  69
/* YYNRULES -- Number of states. */
#define YYNSTATES  126

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   294

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      40,    41,     2,     2,    45,     2,    42,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    46,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    43,     2,    44,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     7,     8,    11,    15,    17,    21,    25,
      27,    31,    33,    35,    37,    39,    41,    45,    46,    49,
      50,    53,    57,    59,    63,    65,    69,    71,    73,    78,
      83,    84,    86,    88,    92,    94,    96,    99,   103,   105,
     107,   109,   111,   113,   115,   117,   121,   125,   129,   133,
     140,   142,   146,   150,   154,   161,   168,   175,   180,   184,
     188,   190,   194,   196,   198,   200,   202,   204,   208,   212
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      48,     0,    -1,    49,    55,    56,    -1,    -1,    28,    50,
      -1,    50,     3,    51,    -1,    51,    -1,    51,     7,    52,
      -1,    51,    29,    52,    -1,    52,    -1,    40,    50,    41,
      -1,    53,    -1,     9,    -1,    54,    -1,    19,    -1,    39,
      -1,    54,    42,    39,    -1,    -1,    10,    64,    -1,    -1,
      30,    57,    -1,    57,     3,    58,    -1,    58,    -1,    58,
       7,    59,    -1,    59,    -1,    40,    57,    41,    -1,    60,
      -1,    31,    -1,    32,    43,    61,    44,    -1,    33,    43,
      61,    44,    -1,    -1,    62,    -1,    63,    -1,    62,    45,
      63,    -1,    20,    -1,    21,    -1,    65,    46,    -1,    64,
      65,    46,    -1,    66,    -1,    67,    -1,    68,    -1,    69,
      -1,    71,    -1,    75,    -1,    76,    -1,    11,     4,    38,
      -1,    12,     4,    38,    -1,    12,     4,    22,    -1,    13,
       4,    38,    -1,    14,    42,    24,    40,    70,    41,    -1,
       8,    -1,    70,    45,     8,    -1,    15,     4,    72,    -1,
      15,     5,    72,    -1,    15,    42,    25,    40,    73,    41,
      -1,    15,    42,    26,    40,    73,    41,    -1,    15,    42,
      27,    40,    73,    41,    -1,    15,    40,    73,    41,    -1,
      15,     6,    72,    -1,    43,    73,    44,    -1,    74,    -1,
      73,    45,    74,    -1,    34,    -1,    35,    -1,    36,    -1,
      38,    -1,    18,    -1,    16,     4,     9,    -1,    16,     4,
      23,    -1,    17,     4,    38,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    45,    45,    48,    49,    52,    53,    56,    57,    58,
      61,    62,    65,    66,    71,    74,    75,    78,    79,    82,
      83,    86,    87,    90,    91,    94,    95,    98,    99,   100,
     103,   104,   107,   108,   111,   112,   115,   116,   119,   120,
     121,   122,   123,   124,   125,   128,   139,   148,   151,   162,
     165,   166,   169,   170,   171,   172,   173,   174,   175,   178,
     181,   182,   185,   186,   187,   188,   198,   201,   202,   205
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OP_OR", "OP_EQUAL", "OP_APPEND",
  "OP_COMPARE", "OP_AND", "TKN_ASNO", "TKN_IPV4", "KEYW_ACTION",
  "TKN_PREF", "TKN_MED", "TKN_DPA", "TKN_ASPATH", "TKN_COMMUNITY",
  "TKN_NEXT_HOP", "TKN_COST", "TKN_COMM_NO", "TKN_RTRSNAME", "TKN_PRFXV4",
  "TKN_PRFXV4RNG", "KEYW_IGP_COST", "KEYW_SELF", "KEYW_PREPEND",
  "KEYW_APPEND", "KEYW_DELETE", "KEYW_CONTAINS", "KEYW_AT", "KEYW_EXCEPT",
  "KEYW_UPON", "KEYW_STATIC", "KEYW_HAVE_COMPONENTS", "KEYW_EXCLUDE",
  "KEYW_INTERNET", "KEYW_NO_EXPORT", "KEYW_NO_ADVERTISE", "KEYW_MASKLEN",
  "TKN_INT", "TKN_DNS", "'('", "')'", "'.'", "'{'", "'}'", "','", "';'",
  "$accept", "inject", "opt_router_expr_with_at", "router_expr",
  "router_expr_term", "router_expr_factor", "router_expr_operand",
  "domain_name", "opt_action", "opt_inject_expr", "inject_expr",
  "inject_expr_term", "inject_expr_factor", "inject_expr_operand",
  "opt_filter_prefix_list", "filter_prefix_list",
  "filter_prefix_list_prefix", "action", "rp_attribute", "pref", "med",
  "dpa", "aspath", "asno_list", "community", "community_list",
  "community_elm_list", "community_elm", "next_hop", "cost", 0
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
      40,    41,    46,   123,   125,    44,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    47,    48,    49,    49,    50,    50,    51,    51,    51,
      52,    52,    53,    53,    53,    54,    54,    55,    55,    56,
      56,    57,    57,    58,    58,    59,    59,    60,    60,    60,
      61,    61,    62,    62,    63,    63,    64,    64,    65,    65,
      65,    65,    65,    65,    65,    66,    67,    67,    68,    69,
      70,    70,    71,    71,    71,    71,    71,    71,    71,    72,
      73,    73,    74,    74,    74,    74,    74,    75,    75,    76
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     3,     0,     2,     3,     1,     3,     3,     1,
       3,     1,     1,     1,     1,     1,     3,     0,     2,     0,
       2,     3,     1,     3,     1,     3,     1,     1,     4,     4,
       0,     1,     1,     3,     1,     1,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     6,
       1,     3,     3,     3,     6,     6,     6,     4,     3,     3,
       1,     3,     1,     1,     1,     1,     1,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       3,     0,     0,    17,    12,    14,    15,     0,     4,     6,
       9,    11,    13,     1,     0,    19,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    18,     0,
      38,    39,    40,    41,    42,    43,    44,     0,     2,    10,
       5,     7,     8,    16,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,    27,     0,     0,
       0,    20,    22,    24,    26,    45,    47,    46,    48,     0,
       0,    52,    53,    58,    66,    62,    63,    64,    65,     0,
      60,     0,     0,     0,    67,    68,    69,    37,    30,    30,
       0,     0,     0,     0,     0,    57,     0,     0,     0,     0,
      34,    35,     0,    31,    32,     0,    25,    21,    23,    50,
       0,    59,    61,     0,     0,     0,    28,     0,    29,    49,
       0,    54,    55,    56,    33,    51
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     2,     3,     8,     9,    10,    11,    12,    15,    38,
      61,    62,    63,    64,   102,   103,   104,    28,    29,    30,
      31,    32,    33,   110,    34,    71,    79,    80,    35,    36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -66
static const yysigned_char yypact[] =
{
     -13,     4,    17,    20,   -66,   -66,   -66,     4,    62,    -1,
     -66,   -66,   -20,   -66,    34,     7,     0,     4,     4,     4,
      27,    63,    64,    65,    28,    -4,    67,    68,    34,    29,
     -66,   -66,   -66,   -66,   -66,   -66,   -66,   -21,   -66,   -66,
      -1,   -66,   -66,   -66,    35,   -14,    36,    52,    37,    37,
      37,    -9,    33,    -2,    39,    32,   -66,   -66,    38,    40,
     -21,    76,    75,   -66,   -66,   -66,   -66,   -66,   -66,    44,
      -9,   -66,   -66,   -66,   -66,   -66,   -66,   -66,   -66,   -27,
     -66,    45,    46,    47,   -66,   -66,   -66,   -66,    19,    19,
       1,   -21,   -21,    80,    10,   -66,    -9,    -9,    -9,    -9,
     -66,   -66,    48,    49,   -66,    51,   -66,    75,   -66,   -66,
     -25,   -66,   -66,   -10,    11,    12,   -66,    19,   -66,   -66,
      81,   -66,   -66,   -66,   -66,   -66
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -66,   -66,   -66,    83,    74,    43,   -66,   -66,   -66,   -66,
      41,     2,     5,   -66,     9,   -66,   -18,   -66,    72,   -66,
     -66,   -66,   -66,   -66,   -66,    14,   -65,     6,   -66,   -66
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      48,    49,    50,    17,    91,    94,    18,    84,    66,    74,
      57,    58,    59,     4,    95,     1,   119,    13,    96,    60,
     120,    85,    20,     5,    67,    75,    76,    77,    19,    78,
      14,   121,   113,   114,   115,    96,    51,    37,    52,   100,
     101,    39,   106,     6,     7,    21,    22,    23,    24,    25,
      26,    27,   122,   123,   111,    96,    96,    96,    81,    82,
      83,    41,    42,    72,    73,    17,    43,    44,    45,    46,
      47,    53,    54,    65,    68,    56,    69,    86,    87,    91,
      70,    88,    92,    89,    93,    97,    98,    99,   109,   125,
      16,    40,   116,   107,   117,   118,     0,   108,   105,   124,
      55,    90,   112
};

static const yysigned_char yycheck[] =
{
       4,     5,     6,     3,     3,    70,     7,     9,    22,    18,
      31,    32,    33,     9,    41,    28,    41,     0,    45,    40,
      45,    23,    42,    19,    38,    34,    35,    36,    29,    38,
      10,    41,    97,    98,    99,    45,    40,    30,    42,    20,
      21,    41,    41,    39,    40,    11,    12,    13,    14,    15,
      16,    17,    41,    41,    44,    45,    45,    45,    25,    26,
      27,    18,    19,    49,    50,     3,    39,     4,     4,     4,
      42,     4,     4,    38,    38,    46,    24,    38,    46,     3,
      43,    43,     7,    43,    40,    40,    40,    40,     8,     8,
       7,    17,    44,    91,    45,    44,    -1,    92,    89,   117,
      28,    60,    96
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    28,    48,    49,     9,    19,    39,    40,    50,    51,
      52,    53,    54,     0,    10,    55,    50,     3,     7,    29,
      42,    11,    12,    13,    14,    15,    16,    17,    64,    65,
      66,    67,    68,    69,    71,    75,    76,    30,    56,    41,
      51,    52,    52,    39,     4,     4,     4,    42,     4,     5,
       6,    40,    42,     4,     4,    65,    46,    31,    32,    33,
      40,    57,    58,    59,    60,    38,    22,    38,    38,    24,
      43,    72,    72,    72,    18,    34,    35,    36,    38,    73,
      74,    25,    26,    27,     9,    23,    38,    46,    43,    43,
      57,     3,     7,    40,    73,    41,    45,    40,    40,    40,
      20,    21,    61,    62,    63,    61,    41,    58,    59,     8,
      70,    44,    74,    73,    73,    73,    44,    45,    44,    41,
      45,    41,    41,    41,    63,     8
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
        case 13:
#line 66 "inject.y"
    {
    if (strlen(yyvsp[0].sval) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", yyvsp[0].sval);
    }
;}
    break;

  case 45:
#line 128 "inject.y"
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

  case 46:
#line 139 "inject.y"
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

  case 48:
#line 151 "inject.y"
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

  case 65:
#line 188 "inject.y"
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

  case 69:
#line 205 "inject.y"
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
#line 1275 "inject.tab.c"

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


#line 216 "inject.y"


#undef injecterror
#undef yyerror

int
injecterror (const char *s)
{
    yyerror(s);
    return 0;
}


