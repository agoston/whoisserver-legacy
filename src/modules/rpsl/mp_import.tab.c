/* A Bison parser, made from mp_import.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse mp_importparse
#define yylex mp_importlex
#define yyerror mp_importerror
#define yylval mp_importlval
#define yychar mp_importchar
#define yydebug mp_importdebug
#define yynerrs mp_importnerrs
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
# define	TKN_DNS	274
# define	TKN_RTRSNAME	275
# define	TKN_PRNGNAME	276
# define	TKN_IPV6	277
# define	TKN_IPV6DC	278
# define	TKN_PRFXV6	279
# define	TKN_PRFXV6DC	280
# define	TKN_PRFXV6RNG	281
# define	TKN_PRFXV6DCRNG	282
# define	KEYW_ACTION	283
# define	KEYW_EXCEPT	284
# define	KEYW_AFI	285
# define	KEYW_AFI_VALUE_V4	286
# define	KEYW_AFI_VALUE_V6	287
# define	KEYW_AFI_VALUE_ANY	288
# define	TKN_PREF	289
# define	TKN_MED	290
# define	TKN_DPA	291
# define	TKN_ASPATH	292
# define	TKN_COMMUNITY	293
# define	TKN_NEXT_HOP	294
# define	TKN_COST	295
# define	TKN_COMM_NO	296
# define	KEYW_IGP_COST	297
# define	KEYW_SELF	298
# define	KEYW_PREPEND	299
# define	KEYW_APPEND	300
# define	KEYW_DELETE	301
# define	KEYW_CONTAINS	302
# define	KEYW_AT	303
# define	KEYW_INTERNET	304
# define	KEYW_NO_EXPORT	305
# define	KEYW_NO_ADVERTISE	306
# define	KEYW_PROTOCOL	307
# define	TKN_PROTOCOL	308
# define	KEYW_INTO	309
# define	KEYW_REFINE	310
# define	KEYW_ACCEPT	311
# define	KEYW_FROM	312
# define	TKN_INT	313

#line 1 "mp_import.y"

/*
  filename: mp_import.y

  description:
    Defines the grammar for an RPSLng mp-import attribute. Derived from
    import.y. 

  notes:
    Defines tokens for the associated lexer, mp_import.l.
*/

