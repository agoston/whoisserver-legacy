%{
/*
  filename: mnt_routes6.l

  description:
    Defines the tokenizer for an RPSSng mnt-routes6 attribute.

  notes:
    Tokens are defined in the associated grammar, mnt_routes6.y.

  $Id: mnt_routes6.l,v 1.1 2004/12/27 16:38:46 can Exp $
*/

/******************
  Copyright (c)                                         RIPE NCC

  All Rights Reserved

  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation, and that the name of the author not be
  used in advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
  AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
  DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  ***************************************/
%}

QUAD           [0-9A-F]{1,4}
BITS           ([1-9]?[0-9])|(1[0-1][0-9])|(12[0-8])
IPV6DC         (({QUAD}:){0,6}{QUAD})?::({QUAD}(:{QUAD}){0,6})?\/{BITS}
MNTNER         [A-Z0-9]([-A-Z0-9]*[A-Z0-9])?

%{
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* tokens defined in the grammar */
#include "mnt_routes6.tab.h"

#define mnt_routes6wrap yywrap
void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}


%%

[ \t\n]+    { ; }

ANY                { return KEYW_ANY; }

{IPV6DC}           { return TKN_IPV6DC; }

{MNTNER}           { return TKN_MNTNER; }

. { return yytext[0]; }

%%

void
mnt_routes6_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}


