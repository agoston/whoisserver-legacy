%{
/*
  filename: inet6num.l

  description:
    Defines the tokenizer for an RPSL inet6num attribute.

  notes:
    Tokens are defined in the associated grammar, inet6num.y.

  $Id: inet6num.l,v 1.1 2004/12/27 16:38:45 can Exp $
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

QUAD           [0-9A-F]{1,4}
BITS           ([1-9]?[0-9])|(1[0-1][0-9])|(12[0-8])
IPV6           {QUAD}(:{QUAD}){7}\/{BITS}
IPV6DC         (({QUAD}:){0,6}{QUAD})?::({QUAD}(:{QUAD}){0,6})?\/{BITS}

%{
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* tokens defined in the grammar */
#include "inet6num.tab.h"

#define inet6numwrap yywrap
void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

{IPV6}    { return TKN_IPV6; }

{IPV6DC}  { 
    int leftquads = 0;
    int rightquads = 0;
    /* count colons to the left of "::" - this is one less than the number of left quads */
    char *p = strchr(yytext, ':');
    leftquads++;
    while (p[1] != ':') {
        leftquads++;
        p = strchr(p+1, ':');
    }
    /* skip "::" */
    p += 2;
    /* count colons to the right of "::" - one less than the number of right quads */
    if (p[0] != '\0') {
        do {
            rightquads++;
            p = strchr(p+1, ':');
        } while (p != NULL);
    }
    /* check for too many quads */
    if ((leftquads + rightquads) > 8) {
        syntax_error("IPv6 address \"%s\" contains too many digits", yytext);
    }
    return TKN_IPV6;
}

. { return yytext[0]; }

%%

void
inet6num_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}


