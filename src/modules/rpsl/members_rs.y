%{
/*
  filename: members_rs.y

  description:
    Defines the grammar for an RPSL members_rs attribute.

  notes:
    Defines tokens for the associated lexer, members_rs.l.
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

%token TKN_ASNO TKN_PRFXV4 TKN_PRFXV4RNG TKN_RSNAME TKN_ASNAME OP_MS

%%

members_rs: members_rs ',' members_item
| members_item
;

members_item: TKN_ASNO
| TKN_ASNO OP_MS
| TKN_ASNAME
| TKN_ASNAME OP_MS
| TKN_PRFXV4
| TKN_PRFXV4RNG
| TKN_RSNAME
| TKN_RSNAME OP_MS
;

%%

#undef members_rserror
#undef yyerror

int
members_rserror (const char *s)
{
    yyerror(s);
    return 0;
}

