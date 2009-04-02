%{
/*
  filename: components.y

  description:
    Defines the grammar for an RPSL component attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Defines tokens for the associated lexer, components.l.
*/
%}

%union {
    char *sval;
}

%token OP_OR OP_AND OP_NOT OP_MS OP_EQUAL OP_APPEND OP_COMPARE
%token KEYW_ANY KEYW_PEERAS
%token ASPATH_POSTFIX
%token TKN_FLTRNAME TKN_ASNO TKN_RSNAME TKN_ASNAME TKN_PRFXV4 TKN_PRFXV4RNG
%token KEYW_ATOMIC 
%token KEYW_PROTOCOL TKN_PROTOCOL
%token KEYW_IGP_COST KEYW_SELF KEYW_PREPEND KEYW_APPEND 
%token KEYW_DELETE KEYW_CONTAINS 
%token KEYW_INTERNET KEYW_NO_EXPORT KEYW_NO_ADVERTISE
%token TKN_PREF TKN_MED TKN_DPA TKN_ASPATH TKN_COMMUNITY
%token TKN_NEXT_HOP TKN_COST TKN_COMM_NO TKN_IPV4

%token <sval> TKN_INT

%{
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

int yyerror(const char *s);

/* defines for unit testing */
#ifdef UNIT_TEST
int main();
#endif /* UNIT_TEST */
%}

%%

components: opt_atomic components_list
;

opt_atomic:
| KEYW_ATOMIC
;

components_list:
| filter
| components_list KEYW_PROTOCOL TKN_PROTOCOL filter
;

filter: filter OP_OR filter_term
| filter filter_term %prec OP_OR
| filter_term
;

filter_term : filter_term OP_AND filter_factor
| filter_factor
;

filter_factor :  OP_NOT filter_factor
| '(' filter ')'
| filter_operand 
;

filter_operand: KEYW_ANY
| '<' filter_aspath '>'
| rp_attribute
| TKN_FLTRNAME
| filter_prefix
;

filter_prefix: filter_prefix_operand OP_MS
|  filter_prefix_operand
;

filter_prefix_operand: TKN_ASNO
| KEYW_PEERAS
| TKN_ASNAME
| TKN_RSNAME
| '{' opt_filter_prefix_list '}'
;

opt_filter_prefix_list:
| filter_prefix_list
;

filter_prefix_list: filter_prefix_list_prefix 
| filter_prefix_list ',' filter_prefix_list_prefix
;

filter_prefix_list_prefix: TKN_PRFXV4
| TKN_PRFXV4RNG
;

filter_aspath: filter_aspath '|' filter_aspath_term
| filter_aspath_term
;

filter_aspath_term: filter_aspath_term filter_aspath_closure
| filter_aspath_closure
;

filter_aspath_closure: filter_aspath_closure '*'
| filter_aspath_closure '?'
| filter_aspath_closure '+'
| filter_aspath_closure ASPATH_POSTFIX
| filter_aspath_factor
;

filter_aspath_factor: '^'
| '$'
| '(' filter_aspath ')'
| filter_aspath_no
;

filter_aspath_no: TKN_ASNO
| KEYW_PEERAS
| TKN_ASNAME
| '.'
| '[' filter_aspath_range ']'
| '[' '^' filter_aspath_range ']'
;

filter_aspath_range:
| filter_aspath_range TKN_ASNO
| filter_aspath_range KEYW_PEERAS
| filter_aspath_range '.'
| filter_aspath_range TKN_ASNO '-' TKN_ASNO
| filter_aspath_range TKN_ASNAME 
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
      if (errno || (val < 1 ) || (val > 4294967295UL) || (*s != '\0')) {
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

/* define a unit test that simply returns "yes" or "no" for filter input */
#ifndef UNIT_TEST

#undef componentserror
#undef yyerror

int
componentserror (const char *s)
{
    yyerror(s);
    return 0;
}
#else /* UNIT_TEST */

int parse_error_count;

/* parse error */
void 
yyerror(const char *s)
{
    fputs(s, stderr);
    putc('\n', stderr);
    parse_error_count++;
}

/* syntax error */
void 
syntax_error(const char *fmt, ...)
{
    va_list args;
    char buf[1024];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    yyerror(buf);
}

int
yywrap()
{
    return 1;
}

/* taken from _lex & yacc_ p.157, by Levine, Mason, & Brown (corrected) */
char *myinputptr;
char *myinputlim;

#undef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))

void
yy_input(char *buf, int *result, int max_size)
{
    int n = min(max_size, myinputlim - myinputptr);

    if (n > 0) {
        memcpy(buf, myinputptr, n);
        myinputptr += n;
    }
    *result = n;
}

/* program entry point */
int 
main()
{
    char buf[4096];

    setbuf(stdout, NULL);
    for (;;) {
        printf("filter> ");
        if (gets(buf) == NULL) {
            printf("\n");
            return(0);
        }
        parse_error_count = 0;
        myinputptr = buf;
        myinputlim = buf + strlen(buf);
        filterparse();
        printf("%d parse errors\n", parse_error_count);
    }
}
#endif /* UNIT_TEST */
