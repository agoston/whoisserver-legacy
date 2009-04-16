%{
/*
  filename: mp_import.l

  description:
    Defines the tokenizer for an RPSLng mp-import attribute. Derived from
    import.l. 

  notes:
    Tokens are defined in the associated grammar, mp_import.y.

  $Id: mp_import.l,v 1.1.14.1 2006/09/29 12:32:29 katie Exp $
*/
%}

FLTRNAME       FLTR-[A-Z0-9_-]*[A-Z0-9]
ASNAME         AS-[A-Z0-9_-]*[A-Z0-9]
RSNAME         RS-[A-Z0-9_-]*[A-Z0-9]
PRNGNAME       PRNG-[A-Z0-9_-]*[A-Z0-9]
RTRSNAME       RTRS-[A-Z0-9_-]*[A-Z0-9]
INT            [0-9]+
QUAD           [0-9A-F]{1,4}
IPV4           {INT}(\.{INT}){3}
IPV6           {QUAD}(:{QUAD}){7}
IPV6DC         (({QUAD}:){0,6}{QUAD})?::({QUAD}(:{QUAD}){0,6})?
PRFXV4         {IPV4}\/{INT}
PRFXV6         {IPV6}\/{INT}
PRFXV6DC       {IPV6DC}\/{INT}
PRFXV4RNG      {PRFXV4}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
PRFXV6RNG      {PRFXV6}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
PRFXV6DCRNG    {PRFXV6DC}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
COMM_NO        {INT}:{INT}
PROTOCOL_NAME  BGP4|MPBGP|OSPF|RIP|IGRP|IS-IS|STATIC|RIPng|DVMRP|PIM-DM|PIM-SM|CBT|MOSPF
AFI            AFI
AFIVALUE_V4    IPV4|IPV4\.UNICAST|IPV4\.MULTICAST
AFIVALUE_V6    IPV6|IPV6\.UNICAST|IPV6\.MULTICAST
AFIVALUE_ANY   ANY\.UNICAST|ANY\.MULTICAST
DNAME          [[:alnum:]]([[:alnum:]-]*[[:alnum:]])?
ASNODOT        [^ ]*AS[0-9]+\.[0-9]+[^ ]*
ASNO           AS([0-9]|[1-9][0-9]{1,8}|[1-3][0-9]{9}|4[0-1][0-9]{8}|42[0-8][0-9]{7}|429[0-3][0-9]{6}|4294[0-8][0-9]{5}|42949[0-5][0-9]{4}|429496[0-6][0-9]{3}|4294967[0-1][0-9]{2}|42949672[0-8][0-9]|429496729[0-5])

%{
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

/* tokens defined in the grammar */
#include "mp_import.tab.h"
void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
#define YY_NO_UNPUT  /* otherwise we get compiler warnings "`yyunput' defined but not used" */

%}

%%

[ \t\n]+    { ; }

OR    { return OP_OR; }
AND   { return OP_AND; }
NOT   { return OP_NOT; }
==    { return OP_COMPARE; }
=     { return OP_EQUAL; }
\.=   { return OP_APPEND; }

\^-         { return OP_MS; }
\^\+        { return OP_MS; }
\^[0-9]+ { 
    unsigned long int val;
    errno = 0;
    val = strtoul(yytext+1, NULL, 10);
    if (errno || (val > 32)) {
        syntax_error("more specifics operator \"%s\" not 0 to 32 bits", 
                     yytext);
    }
    return OP_MS;
}
\^[0-9]+-[0-9]+ {
    unsigned long int val1, val2;
    char *p;
    errno = 0;
    val1 = strtoul(yytext+1, &p, 10);
    if (errno || (val1 > 32)) {
        syntax_error("more specifics operator \"%s\" not 0 to 32 bits", 
                     yytext);
    }
    errno = 0;
    val2 = strtoul(p+1, NULL, 10);
    if (errno || (val2 > 32)) {
        syntax_error("more specifics operator \"%s\" not 0 to 32 bits", 
                     yytext);
    }
    if (val2 < val1) {
        syntax_error("more specifics operator \"%s\" not 0 to 32 bits", 
                     yytext);
    }
    return OP_MS;
}


ANY     { return KEYW_ANY; }
PEERAS  { return KEYW_PEERAS; }

