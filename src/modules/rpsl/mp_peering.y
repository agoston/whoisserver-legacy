%{
/*
  filename: mp_peering.y

  description:
    Defines the grammar for an RPSLng mp-peering attribute. Derived
    from peering.l. 

  notes:
    Defines tokens for the associated lexer, mp_peering.l.
*/
%}

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

%union {
    char *sval;
}

%token OP_OR OP_AND 
%token TKN_ASNO TKN_ASNAME
%token TKN_IPV4 TKN_IPV6 TKN_IPV6DC TKN_DNS TKN_RTRSNAME TKN_PRNGNAME
%token KEYW_EXCEPT
%token KEYW_AT
%token <sval> TKN_DNS
%type <sval> domain_name

%{
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int yyerror(const char *s);
void syntax_error(char *fmt, ...);

%}

%%


mp_peering: as_expr v4_opt_router_expr v4_opt_router_expr_with_at {}
| as_expr v6_opt_router_expr v6_opt_router_expr_with_at {}
| TKN_PRNGNAME {}
;

as_expr: as_expr OP_OR as_expr_term
| as_expr_term 
;

as_expr_term: as_expr_term OP_AND as_expr_factor 
| as_expr_term KEYW_EXCEPT as_expr_factor 
| as_expr_factor
;

as_expr_factor: '(' as_expr ')' 
| as_expr_operand
;

as_expr_operand: TKN_ASNO 
| TKN_ASNAME 
;

v4_opt_router_expr:
| v4_router_expr
;

v6_opt_router_expr:
| v6_router_expr
;

v4_opt_router_expr_with_at: 
| KEYW_AT v4_router_expr
;

v6_opt_router_expr_with_at:
| KEYW_AT v6_router_expr
;

v4_router_expr: v4_router_expr OP_OR v4_router_expr_term
| v4_router_expr_term
;

v6_router_expr: v6_router_expr OP_OR v6_router_expr_term
| v6_router_expr_term
;

v4_router_expr_term: v4_router_expr_term OP_AND v4_router_expr_factor
| v4_router_expr_term KEYW_EXCEPT v4_router_expr_factor
| v4_router_expr_factor
;

v6_router_expr_term: v6_router_expr_term OP_AND v6_router_expr_factor
| v6_router_expr_term KEYW_EXCEPT v6_router_expr_factor
| v6_router_expr_factor
;

v4_router_expr_factor: '(' v4_router_expr ')'
| v4_router_expr_operand
;

v6_router_expr_factor: '(' v6_router_expr ')'
| v6_router_expr_operand
;

v4_router_expr_operand: TKN_IPV4
| domain_name {
    if (strlen($1) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", $1);
    }
}
| TKN_RTRSNAME
;

v6_router_expr_operand: TKN_IPV6
| TKN_IPV6DC
| domain_name {
    if (strlen($1) > 255) {
        syntax_error("Domain name \"%s\" is longer than 255 characters", $1);
    }
}
| TKN_RTRSNAME
;

domain_name: TKN_DNS
| domain_name '.' TKN_DNS
;

%%

#undef mp_peeringerror
#undef yyerror

int
mp_peeringerror (const char *s)
{
    yyerror(s);
    return 0;
}

