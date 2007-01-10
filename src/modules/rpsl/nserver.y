%{
/*
  filename: nserver.y

  description:
    Defines the grammar for an nserver attribute.

  notes:
    Defines tokens for the associated lexer, nserver.l.
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

#include <stdlib.h>
#include <stdio.h>

int yyerror(const char *s);
%}

%union {
  char *sval;
}


%token TKN_IPV6
%token TKN_IPV6DC
%token TKN_IPV4
%token <sval> TKN_HOSTNAME

%%

nserver: TKN_HOSTNAME {
}
| ipv4_glue {
}
| ipv6_glue {
}
;

ipv4_glue: TKN_HOSTNAME TKN_IPV4 {
  check_glue($1);
}
;

ipv6_glue: TKN_HOSTNAME TKN_IPV6 {
  check_glue($1);
}
| TKN_HOSTNAME TKN_IPV6DC {
  check_glue($1);
}
;

;

%%

#undef nservererror
#undef yyerror

int
nservererror (const char *s)
{
    yyerror(s);
    return 0;
}

int check_glue (char *hostname) {
  char *suffix = NULL;
  char *v4tree = NULL;
  char *v6tree = NULL;
  int glue = 0;

  v4tree = (char *) strdup(".in-addr.arpa");
  v6tree = (char *) strdup(".ip6.arpa");

  if (strlen(hostname) > strlen(v4tree)) {
    suffix = hostname + strlen (hostname) - strlen(v4tree);
    if (strcasecmp (suffix, v4tree) == 0) {
      glue = 1;
    }
  }
  if (strlen(hostname) > strlen(v6tree)) {
    suffix = hostname + strlen (hostname) - strlen(v6tree);
    if (strcasecmp (suffix, v6tree) == 0) {
      glue = 1;
    }
  }
  free(v4tree);
  free(v6tree);

  if (glue == 1) {
    syntax_error("Glue records in in-addr.arpa/ip6.arpa are not accepted");
  }
  return glue;
}
