%{
/*
  filename: mnt_routes.y

  description:
    Defines the grammar for an RPSL mnt-routes attribute.  It was mostly
    stolen from the IRRToolSet, simplified by removing ability to parse
    things defined by a dictionary (we use XML for extensibility rather
    than a dictionary).

  notes:
    Defines tokens for the associated lexer, mnt_routes.l.
*/
%}

%token KEYW_ANY
%token TKN_PRFXV4 TKN_PRFXV4RNG TKN_MNTNER

%{
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int yyerror(const char *s);

/* defines for unit testing */
#ifdef UNIT_TEST
int main();
#endif /* UNIT_TEST */
%}

%%

mnt_routes: TKN_MNTNER opt_item_list;

opt_item_list:
| '{' prefix_range_list '}'
| KEYW_ANY
;

prefix_range_list: prefix_range_item
| prefix_range_list ',' prefix_range_item
;

prefix_range_item: TKN_PRFXV4RNG | TKN_PRFXV4
;

%%

/* define a unit test that simply returns "yes" or "no" for filter input */
#ifndef UNIT_TEST

#undef mnt_routeserror
#undef yyerror

int
mnt_routeserror (const char *s)
{
    yyerror(s);
    return 0;
}
#else /* UNIT_TEST */

int parse_error_count;

/* parse error */
void 
yyerror(const char *s)
{
    fputs(s, stderr);
    putc('\n', stderr);
    parse_error_count++;
}

/* syntax error */
void 
syntax_error(const char *fmt, ...)
{
    va_list args;
    char buf[1024];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    yyerror(buf);
}

int
yywrap()
{
    return 1;
}

/* taken from _lex & yacc_ p.157, by Levine, Mason, & Brown (corrected) */
char *myinputptr;
char *myinputlim;

#undef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))

void
yy_input(char *buf, int *result, int max_size)
{
    int n = min(max_size, myinputlim - myinputptr);

    if (n > 0) {
        memcpy(buf, myinputptr, n);
        myinputptr += n;
    }
    *result = n;
}

/* program entry point */
int 
main()
{
    char buf[4096];

    setbuf(stdout, NULL);
    for (;;) {
        printf("filter> ");
        if (gets(buf) == NULL) {
            printf("\n");
            return(0);
        }
        parse_error_count = 0;
        myinputptr = buf;
        myinputlim = buf + strlen(buf);
        mnt_routesparse();
        printf("%d parse errors\n", parse_error_count);
    }
}
#endif /* UNIT_TEST */