#line 14 "mp_import.y"
#ifndef YYSTYPE
typedef union {
    char *sval;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#line 36 "mp_import.y"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void syntax_error(char *fmt, ...);
#undef mp_importerror
#undef yyerror

int yyerror(const char *s);
int
mp_importerror (const char *s)
{
    yyerror(s);
    return 0;
}

#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		239
#define	YYFLAG		-32768
#define	YYNTBASE	78

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 313 ? yytranslate[x] : 130)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     4,     8,    13,    14,    17,    18,    21,    23,
      27,    29,    33,    37,    41,    45,    49,    53,    55,    57,
      59,    61,    64,    68,    70,    74,    77,    81,    85,    89,
      94,    98,   100,   101,   104,   108,   110,   114,   118,   120,
     124,   126,   128,   130,   131,   133,   134,   137,   141,   143,
     147,   151,   153,   157,   159,   161,   163,   165,   167,   169,
     171,   175,   178,   182,   184,   186,   188,   190,   192,   194,
     196,   200,   204,   208,   212,   219,   221,   225,   229,   233,
     240,   247,   254,   259,   263,   267,   269,   273,   275,   277,
     279,   281,   283,   287,   291,   295,   299,   303,   307,   310,
     312,   316,   318,   321,   325,   327,   329,   333,   335,   337,
     339,   342,   344,   346,   348,   350,   352,   356,   357,   359,
     361,   365,   367,   369,   371,   373,   375,   377,   381,   383,
     386,   388,   391,   394,   397,   400,   402,   404,   406,   410,
     412,   414,   416,   418,   420,   424,   429,   430,   433,   436,
     439,   444
};
static const short yyrhs[] =
{
      79,    80,    81,     0,    79,    80,    85,     0,    79,    80,
      85,    60,     0,     0,    53,    54,     0,     0,    55,    54,
       0,    82,     0,    31,    83,    82,     0,    84,     0,    84,
      56,    81,     0,    84,    30,    81,     0,    83,    61,    32,
       0,    83,    61,    33,     0,    83,    61,    34,     0,    83,
      61,    10,     0,    32,     0,    33,     0,    34,     0,    10,
       0,    87,    60,     0,    62,    86,    63,     0,    87,     0,
      31,    83,    87,     0,    87,    60,     0,    86,    87,    60,
       0,    88,    57,   115,     0,    58,    89,    90,     0,    88,
      58,    89,    90,     0,    91,    95,    96,     0,    22,     0,
       0,    29,   102,     0,    91,     3,    92,     0,    92,     0,
      92,     4,    93,     0,    92,    30,    93,     0,    93,     0,
      64,    91,    65,     0,    94,     0,    14,     0,    16,     0,
       0,    97,     0,     0,    49,    97,     0,    97,     3,    98,
       0,    98,     0,    98,     4,    99,     0,    98,    30,    99,
       0,    99,     0,    64,    97,    65,     0,   100,     0,    19,
       0,    23,     0,    24,     0,   101,     0,    21,     0,    20,
       0,   101,    66,    20,     0,   103,    60,     0,   102,   103,
      60,     0,   104,     0,   105,     0,   106,     0,   107,     0,
     109,     0,   113,     0,   114,     0,    35,     7,    59,     0,
      36,     7,    59,     0,    36,     7,    43,     0,    37,     7,
      59,     0,    38,    66,    45,    64,   108,    65,     0,    14,
       0,   108,    61,    14,     0,    39,     7,   110,     0,    39,
       8,   110,     0,    39,    66,    46,    64,   111,    65,     0,
      39,    66,    47,    64,   111,    65,     0,    39,    66,    48,
      64,   111,    65,     0,    39,    64,   111,    65,     0,    39,
       9,   110,     0,    62,   111,    63,     0,   112,     0,   111,
      61,   112,     0,    50,     0,    51,     0,    52,     0,    59,
       0,    42,     0,    40,     7,    19,     0,    40,     7,    23,
       0,    40,     7,    24,     0,    40,     7,    44,     0,    41,
       7,    59,     0,   115,     3,   116,     0,   115,   116,     0,
     116,     0,   116,     4,   117,     0,   117,     0,     5,   117,
       0,    64,   115,    65,     0,   118,     0,    10,     0,    67,
     124,    68,     0,   103,     0,    13,     0,   119,     0,   120,
       6,     0,   120,     0,    14,     0,    11,     0,    16,     0,
      15,     0,    62,   121,    63,     0,     0,   122,     0,   123,
       0,   122,    61,   123,     0,    17,     0,    25,     0,    26,
       0,    18,     0,    27,     0,    28,     0,   124,    69,   125,
       0,   125,     0,   125,   126,     0,   126,     0,   126,    70,
       0,   126,    71,     0,   126,    72,     0,   126,    12,     0,
     127,     0,    73,     0,    74,     0,    64,   124,    65,     0,
     128,     0,    14,     0,    11,     0,    16,     0,    66,     0,
      75,   129,    76,     0,    75,    73,   129,    76,     0,     0,
     129,    14,     0,   129,    11,     0,   129,    66,     0,   129,
      14,    77,    14,     0,   129,    16,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,    57,    58,    59,    62,    63,    66,    67,    70,    71,
      74,    75,    76,    79,    80,    81,    82,    83,    84,    85,
      86,    89,    90,    93,    94,    97,    98,   101,   104,   105,
     108,   109,   112,   113,   116,   117,   120,   121,   122,   125,
     126,   129,   130,   133,   134,   137,   138,   141,   142,   145,
     146,   147,   150,   151,   154,   155,   156,   157,   162,   165,
     166,   169,   170,   173,   174,   175,   176,   177,   178,   179,
     182,   193,   202,   205,   216,   219,   220,   223,   224,   225,
     226,   227,   228,   229,   232,   235,   236,   239,   240,   241,
     242,   252,   255,   256,   257,   258,   261,   272,   273,   274,
     277,   278,   281,   282,   283,   286,   287,   288,   289,   290,
     293,   294,   297,   298,   299,   300,   301,   304,   305,   308,
     309,   312,   313,   314,   315,   316,   317,   320,   321,   324,
     325,   328,   329,   330,   331,   332,   335,   336,   337,   338,
     341,   342,   343,   344,   345,   346,   349,   350,   351,   352,
     353,   354
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "OP_OR", "OP_AND", "OP_NOT", "OP_MS", 
  "OP_EQUAL", "OP_APPEND", "OP_COMPARE", "KEYW_ANY", "KEYW_PEERAS", 
  "ASPATH_POSTFIX", "TKN_FLTRNAME", "TKN_ASNO", "TKN_RSNAME", 
  "TKN_ASNAME", "TKN_PRFXV4", "TKN_PRFXV4RNG", "TKN_IPV4", "TKN_DNS", 
  "TKN_RTRSNAME", "TKN_PRNGNAME", "TKN_IPV6", "TKN_IPV6DC", "TKN_PRFXV6", 
  "TKN_PRFXV6DC", "TKN_PRFXV6RNG", "TKN_PRFXV6DCRNG", "KEYW_ACTION", 
  "KEYW_EXCEPT", "KEYW_AFI", "KEYW_AFI_VALUE_V4", "KEYW_AFI_VALUE_V6", 
  "KEYW_AFI_VALUE_ANY", "TKN_PREF", "TKN_MED", "TKN_DPA", "TKN_ASPATH", 
  "TKN_COMMUNITY", "TKN_NEXT_HOP", "TKN_COST", "TKN_COMM_NO", 
  "KEYW_IGP_COST", "KEYW_SELF", "KEYW_PREPEND", "KEYW_APPEND", 
  "KEYW_DELETE", "KEYW_CONTAINS", "KEYW_AT", "KEYW_INTERNET", 
  "KEYW_NO_EXPORT", "KEYW_NO_ADVERTISE", "KEYW_PROTOCOL", "TKN_PROTOCOL", 
  "KEYW_INTO", "KEYW_REFINE", "KEYW_ACCEPT", "KEYW_FROM", "TKN_INT", 
  "';'", "','", "'{'", "'}'", "'('", "')'", "'.'", "'<'", "'>'", "'|'", 
  "'*'", "'?'", "'+'", "'^'", "'$'", "'['", "']'", "'-'", 
  "mp_import_attribute", "opt_protocol_from", "opt_protocol_into", 
  "afi_import_expr", "import_expr", "afi_list", "import_term", 
  "afi_import_factor", "import_factor_list", "import_factor", 
  "import_peering_action_list", "peering", "opt_action", "as_expr", 
  "as_expr_term", "as_expr_factor", "as_expr_operand", "opt_router_expr", 
  "opt_router_expr_with_at", "router_expr", "router_expr_term", 
  "router_expr_factor", "router_expr_operand", "domain_name", "action", 
  "rp_attribute", "pref", "med", "dpa", "aspath", "asno_list", 
  "community", "community_list", "community_elm_list", "community_elm", 
  "next_hop", "cost", "filter", "filter_term", "filter_factor", 
  "filter_operand", "filter_prefix", "filter_prefix_operand", 
  "opt_filter_prefix_list", "filter_prefix_list", 
  "filter_prefix_list_prefix", "filter_aspath", "filter_aspath_term", 
  "filter_aspath_closure", "filter_aspath_factor", "filter_aspath_no", 
  "filter_aspath_range", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    78,    78,    78,    79,    79,    80,    80,    81,    81,
      82,    82,    82,    83,    83,    83,    83,    83,    83,    83,
      83,    84,    84,    85,    85,    86,    86,    87,    88,    88,
      89,    89,    90,    90,    91,    91,    92,    92,    92,    93,
      93,    94,    94,    95,    95,    96,    96,    97,    97,    98,
      98,    98,    99,    99,   100,   100,   100,   100,   100,   101,
     101,   102,   102,   103,   103,   103,   103,   103,   103,   103,
     104,   105,   105,   106,   107,   108,   108,   109,   109,   109,
     109,   109,   109,   109,   110,   111,   111,   112,   112,   112,
     112,   112,   113,   113,   113,   113,   114,   115,   115,   115,
     116,   116,   117,   117,   117,   118,   118,   118,   118,   118,
     119,   119,   120,   120,   120,   120,   120,   121,   121,   122,
     122,   123,   123,   123,   123,   123,   123,   124,   124,   125,
     125,   126,   126,   126,   126,   126,   127,   127,   127,   127,
     128,   128,   128,   128,   128,   128,   129,   129,   129,   129,
     129,   129
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     3,     3,     4,     0,     2,     0,     2,     1,     3,
       1,     3,     3,     3,     3,     3,     3,     1,     1,     1,
       1,     2,     3,     1,     3,     2,     3,     3,     3,     4,
       3,     1,     0,     2,     3,     1,     3,     3,     1,     3,
       1,     1,     1,     0,     1,     0,     2,     3,     1,     3,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       3,     2,     3,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     6,     1,     3,     3,     3,     6,
       6,     6,     4,     3,     3,     1,     3,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     2,     1,
       3,     1,     2,     3,     1,     1,     3,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     3,     0,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     3,     1,     2,
       1,     2,     2,     2,     2,     1,     1,     1,     3,     1,
       1,     1,     1,     1,     3,     4,     0,     2,     2,     2,
       4,     2
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       4,     0,     6,     5,     0,     0,     7,     0,     0,     0,
       1,     8,    10,     2,    23,     0,    20,    17,    18,    19,
       0,    41,    42,    31,     0,    32,    43,    35,    38,    40,
       0,     0,     0,     0,     3,    21,     0,     0,     0,     9,
      24,     0,     0,    28,     0,    54,    59,    58,    55,    56,
       0,    45,    44,    48,    51,    53,    57,     0,     0,    22,
       0,    25,     0,    12,     0,    11,     0,   105,   113,   108,
     112,   115,   114,     0,     0,     0,     0,     0,     0,     0,
     117,     0,     0,   107,    63,    64,    65,    66,    67,    68,
      69,    27,    99,   101,   104,   109,   111,    32,    16,    13,
      14,    15,    39,    33,     0,    34,     0,     0,    30,     0,
       0,     0,     0,    36,    37,    26,     0,   102,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   121,
     124,   122,   123,   125,   126,     0,   118,   119,     0,   141,
     140,   142,     0,   143,   136,   137,   146,     0,   128,   130,
     135,   139,     0,    98,     0,   110,    29,     0,    61,    52,
      46,    47,    49,    50,    60,    70,    72,    71,    73,     0,
       0,    77,    78,    83,    91,    87,    88,    89,    90,     0,
      85,     0,     0,     0,    92,    93,    94,    95,    96,   116,
       0,   103,     0,   146,     0,   106,     0,   129,   134,   131,
     132,   133,    97,   100,    62,     0,     0,     0,    82,     0,
       0,     0,   120,   138,     0,   148,   147,   151,   149,   144,
     127,    75,     0,    84,    86,     0,     0,     0,   145,     0,
       0,    74,    79,    80,    81,   150,    76,     0,     0,     0
};

