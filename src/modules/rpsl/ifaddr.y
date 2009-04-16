%{
/*
  filename: ifaddr.y

  description:
    Defines the grammar for an RPSL ifaddr attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Defines tokens for the associated lexer, ifaddr.l.
*/
%}

%union {
    char *sval;
}

%token OP_OR OP_EQUAL OP_APPEND OP_COMPARE
%token TKN_ASNO 
%token TKN_IPV4 
%token KEYW_ACTION 
%token TKN_PREF TKN_MED TKN_DPA TKN_ASPATH TKN_COMMUNITY TKN_NEXT_HOP TKN_COST
%token TKN_COMM_NO 
%token KEYW_IGP_COST KEYW_SELF KEYW_PREPEND
%token KEYW_APPEND KEYW_DELETE KEYW_CONTAINS 
%token KEYW_INTERNET KEYW_NO_EXPORT KEYW_NO_ADVERTISE KEYW_MASKLEN
%token <sval> TKN_INT TKN_DNS

%{
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

int yyerror(const char *s);
void syntax_error(char *fmt, ...);

%}

%%

ifaddr: TKN_IPV4 KEYW_MASKLEN masklen
| TKN_IPV4 KEYW_MASKLEN masklen KEYW_ACTION action
;

masklen: TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $1;
      errno = 0;
      val = strtoul(p, &s, 10);
      if (errno || (val > 32) || (*s != '\0')) {
          syntax_error("masklen \"%s\" is not between 0 and 32", p);
      }
}
;

action: rp_attribute ';'
| action rp_attribute ';'
;

rp_attribute: pref
| med
| dpa
| aspath
| community
| next_hop
| cost
;

pref: TKN_PREF OP_EQUAL TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $3;
      errno = 0;
      val = strtoul(p, &s, 10);
      if (errno || (val > 65535)) {
          syntax_error("pref value \"%s\" is not between 0 and 65535", p);
      }
}
;

med: TKN_MED OP_EQUAL TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $3;
      errno = 0;
      val = strtoul(p, &s, 10);
      if (errno || (val > 65535)) {
          syntax_error("med value \"%s\" is not between 0 and 65535", p);
      }
}
| TKN_MED OP_EQUAL KEYW_IGP_COST
;

dpa: TKN_DPA OP_EQUAL TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $3;
      errno = 0;
      val = strtoul(p, &s, 10);
      if (errno || (val > 65535)) {
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
      errno = 0;
      val = strtoul(p, &s, 10);
      if (errno || (val < 1) || (val > 4294967295UL) || (*s != '\0')) {
          syntax_error("community element \"%s\" is not between 1 and 4294967295", 
                       p);
      }
}
| TKN_COMM_NO 
;

next_hop: TKN_NEXT_HOP OP_EQUAL TKN_IPV4
| TKN_NEXT_HOP OP_EQUAL KEYW_SELF
;

cost: TKN_COST OP_EQUAL TKN_INT {
      unsigned long int val;
      char *s, *p;
      p = $3;
      errno = 0;
      val = strtoul(p, &s, 10);
      if (errno || (val > 65535)) {
          syntax_error("cost value \"%s\" is not between 0 and 65535", p);
      }
}
;

%%

#undef ifaddrerror
#undef yyerror

int
ifaddrerror (const char *s)
{
    yyerror(s);
    return 0;
}

