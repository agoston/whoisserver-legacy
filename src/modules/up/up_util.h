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

#ifndef UP_UTIL_H
#define UP_UTIL_H

#include "rip.h"
#include "ep.h"
#include "rt.h"
#include "lg.h"
#include "lu_whois.h"
#include "dbupdate.h"

#include <glib.h>

typedef struct {
  char *key;
  char *key_id;
} key_info_t;


/* function prototypes */

void UP_internal_error(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                     options_struct_t *options, char *err_msg, int no_ack);

void UP_free_options(options_struct_t *options);

char *UP_get_temp_filename(LG_context_t *lg_ctx, char *type);

void UP_add_to_upd_log(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                            options_struct_t *options, FILE *infile);

int UP_strstr_in_attr_list(LG_context_t *lg_ctx, GList *list, 
                            const char *substr);

int UP_process_submission(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                           options_struct_t *options,
                           ep_authenticated_data_t *submission );
void UP_connect_all_servers(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                                 options_struct_t *options,
                                 GList **server_list, GList **source_list,
                                 GList **unique_server_list);
int up_process_object(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                      options_struct_t *options, char *object_str,
                      GList *credentials, int handle_auto_keys);

#endif