FROM        { return KEYW_FROM; }
ACTION    { return KEYW_ACTION; }
IGP_COST  { return KEYW_IGP_COST; }
SELF      { return KEYW_SELF; }
PREPEND   { return KEYW_PREPEND; }
APPEND    { return KEYW_APPEND; }
DELETE    { return KEYW_DELETE; }
CONTAINS  { return KEYW_CONTAINS; }
ACCEPT    { return KEYW_ACCEPT; }

INTERNET      { return KEYW_INTERNET; }
NO_EXPORT     { return KEYW_NO_EXPORT; }
NO_ADVERTISE  { return KEYW_NO_ADVERTISE; }

AT          { return KEYW_AT; }
PROTOCOL    { return KEYW_PROTOCOL; }
INTO        { return KEYW_INTO; }
REFINE      { return KEYW_REFINE; }
EXCEPT      { return KEYW_EXCEPT; }

{PROTOCOL_NAME} { return TKN_PROTOCOL; }

{AFI}       { return KEYW_AFI; }

{AFIVALUE_V4}  { return KEYW_AFI_VALUE_V4; }
{AFIVALUE_V6}  { return KEYW_AFI_VALUE_V6; }
{AFIVALUE_ANY} { return KEYW_AFI_VALUE_ANY; }

PREF        { return TKN_PREF; }
MED         { return TKN_MED; }
DPA         { return TKN_DPA; }
ASPATH      { return TKN_ASPATH; }
COMMUNITY   { return TKN_COMMUNITY; }
NEXT_HOP    { return TKN_NEXT_HOP; }
COST        { return TKN_COST; }

\~\*            { return ASPATH_POSTFIX; }
\~\+            { return ASPATH_POSTFIX; }
\~?\{INT\}      { return ASPATH_POSTFIX; }
\~?\{INT,INT\}  { return ASPATH_POSTFIX; }
\~?\{INT,\}     { return ASPATH_POSTFIX; }

{ASNODOT} { 
    syntax_error("The RIPE Database represents AS numbers in \'ASn\' notation");
}

{ASNO} {
    return TKN_ASNO;
}

(({ASNO}|peeras|{FLTRNAME}):)*{FLTRNAME}(:({ASNO}|peeras|{FLTRNAME}))* {
    return TKN_FLTRNAME;
}

(({ASNO}|peeras|{ASNAME}):)*{ASNAME}(:({ASNO}|peeras|{ASNAME}))* {
    return TKN_ASNAME;
}

(({ASNO}|peeras|{RSNAME}):)*{RSNAME}(:({ASNO}|peeras|{RSNAME}))* {
    return TKN_RSNAME;
}

