%{
/*
  filename: refer.l

  description:
    Defines the tokenizer for an RPSL refer attribute.

  notes:
    Tokens are defined in the associated grammar, refer.y.

  $Id: refer.l,v 1.1 2004/12/27 17:52:40 can Exp $
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

TYPE           SIMPLE|INTERNIC|RIPE|CLIENTADDRESS
INT            [0-9]+
IPV4           {INT}(\.{INT}){3}
DNAME          [A-Z0-9]([-A-Z0-9]*[A-Z0-9])?
HOSTNAME       {DNAME}(\.{DNAME})*
PORT           {INT}

%{
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* tokens defined in the grammar */
#include "refer.tab.h"

void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

[ \t\n]+    { ; }

{TYPE}           { return TKN_TYPE; }

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

{PORT} {
    long int val;
    /* check port is in range */
    val = strtol(yytext, NULL, 10);
    if ((val < 0) || (val > 65535)) {
        syntax_error("Port number %s is not between 0 and 65535", 
                      yytext);
    }
    return TKN_PORT;
}

{HOSTNAME} {
    char *s, *p;
    int len;
    /* verify length is not too long */
    /* XXX: technically 255, but our database limits to 80 */
    if (strlen(yytext) > 80) {
        syntax_error("Host name \"%s\" is too long", 
                     yytext);
    }
    /* check each part of domain is valid */
    s = p = yytext;
    for (;;) {
        p = strchr(s, '.');
        if (p == NULL) {
            break;
        }
        len = (p - s);
        if ((len <= 0) || (len > 63)) {
            syntax_error("Host name \"%s\" is has a domain too long: \"%.*s\"", 
                         yytext, len, s);
        }
        s = p + 1;
    }
    return TKN_HOSTNAME; 
}

. { return yytext[0]; }

%%

void
refer_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}


int yywrap() { return 1; }
