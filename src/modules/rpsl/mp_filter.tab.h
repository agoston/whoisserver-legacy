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
     OP_MS_V6 = 262,
     OP_EQUAL = 263,
     OP_APPEND = 264,
     OP_COMPARE = 265,
     KEYW_ANY = 266,
     KEYW_PEERAS = 267,
     KEYW_IGP_COST = 268,
     KEYW_PREPEND = 269,
     KEYW_APPEND = 270,
     KEYW_DELETE = 271,
     KEYW_CONTAINS = 272,
     KEYW_INTERNET = 273,
     KEYW_NO_EXPORT = 274,
     KEYW_NO_ADVERTISE = 275,
     KEYW_SELF = 276,
     ASPATH_POSTFIX = 277,
     TKN_FLTRNAME = 278,
     TKN_ASNO = 279,
     TKN_RSNAME = 280,
     TKN_ASNAME = 281,
     TKN_PRFXV4 = 282,
     TKN_PRFXV4RNG = 283,
     TKN_PRFXV6 = 284,
     TKN_PRFXV6DC = 285,
     TKN_PRFXV6RNG = 286,
     TKN_PRFXV6DCRNG = 287,
     TKN_PREF = 288,
     TKN_MED = 289,
     TKN_DPA = 290,
     TKN_ASPATH = 291,
     TKN_COMMUNITY = 292,
     TKN_COMM_NO = 293,
     TKN_NEXT_HOP = 294,
     TKN_IPV4 = 295,
     TKN_IPV6 = 296,
     TKN_IPV6DC = 297,
     TKN_COST = 298,
     TKN_INT = 299
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
#define KEYW_IGP_COST 268
#define KEYW_PREPEND 269
#define KEYW_APPEND 270
#define KEYW_DELETE 271
#define KEYW_CONTAINS 272
#define KEYW_INTERNET 273
#define KEYW_NO_EXPORT 274
#define KEYW_NO_ADVERTISE 275
#define KEYW_SELF 276
#define ASPATH_POSTFIX 277
#define TKN_FLTRNAME 278
#define TKN_ASNO 279
#define TKN_RSNAME 280
#define TKN_ASNAME 281
#define TKN_PRFXV4 282
#define TKN_PRFXV4RNG 283
#define TKN_PRFXV6 284
#define TKN_PRFXV6DC 285
#define TKN_PRFXV6RNG 286
#define TKN_PRFXV6DCRNG 287
#define TKN_PREF 288
#define TKN_MED 289
#define TKN_DPA 290
#define TKN_ASPATH 291
#define TKN_COMMUNITY 292
#define TKN_COMM_NO 293
#define TKN_NEXT_HOP 294
#define TKN_IPV4 295
#define TKN_IPV6 296
#define TKN_IPV6DC 297
#define TKN_COST 298
#define TKN_INT 299




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 35 "mp_filter.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 1285 of yacc.c.  */
#line 129 "mp_filter.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE mp_filterlval;



