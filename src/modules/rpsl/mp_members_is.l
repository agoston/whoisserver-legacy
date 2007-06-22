%{
/*
  filename: mp_members_is.l

  description:
    mp-members attribute of rtr-set class, as defined in RPSLng draft.

    NOTE: !!! 

We perhaps need to clarify the syntax of "mp-members:" a little
   in draft-blunk-rpslng-01.txt
   with parantheses. In the draft it is:

list of <inet-rtr-name> or <rtr-set-name> or <ipv4_addresses> or <ipv6_addresses>

   but is it to mean

list of (<inet-rtr-name> or <rtr-set-name> or <ipv4_addresses> or <ipv6_addresses>)
OR
(list of <inet-rtr-name>) or <rtr-set-name> or <ipv4_addresses> or <ipv6_addresses>

I presume the the first one is correct, in which case we can perhaps make
<ipv4_addresses> and <ipv6_addresses> singular, like <ipv4_address> and
<ipv6_address>.

  notes:
    Tokens are defined in the associated grammar, mp_members_is.y.

  $Id: mp_members_is.l,v 1.1.14.1 2006/09/29 12:32:29 katie Exp $
*/

/******************
  Copyright (c)                                         RIPE NCC

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

QUAD           [0-9A-F]{1,4}
INT            [0-9]+
IPV6           {QUAD}(:{QUAD}){7}
IPV6DC         (({QUAD}:){0,6}{QUAD})?::({QUAD}(:{QUAD}){0,6})?
IPV4           {INT}(\.{INT}){3}
RTRSNAME       RTRS-[A-Z0-9_-]*[A-Z0-9]
DNAME          [[:alnum:]]([[:alnum:]-]*[[:alnum:]])?
ASNO32         AS(([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655+[0-2][0-9]|6553[0-5])\.(0|[1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][+0-9]{2}|655[0-2][0-9]|6553[0-5]))
ASNO16         AS(0|[1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])
ASNO           {ASNO32}|{ASNO16}
ASNOSUB        AS[0]+\.(0|[1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])
ASNOMIX        [^ ]*{ASNOSUB}[^ ]*


%{
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* tokens defined in the grammar */
#include "mp_members_is.tab.h"

void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

[ \t\n]+    { ; }

{ASNOMIX} {
    syntax_error("The RIPE Database currently represents 16-bit AS numbers in \'ASn\' notation");
}

(({ASNO}|peeras|{RTRSNAME}):)*{RTRSNAME}(:({ASNO}|peeras|{RTRSNAME}))* {
    return TKN_RTRSNAME;
}

{IPV6}    { return TKN_IPV6; }

{IPV6DC}  {
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

    return TKN_IPV6;
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
mp_members_is_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}

int yywrap() { return 1; }
