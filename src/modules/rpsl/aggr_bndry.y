%{
/*
  filename: aggr_bndry.y

  description:
    Defines the grammar for an RPSL aggr-bndry attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Defines tokens for the associated lexer, aggr_bndry.l.
*/
%}

%token OP_OR OP_AND
%token KEYW_EXCEPT
%token TKN_ASNO TKN_ASNAME

%{
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int yyerror(const char *s);

%}

%%

aggr_bndry: as_expr
;

as_expr: as_expr OP_OR as_expr_term
| as_expr_term
;

as_expr_term: as_expr_term OP_AND as_expr_factor 
| as_expr_term KEYW_EXCEPT as_expr_factor 
| as_expr_factor
;

as_expr_factor: '(' as_expr ')' 
| as_expr_operand
;

as_expr_operand: TKN_ASNO
| TKN_ASNAME
;

%%

#undef aggr_bndryerror
#undef yyerror

int
aggr_bndryerror (const char *s)
{
    yyerror(s);
    return 0;
}


