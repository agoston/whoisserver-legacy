/***************************************
  $Revision: 1.1 $

  GPG module.

  Status: REVIEWED, TESTED

 Author(s):       Tiago Antao

  ******************/ /******************
  Modification History:
        tiago (10/04/2003) Created.
  ******************/ /******************
  Copyright (c) 2003               RIPE NCC
 
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
#ifndef _KM_H_
#define _KM_H_

#include <lg.h>
#include <lu.h>
#include "rt.h"
#include <glib.h>
#include <stdlib.h>

#define KM_FP "finger_print"
#define KM_KO "key_owner"

typedef enum {
  KM_PGP    =0,
  KM_X509   =1
} KM_context_t;

#define KM_context_string(r) \
  (((r) == KM_PGP)                ? "PGP" : \
   ((r) == KM_X509)               ? "X509" : \
                                    "invalid")

typedef enum {
  KM_OK                 =0,
  KM_FAIL               =1,
  KM_NO_FP              =2,
  KM_NO_SJ              =4,
  KM_UNKNOWN_PKI        =8,
  KM_PARAMETER_LACKING  =16,
  KM_NOT_REMOVED        =32,
  KM_MULTIPLE_KEYS      =64,
  KM_NO_KEY             =128,
  KM_KEY_EXISTS         =256,
  KM_SECRET_KEY         =512,
  KM_IDS_DONT_MATCH     =1024,
  KM_INTERNAL           =2048,
  KM_UNEXISTENT_KEY     =4096
} KM_status_t;

#define KM_return_string(r) \
  (((r) == KM_OK)                ? "KM_OK" : \
   ((r) == KM_FAIL)              ? "KM_FAIL" : \
   ((r) == KM_NO_FP)             ? "KM_NO_FP" : \
   ((r) == KM_NO_SJ)             ? "KM_NO_SJ" : \
   ((r) == KM_UNKNOWN_PKI)       ? "KM_UNKNOWN_PKI" : \
   ((r) == KM_PARAMETER_LACKING) ? "KM_PARAMETER_LACKING" : \
   ((r) == KM_NOT_REMOVED)       ? "KM_NOT_REMOVED" : \
   ((r) == KM_MULTIPLE_KEYS)     ? "KM_MULTIPLE_KEYS" : \
   ((r) == KM_NO_KEY)            ? "KM_NO_KEY" : \
   ((r) == KM_KEY_EXISTS)        ? "KM_KEY_EXISTS" : \
   ((r) == KM_SECRET_KEY)        ? "KM_SECRET_KEY" : \
   ((r) == KM_IDS_DONT_MATCH)    ? "KM_IDS_DONT_MATCH" : \
   ((r) == KM_INTERNAL)          ? "KM_INTERNAL" : \
   ((r) == KM_UNEXISTENT_KEY)    ? "KM_UNEXISTENT_KEY" : \
                                   "unknown")

#define KM_return_description(r) \
  (((r) == KM_OK)                ? "KM_OK" : \
   ((r) == KM_FAIL)              ? "KM_FAIL" : \
   ((r) == KM_NO_FP)             ? "KM_NO_FP" : \
   ((r) == KM_NO_SJ)             ? "KM_NO_SJ" : \
   ((r) == KM_UNKNOWN_PKI)       ? "KM_UNKNOWN_PKI" : \
   ((r) == KM_PARAMETER_LACKING) ? "KM_PARAMETER_LACKING" : \
   ((r) == KM_NOT_REMOVED)       ? "KM_NOT_REMOVED" : \
   ((r) == KM_MULTIPLE_KEYS)     ? "KM_MULTIPLE_KEYS" : \
   ((r) == KM_NO_KEY)            ? "KM_NO_KEY" : \
   ((r) == KM_KEY_EXISTS)        ? "Key exists" : \
   ((r) == KM_SECRET_KEY)        ? "KM_SECRET_KEY" : \
   ((r) == KM_IDS_DONT_MATCH)    ? "KM_IDS_DONT_MATCH" : \
   ((r) == KM_INTERNAL)          ? "KM_INTERNAL" : \
   ((r) == KM_UNEXISTENT_KEY)    ? "KM_UNEXISTENT_KEY" : \
                                   "unknown")



typedef void KM_key_return_t;
typedef void KM_context_data_t;

KM_status_t KM_init(KM_context_t context,
                    RT_context_t *_rt_ctx, LG_context_t* ctx,
                    GList* server, GList* sources,
                    gchar *tmp_dir, gchar* path);
void KM_end(KM_context_t context);
void KM_lu_init(GList* server, GList* sources);

gchar* KM_lu_get(gchar* key);
LU_ret_t KM_lu_inverse_lookup(const gchar* key, GList **key_cert_list);

KM_key_return_t* KM_signature_verify(KM_context_t context, gchar* text, gchar* signature);

KM_status_t KM_key_get_info(KM_context_t context, gchar* key, KM_key_return_t** pkr);
KM_status_t KM_key_get_status(KM_context_t context, gchar* key);
//KM_status_t KM_key_add(KM_context_t context, gchar* key);
//KM_status_t KM_key_remove(KM_context_t context, gchar* key_id);
//KM_status_t KM_key_modify(KM_context_t context, gchar* key_id, gchar* key);

//KM_status_t KM_commit(KM_context_t context);
//KM_status_t KM_rollback(KM_context_t context);

const gchar* KM_key_return_get_key_id(KM_key_return_t* key_info);
void KM_key_return_free(KM_key_return_t* key_info);
gboolean KM_key_return_get_signature_ok(KM_key_return_t* key_info);
const gchar* KM_key_return_get_signed_text(KM_key_return_t* key_info);
KM_status_t KM_key_return_get_status(KM_key_return_t* key_info);
const gchar* KM_key_return_get_parameter(KM_key_return_t* key_info, gchar* parameter); 

int KM_certif_match(char *obj_key_str, char *input_key_str);

#endif
