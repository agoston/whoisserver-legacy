/* A Bison parser, made from mp_default.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse mp_defaultparse
#define yylex mp_defaultlex
#define yyerror mp_defaulterror
#define yylval mp_defaultlval
#define yychar mp_defaultchar
#define yydebug mp_defaultdebug
#define yynerrs mp_defaultnerrs
# define	OP_OR	257
# define	OP_AND	258
# define	OP_NOT	259
# define	OP_MS	260
# define	OP_EQUAL	261
# define	OP_APPEND	262
# define	OP_COMPARE	263
# define	KEYW_ANY	264
# define	KEYW_PEERAS	265
# define	ASPATH_POSTFIX	266
# define	TKN_FLTRNAME	267
# define	TKN_ASNO	268
# define	TKN_RSNAME	269
# define	TKN_ASNAME	270
# define	TKN_PRFXV4	271
# define	TKN_PRFXV4RNG	272
# define	TKN_IPV4	273
# define	TKN_RTRSNAME	274
# define	TKN_PRNGNAME	275
# define	TKN_IPV6	276
# define	TKN_IPV6DC	277
# define	TKN_PRFXV6	278
# define	TKN_PRFXV6DC	279
# define	TKN_PRFXV6RNG	280
# define	TKN_PRFXV6DCRNG	281
# define	KEYW_TO	282
# define	KEYW_ACTION	283
# define	KEYW_NETWORKS	284
# define	KEYW_EXCEPT	285
# define	KEYW_AFI	286
# define	KEYW_AFI_VALUE_V4	287
# define	KEYW_AFI_VALUE_V6	288
# define	KEYW_AFI_VALUE_ANY	289
# define	TKN_PREF	290
# define	TKN_MED	291
# define	TKN_DPA	292
# define	TKN_ASPATH	293
# define	TKN_COMMUNITY	294
# define	TKN_NEXT_HOP	295
# define	TKN_COST	296
# define	TKN_COMM_NO	297
# define	KEYW_IGP_COST	298
# define	KEYW_SELF	299
# define	KEYW_PREPEND	300
# define	KEYW_APPEND	301
# define	KEYW_DELETE	302
# define	KEYW_CONTAINS	303
# define	KEYW_AT	304
# define	KEYW_INTERNET	305
# define	KEYW_NO_EXPORT	306
# define	KEYW_NO_ADVERTISE	307
# define	TKN_INT	308
# define	TKN_DNS	309

#line 1 "mp_default.y"

/*
  filename: mp_default.y

  description:
    Defines the grammar for an RPSLng mp-default attribute. Derived
    from default.y.

  notes:
    Defines tokens for the associated lexer, mp_default.l.
*/

