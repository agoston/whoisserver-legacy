%{
/*
  filename: mp_members_rs.l

  description:
    Defines the tokenizer for an RPSLng mp_members_rs attribute.  
    (mp_members attribute of route-set class)

  NOTE: !!! The syntax of this attribute is given as

  mp-members  list of <ipv4-address-prefix-range>
               or <ipv6-address-prefix-range>
               or <route-set-name>
               or <route-set-name><range-operator>

  in draft-blunk-rpslng-01.txt.
 
  However there is no parantheses so we are not sure if this is
  to mean

  mp-members  list of (<ipv4-address-prefix-range>   
               or <ipv6-address-prefix-range>
               or <route-set-name>
               or <route-set-name><range-operator>) 

  OR

  mp-members  (list of <ipv4-address-prefix-range>)   
               or <ipv6-address-prefix-range>
               or <route-set-name>
               or <route-set-name><range-operator>

  The first one makes sense so we implement the first one.

  notes:
    Tokens are defined in the associated grammar, mp_members_rs.y.

  $Id: mp_members_rs.l,v 1.1.14.1 2006/09/29 12:32:29 katie Exp $
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

ASNAME         AS-[A-Z0-9_-]*[A-Z0-9]
RSNAME         RS-[A-Z0-9_-]*[A-Z0-9]
INT            [0-9]+
QUAD           [0-9A-F]{1,4}
IPV4           {INT}(\.{INT}){3}
IPV6           ({QUAD}|{INT})(:({QUAD}|{INT})){7}
IPV6DC         ((({QUAD}|{INT}):){0,6}({QUAD}|{INT}))?::(({QUAD}|{INT})(:({QUAD}|{INT})){0,6})?
PRFXV4         {IPV4}\/{INT}
PRFXV6         {IPV6}\/{INT}
PRFXV6DC       {IPV6DC}\/{INT}
PRFXV4RNG      {PRFXV4}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
PRFXV6RNG      {PRFXV6}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
PRFXV6DCRNG    {PRFXV6DC}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
ASNODOT        [^ ]*AS[0-9]+\.[0-9]+[^ ]*
ASNO           AS([0-9]|[1-9][0-9]{1,8}|[1-3][0-9]{9}|4[0-1][0-9]{8}|42[0-8][0-9]{7}|429[0-3][0-9]{6}|4294[0-8][0-9]{5}|42949[0-5][0-9]{4}|429496[0-6][0-9]{3}|4294967[0-1][0-9]{2}|42949672[0-8][0-9]|429496729[0-5])

%{
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

/* tokens defined in the grammar */
#include "mp_members_rs.tab.h"

void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

[ \t\n]+    { ; }

\^-         { return OP_MS; }
\^\+        { return OP_MS; }
\^[0-9]+ { 
    unsigned long int val;
    errno = 0;
    val = strtoul(yytext+1, NULL, 10);
    if (errno || (val > 128)) {
        syntax_error("more specifics operator \"%s\" not 0 to 128 bits", 
                     yytext);
    }
    return OP_MS;
}
\^[0-9]+-[0-9]+ {
    unsigned long int val1, val2;
    char *p;
    errno = 0;
    val1 = strtoul(yytext+1, &p, 10);
    if (errno || (val1 > 128)) {
        syntax_error("more specifics operator \"%s\" not 0 to 128 bits", 
                     yytext);
    }
    errno = 0;
    val2 = strtoul(p+1, NULL, 10);
    if (errno || (val2 > 128)) {
        syntax_error("more specifics operator \"%s\" not 0 to 128 bits", 
                     yytext);
    }
    if (val2 < val1) {
        syntax_error("more specifics operator \"%s\" not 0 to 128 bits", 
                     yytext);
    }
    return OP_MS;
}

{ASNODOT} {
    syntax_error("The RIPE Database represents AS numbers in \'ASn\' notation");
}

{ASNO} { return TKN_ASNO; }


(({ASNO}|peeras|{RSNAME}):)*{RSNAME}(:({ASNO}|peeras|{RSNAME}))* {
    return TKN_RSNAME;
}

(({ASNO}|peeras|{ASNAME}):)*{ASNAME}(:({ASNO}|peeras|{ASNAME}))* {
    return TKN_ASNAME;
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
        syntax_error("IPv6 prefix \"%s\" contains too many digits", yytext);
    }

    return TKN_PRFXV6DC;
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


. { return yytext[0]; }

%%

void
mp_members_rs_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}


int yywrap() { return 1; }
