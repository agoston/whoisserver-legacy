%{
/*
  filename: mnt_routes6.y

  description:
    Defines the grammar for an RPSSng mnt-routes attribute
    of inet6num class.

  notes:
    Defines tokens for the associated lexer, mnt_routes6.l.
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

#include <stdlib.h>
#include <errno.h>

int yyerror(const char *s);
%}

%token TKN_PRFXV4 TKN_PRFXV6 TKN_PRFXV6DC TKN_MNTNER KEYW_ANY TKN_PRFXV4RNG TKN_PRFXV6RNG TKN_PRFXV6DCRNG

%%

mnt_routes_an: TKN_MNTNER
| TKN_MNTNER '{' ip_prefix_list '}'
| TKN_MNTNER KEYW_ANY 
;

ip_prefix_list: TKN_PRFXV6
| TKN_PRFXV6RNG
| TKN_PRFXV6DC
| TKN_PRFXV6DCRNG
| ip_prefix_list ',' TKN_PRFXV6
| ip_prefix_list ',' TKN_PRFXV6RNG
| ip_prefix_list ',' TKN_PRFXV6DC
| ip_prefix_list ',' TKN_PRFXV6DCRNG
;

%%

#undef mnt_routes6error
#undef yyerror

int
mnt_routes6error (const char *s)
{
    yyerror(s);
    return 0;
}

