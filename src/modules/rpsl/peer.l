%{
/*
  filename: peer.l

  description:
    Defines the tokenizer for an RPSL peer attribute.

  notes:
    Tokens are defined in the associated grammar, peer.y.

  $Id: peer.l,v 1.5 2003/07/03 07:55:37 engin Exp $
*/

/******************
  Copyright (c) 2002                              RIPE NCC

  All Rights Reserved

  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation, and that the name of the author not be
  used in advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
  AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
  DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  ***************************************/
%}

INT            [0-9]+
ASNO           AS{INT}
IPV4           {INT}(\.{INT}){3}
DNAME          [A-Z0-9]([-A-Z0-9]*[A-Z0-9])?
PRNGNAME       PRNG-[A-Z0-9_-]*[A-Z0-9]
RTRSNAME       RTRS-[A-Z0-9_-]*[A-Z0-9]

%{
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* tokens defined in the grammar */
#include "peer.tab.h"

#define peerwrap yywrap
void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

[ \t\n]+    { ; }

ASNO       { return KEYW_ASNO; }
FLAP_DAMP  { return KEYW_FLAP_DAMP; }

OSPF|RIP|IGRP|IS-IS|STATIC|RIPng|DVMRP|PIM-DM|PIM-SM|CBT|MOSPF {
    return TKN_SIMPLE_PROTOCOL;
}

BGP4 { 
    return TKN_BGP4;
}

(({ASNO}|peeras|{RTRSNAME}):)*{RTRSNAME}(:({ASNO}|peeras|{RTRSNAME}))* {
    return TKN_RTRSNAME;
}

(({ASNO}|peeras|{PRNGNAME}):)*{PRNGNAME}(:({ASNO}|peeras|{PRNGNAME}))* {
    return TKN_PRNGNAME;
}

{IPV4} {
    /* check each number of 1.2.3.4 in address is valid */
    long int val;
    char *s, *p;
    p = s = yytext;
    while (*s != '\0') {
        val = strtol(p, &s, 10);
        if ((val < 0) || (val > 255)) {
             syntax_error("IP address \"%s\" contains an invalid octet, %ld", 
                          yytext, val);
        }
        p = s + 1;
    }
    return TKN_IPV4;
}

{ASNO} {
    long int val;
    char *s;
    val = strtol(yytext+2, &s, 10);
    if ((val < 0) || (val > 65535) || (*s != '\0')) {
        syntax_error("AS number \"%s\" is not between 0 and 65535", yytext);
    }
    return TKN_ASNO;
}

{INT} {
    long int val;
    /* check port is in range */
    val = strtol(yytext, NULL, 10);
    if ((val < 0) || (val > 65535)) {
        syntax_error("Numeric value \"%s\" must be between 0 and 65535", 
                      yytext);
    }
    return TKN_SMALLINT;
}

{DNAME} {
    /* verify length is not too long */
    if (strlen(yytext) > 63) {
       syntax_error("Portion of domain name \"%s\" is longer "
                                                           "than 63 characters",                    yytext);
    }
    peerlval.sval = yytext;
    return TKN_DNS; 
}

. { return yytext[0]; }

%%

void
peer_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}


