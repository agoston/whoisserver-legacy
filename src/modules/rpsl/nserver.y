%{
/*
  filename: nserver.y

  description:
    Defines the grammar for an nserver attribute.

  notes:
    Defines tokens for the associated lexer, nserver.l.
*/

/******************
  Copyright (c) 2002                              RIPE NCC

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

int yyerror(const char *s);
%}

%token TKN_IPV6
%token TKN_IPV6DC
%token TKN_IPV4
%token TKN_HOSTNAME

%%

nserver: TKN_HOSTNAME
| ipv4_glue
| ipv6_glue
;

ipv4_glue: TKN_HOSTNAME TKN_IPV4
;

ipv6_glue: TKN_HOSTNAME TKN_IPV6
| TKN_HOSTNAME TKN_IPV6DC
;

%%

#undef nservererror
#undef yyerror

int
nservererror (const char *s)
{
    yyerror(s);
    return 0;
}

