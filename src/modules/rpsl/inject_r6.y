%{
/*
  filename: inject_r6.y

  description:
    Defines the grammar for an RPSLng route6 inject attribute.  It is 
    derived from inject.y.

  notes:
    Defines tokens for the associated lexer, inject_r6.l.
*/
%}

%union {
    char *sval;
}

%token OP_OR OP_EQUAL OP_APPEND OP_COMPARE OP_AND
%token TKN_ASNO 
%token TKN_IPV4 TKN_IPV6 TKN_IPV6DC
%token KEYW_ACTION 
%token KEYW_AFI KEYW_AFI_VALUE
%token TKN_PREF TKN_MED TKN_DPA TKN_ASPATH TKN_COMMUNITY TKN_NEXT_HOP TKN_COST
%token TKN_COMM_NO TKN_RTRSNAME TKN_PRFXV4 TKN_PRFXV4RNG
%token TKN_PRFXV6 TKN_PRFXV6DC TKN_PRFXV6RNG TKN_PRFXV6DCRNG
%token KEYW_IGP_COST KEYW_SELF KEYW_PREPEND
%token KEYW_APPEND KEYW_DELETE KEYW_CONTAINS KEYW_AT KEYW_EXCEPT KEYW_UPON
%token KEYW_STATIC KEYW_HAVE_COMPONENTS KEYW_EXCLUDE
%token KEYW_INTERNET KEYW_NO_EXPORT KEYW_NO_ADVERTISE KEYW_MASKLEN
%token <sval> TKN_INT TKN_DNS
%type <sval> domain_name

%{
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int yyerror(const char *s);
void syntax_error(char *fmt, ...);

%}

%%

inject_r6: mp_opt_router_expr_with_at mp_opt_action mp_opt_inject_expr
;

mp_opt_router_expr_with_at:
| KEYW_AT mp_router_expr
;

mp_router_expr: mp_router_expr OP_OR mp_router_expr_term
| mp_router_expr_term
;

mp_router_expr_term: mp_router_expr_term OP_AND mp_router_expr_factor
| mp_router_expr_term KEYW_EXCEPT mp_router_expr_factor
| mp_router_expr_factor
;

mp_router_expr_factor: '(' mp_router_expr ')'
| mp_router_expr_operand
;

mp_router_expr_operand: TKN_IPV4
| TKN_IPV6
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

mp_opt_action:
| KEYW_ACTION mp_action
;

mp_opt_inject_expr: 
| KEYW_UPON mp_inject_expr 
;

mp_inject_expr: mp_inject_expr OP_OR mp_inject_expr_term
| mp_inject_expr_term
;

mp_inject_expr_term: mp_inject_expr_term OP_AND mp_inject_expr_factor
| mp_inject_expr_factor
;

mp_inject_expr_factor: '(' mp_inject_expr ')' 
| mp_inject_expr_operand
;

mp_inject_expr_operand: KEYW_STATIC 
| KEYW_HAVE_COMPONENTS '{' mp_opt_filter_prefix_list '}' 
| KEYW_EXCLUDE '{' mp_opt_filter_prefix_list '}' 
;

mp_opt_filter_prefix_list:
| mp_filter_prefix_list
;

mp_filter_prefix_list: mp_filter_prefix_list_prefix 
| mp_filter_prefix_list ',' mp_filter_prefix_list_prefix 
;

mp_filter_prefix_list_prefix: TKN_PRFXV4 
| TKN_PRFXV6
| TKN_PRFXV6DC
| TKN_PRFXV4RNG 
| TKN_PRFXV6RNG
| TKN_PRFXV6DCRNG
;

mp_action: mp_rp_attribute ';'
| mp_action mp_rp_attribute ';'
;

mp_rp_attribute: pref
| med
| dpa
| aspath
| community
| mp_next_hop
| cost
;

pref: TKN_PREF OP_EQUAL TKN_INT {
      long int val;
      char *s, *p;
      p = $3;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("pref value \"%s\" is not between 0 and 65535", p);
      }
}
;

med: TKN_MED OP_EQUAL TKN_INT {
      long int val;
      char *s, *p;
      p = $3;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("med value \"%s\" is not between 0 and 65535", p);
      }
}
| TKN_MED OP_EQUAL KEYW_IGP_COST
;

dpa: TKN_DPA OP_EQUAL TKN_INT {
      long int val;
      char *s, *p;
      p = $3;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("dpa value \"%s\" is not between 0 and 65535", p);
      }
}
;

aspath: TKN_ASPATH '.' KEYW_PREPEND '(' asno_list ')'
;

asno_list: TKN_ASNO
| asno_list ',' TKN_ASNO
;

community: TKN_COMMUNITY OP_EQUAL community_list
| TKN_COMMUNITY OP_APPEND community_list
| TKN_COMMUNITY '.' KEYW_APPEND '(' community_elm_list ')'
| TKN_COMMUNITY '.' KEYW_DELETE '(' community_elm_list ')'
| TKN_COMMUNITY '.' KEYW_CONTAINS '(' community_elm_list ')'
| TKN_COMMUNITY '(' community_elm_list ')'
| TKN_COMMUNITY OP_COMPARE community_list
;

community_list: '{' community_elm_list '}'
;

community_elm_list: community_elm
| community_elm_list ',' community_elm
;

community_elm: KEYW_INTERNET
| KEYW_NO_EXPORT
| KEYW_NO_ADVERTISE
| TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $1;
      val = strtoul(p, &s, 10);
      if ((val < 1) || (val > 4294967295UL) || (*s != '\0')) {
          syntax_error("community element \"%s\" is not between 1 and 4294967295", 
                       p);
      }
}
| TKN_COMM_NO 
;

mp_next_hop: TKN_NEXT_HOP OP_EQUAL TKN_IPV4
| TKN_NEXT_HOP OP_EQUAL TKN_IPV6
| TKN_NEXT_HOP OP_EQUAL TKN_IPV6DC
| TKN_NEXT_HOP OP_EQUAL KEYW_SELF
;

cost: TKN_COST OP_EQUAL TKN_INT {
      long int val;
      char *s, *p;
      p = $3;
      val = strtol(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("cost value \"%s\" is not between 0 and 65535", p);
      }
}
;

%%

#undef inject_r6error
#undef yyerror

int
inject_r6error (const char *s)
{
    yyerror(s);
    return 0;
}

