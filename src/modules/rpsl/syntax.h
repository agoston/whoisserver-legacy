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

#ifndef SYNTAX_H
#define SYNTAX_H

#include <glib.h>
#include <sys/types.h>
#include <regex.h>

typedef struct {
    char *name;
    char *core_regex_pattern;
    regex_t *core_regex;
    char *core_reserved_regex_pattern;
    regex_t *core_reserved_regex;
    void (*core_parser_reset)();
    int (*core_parser)();
    char *front_end_regex_pattern;
    regex_t *front_end_regex;
    char *front_end_reserved_regex_pattern;
    regex_t *front_end_reserved_regex;
    void (*front_end_parser_reset)();
    int (*front_end_parser)();
} syntax_t;

/* types of checks */
typedef enum {
    SYNTAX_CHECK_CORE = 1,
    SYNTAX_CHECK_FRONT_END = 2
} syntax_check_t;

/* functions */
GPtrArray *syntax_check_by_offset(int offset, 
                                  syntax_check_t type,
                                  const char *s);
GPtrArray *syntax_check_by_name(const char *name, 
                                syntax_check_t type, 
                                const char *s);

#endif /* SYNTAX_H */
