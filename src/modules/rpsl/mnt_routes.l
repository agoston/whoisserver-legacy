%{
/*
  filename: mnt_routes.l

  description:
    Defines the tokenizer for an RPSL mnt-routes attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Tokens are defined in the associated grammar, mnt_routes.y.

*/
%}

MNTNAME        [A-Z]([A-Z0-9_-]*[A-Z0-9])?
INT            [0-9]+
IPV4           {INT}(\.{INT}){3}
PRFXV4         {IPV4}\/{INT}
PRFXV4RNG      {PRFXV4}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
ANY            ANY

%{
#include <stdlib.h>
#include <ctype.h>
#include <iproutines.h>

/* tokens defined in the grammar */
#include "mnt_routes.tab.h"

#define mnt_routeswrap yywrap
void syntax_error(char *fmt, ...);
void yy_input(char *buf, int *result, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf,&result,max_size)
%}

%%

[ \t\n]+    { ; }

{ANY}     { return KEYW_ANY; }

{MNTNAME} { return TKN_MNTNER; }

{PRFXV4RNG} {
    /* check the prefix range by converting it and 
       issue syntax errors based on the error codes */

    ip_prefix_range_t ptr;
    int retval;

    retval = IP_pref_rang_t2b(&ptr, yytext, IP_PLAIN);
    switch (retval) {
      /* the only returns we may really get are:
         IP_OK, IP_INVBIT, IP_NOLEN, IP_NORANG 
         all the rest will be caught by the regexp and give general parse error
      */
      case IP_OK: return TKN_PRFXV4RNG;
                  break;
      case IP_NOCAP: syntax_error("IP prefix range \"%s\" contains no \"^\"", yytext);
                     break;  
      case IP_ADTOLO: syntax_error("IP prefix range \"%s\" address too long", yytext);
                     break; 
      case IP_NOSLAS: syntax_error("IP prefix range \"%s\" contains no slash", yytext);
                     break;  
      case IP_INVIP4: syntax_error("IP prefix range \"%s\" contains invalid IP address", yytext);
                     break;
      case IP_PRTOLO: syntax_error("IP prefix range \"%s\" prefix too long", yytext);
                     break;
      case IP_INVPRF: syntax_error("IP prefix range \"%s\" contains invalid characters", yytext);
                     break;
      case IP_INVBIT: syntax_error("IP prefix range \"%s\" doesn't start on the correct boundary", yytext);
                     break;
      case IP_INVPRN: syntax_error("IP prefix range \"%s\" has invalid format", yytext);
                     break;
      case IP_NOLEN: syntax_error("IP prefix range \"%s\" range is not within the prefix length", yytext);
                     break;
      case IP_NORANG: syntax_error("IP prefix range \"%s\" range end is less than end start", yytext);
                     break;
      case IP_WROLEN: syntax_error("IP prefix range \"%s\" contains invalid prefix length", yytext);
                     break;
      case IP_WRORANG: syntax_error("IP prefix range \"%s\" contains invalid range(s)", yytext);
                     break;
      default: syntax_error("IP prefix range \"%s\" contains invalid prefix range", yytext);
                     break;
    }
}

. { return yytext[0]; }

%%

void
mnt_routes_reset ()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
}


