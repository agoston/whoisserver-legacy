%{
/*
  filename: interface.y

  description:
    Defines the grammar for an RPSLng interface attribute. Derived
    from ifaddr.y.

  notes:
    Defines tokens for the associated lexer, interface.l.
*/
%}

%union {
    char *sval;
}

%token OP_OR OP_EQUAL OP_APPEND OP_COMPARE
%token TKN_ASNO 
%token TKN_IPV4 TKN_IPV6 TKN_IPV6DC
%token KEYW_ACTION 
%token TKN_PREF TKN_MED TKN_DPA TKN_ASPATH TKN_COMMUNITY TKN_NEXT_HOP TKN_COST
%token TKN_COMM_NO 
%token KEYW_IGP_COST KEYW_SELF KEYW_PREPEND
%token KEYW_APPEND KEYW_DELETE KEYW_CONTAINS 
%token KEYW_INTERNET KEYW_NO_EXPORT KEYW_NO_ADVERTISE KEYW_MASKLEN
%token KEYW_AFI KEYW_ENCAPSULATION KEYW_TUNNEL
%token <sval> TKN_INT

%{
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int yyerror(const char *s);
void syntax_error(char *fmt, ...);

%}

%%

interface:
  TKN_IPV4 KEYW_MASKLEN     v4_masklen
| TKN_IPV4 KEYW_MASKLEN     v4_masklen KEYW_ACTION v4_action
| ipv6_address KEYW_MASKLEN v6_masklen 
| ipv6_address KEYW_MASKLEN v6_masklen KEYW_ACTION v6_action
| ipv6_address KEYW_MASKLEN v6_masklen KEYW_TUNNEL
         ipv6_address ',' KEYW_ENCAPSULATION
| ipv6_address KEYW_MASKLEN v6_masklen KEYW_ACTION v6_action
         KEYW_TUNNEL ipv6_address ',' KEYW_ENCAPSULATION
;

v4_masklen: TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $1;
      val = strtoul(p, &s, 10);
      if ((val < 0) || (val > 32) || (*s != '\0')) {
          syntax_error("masklen \"%s\" is not between 0 and 32", p);
      }
}
;

v6_masklen: TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $1;
      val = strtoul(p, &s, 10);
      if ((val < 0) || (val > 128) || (*s != '\0')) {
          syntax_error("masklen \"%s\" is not between 0 and 128", p);
      }
}
;

v4_action: v4_rp_attribute ';'
| v4_action v4_rp_attribute ';'
;

v6_action: v6_rp_attribute ';'
| v6_action v6_rp_attribute ';'
;

v4_rp_attribute: pref
| med
| dpa
| aspath
| community
| v4_next_hop
| cost
;

v6_rp_attribute: pref
| med
| dpa
| aspath
| community
| v6_next_hop
| cost
;

pref: TKN_PREF OP_EQUAL TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $3;
      val = strtoul(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("pref value \"%s\" is not between 0 and 65535", p);
      }
}
;

med: TKN_MED OP_EQUAL TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $3;
      val = strtoul(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("med value \"%s\" is not between 0 and 65535", p);
      }
}
| TKN_MED OP_EQUAL KEYW_IGP_COST
;

dpa: TKN_DPA OP_EQUAL TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $3;
      val = strtoul(p, &s, 10);
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

v4_next_hop: TKN_NEXT_HOP OP_EQUAL TKN_IPV4
| TKN_NEXT_HOP OP_EQUAL KEYW_SELF
;

v6_next_hop: TKN_NEXT_HOP OP_EQUAL ipv6_address
| TKN_NEXT_HOP OP_EQUAL KEYW_SELF
;

cost: TKN_COST OP_EQUAL TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $3;
      val = strtoul(p, &s, 10);
      if ((val < 0) || (val > 65535)) {
          syntax_error("cost value \"%s\" is not between 0 and 65535", p);
      }
}
;

ipv6_address: TKN_IPV6
| TKN_IPV6DC
;

%%

#undef interfaceerror
#undef yyerror

int
interfaceerror (const char *s)
{
    yyerror(s);
    return 0;
}