#line 14 "mp_default.y"
#ifndef YYSTYPE
typedef union {
    char *sval;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#line 34 "mp_default.y"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

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

#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		209
#define	YYFLAG		-32768
#define	YYNTBASE	74

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 309 ? yytranslate[x] : 118)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     7,    12,    16,    20,    24,    26,    28,    30,
      34,    36,    37,    40,    41,    44,    48,    50,    54,    58,
      60,    64,    66,    68,    70,    71,    73,    74,    77,    81,
      83,    87,    91,    93,    97,    99,   101,   103,   105,   107,
     109,   111,   115,   118,   122,   124,   126,   128,   130,   132,
     134,   136,   140,   144,   148,   152,   159,   161,   165,   169,
     173,   180,   187,   194,   199,   203,   207,   209,   213,   215,
     217,   219,   221,   223,   227,   231,   235,   239,   243,   247,
     250,   252,   256,   258,   261,   265,   267,   269,   273,   275,
     277,   279,   282,   284,   286,   288,   290,   292,   296,   297,
     299,   301,   305,   307,   309,   311,   313,   315,   317,   321,
     323,   326,   328,   331,   334,   337,   340,   342,   344,   346,
     350,   352,   354,   356,   358,   360,   364,   369,   370,   373,
     376,   379,   384
};
static const short yyrhs[] =
{
      32,    75,    28,    76,    77,    78,     0,    28,    76,    77,
      78,     0,    75,    56,    33,     0,    75,    56,    34,     0,
      75,    56,    35,     0,    33,     0,    34,     0,    35,     0,
      79,    83,    84,     0,    21,     0,     0,    29,    90,     0,
       0,    30,   103,     0,    79,     3,    80,     0,    80,     0,
      80,     4,    81,     0,    80,    31,    81,     0,    81,     0,
      57,    79,    58,     0,    82,     0,    14,     0,    16,     0,
       0,    85,     0,     0,    50,    85,     0,    85,     3,    86,
       0,    86,     0,    86,     4,    87,     0,    86,    31,    87,
       0,    87,     0,    57,    85,    58,     0,    88,     0,    19,
       0,    22,     0,    23,     0,    89,     0,    20,     0,    55,
       0,    89,    59,    55,     0,    91,    60,     0,    90,    91,
      60,     0,    92,     0,    93,     0,    94,     0,    95,     0,
      97,     0,   101,     0,   102,     0,    36,     7,    54,     0,
      37,     7,    54,     0,    37,     7,    44,     0,    38,     7,
      54,     0,    39,    59,    46,    57,    96,    58,     0,    14,
       0,    96,    56,    14,     0,    40,     7,    98,     0,    40,
       8,    98,     0,    40,    59,    47,    57,    99,    58,     0,
      40,    59,    48,    57,    99,    58,     0,    40,    59,    49,
      57,    99,    58,     0,    40,    57,    99,    58,     0,    40,
       9,    98,     0,    61,    99,    62,     0,   100,     0,    99,
      56,   100,     0,    51,     0,    52,     0,    53,     0,    54,
       0,    43,     0,    41,     7,    19,     0,    41,     7,    22,
       0,    41,     7,    23,     0,    41,     7,    45,     0,    42,
       7,    54,     0,   103,     3,   104,     0,   103,   104,     0,
     104,     0,   104,     4,   105,     0,   105,     0,     5,   105,
       0,    57,   103,    58,     0,   106,     0,    10,     0,    63,
     112,    64,     0,    91,     0,    13,     0,   107,     0,   108,
       6,     0,   108,     0,    14,     0,    11,     0,    16,     0,
      15,     0,    61,   109,    62,     0,     0,   110,     0,   111,
       0,   110,    56,   111,     0,    17,     0,    24,     0,    25,
       0,    18,     0,    26,     0,    27,     0,   112,    65,   113,
       0,   113,     0,   113,   114,     0,   114,     0,   114,    66,
       0,   114,    67,     0,   114,    68,     0,   114,    12,     0,
     115,     0,    69,     0,    70,     0,    57,   112,    58,     0,
     116,     0,    14,     0,    11,     0,    16,     0,    59,     0,
      71,   117,    72,     0,    71,    69,   117,    72,     0,     0,
     117,    14,     0,   117,    11,     0,   117,    59,     0,   117,
      14,    73,    14,     0,   117,    16,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,    57,    58,    61,    62,    63,    64,    65,    66,    69,
      70,    73,    74,    77,    78,    81,    82,    85,    86,    87,
      90,    91,    94,    95,    98,    99,   102,   103,   106,   107,
     110,   111,   112,   115,   116,   119,   120,   121,   122,   127,
     130,   131,   134,   135,   138,   139,   140,   141,   142,   143,
     144,   147,   158,   167,   170,   181,   184,   185,   188,   189,
     190,   191,   192,   193,   194,   197,   200,   201,   204,   205,
     206,   207,   217,   220,   221,   222,   223,   226,   237,   238,
     239,   242,   243,   246,   247,   248,   251,   252,   253,   254,
     255,   258,   259,   262,   263,   264,   265,   266,   269,   270,
     273,   274,   277,   278,   279,   280,   281,   282,   285,   286,
     289,   290,   293,   294,   295,   296,   297,   300,   301,   302,
     303,   306,   307,   308,   309,   310,   311,   314,   315,   316,
     317,   318,   319
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "OP_OR", "OP_AND", "OP_NOT", "OP_MS", 
  "OP_EQUAL", "OP_APPEND", "OP_COMPARE", "KEYW_ANY", "KEYW_PEERAS", 
  "ASPATH_POSTFIX", "TKN_FLTRNAME", "TKN_ASNO", "TKN_RSNAME", 
  "TKN_ASNAME", "TKN_PRFXV4", "TKN_PRFXV4RNG", "TKN_IPV4", "TKN_RTRSNAME", 
  "TKN_PRNGNAME", "TKN_IPV6", "TKN_IPV6DC", "TKN_PRFXV6", "TKN_PRFXV6DC", 
  "TKN_PRFXV6RNG", "TKN_PRFXV6DCRNG", "KEYW_TO", "KEYW_ACTION", 
  "KEYW_NETWORKS", "KEYW_EXCEPT", "KEYW_AFI", "KEYW_AFI_VALUE_V4", 
  "KEYW_AFI_VALUE_V6", "KEYW_AFI_VALUE_ANY", "TKN_PREF", "TKN_MED", 
  "TKN_DPA", "TKN_ASPATH", "TKN_COMMUNITY", "TKN_NEXT_HOP", "TKN_COST", 
  "TKN_COMM_NO", "KEYW_IGP_COST", "KEYW_SELF", "KEYW_PREPEND", 
  "KEYW_APPEND", "KEYW_DELETE", "KEYW_CONTAINS", "KEYW_AT", 
  "KEYW_INTERNET", "KEYW_NO_EXPORT", "KEYW_NO_ADVERTISE", "TKN_INT", 
  "TKN_DNS", "','", "'('", "')'", "'.'", "';'", "'{'", "'}'", "'<'", 
  "'>'", "'|'", "'*'", "'?'", "'+'", "'^'", "'$'", "'['", "']'", "'-'", 
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

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    74,    74,    75,    75,    75,    75,    75,    75,    76,
      76,    77,    77,    78,    78,    79,    79,    80,    80,    80,
      81,    81,    82,    82,    83,    83,    84,    84,    85,    85,
      86,    86,    86,    87,    87,    88,    88,    88,    88,    88,
      89,    89,    90,    90,    91,    91,    91,    91,    91,    91,
      91,    92,    93,    93,    94,    95,    96,    96,    97,    97,
      97,    97,    97,    97,    97,    98,    99,    99,   100,   100,
     100,   100,   100,   101,   101,   101,   101,   102,   103,   103,
     103,   104,   104,   105,   105,   105,   106,   106,   106,   106,
     106,   107,   107,   108,   108,   108,   108,   108,   109,   109,
     110,   110,   111,   111,   111,   111,   111,   111,   112,   112,
     113,   113,   114,   114,   114,   114,   114,   115,   115,   115,
     115,   116,   116,   116,   116,   116,   116,   117,   117,   117,
     117,   117,   117
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     6,     4,     3,     3,     3,     1,     1,     1,     3,
       1,     0,     2,     0,     2,     3,     1,     3,     3,     1,
       3,     1,     1,     1,     0,     1,     0,     2,     3,     1,
       3,     3,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     3,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     6,     1,     3,     3,     3,
       6,     6,     6,     4,     3,     3,     1,     3,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     2,
       1,     3,     1,     2,     3,     1,     1,     3,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     3,     0,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     3,     1,
       2,     1,     2,     2,     2,     2,     1,     1,     1,     3,
       1,     1,     1,     1,     1,     3,     4,     0,     2,     2,
       2,     4,     2
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     0,     0,    22,    23,    10,     0,    11,    24,    16,
      19,    21,     6,     7,     8,     0,     0,     0,    13,     0,
      35,    39,    36,    37,    40,     0,    26,    25,    29,    32,
      34,    38,     0,     0,     0,     0,    20,     0,     0,     0,
       0,     0,     0,     0,    12,     0,    44,    45,    46,    47,
      48,    49,    50,     0,     2,    15,     0,     0,     9,     0,
       0,     0,     0,    17,    18,    11,     3,     4,     5,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    42,     0,    86,    94,    89,    93,    96,    95,     0,
      98,     0,    88,    14,    80,    82,    85,    90,    92,    33,
      27,    28,    30,    31,    41,    13,    51,    53,    52,    54,
       0,     0,    58,    59,    64,    72,    68,    69,    70,    71,
       0,    66,     0,     0,     0,    73,    74,    75,    76,    77,
      43,    83,     0,   102,   105,   103,   104,   106,   107,     0,
      99,   100,   122,   121,   123,     0,   124,   117,   118,   127,
       0,   109,   111,   116,   120,     0,    79,     0,    91,     1,
       0,     0,     0,    63,     0,     0,     0,    84,    97,     0,
       0,   127,     0,    87,     0,   110,   115,   112,   113,   114,
      78,    81,    56,     0,    65,    67,     0,     0,     0,   101,
     119,     0,   129,   128,   132,   130,   125,   108,     0,    55,
      60,    61,    62,   126,     0,    57,   131,     0,     0,     0
};