(({ASNO}|peeras|{PRNGNAME}):)*{PRNGNAME}(:({ASNO}|peeras|{PRNGNAME}))* { 
    return TKN_PRNGNAME;
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
        syntax_error("IP prefix range \"%s\" contains an invalid prefix length",
                     yytext);
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

{PRFXV6RNG} {
    /* check each quad of 1A:3:7:8:AAAA:BBBB:DEAD:BEEF/55 in prefix is valid, as
       well as any bit ranges specified */
    unsigned long int val;
    unsigned long int endval;
    char *s, *p;
    p = s = yytext;
    while (*s != '/') {
        errno = 0;
        val = strtoul(p, &s, 16);
        if (errno || (val > 65535)) {
             syntax_error("IPv6 prefix \"%s\" contains an invalid quad",
                          yytext);
        }
        p = s + 1;
    }
    errno = 0;
    val = strtoul(p, &s, 10);
    if (errno || (val > 128)) {
        syntax_error("IPv6 prefix range \"%s\" contains an invalid prefix length",
                     yytext);
    }
    p = s + 1;
    if (isdigit((int)*p)) {
        errno = 0;
        val = strtoul(p, &s, 10);
        if (errno || (val > 128)) {
            syntax_error("IPv6 prefix range \"%s\" contains an invalid range",
                         yytext);
        }
        p = s + 1;
        if (isdigit((int)*p)) {
            errno = 0;
            endval = strtoul(p, NULL, 10);
            if (errno || (endval > 128)) {
                syntax_error("IPv6 prefix \"%s\" contains an invalid prefix range",
                             yytext);
            }
            if (endval < val) {
                syntax_error("IPv6 prefix \"%s\" range end is less than range start",
                             yytext);
            }
        }
    }
    return TKN_PRFXV6RNG;
}

{PRFXV6DCRNG} {
    /* check each quad of 1A:3:7:8:AAAA:BBBB:DEAD:BEEF/55 in prefix is valid, as
       well as any bit ranges specified */
    unsigned long int val;
    unsigned long int endval;
    char *s, *p, *r;
    int leftquads = 0;
    int rightquads = 0;

    p = s = yytext;
    while (*s != '/') {
        errno = 0;
        val = strtoul(p, &s, 16);
        if (errno || (val > 65535)) {
             syntax_error("IPv6 prefix \"%s\" contains an invalid quad",
                          yytext);
        }
        p = s + 1;
    }
    errno = 0;
    val = strtoul(p, &s, 10);
    if (errno || (val > 128)) {
        syntax_error("IPv6 prefix range \"%s\" contains an invalid prefix length",
                     yytext);
    }
    p = s + 1;
    if (isdigit((int)*p)) {
        errno = 0;
        val = strtoul(p, &s, 10);
        if (errno || (val > 128)) {
            syntax_error("IPv6 prefix range \"%s\" contains an invalid range",
                         yytext);
        }
        p = s + 1;
        if (isdigit((int)*p)) {
            errno = 0;
            endval = strtoul(p, NULL, 10);
            if (errno || (endval > 128)) {
                syntax_error("IPv6 prefix \"%s\" contains an invalid prefix range",
                             yytext);
            }
            if (endval < val) {
                syntax_error("IPv6 prefix \"%s\" range end is less than range start",
                             yytext);
            }
        }
    }

    /* And now, count colons to the left of "::" - this is
       one less than the number of left quads */

    r = strchr(yytext, ':');
    leftquads++;
    while (r[1] != ':') {
        leftquads++;
        r = strchr(r+1, ':');
    }
    /* skip "::" */
    r += 2;
    /* count colons to the right of "::" - one less than the number of right quads */
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

    return TKN_PRFXV6DCRNG;
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

{PRFXV6} {
    /* check each quad of 1A:3:7:8:AAAA:BBBB:DEAD:BEEF/55 in prefix is valid */
    unsigned long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '/') {
        errno = 0;
        val = strtoul(p, &s, 16);
        if (errno || (val > 65535)) {
             syntax_error("IPv6 prefix \"%s\" contains an invalid quad",
                          yytext);
        }
        p = s + 1;
    }
    errno = 0;
    val = strtoul(p, NULL, 10);
    if (errno || (val > 128)) {
        syntax_error("IPv6 prefix \"%s\" contains an invalid prefix length",
                     yytext);
    }
    return TKN_PRFXV6;
}

{PRFXV6DC} {
    /* check each quad of 1A::AAAA:BBBB:DEAD:BEEF/55 in prefix is valid */
    unsigned long int val;
    char *s, *p, *r;
    int leftquads = 0;
    int rightquads = 0;

    r = p = s = yytext;
    while (*s != '/') {
        errno = 0;
        val = strtoul(p, &s, 16);
        if (errno || (val > 65535)) {
             syntax_error("IPv6 prefix \"%s\" contains an invalid quad",
                          yytext);
        }
        p = s + 1;
    }
    errno = 0;
    val = strtoul(p, NULL, 10);
    if (errno || (val > 128)) {
        syntax_error("IPv6 prefix \"%s\" contains an invalid prefix length",
                     yytext);
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

    return TKN_PRFXV6DC;
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

{IPV6} {
    /* check each quad of 1A:3:7:8:AAAA:BBBB:DEAD:BEEF in address is valid */
    unsigned long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '\0') {
        errno = 0;
        val = strtoul(p, &s, 16);
        if (errno || (val > 65535)) {
             syntax_error("IPv6 address \"%s\" contains an invalid quad",
                          yytext);
        }
        p = s + 1;
    }
    return TKN_IPV6;
}

{IPV6DC} {
    /* check each quad of 1A::AAAA:BBBB:DEAD:BEEF in address is valid */
    unsigned long int val;
    char *s, *p, *r;
    int leftquads = 0;
    int rightquads = 0;

    r = p = s = yytext;
    while (*s != '\0') {
        errno = 0;
        val = strtoul(p, &s, 16);
        if (errno || (val > 65535)) {
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
    mp_importlval.sval = yytext;
    return TKN_INT; 
}

{DNAME} { 
    /* check the length */
    if (strlen(yytext) > 63) {
       syntax_error("Portion of domain name \"%s\" is longer "
                                                           "than 63 characters",
                    yytext); 
    }
    mp_importlval.sval = yytext;
    return TKN_DNS; 
}

. { return yytext[0]; }

%%

void 
mp_import_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}

int yywrap() { return 1; }
