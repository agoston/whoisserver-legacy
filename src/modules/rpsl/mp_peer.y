%{
/*
  filename: mp_peer.y

  description:
    Defines the grammar for an RPSLng mp-peer attribute.
    Derived from peer.y.

  notes:
    Defines tokens for the associated lexer, mp_peer.l.
*/

/******************
  Copyright (c) 2002                                        RIPE NCC

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

%union {
    char *sval;
}

%token TKN_SIMPLE_PROTOCOL TKN_NON_SIMPLE_PROTOCOL
%token TKN_IPV4 TKN_IPV6 TKN_IPV6DC TKN_RTRSNAME TKN_PRNGNAME
%token TKN_ASNO TKN_SMALLINT
%token KEYW_ASNO KEYW_FLAP_DAMP
%token <sval> TKN_DNS
%type <sval> domain_name

%%

mp_peer: TKN_SIMPLE_PROTOCOL TKN_IPV4
| TKN_SIMPLE_PROTOCOL TKN_IPV6
| TKN_SIMPLE_PROTOCOL TKN_IPV6DC
| TKN_SIMPLE_PROTOCOL domain_name {
    if (strlen($2) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", $2);
    }
}
| TKN_SIMPLE_PROTOCOL TKN_RTRSNAME
| TKN_SIMPLE_PROTOCOL TKN_PRNGNAME
| TKN_NON_SIMPLE_PROTOCOL TKN_IPV4   bgp_opt
| TKN_NON_SIMPLE_PROTOCOL TKN_IPV6   bgp_opt
| TKN_NON_SIMPLE_PROTOCOL TKN_IPV6DC bgp_opt
| TKN_NON_SIMPLE_PROTOCOL domain_name bgp_opt {
    if (strlen($2) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", $2);
    }
}
| TKN_NON_SIMPLE_PROTOCOL TKN_RTRSNAME bgp_opt
| TKN_NON_SIMPLE_PROTOCOL TKN_PRNGNAME bgp_opt
;

domain_name: TKN_DNS
| domain_name '.' TKN_DNS
;

bgp_opt: KEYW_ASNO '(' TKN_ASNO ')'
| flap_damp ',' KEYW_ASNO '(' TKN_ASNO ')'
| KEYW_ASNO '(' TKN_ASNO ')' ',' flap_damp
;

flap_damp: KEYW_FLAP_DAMP '(' ')'
| KEYW_FLAP_DAMP '(' TKN_SMALLINT ',' 
                     TKN_SMALLINT ',' 
                     TKN_SMALLINT ',' 
                     TKN_SMALLINT ',' 
                     TKN_SMALLINT ',' 
                     TKN_SMALLINT ')'
;


%%

#undef mp_peererror
#undef yyerror

int
mp_peererror (const char *s)
{
    yyerror(s);
    return 0;
}

