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

/* could use per-lexer locks for speed, but it's probably unnecessary */

#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <glib.h>
#include <assert.h>
#include "syntax.h"
#include "syntax_tab.h"

#define SYNTAX_TAB_LEN (sizeof(syntax_tab)/sizeof(syntax_tab[0]))

static void
syntax_init()
{
    int i;
    int regcomp_ret;

    for (i=0; i<SYNTAX_TAB_LEN; i++) {
        if (syntax_tab[i].core_regex_pattern != NULL) {
            syntax_tab[i].core_regex = g_new(regex_t, 1);
            regcomp_ret = regcomp(syntax_tab[i].core_regex,
                                  syntax_tab[i].core_regex_pattern,
                                  REG_EXTENDED | REG_ICASE | REG_NOSUB);
            assert(regcomp_ret == 0);
        }
        if (syntax_tab[i].core_reserved_regex_pattern != NULL) {
            syntax_tab[i].core_reserved_regex = g_new(regex_t, 1);
            regcomp_ret = regcomp(syntax_tab[i].core_reserved_regex,
                                  syntax_tab[i].core_reserved_regex_pattern,
                                  REG_EXTENDED | REG_ICASE | REG_NOSUB);
            assert(regcomp_ret == 0);
        }
        if (syntax_tab[i].front_end_regex_pattern != NULL) {
            syntax_tab[i].front_end_regex = g_new(regex_t, 1);
            regcomp_ret = regcomp(syntax_tab[i].front_end_regex,
                                  syntax_tab[i].front_end_regex_pattern,
                                  REG_EXTENDED | REG_ICASE | REG_NOSUB);
            assert(regcomp_ret == 0);
        }
        if (syntax_tab[i].front_end_reserved_regex_pattern != NULL) {
            syntax_tab[i].front_end_reserved_regex = g_new(regex_t, 1);
            regcomp_ret = regcomp(syntax_tab[i].front_end_reserved_regex,
                                 syntax_tab[i].front_end_reserved_regex_pattern,
                                 REG_EXTENDED | REG_ICASE | REG_NOSUB);
            assert(regcomp_ret == 0);
        }
    }
}

int 
yywrap()
{
    return 1;
}

/* array of errors (initialize before parsing) */
GPtrArray *parse_errors;

/* parse error */
void
yyerror(const char *s)
{
    gchar *copy;

    copy = g_strdup(s);
    g_ptr_array_add(parse_errors, copy);
}

/* syntax error (formatted parse error) */
void
syntax_error(const char *fmt, ...)
{
    va_list args;
    guint len;
    char *buf;

    va_start(args, fmt);
    vasprintf(&buf, fmt, args);
    va_end(args);
    yyerror(buf);
    free(buf);
}

/* taken from _lex & yacc_ p.157, by Levine, Mason, & Brown (corrected) */
static const char *syntax_inputptr;
static const char *syntax_inputlim;

#undef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))

void
yy_input (char *buf, int *result, int max_size)
{
    int n = min(max_size, syntax_inputlim - syntax_inputptr);

    if (n > 0) {
        memcpy(buf, syntax_inputptr, n);
        syntax_inputptr += n;
    }
    *result = n;
}

static pthread_once_t init_control = PTHREAD_ONCE_INIT;

