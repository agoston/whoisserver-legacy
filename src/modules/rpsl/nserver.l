%{
/*
  filename: nserver.l

  description:
    Defines the tokenizer for an nserver attribute.

  notes:
    Tokens are defined in the associated grammar, nserver.y.

//<<<<<<< nserver.l
//  $Id: nserver.l,v 1.4 2006/10/17 13:38:15 katie Exp $
//=======
  $Id: nserver.l,v 1.3.2.1 2006/09/29 12:32:29 katie Exp $
//>>>>>>> 1.3.2.1
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

/* we accept IPv4: a.b.c.d
             IPv6: abcd:a0b4:d3a4:0000:0034:2435:2345:2039 (lowercase/uppercase, fully expanded)
             IPv6: abcd:: (lowercase/uppercase, compressed)
             all is converted into the fully expanded by dbupdate.
             hostname: RFC1123
                 label may not start with '-'
                 last label (TLD) must have at least one letter
*/
%}

INT            [0-9]+
IPV4           {INT}(\.{INT}){3}
QUAD           [0-9a-fA-F]{1,4}
IPV6           {QUAD}(:{QUAD}){7}
IPV6DC         (({QUAD}:){0,6}{QUAD})?::({QUAD}(:{QUAD}){0,6})?
LABEL          [A-Za-z0-9]+([-A-Za-z0-9]*[A-Za-z0-9]+)*
TLD            ({LABEL})*[A-Za-z]({LABEL})*
HOSTNAME       ({LABEL}\.)*{TLD}(\.)?

%{
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <iproutines.h>

/* tokens defined in the grammar */
#include "nserver.tab.h"

void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

[ \t\n]+    { ; }

{IPV4}    { 

    ip_addr_t ptr;
    int retval;


    retval = IP_addr_t2b(&ptr, yytext, IP_PLAIN);
    switch (retval) {
      /* we explicitly mention the errors from ip module that cannot be catched by the parser, 
        all the rest is parser error */
      case IP_OK: return TKN_IPV4;
                  break;
      default: syntax_error("IP address \"%s\" contains invalid IP address", yytext);
                  break;
    }
}

{IPV6} {
    ip_addr_t ptr;
    int retval;

    retval = IP_addr_t2b(&ptr, yytext, IP_PLAIN);
    switch (retval) {
      /* we explicitly mention the errors from ip module that cannot be catched by the parser, 
        all the rest is parser error */
      case IP_OK: return TKN_IPV6;
                  break;
      default: syntax_error("IPv6 address \"%s\" contains invalid IPv6 address", yytext);
                  break;
    }
}

{IPV6DC} {
    ip_addr_t ptr;
    int retval;

    retval = IP_addr_t2b(&ptr, yytext, IP_PLAIN);
    switch (retval) {
      /* we explicitly mention the errors from ip module that cannot be catched by the parser, 
        all the rest is parser error */
      case IP_OK: return TKN_IPV6DC;
                  break;
      default: syntax_error("IPv6 address \"%s\" contains invalid IPv6 address", yytext);
                  break;
    }
}
    
{HOSTNAME} {
    if (strlen(yytext) > 255) 
    {
       syntax_error("Hostname \"%s\" is too long", yytext);
    }
    return TKN_HOSTNAME; 
}

. { return yytext[0]; }

%%

void
nserver_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}
int yywrap ()
{
    return(1);
}


int yywrap() { return 1; }
