%{
/*
  filename: interface.l

  description:
    Defines the tokenizer for an RPSLng interface attribute.  Derived
    from ifaddr.l.

  notes:
    Tokens are defined in the associated grammar, interface.y.

  $Id: interface.l,v 1.1 2004/12/27 16:38:45 can Exp $
*/
%}

FLTRNAME       FLTR-[A-Z0-9_-]*[A-Z0-9]
ASNAME         AS-[A-Z0-9_-]*[A-Z0-9]
RSNAME         RS-[A-Z0-9_-]*[A-Z0-9]
PRNGNAME       PRNG-[A-Z0-9_-]*[A-Z0-9]
RTRSNAME       RTRS-[A-Z0-9_-]*[A-Z0-9]
INT            [0-9]+
ASNO           AS{INT}
QUAD           [0-9A-F]{1,4}
IPV4           {INT}(\.{INT}){3}
IPV6           {QUAD}(:{QUAD}){7}
IPV6DC         (({QUAD}:){0,6}{QUAD})?::({QUAD}(:{QUAD}){0,6})?
COMM_NO        {INT}:{INT}
ENCAPSULATION  GRE|IPINIP

%{
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* tokens defined in the grammar */
#include "interface.tab.h"

#define interfacewrap yywrap
void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
#define YY_NO_UNPUT  /* otherwise we get compiler warnings "`yyunput' defined but not used" */
%}

%%

[ \t\n]+    { ; }

OR    { return OP_OR; }
==    { return OP_COMPARE; }
=     { return OP_EQUAL; }
\.=   { return OP_APPEND; }


ACTION    { return KEYW_ACTION; }
IGP_COST  { return KEYW_IGP_COST; }
SELF      { return KEYW_SELF; }
PREPEND   { return KEYW_PREPEND; }
APPEND    { return KEYW_APPEND; }
DELETE    { return KEYW_DELETE; }
CONTAINS  { return KEYW_CONTAINS; }

INTERNET      { return KEYW_INTERNET; }
NO_EXPORT     { return KEYW_NO_EXPORT; }
NO_ADVERTISE  { return KEYW_NO_ADVERTISE; }

MASKLEN     { return KEYW_MASKLEN; }

TUNNEL      { return KEYW_TUNNEL; }
GRE|IPV6INIPV4|IPINIP|DVMRP { return KEYW_ENCAPSULATION; }

PREF        { return TKN_PREF; }
MED         { return TKN_MED; }
DPA         { return TKN_DPA; }
ASPATH      { return TKN_ASPATH; }
COMMUNITY   { return TKN_COMMUNITY; }
NEXT_HOP    { return TKN_NEXT_HOP; }
COST        { return TKN_COST; }

{ASNO} {
    long int val;
    char *s;
    val = strtol(yytext+2, &s, 10);
    if ((val < 0) || (val > 65535) || (*s != '\0')) {
        syntax_error("AS number \"%s\" is not between 0 and 65535", yytext);
    }
    return TKN_ASNO;
}

{IPV4} {
    /* check each number of 1.2.3.4 is valid */
    long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '\0') {
        val = strtol(p, &s, 10);
        if ((val < 0) || (val > 255)) {
             syntax_error("IP address \"%s\" contains an invalid octet", 
                          yytext);
        }
        p = s + 1;
    }
    return TKN_IPV4;
}

{IPV6} {
    /* check each quad of 1A:3:7:8:AAAA:BBBB:DEAD:BEEF in address is valid */
    long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '\0') {
        val = strtol(p, &s, 16);
        if ((val < 0) || (val > 65535)) {
             syntax_error("IPv6 address \"%s\" contains an invalid quad",
                          yytext);
        }
        p = s + 1;
    }
    return TKN_IPV6;
}

{IPV6DC} {
    /* check each quad of 1A::AAAA:BBBB:DEAD:BEEF in address is valid */
    long int val;
    char *s, *p, *r;
    int leftquads = 0;
    int rightquads = 0;

    r = p = s = yytext;
    while (*s != '\0') {
        val = strtol(p, &s, 16);
        if ((val < 0) || (val > 65535)) {
             syntax_error("IPv6 address \"%s\" contains an invalid quad",
                          yytext);
        }
        p = s + 1;
    }
    /* And now, count colons to the left of "::" - this is
       one less then the number of left quads */

    r = strchr(yytext, ':');
    leftquads++;
    while (r[1] != ':') {
        leftquads++;
        r = strchr(r+1, ':');
    }
    /* skip "::" */
    r += 2;
    /* count colons to the right of "::" - one less then the number of right quads */
    if (r[0] != '\0') {
        do {
            rightquads++;
            r = strchr(r+1, ':');
        } while (r != NULL);
    }
    /* check for too many quads */
    if ((leftquads + rightquads) > 8) {
        syntax_error("IPv6 address \"%s\" contains too many digits", yytext);
    }

    return TKN_IPV6DC;
}

{COMM_NO} {
    /* verify each part is a 16-bit number */
    long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '\0') {
        val = strtol(p, &s, 10);
        if ((val < 0) || (val > 65535)) {
             syntax_error("Community number \"%s\" contains an invalid number", 
                           yytext);
        }
        p = s + 1;
    }
    return TKN_COMM_NO;
}

{INT} { 
    interfacelval.sval = yytext;
    return TKN_INT; 
}

. { return yytext[0]; }

%%

void 
interface_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}

