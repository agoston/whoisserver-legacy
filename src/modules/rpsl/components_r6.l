%{
/*
  filename: components_r6.l

  description:
    Defines the tokenizer for an RPSLng route6 components attribute.  It is
    derived from components.l.

  notes:
    Tokens are defined in the associated grammar, components.l.

  $Id: components_r6.l,v 1.1 2004/12/27 16:38:44 can Exp $
*/
%}

ASNO           AS([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-4])
FLTRNAME       FLTR-[A-Z0-9_-]*[A-Z0-9]
ASNAME         AS-[A-Z0-9_-]*[A-Z0-9]
RSNAME         RS-[A-Z0-9_-]*[A-Z0-9]
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
PROTOCOL_NAME  BGP4|MPBGP|OSPF|RIP|IGRP|IS-IS|STATIC|RIPng|DVMRP|PIM-DM|PIM-SM|CBT|MOSPF
COMM_NO        {INT}:{INT}

%{
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* tokens defined in the grammar */
#include "components_r6.tab.h"

#define components_r6wrap yywrap
void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

[ \t\n]+    { ; }

OR    { return OP_OR; }
AND   { return OP_AND; }
NOT   { return OP_NOT; }
=     { return OP_EQUAL; }
==    { return OP_COMPARE; }
\.=   { return OP_APPEND; }

\^-         { return OP_MS; }
\^\+        { return OP_MS; }
\^[0-9]+ { 
    long int val;
    val = strtol(yytext+1, NULL, 10);
    if ((val < 0) || (val > 128)) {
        syntax_error("more specifics operator \"%s\" not 0 to 128 bits",
                     yytext);
    }
    if (val <= 32) {
      return OP_MS;
    }
    return OP_MS_V6;
}
\^[0-9]+-[0-9]+ {
    long int val1, val2;
    char *p;
    val1 = strtol(yytext+1, &p, 10);
    if ((val1 < 0) || (val1 > 128)) {
        syntax_error("more specifics operator \"%s\" not 0 to 128 bits",
                     yytext);
    }
    val2 = strtol(p+1, NULL, 10);
    if ((val2 < 0) || (val2 > 128)) {
        syntax_error("more specifics operator \"%s\" not 0 to 128 bits",
                     yytext);
    }
    if (val2 < val1) {
        syntax_error("more specifics range operator \"%s\" is invalid, range end less than range beginning",
                     yytext);
    }
    if(val2 <= 32) {
      return OP_MS;
    }
    return OP_MS_V6;
}


ATOMIC  { return KEYW_ATOMIC; }
ANY     { return KEYW_ANY; }
PEERAS  { return KEYW_PEERAS; }

IGP_COST  { return KEYW_IGP_COST; }
SELF      { return KEYW_SELF; }
PREPEND   { return KEYW_PREPEND; }
APPEND    { return KEYW_APPEND; }
DELETE    { return KEYW_DELETE; }
CONTAINS  { return KEYW_CONTAINS; }

INTERNET      { return KEYW_INTERNET; }
NO_EXPORT     { return KEYW_NO_EXPORT; }
NO_ADVERTISE  { return KEYW_NO_ADVERTISE; }

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

PROTOCOL         { return KEYW_PROTOCOL; }
{PROTOCOL_NAME}  { return TKN_PROTOCOL; }

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

{PRFXV4} {
    /* check each number of 1.2.3.4/5 in prefix is valid */
    long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '/') {
        val = strtol(p, &s, 10);
        if ((val < 0) || (val > 255)) {
             syntax_error("IP prefix \"%s\" contains an invalid octet", 
                          yytext);
        }
        p = s + 1;
    }
    val = strtol(p, NULL, 10);
    if ((val < 0) || (val > 32)) {
        syntax_error("IP prefix \"%s\" contains an invalid prefix length", 
                     yytext);
    }
    return TKN_PRFXV4;
}

