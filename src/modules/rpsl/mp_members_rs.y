%{
/*
  filename: mp_members_rs.y

  description:
    Defines the grammar for an RPSLng mp_members_rs attribute.
    (mp-members attribute of route-set class).

  notes:
    See the notes in mp_members_rs.l.
    Defines tokens for the associated lexer, mp_members_rs.l.
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

%token TKN_ASNO TKN_PRFXV4 TKN_PRFXV4RNG 
%token TKN_PRFXV6 TKN_PRFXV6DC TKN_PRFXV6RNG TKN_PRFXV6DCRNG TKN_RSNAME TKN_ASNAME OP_MS

%%

mp_members_rs: mp_members_rs ',' mp_members_item
| mp_members_item
;

mp_members_item: TKN_ASNO
| TKN_ASNO OP_MS
| TKN_ASNAME
| TKN_ASNAME OP_MS
| TKN_PRFXV4
| TKN_PRFXV4RNG
| TKN_PRFXV6
| TKN_PRFXV6DC
| TKN_PRFXV6RNG
| TKN_PRFXV6DCRNG
| TKN_RSNAME
| TKN_RSNAME OP_MS
;

%%

#undef mp_members_rserror
#undef yyerror

int
mp_members_rserror (const char *s)
{
    yyerror(s);
    return 0;
}