static const short yydefgoto[] =
{
     237,     2,     5,    10,    11,    20,    12,    13,    30,    64,
      15,    25,    43,    26,    27,    28,    29,    51,   108,    52,
      53,    54,    55,    56,   103,    83,    84,    85,    86,    87,
     222,    88,   171,   179,   180,    89,    90,    91,    92,    93,
      94,    95,    96,   135,   136,   137,   147,   148,   149,   150,
     151,   194
};

static const short yypact[] =
{
     -48,   -34,   -28,-32768,   -19,    64,-32768,   127,    38,    34,
  -32768,-32768,   -18,    -9,    44,    43,-32768,-32768,-32768,-32768,
     149,-32768,-32768,-32768,    83,    88,   104,    52,-32768,-32768,
     -30,    51,    77,    77,-32768,-32768,   105,    38,   147,-32768,
      44,    11,   165,-32768,    83,-32768,-32768,-32768,-32768,-32768,
     110,   114,   162,   102,-32768,-32768,   109,    83,    83,-32768,
     124,-32768,   127,-32768,    44,-32768,   105,-32768,-32768,-32768,
  -32768,-32768,-32768,   183,   201,   212,   154,    72,   214,   215,
     171,   105,    39,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,    26,   219,-32768,-32768,-32768,   218,    88,-32768,-32768,
  -32768,-32768,-32768,   165,   166,    52,    12,   110,-32768,   110,
     110,   110,   205,-32768,-32768,-32768,   149,-32768,   168,   -27,
     169,   184,   170,   170,   170,   135,   106,   132,   172,-32768,
  -32768,-32768,-32768,-32768,-32768,   167,   173,-32768,     8,-32768,
  -32768,-32768,    39,-32768,-32768,-32768,   160,    18,    39,    13,
  -32768,-32768,   105,   219,   105,-32768,-32768,   175,-32768,-32768,
     162,   102,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   174,
     135,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -35,
  -32768,   176,   177,   178,-32768,-32768,-32768,-32768,-32768,-32768,
     171,-32768,    29,-32768,    -8,-32768,    39,    13,-32768,-32768,
  -32768,-32768,   219,-32768,-32768,   222,   103,   135,-32768,   135,
     135,   135,-32768,-32768,    -7,-32768,   179,-32768,-32768,-32768,
      39,-32768,   117,-32768,-32768,   130,   148,   151,-32768,   223,
     225,-32768,-32768,-32768,-32768,-32768,-32768,   243,   244,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,   138,   -20,   185,-32768,-32768,-32768,   153,
  -32768,   208,   152,   224,   202,   157,-32768,-32768,-32768,   -16,
     141,    82,-32768,-32768,-32768,   -25,-32768,-32768,-32768,-32768,
  -32768,-32768,    94,   -61,    45,-32768,-32768,   180,   -81,   -65,
  -32768,-32768,-32768,-32768,-32768,    61,   111,    58,  -146,-32768,
  -32768,    62
};


