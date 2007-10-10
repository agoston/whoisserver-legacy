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
#ifndef _KM_INTERNAL_H_
#define _KM_INTERNAL_H_

#include "km.h"
#include "lu.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  gchar* key_id;
  gboolean signature_ok;
  gchar* signed_text;
  KM_status_t status;
  GHashTable *hash;
} km_key_return_t;

km_key_return_t* km_key_return_new(
  gchar* key_id, gboolean signature_ok, gchar* signed_text, KM_status_t status);

void km_key_return_add(km_key_return_t* kr,
  gchar* key, gchar* value);

typedef struct {
  KM_status_t (*init)(LG_context_t*, GList*, GList*, gchar*, gchar*);
  void (*end)(void);
  KM_key_return_t* (*signature_verify)(gchar*, gchar*);
  //KM_status_t (*key_remove)(gchar*);
  //KM_status_t (*key_add)(gchar*);
  KM_key_return_t* (*key_get_info)(gchar*);
  // KM_status_t (*key_update)(gchar*, gchar*);
  //  KM_status_t (*commit)(void);
  //KM_status_t (*rollback)(void);
} PKI;

PKI km_pgp_register();
PKI km_X509_register(LG_context_t *lg_ctx);

KM_key_return_t* km_pgp_key_add_internal(gchar* key, gchar *key_ring);
void km_key_return_set_status(KM_key_return_t *key_info, int status);
#endif
