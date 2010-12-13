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

#ifndef UP_PRE_PROCESS_H
#define UP_PRE_PROCESS_H

#include <glib.h>

#include "rt.h"
#include "lg.h"
#include "km.h"
#include "syntax_api.h"
#include "up_util.h"

typedef struct {
    char *nic;
    char *mntner;
} nic_info_t;

typedef struct {
    GList *nic_list;
    GList *nic_mnt_list;
} nic_list_info_t;


/* function prototypes */
int UP_check_domain(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                    options_struct_t *options, rpsl_object_t *preproc_obj,
                    int operation, LU_server_t *server, char *obj_source);


int UP_check_country_attr(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj, char **countries);

int UP_check_nicsuffixes(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             options_struct_t *options,
                             rpsl_object_t *preproc_obj, char **countries);

int UP_check_mnt_by(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                     options_struct_t *options, rpsl_object_t *preproc_obj, 
                     int operation, LU_server_t *server, char *obj_source);

char *UP_get_current_date();

int UP_check_changed_attr(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj);

int UP_check_disallowmnt(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj);

int UP_generate_keycert_attrs(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                           key_info_t *key_info, rpsl_object_t *preproc_obj,
                           KM_context_t key_cert_type);

int UP_get_key_data(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                         key_info_t *key_info, KM_key_return_t **key_data,
                         rpsl_object_t *preproc_obj,
                         KM_context_t key_cert_type);

char *UP_get_certif_data(LG_context_t *lg_ctx, rpsl_object_t *preproc_obj);

int up_convert_inetnum_prefix(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                        rpsl_object_t *preproc_obj, int *inetnum_key_converted);

int up_is_inetnum_cidr(rpsl_object_t *object);

int UP_check_filter_set_object(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj);

int UP_check_peering_set_object(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj);

int UP_check_org_attr(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj);

int UP_check_organisation(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                          rpsl_object_t *preproc_obj, int operation);

int up_normalise_nserver(RT_context_t *rt_ctx, LG_context_t *lg_ctx, rpsl_object_t *preproc_obj);

int UP_check_filter_set_object(RT_context_t *rt_ctx, LG_context_t *lg_ctx, rpsl_object_t *preproc_obj);
int UP_check_peering_set_object(RT_context_t *rt_ctx, LG_context_t *lg_ctx, rpsl_object_t *preproc_obj);
int up_check_as_block(RT_context_t *rt_ctx, LG_context_t *lg_ctx, char *key_value);

int UP_check_available_nichdl(RT_context_t *rt_ctx, LG_context_t *lg_ctx, options_struct_t *options, rpsl_object_t *preproc_obj, source_data_t *source_data);
#endif