static const short yydefgoto[] =
{
     207,    15,     7,    18,    54,     8,     9,    10,    11,    26,
      58,    27,    28,    29,    30,    31,    44,    92,    46,    47,
      48,    49,   183,    50,   112,   120,   121,    51,    52,    93,
      94,    95,    96,    97,    98,   139,   140,   141,   150,   151,
     152,   153,   154,   172
};

static const short yypact[] =
{
      21,    79,   135,-32768,-32768,-32768,    -3,    -1,   102,    70,
  -32768,-32768,-32768,-32768,-32768,    22,     9,   140,   -28,    -3,
  -32768,-32768,-32768,-32768,-32768,   107,   -11,    82,    80,-32768,
  -32768,    29,    -3,    -3,    79,   150,-32768,   116,   128,   136,
      50,    99,   153,   164,   140,   137,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,     5,-32768,    70,    11,   107,-32768,   107,
     107,   107,   100,-32768,-32768,    -1,-32768,-32768,-32768,   141,
     -18,   142,   152,   138,   138,   138,    95,   139,   122,   146,
     143,-32768,     5,-32768,-32768,-32768,-32768,-32768,-32768,     5,
     148,    83,-32768,    76,   197,-32768,-32768,-32768,   196,-32768,
      82,    80,-32768,-32768,-32768,   -28,-32768,-32768,-32768,-32768,
     147,    95,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
     -33,-32768,   149,   151,   154,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,    19,-32768,-32768,-32768,-32768,-32768,-32768,   145,
     156,-32768,-32768,-32768,-32768,    83,-32768,-32768,-32768,   144,
      55,    83,    36,-32768,-32768,     5,   197,     5,-32768,-32768,
     191,    72,    95,-32768,    95,    95,    95,-32768,-32768,   148,
     -27,-32768,    -8,-32768,    83,    36,-32768,-32768,-32768,-32768,
     197,-32768,-32768,    40,-32768,-32768,   105,   133,   134,-32768,
  -32768,    -7,-32768,   155,-32768,-32768,-32768,    83,   195,-32768,
  -32768,-32768,-32768,-32768,   200,-32768,-32768,   210,   215,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,   182,   157,   112,   212,   201,   118,-32768,-32768,
  -32768,   -20,   160,    71,-32768,-32768,-32768,    66,-32768,-32768,
  -32768,-32768,-32768,-32768,   119,   -94,    59,-32768,-32768,   158,
     -92,   -82,-32768,-32768,-32768,-32768,-32768,    54,    81,    51,
    -124,-32768,-32768,    53
};


