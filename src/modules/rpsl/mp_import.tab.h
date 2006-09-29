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
     ASPATH_POSTFIX = 267,
     TKN_FLTRNAME = 268,
     TKN_ASNO = 269,
     TKN_RSNAME = 270,
     TKN_ASNAME = 271,
     TKN_PRFXV4 = 272,
     TKN_PRFXV4RNG = 273,
     TKN_IPV4 = 274,
     TKN_DNS = 275,
     TKN_RTRSNAME = 276,
     TKN_PRNGNAME = 277,
     TKN_IPV6 = 278,
     TKN_IPV6DC = 279,
     TKN_PRFXV6 = 280,
     TKN_PRFXV6DC = 281,
     TKN_PRFXV6RNG = 282,
     TKN_PRFXV6DCRNG = 283,
     KEYW_ACTION = 284,
     KEYW_EXCEPT = 285,
     KEYW_AFI = 286,
     KEYW_AFI_VALUE_V4 = 287,
     KEYW_AFI_VALUE_V6 = 288,
     KEYW_AFI_VALUE_ANY = 289,
     TKN_PREF = 290,
     TKN_MED = 291,
     TKN_DPA = 292,
     TKN_ASPATH = 293,
     TKN_COMMUNITY = 294,
     TKN_NEXT_HOP = 295,
     TKN_COST = 296,
     TKN_COMM_NO = 297,
     KEYW_IGP_COST = 298,
     KEYW_SELF = 299,
     KEYW_PREPEND = 300,
     KEYW_APPEND = 301,
     KEYW_DELETE = 302,
     KEYW_CONTAINS = 303,
     KEYW_AT = 304,
     KEYW_INTERNET = 305,
     KEYW_NO_EXPORT = 306,
     KEYW_NO_ADVERTISE = 307,
     KEYW_PROTOCOL = 308,
     TKN_PROTOCOL = 309,
     KEYW_INTO = 310,
     KEYW_REFINE = 311,
     KEYW_ACCEPT = 312,
     KEYW_FROM = 313,
     TKN_INT = 314
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
#define TKN_DNS 275
#define TKN_RTRSNAME 276
#define TKN_PRNGNAME 277
#define TKN_IPV6 278
#define TKN_IPV6DC 279
#define TKN_PRFXV6 280
#define TKN_PRFXV6DC 281
#define TKN_PRFXV6RNG 282
#define TKN_PRFXV6DCRNG 283
#define KEYW_ACTION 284
#define KEYW_EXCEPT 285
#define KEYW_AFI 286
#define KEYW_AFI_VALUE_V4 287
#define KEYW_AFI_VALUE_V6 288
#define KEYW_AFI_VALUE_ANY 289
#define TKN_PREF 290
#define TKN_MED 291
#define TKN_DPA 292
#define TKN_ASPATH 293
#define TKN_COMMUNITY 294
#define TKN_NEXT_HOP 295
#define TKN_COST 296
#define TKN_COMM_NO 297
#define KEYW_IGP_COST 298
#define KEYW_SELF 299
#define KEYW_PREPEND 300
#define KEYW_APPEND 301
#define KEYW_DELETE 302
#define KEYW_CONTAINS 303
#define KEYW_AT 304
#define KEYW_INTERNET 305
#define KEYW_NO_EXPORT 306
#define KEYW_NO_ADVERTISE 307
#define KEYW_PROTOCOL 308
#define TKN_PROTOCOL 309
#define KEYW_INTO 310
#define KEYW_REFINE 311
#define KEYW_ACCEPT 312
#define KEYW_FROM 313
#define TKN_INT 314




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 14 "mp_import.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 1285 of yacc.c.  */
#line 159 "mp_import.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE mp_importlval;



