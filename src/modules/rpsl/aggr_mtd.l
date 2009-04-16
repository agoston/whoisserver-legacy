%{
/*
  filename: aggr_mtd.l

  description:
    Defines the tokenizer for an RPSL aggr-mtd attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Tokens are defined in the associated grammar, aggr_mtd.y.

  $Id: aggr_mtd.l,v 1.1.14.1 2006/09/29 12:32:29 katie Exp $
*/
%}

ASNODOT        [^ ]*AS[0-9]+\.[0-9]+[^ ]*
ASNO           AS([0-9]|[1-9][0-9]{1,8}|[1-3][0-9]{9}|4[0-1][0-9]{8}|42[0-8][0-9]{7}|429[0-3][0-9]{6}|4294[0-8][0-9]{5}|42949[0-5][0-9]{4}|429496[0-6][0-9]{3}|4294967[0-1][0-9]{2}|42949672[0-8][0-9]|429496729[0-5])
ASNAME         AS-[A-Z0-9_-]*[A-Z0-9]

%{
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

/* tokens defined in the grammar */
#include "aggr_mtd.tab.h"

void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

[ \t\n]+    { ; }

INBOUND    { return KEYW_INBOUND; }
OUTBOUND   { return KEYW_OUTBOUND; }
EXCEPT     { return KEYW_EXCEPT; }

OR    { return OP_OR; }
AND   { return OP_AND; }

{ASNODOT} {
    syntax_error("The RIPE Database represents AS numbers in \'ASn\' notation");
}

{ASNO} {
    return TKN_ASNO;
}

(({ASNO}|peeras|{ASNAME}):)*{ASNAME}(:({ASNO}|peeras|{ASNAME}))* {
    return TKN_ASNAME;
}

. { return yytext[0]; }

%%

void
aggr_mtd_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}


int
yywrap()
{
    return 1;
}

