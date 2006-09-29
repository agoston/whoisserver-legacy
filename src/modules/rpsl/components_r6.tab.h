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




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 14 "components_r6.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 1285 of yacc.c.  */
#line 135 "components_r6.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE components_r6lval;



