%{
/*
  filename: mnt_routes6.l

  description:
    Defines the tokenizer for an RPSSng mnt-routes attribute
    of inet6num class.

  notes:
    Tokens are defined in the associated grammar, mnt_routes6.y.

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
PRFXV4RNG      {PRFXV4}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
PRFXV6         {IPV6}\/{INT}
PRFXV6RNG      {PRFXV6}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
PRFXV6DC       {IPV6DC}\/{INT}
PRFXV6DCRNG    {PRFXV6DC}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
MNTNER         [A-Z0-9]([A-Z0-9_-]*[A-Z0-9])?

%{
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <iproutines.h>

/* tokens defined in the grammar */
#include "mnt_routes6.tab.h"

#define mnt_routes6wrap yywrap
void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}


%%

[ \t\n]+    { ; }

ANY     { return KEYW_ANY; }

{MNTNER}           { return TKN_MNTNER; }


{PRFXV4} {
    ip_prefix_t ptr;
    int retval;

    retval = IP_pref_t2b(&ptr, yytext, IP_PLAIN);
    switch (retval) {
      /* we explicitly mention the errors from ip module that cannot be catched by the parser, 
        all the rest is parser error */
      case IP_OK: return TKN_PRFXV4;
                  break;
      case IP_INVBIT: syntax_error("IP prefix \"%s\" doesn't start on the correct boundary", yytext);
                     break;
      default: syntax_error("IP prefix \"%s\" contains invalid prefix", yytext);
                     break;
    }
    return TKN_PRFXV4;
}

{PRFXV6} {
    ip_prefix_t ptr;
    int retval;

    retval = IP_pref_t2b(&ptr, yytext, IP_PLAIN);
    switch (retval) {
      /* we explicitly mention the errors from ip module that cannot be catched by the parser, 
        all the rest is parser error */
      case IP_OK: return TKN_PRFXV6;
                  break;
      case IP_INVBIT: syntax_error("IPv6 prefix \"%s\" doesn't start on the correct boundary", yytext);
                     break;
      default: syntax_error("IPv6 prefix \"%s\" contains invalid prefix", yytext);
                     break;
    }
    return TKN_PRFXV6;
}
{PRFXV6DC} {
    ip_prefix_t ptr;
    int retval;

    retval = IP_pref_t2b(&ptr, yytext, IP_PLAIN);
    switch (retval) {
      /* we explicitly mention the errors from ip module that cannot be catched by the parser, 
        all the rest is parser error */
      case IP_OK: return TKN_PRFXV6DC;
                  break;
      case IP_INVBIT: syntax_error("IPv6 prefix \"%s\" doesn't start on the correct boundary", yytext);
                     break;
      default: syntax_error("IPv6 prefix \"%s\" contains invalid prefix", yytext);
                     break;
    }
    return TKN_PRFXV6DC;
}


{PRFXV4RNG} {
    /* check the prefix range by converting it and 
       issue syntax errors based on the error codes */

    ip_prefix_range_t ptr;
    int retval;

    retval = IP_pref_rang_t2b(&ptr, yytext, IP_PLAIN);
    switch (retval) {
      /* we explicitly mention the errors from ip module that cannot be catched by the parser, 
         all the rest is parser error */
      case IP_OK: return TKN_PRFXV4RNG;
                  break;
      case IP_INVBIT: syntax_error("IP prefix range \"%s\" doesn't start on the correct boundary", yytext);
                     break;
      case IP_NOLEN: syntax_error("IP prefix range \"%s\" range is not within the prefix length", yytext);
                     break;
      case IP_NORANG: syntax_error("IP prefix range \"%s\" range end is less than end start", yytext);
                     break;
      default: syntax_error("IP prefix range \"%s\" contains invalid prefix range", yytext);
                     break;
    }
}


{PRFXV6RNG} {
    /* check the prefix range by converting it and 
       issue syntax errors based on the error codes */

    ip_prefix_range_t ptr;
    int retval;

    retval = IP_pref_rang_t2b(&ptr, yytext, IP_PLAIN);
    switch (retval) {
      /* we explicitly mention the errors from ip module that cannot be catched by the parser, 
         all the rest is parser error */
      case IP_OK: return TKN_PRFXV6RNG;
                  break;
      case IP_INVBIT: syntax_error("IP prefix range \"%s\" doesn't start on the correct boundary", yytext);
                     break;
      case IP_NOLEN: syntax_error("IP prefix range \"%s\" range is not within the prefix length", yytext);
                     break;
      case IP_NORANG: syntax_error("IP prefix range \"%s\" range end is less than end start", yytext);
                     break;
      default: syntax_error("IP prefix range \"%s\" contains invalid prefix range", yytext);
                     break;
    }
}


{PRFXV6DCRNG} {
    /* check the prefix range by converting it and 
       issue syntax errors based on the error codes */

    ip_prefix_range_t ptr;
    int retval;

    retval = IP_pref_rang_t2b(&ptr, yytext, IP_PLAIN);
    switch (retval) {
      /* we explicitly mention the errors from ip module that cannot be catched by the parser, 
         all the rest is parser error */
      case IP_OK: return TKN_PRFXV6DCRNG;
                  break;
      case IP_INVBIT: syntax_error("IP prefix range \"%s\" doesn't start on the correct boundary", yytext);
                     break;
      case IP_NOLEN: syntax_error("IP prefix range \"%s\" range is not within the prefix length", yytext);
                     break;
      case IP_NORANG: syntax_error("IP prefix range \"%s\" range end is less than end start", yytext);
                     break;
      default: syntax_error("IP prefix range \"%s\" contains invalid prefix range", yytext);
                     break;
    }
}

. { return yytext[0]; }

%%

void
mnt_routes6_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}


