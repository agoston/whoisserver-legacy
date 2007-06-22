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
     TKN_ASNO = 262,
     TKN_IPV4 = 263,
     KEYW_ACTION = 264,
     TKN_PREF = 265,
     TKN_MED = 266,
     TKN_DPA = 267,
     TKN_ASPATH = 268,
     TKN_COMMUNITY = 269,
     TKN_NEXT_HOP = 270,
     TKN_COST = 271,
     TKN_COMM_NO = 272,
     KEYW_IGP_COST = 273,
     KEYW_SELF = 274,
     KEYW_PREPEND = 275,
     KEYW_APPEND = 276,
     KEYW_DELETE = 277,
     KEYW_CONTAINS = 278,
     KEYW_INTERNET = 279,
     KEYW_NO_EXPORT = 280,
     KEYW_NO_ADVERTISE = 281,
     KEYW_MASKLEN = 282,
     TKN_INT = 283,
     TKN_DNS = 284
   };
#endif
#define OP_OR 258
#define OP_EQUAL 259
#define OP_APPEND 260
#define OP_COMPARE 261
#define TKN_ASNO 262
#define TKN_IPV4 263
#define KEYW_ACTION 264
#define TKN_PREF 265
#define TKN_MED 266
#define TKN_DPA 267
#define TKN_ASPATH 268
#define TKN_COMMUNITY 269
#define TKN_NEXT_HOP 270
#define TKN_COST 271
#define TKN_COMM_NO 272
#define KEYW_IGP_COST 273
#define KEYW_SELF 274
#define KEYW_PREPEND 275
#define KEYW_APPEND 276
#define KEYW_DELETE 277
#define KEYW_CONTAINS 278
#define KEYW_INTERNET 279
#define KEYW_NO_EXPORT 280
#define KEYW_NO_ADVERTISE 281
#define KEYW_MASKLEN 282
#define TKN_INT 283
#define TKN_DNS 284




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 16 "ifaddr.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 1285 of yacc.c.  */
#line 99 "ifaddr.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE ifaddrlval;



