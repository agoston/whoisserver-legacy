%{
/*
  filename: members_rs.l

  description:
    Defines the tokenizer for an RPSL members_rs attribute.  

  notes:
    Tokens are defined in the associated grammar, members_rs.y.

  $Id: members_rs.l,v 1.1.14.1 2006/09/29 12:32:29 katie Exp $
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

ASNAME         AS-[A-Z0-9_-]*[A-Z0-9]
RSNAME         RS-[A-Z0-9_-]*[A-Z0-9]
INT            [0-9]+
IPV4           {INT}(\.{INT}){3}
PRFXV4         {IPV4}\/{INT}
PRFXV4RNG      {PRFXV4}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
ASNODOT        [^ ]*AS[0-9]+\.[0-9]+[^ ]*
ASNO           AS([0-9]|[1-9][0-9]{1,8}|[1-3][0-9]{9}|4[0-1][0-9]{8}|42[0-8][0-9]{7}|429[0-3][0-9]{6}|4294[0-8][0-9]{5}|42949[0-5][0-9]{4}|429496[0-6][0-9]{3}|4294967[0-1][0-9]{2}|42949672[0-8][0-9]|429496729[0-5])

%{
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

/* tokens defined in the grammar */
#include "members_rs.tab.h"
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

{ASNO} { return TKN_ASNO; }

{ASNODOT} { 
    syntax_error("The RIPE Database represents AS numbers in \'ASn\' notation");
}

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

. { return yytext[0]; }

%%

void
members_rs_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}


int yywrap() { return 1; }