#define	YYLAST		261


static const short yytable[] =
{
      39,   117,   197,   215,   215,     1,   216,   216,   217,   217,
     153,   152,    32,    66,    44,   109,   166,   104,    67,    68,
       3,    69,    70,    71,    72,   198,   207,     4,     8,   152,
     208,    66,   167,    59,   106,     6,    67,    68,    33,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
     139,    34,    21,   140,    22,   141,    57,   153,   218,   218,
      23,    73,    74,    75,    76,    77,    78,    79,   219,   228,
      80,   202,    81,   191,   197,    82,   102,   159,   157,   122,
     123,   124,    58,   199,   200,   201,   195,   196,    80,   203,
      81,   160,     8,    82,   213,     7,    39,    21,   196,    22,
      36,    37,    24,   142,    35,   143,   110,    44,    62,   206,
      66,    61,   144,   145,   146,    67,    68,    42,    69,    70,
      71,    72,     8,    45,    46,    47,     9,    48,    49,    45,
      46,    47,   111,    48,    49,     8,   125,    16,   126,     9,
      73,    74,    75,    76,    77,    78,    79,    24,   225,   226,
     227,   184,   181,   182,   183,   185,   186,    98,    14,    17,
      18,    19,    31,   107,   207,   109,   223,    80,    50,    81,
      63,    65,    82,    40,    50,   112,   187,   174,   230,    99,
     100,   101,   231,    60,   115,   175,   176,   177,   129,   130,
     118,   207,   162,   163,   178,   232,   131,   132,   133,   134,
      73,    74,    75,    76,    77,    78,    79,     8,   119,   207,
      38,     9,   207,   233,   113,   114,   234,   172,   173,   120,
     121,   127,   128,   154,   155,   164,   158,   165,   168,   169,
     189,   188,   170,   193,   190,   204,   221,   235,   205,   236,
     209,   210,   211,   238,   239,    97,   105,   116,    41,   156,
     161,   212,   224,   192,   220,   214,   229,     0,     0,     0,
       0,   138
};

