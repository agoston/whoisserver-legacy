%{
/*
  filename: peering.l

  description:
    Defines the tokenizer for an RPSL peering attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Tokens are defined in the associated grammar, peering.y.

  $Id: peering.l,v 1.1.14.1 2006/09/29 12:32:29 katie Exp $
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
#include "peering.tab.h"

void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

[ \t\n]+    { ; }

OR    { return OP_OR; }
AND   { return OP_AND; }

AT          { return KEYW_AT; }
EXCEPT      { return KEYW_EXCEPT; }

{ASNODOT} {
    syntax_error("The RIPE Database represents AS numbers in \'ASn\' notation");
}

{ASNO} {
    return TKN_ASNO;
}

(({ASNO}|peeras|{ASNAME}):)*{ASNAME}(:({ASNO}|peeras|{ASNAME}))* {
    return TKN_ASNAME;
}

(({ASNO}|peeras|{PRNGNAME}):)*{PRNGNAME}(:({ASNO}|peeras|{PRNGNAME}))* { 
    return TKN_PRNGNAME;
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

{DNAME} { 
    /* check the length */
    if (strlen(yytext) > 63) {
       syntax_error("Portion of domain name \"%s\" is longer "
                                                           "than 63 characters",
                    yytext); 
    }
    peeringlval.sval = yytext;
    return TKN_DNS; 
}

. { return yytext[0]; }

%%

void 
peering_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}

int yywrap() { return 1; }
