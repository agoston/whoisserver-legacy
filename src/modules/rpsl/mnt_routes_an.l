%{
/*
  filename: mnt_routes_an.l

  description:
    Defines the tokenizer for an RPSSng mnt-routes attribute
    of aut-num class.

  notes:
    Tokens are defined in the associated grammar, mnt_routes_an.y.

  $Id: mnt_routes_an.l,v 1.1 2004/12/27 17:52:39 can Exp $
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

INT            [0-9]+
QUAD           [0-9A-F]{1,4}
IPV4           {INT}(\.{INT}){3}
IPV6           {QUAD}(:{QUAD}){7}
IPV6DC         (({QUAD}:){0,6}{QUAD})?::({QUAD}(:{QUAD}){0,6})?
PRFXV4         {IPV4}\/{INT}
PRFXV6         {IPV6}\/{INT}
PRFXV6DC       {IPV6DC}\/{INT}
MNTNER         [A-Z0-9]([A-Z0-9_-]*[A-Z0-9])?

%{
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* tokens defined in the grammar */
#include "mnt_routes_an.tab.h"

#define mnt_routes_anwrap yywrap
void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}


%%

[ \t\n]+    { ; }

ANY     { return KEYW_ANY; }

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

{MNTNER}           { return TKN_MNTNER; }

. { return yytext[0]; }

%%

void
mnt_routes_an_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}


