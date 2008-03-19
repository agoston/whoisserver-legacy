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
     TKN_SIMPLE_PROTOCOL = 258,
     TKN_BGP4 = 259,
     TKN_IPV4 = 260,
     TKN_RTRSNAME = 261,
     TKN_PRNGNAME = 262,
     TKN_ASNO = 263,
     TKN_SMALLINT = 264,
     KEYW_ASNO = 265,
     KEYW_FLAP_DAMP = 266,
     TKN_DNS = 267
   };
#endif
#define TKN_SIMPLE_PROTOCOL 258
#define TKN_BGP4 259
#define TKN_IPV4 260
#define TKN_RTRSNAME 261
#define TKN_PRNGNAME 262
#define TKN_ASNO 263
#define TKN_SMALLINT 264
#define KEYW_ASNO 265
#define KEYW_FLAP_DAMP 266
#define TKN_DNS 267




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 39 "peer.y"
typedef union YYSTYPE {
    char *sval;
} YYSTYPE;
/* Line 1285 of yacc.c.  */
#line 65 "peer.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE peerlval;