/* returns an array of strings with error descriptions (may be empty) */
GPtrArray *
syntax_check_by_offset (int offset, syntax_check_t type, const char *s)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    GPtrArray *errors;
    GString *tmp;

    pthread_once(&init_control, syntax_init);

    errors = g_ptr_array_new();
    tmp = g_string_new("");

    if (type == SYNTAX_CHECK_CORE) {
        if (syntax_tab[offset].core_regex_pattern != NULL) {
            if (regexec(syntax_tab[offset].core_regex, s, 0, NULL, 0) != 0) {
                g_string_sprintf(tmp, "Syntax error in \"%s\"", s);
                g_ptr_array_add(errors, g_strdup(tmp->str));
                goto syntax_check_by_offset_exit;
            }
        }
        if (syntax_tab[offset].core_reserved_regex_pattern != NULL) {
            if (regexec(syntax_tab[offset].core_reserved_regex, s, 
                        0, NULL, 0) == 0) 
            {
                g_string_sprintf(tmp, "\"%s\" contains a reserved word", s);
                g_ptr_array_add(errors, g_strdup(tmp->str));
                goto syntax_check_by_offset_exit;
            }
        }
        if (syntax_tab[offset].core_parser != NULL) {
            pthread_mutex_lock(&mutex);
            (syntax_tab[offset].core_parser_reset)();
            syntax_inputptr = s;
            syntax_inputlim = s + strlen(s);
            parse_errors = errors;
            (syntax_tab[offset].core_parser)();
            pthread_mutex_unlock(&mutex);
            if (errors->len > 0) {
                goto syntax_check_by_offset_exit;
            } 
        }
    } else {
        if (syntax_tab[offset].front_end_regex_pattern != NULL) {
            if (regexec(syntax_tab[offset].front_end_regex, s, 0, NULL, 0)!=0) {
                g_string_sprintf(tmp, "Syntax error in \"%s\"", s);
                g_ptr_array_add(errors, g_strdup(tmp->str));
                goto syntax_check_by_offset_exit;
            }
        }
        if (syntax_tab[offset].front_end_reserved_regex_pattern != NULL) {
            if (regexec(syntax_tab[offset].front_end_reserved_regex, s, 
                        0, NULL, 0) == 0) 
            {
                g_string_sprintf(tmp, "\"%s\" contains a reserved word", s);
                g_ptr_array_add(errors, g_strdup(tmp->str));
                goto syntax_check_by_offset_exit;
            }
        }
        if (syntax_tab[offset].front_end_parser != NULL) {
            pthread_mutex_lock(&mutex);
            (syntax_tab[offset].front_end_parser_reset)();
            syntax_inputptr = s;
            syntax_inputlim = s + strlen(s);
            parse_errors = errors;
            (syntax_tab[offset].front_end_parser)();
            pthread_mutex_unlock(&mutex);
            if (errors->len > 0) {
                goto syntax_check_by_offset_exit;
            } 
        }
    }

syntax_check_by_offset_exit:
    g_string_free(tmp, TRUE);
    return errors;
}

GPtrArray *
syntax_check_by_name (const char *name, syntax_check_t type, const char *s)
{
    int i;
    GPtrArray *errors;

    for (i=0; i<SYNTAX_TAB_LEN; i++) {
        if (strcasecmp(syntax_tab[i].name, name) == 0) {
            return syntax_check_by_offset(i, type, s);
        }
    }
    errors = g_ptr_array_new();
    g_ptr_array_add(errors, 
                    "Internal error: check of unknown syntax requested");
    return errors;
}

#ifdef UNIT_TEST
int main()
{
    int i, j;
    GPtrArray *errors;

    /* call all syntax checks */
    for (i=0; i<SYNTAX_TAB_LEN; i++) {
        errors = syntax_check_by_offset(i, SYNTAX_CHECK_CORE, "");
        for (j=0; j<errors->len; j++) {
            g_free(g_ptr_array_index(errors, j));
        }
        g_ptr_array_free(errors, TRUE);
        errors = syntax_check_by_offset(i, SYNTAX_CHECK_FRONT_END, "");
        for (j=0; j<errors->len; j++) {
            g_free(g_ptr_array_index(errors, j));
        }
        g_ptr_array_free(errors, TRUE);
    }
}
#endif /* UNIT_TEST */

/*
int parse_error_count = 0;

int main()
{
    char buf[256];
    syntax_init();
    printf("check filter: ");
    gets(buf);
    printf("result=%d\n", syntax_check_by_name("filter", SYNTAX_CHECK_FRONT_END, buf));
}
*/

