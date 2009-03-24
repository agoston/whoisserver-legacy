%{
/*
  filename: import.l

  description:
    Defines the tokenizer for an RPSL import attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Tokens are defined in the associated grammar, import.y.

  $Id: import.l,v 1.1.14.1 2006/09/29 12:32:29 katie Exp $
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
COMM_32_NO     [0-9]|[1-9][0-9]{1,8}|[1-3][0-9]{9}|4[0-1][0-9]{8}|42[0-8][0-9]{7}|429[0-3][0-9]{6}|4294[0-8][0-9]{5}|42949[0-5][0-9]{4}|429496[0-6][0-9]{3}|4294967[0-1][0-9]{2}|42949672[0-8][0-9]|429496729[0-5]
COMM_NO        {COMM_32_NO}:{COMM_32_NO}
PROTOCOL_NAME  BGP4|OSPF|RIP|IGRP|IS-IS|STATIC|RIPng|DVMRP|PIM-DM|PIM-SM|CBT|MOSPF
DNAME          [[:alnum:]]([[:alnum:]-]*[[:alnum:]])?
ASNODOT        [^ ]*AS[0-9]+\.[0-9]+[^ ]*
ASNO           AS([0-9]|[1-9][0-9]{1,8}|[1-3][0-9]{9}|4[0-1][0-9]{8}|42[0-8][0-9]{7}|429[0-3][0-9]{6}|4294[0-8][0-9]{5}|42949[0-5][0-9]{4}|429496[0-6][0-9]{3}|4294967[0-1][0-9]{2}|42949672[0-8][0-9]|429496729[0-5])

%{
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* tokens defined in the grammar */
#include "import.tab.h"
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
==    { return OP_COMPARE; }
=     { return OP_EQUAL; }
\.=   { return OP_APPEND; }

\^-         { return OP_MS; }
\^\+        { return OP_MS; }
\^[0-9]+ { 
    unsigned long int val;
    val = strtoul(yytext+1, NULL, 10);
    if ((val < 0) || (val > 32)) {
        syntax_error("more specifics operator \"%s\" not 0 to 32 bits", 
                     yytext);
    }
    return OP_MS;
}
\^[0-9]+-[0-9]+ {
    unsigned long int val1, val2;
    char *p;
    val1 = strtoul(yytext+1, &p, 10);
    if ((val1 < 0) || (val1 > 32)) {
        syntax_error("more specifics operator \"%s\" not 0 to 32 bits", 
                     yytext);
    }
    val2 = strtoul(p+1, NULL, 10);
    if ((val2 < 0) || (val2 > 32)) {
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
FROM        { return KEYW_FROM; }

BGP4|OSPF|RIP|IGRP|IS-IS|STATIC|RIPng|PIM-DM|PIM-SM|CBT|MOSPF {
    return TKN_PROTOCOL;
}

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
        val = strtoul(p, &s, 10);
        if ((val < 0) || (val > 255)) {
             syntax_error("IP prefix \"%s\" contains an invalid octet", 
                          yytext);
        }
        p = s + 1;
    }
    val = strtoul(p, &s, 10);
    if ((val < 0) || (val > 32)) {
        syntax_error("IP prefix range \"%s\" contains an invalid prefix length",
                     yytext);
    }
    p = s + 1;
    if (isdigit((int)*p)) {
        val = strtoul(p, &s, 10);
        if ((val < 0) || (val > 32)) {
            syntax_error("IP prefix range \"%s\" contains an invalid range",
                         yytext);
        }
        p = s + 1;
        if (isdigit((int)*p)) {
            endval = strtoul(p, NULL, 10);
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

{PRFXV4} {
    /* check each number of 1.2.3.4/5 in prefix is valid */
    unsigned long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '/') {
        val = strtoul(p, &s, 10);
        if ((val < 0) || (val > 255)) {
             syntax_error("IP prefix \"%s\" contains an invalid octet", 
                          yytext);
        }
        p = s + 1;
    }
    val = strtoul(p, NULL, 10);
    if ((val < 0) || (val > 32)) {
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
        val = strtoul(p, &s, 10);
        if ((val < 0) || (val > 255)) {
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
        val = strtoul(p, &s, 10);
        if ((val < 0) || (val > 4294967295)) {
             syntax_error("Community number \"%s\" contains an invalid number", 
                           yytext);
        }
        p = s + 1;
    }
    return TKN_COMM_NO;
}

{INT} { 
    importlval.sval = yytext;
    return TKN_INT; 
}

{DNAME} { 
    /* check the length */
    if (strlen(yytext) > 63) {
       syntax_error("Portion of domain name \"%s\" is longer "
                                                           "than 63 characters",
                    yytext); 
    }
    importlval.sval = yytext;
    return TKN_DNS; 
}

. { return yytext[0]; }

%%

void 
import_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}

int yywrap() { return 1; }
