%{
/*
  filename: inject.l

  description:
    Defines the tokenizer for an RPSL inject attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Tokens are defined in the associated grammar, inject.y.

  $Id: inject.l,v 1.1.14.1 2006/09/29 12:32:29 katie Exp $
*/
%}

FLTRNAME       FLTR-[A-Z0-9_-]*[A-Z0-9]
ASNAME         AS-[A-Z0-9_-]*[A-Z0-9]
RSNAME         RS-[A-Z0-9_-]*[A-Z0-9]
PRNGNAME       PRNG-[A-Z0-9_-]*[A-Z0-9]
RTRSNAME       RTRS-[A-Z0-9_-]*[A-Z0-9]
INT            [0-9]+
IPV4           {INT}(\.{INT}){3}
PRFXV4         {IPV4}\/{INT}
PRFXV4RNG      {PRFXV4}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
COMM_NO        {INT}:{INT}
PROTOCOL_NAME  BGP4|OSPF|RIP|IGRP|IS-IS|STATIC|RIPng|DVMRP|PIM-DM|PIM-SM|CBT|MOSPF
DNAME          [[:alnum:]]([[:alnum:]-]*[[:alnum:]])?
ASNODOT        [^ ]*AS[0-9]+\.[0-9]+[^ ]*
ASNO           AS([0-9]|[1-9][0-9]{1,8}|[1-3][0-9]{9}|4[0-1][0-9]{8}|42[0-8][0-9]{7}|429[0-3][0-9]{6}|4294[0-8][0-9]{5}|42949[0-5][0-9]{4}|429496[0-6][0-9]{3}|4294967[0-1][0-9]{2}|42949672[0-8][0-9]|429496729[0-5])

%{
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

/* tokens defined in the grammar */
#include "inject.tab.h"

#define injectwrap yywrap
void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

[ \t\n]+    { ; }

AND   { return OP_AND; }
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

AT          { return KEYW_AT; }
EXCEPT      { return KEYW_EXCEPT; }
UPON        { return KEYW_UPON; }
STATIC      { return KEYW_STATIC; }
EXCLUDE     { return KEYW_EXCLUDE; }

HAVE-COMPONENTS   { return KEYW_HAVE_COMPONENTS; }

MASKLEN     { return KEYW_MASKLEN; }

PREF        { return TKN_PREF; }
MED         { return TKN_MED; }
DPA         { return TKN_DPA; }
ASPATH      { return TKN_ASPATH; }
COMMUNITY   { return TKN_COMMUNITY; }
NEXT_HOP    { return TKN_NEXT_HOP; }
COST        { return TKN_COST; }

{ASNODOT} {
    syntax_error("The RIPE Database represents AS numbers in \'ASn\' notation");
}

{ASNO} {
    return TKN_ASNO;
}

(({ASNO}|peeras|{RTRSNAME}):)*{RTRSNAME}(:({ASNO}|peeras|{RTRSNAME}))* {
    return TKN_RTRSNAME;
}

{PRFXV4RNG} {
    /* check each number of 1.2.3.4/5 in prefix is valid, as
       well as any bit ranges specified */
    unsigned long int val;
    unsigned long int endval;
    char *s, *p;
    p = s = yytext;
    while (*s != '/') {
        errno = 0;
        val = strtoul(p, &s, 10);
        if (errno || (val > 255)) {
             syntax_error("IP prefix \"%s\" contains an invalid octet",
                          yytext);
        }
        p = s + 1;
    }
    errno = 0;
    val = strtoul(p, &s, 10);
    if (errno || (val > 32)) {
        syntax_error("IP prefix range \"%s\" contains an invalid prefix length",                     yytext);
    }
    p = s + 1;
    if (isdigit((int)*p)) {
        errno = 0;
        val = strtoul(p, &s, 10);
        if (errno || (val > 32)) {
            syntax_error("IP prefix range \"%s\" contains an invalid range",
                         yytext);
        }
        p = s + 1;
        if (isdigit((int)*p)) {
            errno = 0;
            endval = strtoul(p, NULL, 10);
            if (errno || (endval > 32)) {
                syntax_error("IP prefix \"%s\" contains an invalid prefix range",
                             yytext);
            }
            if (endval < val) {
                syntax_error("IP prefix \"%s\" range end is less than range start",
                             yytext);
            }
        }
    }
    return TKN_PRFXV4RNG;
}

{PRFXV4} {
    /* check each number of 1.2.3.4/5 in prefix is valid */
    unsigned long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '/') {
        errno = 0;
        val = strtoul(p, &s, 10);
        if (errno || (val > 255)) {
             syntax_error("IP prefix \"%s\" contains an invalid octet",
                          yytext);
        }
        p = s + 1;
    }
    errno = 0;
    val = strtoul(p, NULL, 10);
    if (errno || (val > 32)) {
        syntax_error("IP prefix \"%s\" contains an invalid prefix length",
                     yytext);
    }
    return TKN_PRFXV4;
}

{IPV4} {
    /* check each number of 1.2.3.4 is valid */
    unsigned long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '\0') {
        errno = 0;
        val = strtoul(p, &s, 10);
        if (errno || (val > 255)) {
             syntax_error("IP address \"%s\" contains an invalid octet", 
                          yytext);
        }
        p = s + 1;
    }
    return TKN_IPV4;
}

{COMM_NO} {
    /* verify each part is a 16-bit number */
    unsigned long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '\0') {
        errno = 0;
        val = strtoul(p, &s, 10);
        if (errno || (val > 4294967295UL)) {
             syntax_error("Community number \"%s\" contains an invalid number", 
                           yytext);
        }
        p = s + 1;
    }
    return TKN_COMM_NO;
}

{INT} { 
    injectlval.sval = yytext;
    return TKN_INT; 
}

{DNAME} { 
    /* check the length */
    if (strlen(yytext) > 63) {
       syntax_error("Portion of domain name \"%s\" is longer "
                                                           "than 63 characters",                    yytext);
    }
    injectlval.sval = yytext;
    return TKN_DNS;
}

. { return yytext[0]; }

%%

void 
inject_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}

int yywrap() { return 1; }
