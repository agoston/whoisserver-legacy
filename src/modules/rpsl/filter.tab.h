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
     KEYW_IGP_COST = 267,
     KEYW_PREPEND = 268,
     KEYW_APPEND = 269,
     KEYW_DELETE = 270,
     KEYW_CONTAINS = 271,
     KEYW_INTERNET = 272,
     KEYW_NO_EXPORT = 273,
     KEYW_NO_ADVERTISE = 274,
     KEYW_SELF = 275,
     ASPATH_POSTFIX = 276,
     TKN_FLTRNAME = 277,
     TKN_ASNO = 278,
     TKN_RSNAME = 279,
     TKN_ASNAME = 280,
     TKN_PRFXV4 = 281,
     TKN_PRFXV4RNG = 282,
     TKN_PREF = 283,
     TKN_MED = 284,
     TKN_DPA = 285,
     TKN_ASPATH = 286,
     TKN_COMMUNITY = 287,
     TKN_COMM_NO = 288,
     TKN_NEXT_HOP = 289,
     TKN_IPV4 = 290,
     TKN_COST = 291,
     TKN_INT = 292
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
#define KEYW_IGP_COST 267
#define KEYW_PREPEND 268
#define KEYW_APPEND 269
#define KEYW_DELETE 270
#define KEYW_CONTAINS 271
#define KEYW_INTERNET 272
#define KEYW_NO_EXPORT 273
#define KEYW_NO_ADVERTISE 274
#define KEYW_SELF 275
#define ASPATH_POSTFIX 276
#define TKN_FLTRNAME 277
#define TKN_ASNO 278
#define TKN_RSNAME 279
#define TKN_ASNAME 280
#define TKN_PRFXV4 281
#define TKN_PRFXV4RNG 282
#define TKN_PREF 283
#define TKN_MED 284
#define TKN_DPA 285
#define TKN_ASPATH 286
#define TKN_COMMUNITY 287
#define TKN_COMM_NO 288
#define TKN_NEXT_HOP 289
#define TKN_IPV4 290
#define TKN_COST 291
#define TKN_INT 292




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 16 "filter.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 1285 of yacc.c.  */
#line 115 "filter.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE filterlval;