#define	YYLAST		247


static const short yytable[] =
{
     131,   156,    53,   192,   192,    56,   193,   193,   194,   194,
      82,     3,    19,     4,    59,    83,    84,   161,    85,    86,
      87,    88,   155,   162,    82,   163,   107,   175,    17,    83,
      84,   190,    85,    86,    87,    88,   108,   100,   174,    57,
     156,    37,    38,    39,    40,    41,    42,    43,   176,     1,
      34,   195,   195,     2,     6,    37,    38,    39,    40,    41,
      42,    43,    89,   180,   196,   203,    90,    36,    91,    99,
     186,   187,   188,   175,    32,   181,    89,   167,    35,   155,
      90,    82,    91,    45,    60,    59,    83,    84,    62,    85,
      86,    87,    88,     3,   142,     4,   198,   143,   199,   144,
       5,    33,   177,   178,   179,    19,    73,    74,    75,    72,
      80,    61,    37,    38,    39,    40,    41,    42,    43,   173,
     174,    20,    21,    69,    22,    23,    20,    21,   162,    22,
      23,   102,   103,    89,   184,    70,     6,    90,   115,    91,
     145,   125,   146,    71,   126,   127,   116,   117,   118,   119,
      63,    64,   147,   148,   149,   104,    76,    24,    77,    25,
      78,   162,    24,   200,    25,   133,   134,   128,    12,    13,
      14,    79,   135,   136,   137,   138,    37,    38,    39,    40,
      41,    42,    43,    66,    67,    68,   122,   123,   124,   162,
     162,   201,   202,   113,   114,   106,   109,    81,   110,   111,
     129,   157,   158,   130,   160,   182,   164,   168,   165,   205,
     208,   166,   169,   171,   206,   209,    65,   159,    16,   101,
      55,   185,   105,   189,   191,   197,   170,     0,   204,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   132
};

