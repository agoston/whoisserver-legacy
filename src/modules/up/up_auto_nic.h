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

#ifndef UP_AUTO_NIC_H
#define UP_AUTO_NIC_H

#include <glib.h>

#include "rt.h"
#include "lg.h"
#include "syntax_api.h"
#include "dbupdate.h"


/* function prototypes */

int UP_replace_AUTO_key(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                              options_struct_t *options, rpsl_object_t *preproc_obj,
                              char *auto_key, char *obj_source);

int UP_replace_refs_to_AUTO_key(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                                         options_struct_t *options,
                                         rpsl_object_t *preproc_obj,
                                         int report_errors);

int UP_replace_refs_to_AUTO_key_from_string(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                                         options_struct_t *options,
                                         char **object_str,
                                         int report_errors);

int UP_put_assigned_key(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                           options_struct_t *options, char *auto_key, 
                           char *assigned_key, rpsl_object_t *postproc_obj);

int UP_has_AUTO_key(LG_context_t *lg_ctx, const rpsl_object_t *preproc_obj);

int UP_has_ref_to_AUTO_key(LG_context_t *lg_ctx, rpsl_object_t *object);

int UP_has_ref_to_AUTO_key_from_string(LG_context_t *lg_ctx, char *object_str);

int UP_get_assigned_key(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                              options_struct_t *options,
                              char *string, char *key );

/**************************************************************************************/    
/*                     TEMPORARY CODE TO USE EXISTING RIPUPD                          */
int get_assigned_key(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                              options_struct_t *options,
                              char *string, char *key );
/**************************************************************************************/    



#endif
