/* A Bison parser, made from export.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse exportparse
#define yylex exportlex
#define yyerror exporterror
#define yylval exportlval
#define yychar exportchar
#define yydebug exportdebug
#define yynerrs exportnerrs
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
# define	KEYW_TO	276
# define	KEYW_ACTION	277
# define	KEYW_EXCEPT	278
# define	TKN_PREF	279
# define	TKN_MED	280
# define	TKN_DPA	281
# define	TKN_ASPATH	282
# define	TKN_COMMUNITY	283
# define	TKN_NEXT_HOP	284
# define	TKN_COST	285
# define	TKN_COMM_NO	286
# define	KEYW_IGP_COST	287
# define	KEYW_SELF	288
# define	KEYW_PREPEND	289
# define	KEYW_APPEND	290
# define	KEYW_DELETE	291
# define	KEYW_CONTAINS	292
# define	KEYW_AT	293
# define	KEYW_INTERNET	294
# define	KEYW_NO_EXPORT	295
# define	KEYW_NO_ADVERTISE	296
# define	KEYW_PROTOCOL	297
# define	TKN_PROTOCOL	298
# define	KEYW_INTO	299
# define	KEYW_REFINE	300
# define	KEYW_ANNOUNCE	301
# define	TKN_INT	302
# define	TKN_DNS	303

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

#line 16 "export.y"
#ifndef YYSTYPE
typedef union {
    char *sval;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#line 36 "export.y"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int yyerror(const char *s);
void syntax_error(char *fmt, ...);

#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		213
#define	YYFLAG		-32768
#define	YYNTBASE	68

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 303 ? yytranslate[x] : 117)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     4,     8,     9,    12,    13,    16,    18,    22,
      26,    29,    33,    36,    40,    44,    48,    53,    57,    59,
      60,    63,    67,    69,    73,    77,    79,    83,    85,    87,
      89,    90,    92,    93,    96,   100,   102,   106,   110,   112,
     116,   118,   120,   122,   124,   126,   130,   133,   137,   139,
     141,   143,   145,   147,   149,   151,   155,   159,   163,   167,
     174,   176,   180,   184,   188,   195,   202,   209,   214,   218,
     222,   224,   228,   230,   232,   234,   236,   238,   242,   246,
     250,   254,   257,   259,   263,   265,   268,   272,   274,   276,
     278,   282,   284,   286,   289,   291,   293,   295,   297,   299,
     303,   304,   306,   308,   312,   314,   316,   320,   322,   325,
     327,   330,   333,   336,   339,   341,   343,   345,   349,   351,
     353,   355,   357,   359,   363,   368,   369,   372,   375,   378,
     383
};
static const short yyrhs[] =
{
      69,    70,    71,     0,    69,    70,    74,     0,     0,    43,
      44,     0,     0,    45,    44,     0,    72,     0,    72,    46,
      71,     0,    72,    24,    71,     0,    74,    50,     0,    51,
      73,    52,     0,    74,    50,     0,    73,    74,    50,     0,
      75,    47,   102,     0,    22,    76,    77,     0,    75,    22,
      76,    77,     0,    78,    82,    83,     0,    21,     0,     0,
      23,    89,     0,    78,     3,    79,     0,    79,     0,    79,
       4,    80,     0,    79,    24,    80,     0,    80,     0,    53,
      78,    54,     0,    81,     0,    14,     0,    16,     0,     0,
      84,     0,     0,    39,    84,     0,    84,     3,    85,     0,
      85,     0,    85,     4,    86,     0,    85,    24,    86,     0,
      86,     0,    53,    84,    54,     0,    87,     0,    19,     0,
      88,     0,    20,     0,    49,     0,    88,    55,    49,     0,
      90,    50,     0,    89,    90,    50,     0,    91,     0,    92,
       0,    93,     0,    94,     0,    96,     0,   100,     0,   101,
       0,    25,     7,    48,     0,    26,     7,    48,     0,    26,
       7,    33,     0,    27,     7,    48,     0,    28,    55,    35,
      53,    95,    54,     0,    14,     0,    95,    56,    14,     0,
      29,     7,    97,     0,    29,     8,    97,     0,    29,    55,
      36,    53,    98,    54,     0,    29,    55,    37,    53,    98,
      54,     0,    29,    55,    38,    53,    98,    54,     0,    29,
      53,    98,    54,     0,    29,     9,    97,     0,    51,    98,
      52,     0,    99,     0,    98,    56,    99,     0,    40,     0,
      41,     0,    42,     0,    48,     0,    32,     0,    30,     7,
      19,     0,    30,     7,    34,     0,    31,     7,    48,     0,
     102,     3,   103,     0,   102,   103,     0,   103,     0,   103,
       4,   104,     0,   104,     0,     5,   104,     0,    53,   102,
      54,     0,   105,     0,    10,     0,    90,     0,    57,   111,
      58,     0,    13,     0,   106,     0,   107,     6,     0,   107,
       0,    14,     0,    11,     0,    16,     0,    15,     0,    51,
     108,    52,     0,     0,   109,     0,   110,     0,   109,    56,
     110,     0,    17,     0,    18,     0,   111,    59,   112,     0,
     112,     0,   112,   113,     0,   113,     0,   113,    60,     0,
     113,    61,     0,   113,    62,     0,   113,    12,     0,   114,
       0,    63,     0,    64,     0,    53,   111,    54,     0,   115,
       0,    14,     0,    11,     0,    16,     0,    55,     0,    65,
     116,    66,     0,    65,    63,   116,    66,     0,     0,   116,
      14,     0,   116,    11,     0,   116,    55,     0,   116,    14,
      67,    14,     0,   116,    16,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,    48,    49,    52,    53,    56,    57,    60,    61,    62,
      65,    66,    69,    70,    73,    76,    77,    80,    81,    84,
      85,    88,    89,    92,    93,    94,    97,    98,   101,   102,
     105,   106,   109,   110,   113,   114,   117,   118,   119,   122,
     123,   126,   127,   132,   135,   136,   139,   140,   143,   144,
     145,   146,   147,   148,   149,   152,   163,   172,   175,   186,
     189,   190,   193,   194,   195,   196,   197,   198,   199,   202,
     205,   206,   209,   210,   211,   212,   222,   225,   226,   229,
     240,   241,   242,   245,   246,   249,   250,   251,   254,   255,
     256,   257,   258,   261,   262,   265,   266,   267,   268,   269,
     272,   273,   276,   277,   280,   281,   284,   285,   288,   289,
     292,   293,   294,   295,   296,   299,   300,   301,   302,   305,
     306,   307,   308,   309,   310,   313,   314,   315,   316,   317,
     318
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
  "TKN_PRNGNAME", "KEYW_TO", "KEYW_ACTION", "KEYW_EXCEPT", "TKN_PREF", 
  "TKN_MED", "TKN_DPA", "TKN_ASPATH", "TKN_COMMUNITY", "TKN_NEXT_HOP", 
  "TKN_COST", "TKN_COMM_NO", "KEYW_IGP_COST", "KEYW_SELF", "KEYW_PREPEND", 
  "KEYW_APPEND", "KEYW_DELETE", "KEYW_CONTAINS", "KEYW_AT", 
  "KEYW_INTERNET", "KEYW_NO_EXPORT", "KEYW_NO_ADVERTISE", "KEYW_PROTOCOL", 
  "TKN_PROTOCOL", "KEYW_INTO", "KEYW_REFINE", "KEYW_ANNOUNCE", "TKN_INT", 
  "TKN_DNS", "';'", "'{'", "'}'", "'('", "')'", "'.'", "','", "'<'", 
  "'>'", "'|'", "'*'", "'?'", "'+'", "'^'", "'$'", "'['", "']'", "'-'", 
  "export", "opt_protocol_from", "opt_protocol_into", "export_expr", 
  "export_term", "export_factor_list", "export_factor", 
  "export_peering_action_list", "peering", "opt_action", "as_expr", 
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
       0,    68,    68,    69,    69,    70,    70,    71,    71,    71,
      72,    72,    73,    73,    74,    75,    75,    76,    76,    77,
      77,    78,    78,    79,    79,    79,    80,    80,    81,    81,
      82,    82,    83,    83,    84,    84,    85,    85,    85,    86,
      86,    87,    87,    87,    88,    88,    89,    89,    90,    90,
      90,    90,    90,    90,    90,    91,    92,    92,    93,    94,
      95,    95,    96,    96,    96,    96,    96,    96,    96,    97,
      98,    98,    99,    99,    99,    99,    99,   100,   100,   101,
     102,   102,   102,   103,   103,   104,   104,   104,   105,   105,
     105,   105,   105,   106,   106,   107,   107,   107,   107,   107,
     108,   108,   109,   109,   110,   110,   111,   111,   112,   112,
     113,   113,   113,   113,   113,   114,   114,   114,   114,   115,
     115,   115,   115,   115,   115,   116,   116,   116,   116,   116,
     116
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     3,     3,     0,     2,     0,     2,     1,     3,     3,
       2,     3,     2,     3,     3,     3,     4,     3,     1,     0,
       2,     3,     1,     3,     3,     1,     3,     1,     1,     1,
       0,     1,     0,     2,     3,     1,     3,     3,     1,     3,
       1,     1,     1,     1,     1,     3,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     6,
       1,     3,     3,     3,     6,     6,     6,     4,     3,     3,
       1,     3,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     2,     1,     3,     1,     2,     3,     1,     1,     1,
       3,     1,     1,     2,     1,     1,     1,     1,     1,     3,
       0,     1,     1,     3,     1,     1,     3,     1,     2,     1,
       2,     2,     2,     2,     1,     1,     1,     3,     1,     1,
       1,     1,     1,     3,     4,     0,     2,     2,     2,     4,
       2
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       3,     0,     5,     4,     0,     0,     6,     0,     0,     1,
       7,     2,     0,    28,    29,    18,     0,    19,    30,    22,
      25,    27,     0,     0,     0,     0,    10,     0,     0,     0,
       0,    15,     0,    41,    43,    44,     0,    32,    31,    35,
      38,    40,    42,     0,     0,    11,     0,    12,     9,     0,
       8,    19,     0,    88,    96,    91,    95,    98,    97,     0,
       0,     0,     0,     0,     0,     0,   100,     0,     0,    89,
      48,    49,    50,    51,    52,    53,    54,    14,    82,    84,
      87,    92,    94,    26,    20,     0,    21,     0,     0,    17,
       0,     0,     0,     0,    23,    24,    13,    16,    85,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     104,   105,     0,   101,   102,     0,   120,   119,   121,     0,
     122,   115,   116,   125,     0,   107,   109,   114,   118,     0,
      81,     0,    93,     0,    46,    39,    33,    34,    36,    37,
      45,    55,    57,    56,    58,     0,     0,    62,    63,    68,
      76,    72,    73,    74,    75,     0,    70,     0,     0,     0,
      77,    78,    79,    99,     0,    86,     0,   125,     0,    90,
       0,   108,   113,   110,   111,   112,    80,    83,    47,     0,
       0,    67,     0,     0,     0,     0,   103,   117,     0,   127,
     126,   130,   128,   123,   106,    60,     0,    69,    71,     0,
       0,     0,   124,     0,    59,     0,    64,    65,    66,   129,
      61,     0,     0,     0
};

static const short yydefgoto[] =
{
     211,     2,     5,     9,    10,    22,    49,    12,    17,    31,
      18,    19,    20,    21,    37,    89,    38,    39,    40,    41,
      42,    84,    69,    70,    71,    72,    73,   196,    74,   147,
     155,   156,    75,    76,    77,    78,    79,    80,    81,    82,
     112,   113,   114,   124,   125,   126,   127,   128,   168
};

static const short yypact[] =
{
     -13,    -3,    49,-32768,    84,     9,-32768,    -4,   103,-32768,
       2,   101,    -2,-32768,-32768,-32768,    99,   135,    77,    28,
  -32768,-32768,    21,   112,     9,     9,-32768,    -4,   106,     1,
     139,-32768,    99,-32768,-32768,-32768,    90,   132,   179,   120,
  -32768,-32768,   133,    99,    99,-32768,   140,-32768,-32768,   101,
  -32768,   135,   106,-32768,-32768,-32768,-32768,-32768,-32768,   180,
     182,   184,   138,    74,   185,   187,    67,   106,    40,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,    61,   191,-32768,
  -32768,-32768,   190,-32768,   139,   147,    28,    24,    90,-32768,
      90,    90,    90,    93,-32768,-32768,-32768,-32768,-32768,   150,
     -19,   151,   165,   152,   152,   152,   108,   117,   104,   153,
  -32768,-32768,   154,   146,-32768,     8,-32768,-32768,-32768,    40,
  -32768,-32768,-32768,   141,    43,    40,    38,-32768,-32768,   106,
     191,   106,-32768,   155,-32768,-32768,   179,   120,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,   156,   108,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,    91,-32768,   157,   158,   159,
  -32768,-32768,-32768,-32768,    67,-32768,    87,-32768,    -9,-32768,
      40,    38,-32768,-32768,-32768,-32768,   191,-32768,-32768,   193,
     -12,-32768,   108,   108,   108,   108,-32768,-32768,    -8,-32768,
     148,-32768,-32768,-32768,    40,-32768,   118,-32768,-32768,   119,
     122,   123,-32768,   194,-32768,   199,-32768,-32768,-32768,-32768,
  -32768,   214,   216,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,   136,-32768,-32768,    20,-32768,   192,   166,
     202,   188,   137,-32768,-32768,-32768,   -20,   131,    92,-32768,
  -32768,-32768,   -21,-32768,-32768,-32768,-32768,-32768,-32768,    81,
     -77,    41,-32768,-32768,   160,   -62,   -52,-32768,-32768,-32768,
  -32768,-32768,    58,   105,    55,  -124,-32768,-32768,    59
};


#define	YYLAST		227


static const short yytable[] =
{
      98,   171,   189,   189,    32,   190,   190,   191,   191,    85,
      13,   129,    14,    52,   142,   130,    87,    15,    53,    54,
      27,    55,    56,    57,    58,    11,    24,    90,    23,   143,
       1,     7,    43,    59,    60,    61,    62,    63,    64,    65,
     197,     3,    46,     7,   182,    28,   192,   192,    25,    16,
     172,   116,    44,   130,   117,    83,   118,   193,   202,    66,
       8,    67,   165,   133,   129,    68,    52,   176,   136,   180,
     171,    53,    54,    45,    55,    56,    57,    58,   135,   177,
      32,   103,   104,   105,   110,   111,    59,    60,    61,    62,
      63,    64,    65,   119,     4,   120,    33,    34,   173,   174,
     175,   169,   170,   121,   122,   123,   199,   200,   201,    33,
      34,    52,    66,    13,    67,    14,    53,    54,    68,    55,
      56,    57,    58,   160,    91,     7,    35,   106,     6,   107,
      36,    59,    60,    61,    62,    63,    64,    65,   161,    35,
     150,   187,   140,    36,    92,   181,   170,   182,   151,   152,
     153,    26,    16,   157,   158,   159,   154,    66,    30,    67,
      48,    50,    47,    68,    59,    60,    61,    62,    63,    64,
      65,    88,   204,   206,   205,   182,   207,   208,   182,   182,
      94,    95,    90,   138,   139,   148,   149,    99,    93,   100,
      96,   101,   108,   102,   109,   131,   132,   134,   141,   144,
     145,   162,   164,   146,   167,   178,   163,   195,   209,   179,
     183,   184,   185,   210,   212,   203,   213,    97,    29,    51,
      86,   137,   186,   198,   166,   194,   188,   115
};

static const short yycheck[] =
{
      52,   125,    11,    11,     3,    14,    14,    16,    16,    30,
      14,     3,    16,     5,    33,    77,    36,    21,    10,    11,
      22,    13,    14,    15,    16,     5,    24,     3,     8,    48,
      43,    22,     4,    25,    26,    27,    28,    29,    30,    31,
      52,    44,    22,    22,    56,    47,    55,    55,    46,    53,
      12,    11,    24,   115,    14,    54,    16,    66,    66,    51,
      51,    53,    54,    84,     3,    57,     5,   129,    88,   146,
     194,    10,    11,    52,    13,    14,    15,    16,    54,   131,
       3,     7,     8,     9,    17,    18,    25,    26,    27,    28,
      29,    30,    31,    53,    45,    55,    19,    20,    60,    61,
      62,    58,    59,    63,    64,    65,   183,   184,   185,    19,
      20,     5,    51,    14,    53,    16,    10,    11,    57,    13,
      14,    15,    16,    19,     4,    22,    49,    53,    44,    55,
      53,    25,    26,    27,    28,    29,    30,    31,    34,    49,
      32,    54,    49,    53,    24,    54,    59,    56,    40,    41,
      42,    50,    53,    36,    37,    38,    48,    51,    23,    53,
      24,    25,    50,    57,    25,    26,    27,    28,    29,    30,
      31,    39,    54,    54,    56,    56,    54,    54,    56,    56,
      43,    44,     3,    91,    92,   104,   105,     7,    55,     7,
      50,     7,     7,    55,     7,     4,     6,    50,    48,    48,
      35,    48,    56,    51,    63,    50,    52,    14,    14,    53,
      53,    53,    53,    14,     0,    67,     0,    51,    16,    27,
      32,    90,   164,   182,   119,   170,   167,    67
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

case 42:
#line 127 "export.y"
{
    if (strlen(yyvsp[0].sval) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", yyvsp[0].sval);
    }
;
    break;}
case 55:
#line 152 "export.y"
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
case 56:
#line 163 "export.y"
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
case 58:
#line 175 "export.y"
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
case 75:
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
;
    break;}
case 79:
#line 229 "export.y"
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
#line 321 "export.y"


#undef exporterror
#undef yyerror

int
exporterror (const char *s)
{
    yyerror(s);
    return 0;
}