static const short yycheck[] =
{
      82,    93,    30,    11,    11,    25,    14,    14,    16,    16,
       5,    14,     3,    16,     3,    10,    11,   111,    13,    14,
      15,    16,     3,    56,     5,    58,    44,   151,    29,    10,
      11,    58,    13,    14,    15,    16,    54,    57,    65,    50,
     132,    36,    37,    38,    39,    40,    41,    42,    12,    28,
      28,    59,    59,    32,    57,    36,    37,    38,    39,    40,
      41,    42,    57,   155,    72,    72,    61,    58,    63,    58,
     164,   165,   166,   197,     4,   157,    57,    58,    56,     3,
      61,     5,    63,    17,     4,     3,    10,    11,    59,    13,
      14,    15,    16,    14,    11,    16,    56,    14,    58,    16,
      21,    31,    66,    67,    68,     3,     7,     8,     9,    59,
      44,    31,    36,    37,    38,    39,    40,    41,    42,    64,
      65,    19,    20,     7,    22,    23,    19,    20,    56,    22,
      23,    60,    61,    57,    62,     7,    57,    61,    43,    63,
      57,    19,    59,     7,    22,    23,    51,    52,    53,    54,
      32,    33,    69,    70,    71,    55,    57,    55,    59,    57,
       7,    56,    55,    58,    57,    17,    18,    45,    33,    34,
      35,     7,    24,    25,    26,    27,    36,    37,    38,    39,
      40,    41,    42,    33,    34,    35,    47,    48,    49,    56,
      56,    58,    58,    74,    75,    54,    54,    60,    46,    61,
      54,     4,     6,    60,    57,    14,    57,    62,    57,    14,
       0,    57,    56,    69,    14,     0,    34,   105,     6,    59,
      19,   162,    65,   169,   171,   174,   145,    -1,    73,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    89
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
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
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
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
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


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
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
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
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


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
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
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

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

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
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
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
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
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

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
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
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 38:
#line 122 "mp_default.y"
{
    if (strlen(yyvsp[0].sval) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", yyvsp[0].sval);
    }
;
    break;}
case 51:
#line 147 "mp_default.y"
{
      long int val;
      char *s, *p;
      p = yyvsp[0].sval;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("pref value \"%s\" is not between 0 and 65535", p);
      }
;
    break;}
case 52:
#line 158 "mp_default.y"
{
      long int val;
      char *s, *p;
      p = yyvsp[0].sval;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("med value \"%s\" is not between 0 and 65535", p);
      }
;
    break;}
case 54:
#line 170 "mp_default.y"
{
      long int val;
      char *s, *p;
      p = yyvsp[0].sval;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("dpa value \"%s\" is not between 0 and 65535", p);
      }
;
    break;}
case 71:
#line 207 "mp_default.y"
{
      unsigned long int val;
      char *s, *p;
      p = yyvsp[0].sval;
      val = strtoul(p, &s, 10);
      if ((val < 1) || (val > 4294967295UL) || (*s != '\0')) {
          syntax_error("community element \"%s\" is not between 1 and 4294967295", 
                       p);
      }
;
    break;}
case 77:
#line 226 "mp_default.y"
{
      long int val;
      char *s, *p;
      p = yyvsp[0].sval;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("cost value \"%s\" is not between 0 and 65535", p);
      }
;
    break;}
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 322 "mp_default.y"


