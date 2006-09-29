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
     KEYW_ATOMIC = 274,
     KEYW_PROTOCOL = 275,
     TKN_PROTOCOL = 276,
     KEYW_IGP_COST = 277,
     KEYW_SELF = 278,
     KEYW_PREPEND = 279,
     KEYW_APPEND = 280,
     KEYW_DELETE = 281,
     KEYW_CONTAINS = 282,
     KEYW_INTERNET = 283,
     KEYW_NO_EXPORT = 284,
     KEYW_NO_ADVERTISE = 285,
     TKN_PREF = 286,
     TKN_MED = 287,
     TKN_DPA = 288,
     TKN_ASPATH = 289,
     TKN_COMMUNITY = 290,
     TKN_NEXT_HOP = 291,
     TKN_COST = 292,
     TKN_COMM_NO = 293,
     TKN_IPV4 = 294,
     TKN_INT = 295
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
#define KEYW_ATOMIC 274
#define KEYW_PROTOCOL 275
#define TKN_PROTOCOL 276
#define KEYW_IGP_COST 277
#define KEYW_SELF 278
#define KEYW_PREPEND 279
#define KEYW_APPEND 280
#define KEYW_DELETE 281
#define KEYW_CONTAINS 282
#define KEYW_INTERNET 283
#define KEYW_NO_EXPORT 284
#define KEYW_NO_ADVERTISE 285
#define TKN_PREF 286
#define TKN_MED 287
#define TKN_DPA 288
#define TKN_ASPATH 289
#define TKN_COMMUNITY 290
#define TKN_NEXT_HOP 291
#define TKN_COST 292
#define TKN_COMM_NO 293
#define TKN_IPV4 294
#define TKN_INT 295




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 16 "components.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 1285 of yacc.c.  */
#line 121 "components.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE componentslval;



