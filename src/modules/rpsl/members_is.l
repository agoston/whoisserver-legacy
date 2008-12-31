%{
/*
  filename: members_is.l

  description:
    Defines the tokenizer for an RPSL members_is attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Tokens are defined in the associated grammar, members_is.y.

  $Id: members_is.l,v 1.1.14.1 2006/09/29 12:32:29 katie Exp $
*/
%}

INT            [0-9]+
IPV4           {INT}(\.{INT}){3}
RTRSNAME       RTRS-[A-Z0-9_-]*[A-Z0-9]
DNAME          [[:alnum:]]([[:alnum:]-]*[[:alnum:]])?
ASNODOT        [^ ]*AS[0-9]+\.[0-9]+[^ ]*
ASNO           AS(0|[1-9][0-9]{1,8}|[1-3][0-9]{9}|4[0-1][0-9]{8}|42[0-8][0-9]{7}|429[0-3][0-9]{6}|4294[0-8][0-9]{5}|42949[0-5][0-9]{4}|429496[0-6][0-9]{3}|4294967[0-1][0-9]{2}|42949672[0-8][0-9]|429496729[0-5])
ASNAME         AS-[A-Z0-9_-]*[A-Z0-9]

%{
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* tokens defined in the grammar */
#include "members_is.tab.h"
void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

[ \t\n]+    { ; }

{ASNODOT} {
     syntax_error("The RIPE Database represents AS numbers in \'ASn\' notation");
}

{ASNO} {
    return TKN_ASNO;
}

(({ASNO}|peeras|{RTRSNAME}):)*{RTRSNAME}(:({ASNO}|peeras|{RTRSNAME}))* {
    return TKN_RTRSNAME;
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

{DNAME}(\.{DNAME})+ { 
    char *s, *p;
    int len;

    /* check the length */
    if (strlen(yytext) > 255) {
       syntax_error("Domain name \"%s\" is longer than 255 characters", yytext);
    }
    /* check the length of each part */
    p = yytext;
    do {
        s = strchr(p, '.');
        if (s == NULL) {
            len = strlen(p);
        } else {
            len = s - p;
            p = s + 1;
        }
        if (len > 63) {
            syntax_error("Part of domain name \"%s\" is longer than 63 characters",
                         yytext);
        }
    } while (s != NULL);
    return TKN_DOMAIN;
}

. { return yytext[0]; }

%%

void 
members_is_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}

int yywrap() { return 1; }
