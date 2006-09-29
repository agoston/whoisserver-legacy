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

  $Id: aggr_mtd.l,v 1.1 2004/12/27 17:52:37 can Exp $
*/
%}

ASNO32         AS(([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655+[0-2][0-9]|6553[0-5])\.(0|[1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][+0-9]{2}|655[0-2][0-9]|6553[0-5]))
ASNO16         AS(0|[1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])
ASNOSUB        AS[0]+\.(0|[1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])
ASNOMIX        [^ ]*{ASNOSUB}[^ ]*
ASNO           {ASNO32}|{ASNO16}
ASNAME         AS-[A-Z0-9_-]*[A-Z0-9]

%{
#include <stdlib.h>
#include <ctype.h>

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

[^ ]*{ASNOMIX}[^ ]* {
    syntax_error("The RIPE Database currently represents 16-bit AS numbers in \'ASn\' notation");
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