static const short yycheck[] =
{
      20,    66,   148,    11,    11,    53,    14,    14,    16,    16,
      91,     3,    30,     5,     3,     3,    43,    42,    10,    11,
      54,    13,    14,    15,    16,    12,    61,    55,    58,     3,
      65,     5,    59,    63,    50,    54,    10,    11,    56,    13,
      14,    15,    16,    35,    36,    37,    38,    39,    40,    41,
      11,    60,    14,    14,    16,    16,     4,   138,    66,    66,
      22,    35,    36,    37,    38,    39,    40,    41,    76,    76,
      62,   152,    64,    65,   220,    67,    65,    65,   103,     7,
       8,     9,    30,    70,    71,    72,    68,    69,    62,   154,
      64,   107,    58,    67,    65,    31,   116,    14,    69,    16,
      57,    58,    64,    64,    60,    66,     4,     3,    31,   170,
       5,    60,    73,    74,    75,    10,    11,    29,    13,    14,
      15,    16,    58,    19,    20,    21,    62,    23,    24,    19,
      20,    21,    30,    23,    24,    58,    64,    10,    66,    62,
      35,    36,    37,    38,    39,    40,    41,    64,   209,   210,
     211,    19,    46,    47,    48,    23,    24,    10,     5,    32,
      33,    34,     9,    49,    61,     3,    63,    62,    64,    64,
      32,    33,    67,    20,    64,    66,    44,    42,    61,    32,
      33,    34,    65,    30,    60,    50,    51,    52,    17,    18,
       7,    61,   110,   111,    59,    65,    25,    26,    27,    28,
      35,    36,    37,    38,    39,    40,    41,    58,     7,    61,
      61,    62,    61,    65,    57,    58,    65,   123,   124,     7,
      66,     7,     7,     4,     6,    20,    60,    59,    59,    45,
      63,    59,    62,    73,    61,    60,    14,    14,    64,    14,
      64,    64,    64,     0,     0,    37,    44,    62,    24,    97,
     109,   190,   207,   142,   196,   193,    77,    -1,    -1,    -1,
      -1,    81
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

case 57:
#line 157 "mp_import.y"
{
    if (strlen(yyvsp[0].sval) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", yyvsp[0].sval);
    }
;
    break;}
case 70:
#line 182 "mp_import.y"
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
case 71:
#line 193 "mp_import.y"
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
case 73:
#line 205 "mp_import.y"
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
case 90:
#line 242 "mp_import.y"
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
case 96:
#line 261 "mp_import.y"
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
#line 357 "mp_import.y"


