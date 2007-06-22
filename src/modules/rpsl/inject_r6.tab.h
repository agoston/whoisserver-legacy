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
     OP_EQUAL = 259,
     OP_APPEND = 260,
     OP_COMPARE = 261,
     OP_AND = 262,
     TKN_ASNO = 263,
     TKN_IPV4 = 264,
     TKN_IPV6 = 265,
     TKN_IPV6DC = 266,
     KEYW_ACTION = 267,
     KEYW_AFI = 268,
     KEYW_AFI_VALUE = 269,
     TKN_PREF = 270,
     TKN_MED = 271,
     TKN_DPA = 272,
     TKN_ASPATH = 273,
     TKN_COMMUNITY = 274,
     TKN_NEXT_HOP = 275,
     TKN_COST = 276,
     TKN_COMM_NO = 277,
     TKN_RTRSNAME = 278,
     TKN_PRFXV4 = 279,
     TKN_PRFXV4RNG = 280,
     TKN_PRFXV6 = 281,
     TKN_PRFXV6DC = 282,
     TKN_PRFXV6RNG = 283,
     TKN_PRFXV6DCRNG = 284,
     KEYW_IGP_COST = 285,
     KEYW_SELF = 286,
     KEYW_PREPEND = 287,
     KEYW_APPEND = 288,
     KEYW_DELETE = 289,
     KEYW_CONTAINS = 290,
     KEYW_AT = 291,
     KEYW_EXCEPT = 292,
     KEYW_UPON = 293,
     KEYW_STATIC = 294,
     KEYW_HAVE_COMPONENTS = 295,
     KEYW_EXCLUDE = 296,
     KEYW_INTERNET = 297,
     KEYW_NO_EXPORT = 298,
     KEYW_NO_ADVERTISE = 299,
     KEYW_MASKLEN = 300,
     TKN_INT = 301,
     TKN_DNS = 302
   };
#endif
#define OP_OR 258
#define OP_EQUAL 259
#define OP_APPEND 260
#define OP_COMPARE 261
#define OP_AND 262
#define TKN_ASNO 263
#define TKN_IPV4 264
#define TKN_IPV6 265
#define TKN_IPV6DC 266
#define KEYW_ACTION 267
#define KEYW_AFI 268
#define KEYW_AFI_VALUE 269
#define TKN_PREF 270
#define TKN_MED 271
#define TKN_DPA 272
#define TKN_ASPATH 273
#define TKN_COMMUNITY 274
#define TKN_NEXT_HOP 275
#define TKN_COST 276
#define TKN_COMM_NO 277
#define TKN_RTRSNAME 278
#define TKN_PRFXV4 279
#define TKN_PRFXV4RNG 280
#define TKN_PRFXV6 281
#define TKN_PRFXV6DC 282
#define TKN_PRFXV6RNG 283
#define TKN_PRFXV6DCRNG 284
#define KEYW_IGP_COST 285
#define KEYW_SELF 286
#define KEYW_PREPEND 287
#define KEYW_APPEND 288
#define KEYW_DELETE 289
#define KEYW_CONTAINS 290
#define KEYW_AT 291
#define KEYW_EXCEPT 292
#define KEYW_UPON 293
#define KEYW_STATIC 294
#define KEYW_HAVE_COMPONENTS 295
#define KEYW_EXCLUDE 296
#define KEYW_INTERNET 297
#define KEYW_NO_EXPORT 298
#define KEYW_NO_ADVERTISE 299
#define KEYW_MASKLEN 300
#define TKN_INT 301
#define TKN_DNS 302




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 14 "inject_r6.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 1285 of yacc.c.  */
#line 135 "inject_r6.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE inject_r6lval;