{PRFXV6} {
    /* check each quad of 1A:3:7:8:AAAA:BBBB:DEAD:BEEF/55 in prefix is valid */
    long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '/') {
        val = strtol(p, &s, 16);
        if ((val < 0) || (val > 65535)) {
             syntax_error("IPv6 prefix \"%s\" contains an invalid quad",
                          yytext);
        }
        p = s + 1;
    }
    val = strtol(p, NULL, 10);
    if ((val < 0) || (val > 128)) {
        syntax_error("IPv6 prefix \"%s\" contains an invalid prefix length",
                     yytext);
    }
    return TKN_PRFXV6;
}

{PRFXV6DC} {
    /* check each quad of 1A::AAAA:BBBB:DEAD:BEEF/55 in prefix is valid */
    long int val;
    char *s, *p, *r;
    int leftquads = 0;
    int rightquads = 0;

    r = p = s = yytext;
    while (*s != '/') {
        val = strtol(p, &s, 16);
        if ((val < 0) || (val > 65535)) {
             syntax_error("IPv6 prefix \"%s\" contains an invalid quad",
                          yytext);
        }
        p = s + 1;
    }
    val = strtol(p, NULL, 10);
    if ((val < 0) || (val > 128)) {
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

{PRFXV4RNG} {
    /* check each number of 1.2.3.4/5 in prefix is valid, as
       well as any bit ranges specified */
    long int val;
    long int endval;
    char *s, *p;
    p = s = yytext;
    while (*s != '/') {
        val = strtol(p, &s, 10);
        if ((val < 0) || (val > 255)) {
             syntax_error("IP prefix \"%s\" contains an invalid octet", 
                          yytext);
        }
        p = s + 1;
    }
    val = strtol(p, &s, 10);
    if ((val < 0) || (val > 32)) {
        syntax_error("IP prefix range \"%s\" contains an invalid prefix length",
                     yytext);
    }
    p = s + 1;
    if (isdigit((int)*p)) {
        val = strtol(p, &s, 10);
        if ((val < 0) || (val > 32)) {
            syntax_error("IP prefix range \"%s\" contains an invalid range",
                         yytext);
        }
        p = s + 1;
        if (isdigit((int)*p)) {
            endval = strtol(p, NULL, 10);
            if ((endval < 0) || (endval > 32)) {
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
    long int val;
    long int endval;
    char *s, *p;
    p = s = yytext;
    while (*s != '/') {
        val = strtol(p, &s, 16);
        if ((val < 0) || (val > 65535)) {
             syntax_error("IPv6 prefix \"%s\" contains an invalid quad",
                          yytext);
        }
        p = s + 1;
    }
    val = strtol(p, &s, 10);
    if ((val < 0) || (val > 128)) {
        syntax_error("IPv6 prefix range \"%s\" contains an invalid prefix length",
                     yytext);
    }
    p = s + 1;
    if (isdigit((int)*p)) {
        val = strtol(p, &s, 10);
        if ((val < 0) || (val > 128)) {
            syntax_error("IPv6 prefix range \"%s\" contains an invalid range",
                         yytext);
        }
        p = s + 1;
        if (isdigit((int)*p)) {
            endval = strtol(p, NULL, 10);
            if ((endval < 0) || (endval > 128)) {
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
    long int val;
    long int endval;
    char *s, *p, *r;
    int leftquads = 0;
    int rightquads = 0;

    p = s = yytext;
    while (*s != '/') {
        val = strtol(p, &s, 16);
        if ((val < 0) || (val > 65535)) {
             syntax_error("IPv6 prefix \"%s\" contains an invalid quad",
                          yytext);
        }
        p = s + 1;
    }
    val = strtol(p, &s, 10);
    if ((val < 0) || (val > 128)) {
        syntax_error("IPv6 prefix range \"%s\" contains an invalid prefix length",
                     yytext);
    }
    p = s + 1;
    if (isdigit((int)*p)) {
        val = strtol(p, &s, 10);
        if ((val < 0) || (val > 128)) {
            syntax_error("IPv6 prefix range \"%s\" contains an invalid range",
                         yytext);
        }
        p = s + 1;
        if (isdigit((int)*p)) {
            endval = strtol(p, NULL, 10);
            if ((endval < 0) || (endval > 128)) {
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
    components_r6lval.sval = yytext;
    return TKN_INT;
}

. { return yytext[0]; }

%%

void
components_r6_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}


