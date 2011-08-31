#include "km_internal.h"

/* The PKI systems known. */
PKI pki[3];

/* The logging context pointer will be reset with each call to KM_init.
   This is not good, but in practice dbupdate only has one logging
   context for debug output, so it will keep getting reset to the
   same value. 
   This must be changed when the KM context issue is reviewed */
LG_context_t* lg_ctx;
RT_context_t *rt_ctx;

/*
  Initializes a specific key system.

  context       - System type.
  configuration - Specific configuration parameters.

  return - The status of initialization.
 */
KM_status_t KM_init(KM_context_t context,
                    RT_context_t *_rt_ctx,
                    LG_context_t* ctx, GList* servers,
                    GList* sources, gchar *tmp_dir,
                    gchar* path) {

KM_status_t retval;

  LG_log(ctx, LG_FUNC,">KM_init: entered");
  lg_ctx = ctx;
  rt_ctx = _rt_ctx;
  
  switch (context) {
    case KM_PGP:
      LG_log(ctx, LG_DEBUG,"KM_init: initialising pgp");
      LG_log(ctx, LG_DEBUG,"KM_init: context [%d]", (int)context);
      pki[context] = km_pgp_register();
      break;
    case KM_X509:
      LG_log(ctx, LG_DEBUG,"KM_init: initialising X509");
      LG_log(ctx, LG_DEBUG,"KM_init: context [%d]", (int)context);
      pki[context] = km_X509_register(ctx);
      break;
    default:
      return KM_UNKNOWN_PKI;
  }
  retval =  pki[context].init(ctx, rt_ctx, servers, sources, tmp_dir, path);
  LG_log(ctx, LG_FUNC,"<KM_init: exiting with status %s", KM_return_string((int)retval));
  return retval;
}

/*
  Termininates a specific context.

  context - Context to terminate.
 */
void KM_end(KM_context_t context) {
  if (pki[context].end) {
    pki[context].end();
  }
}

/*
  Verifies the signature of a document.

  context   - System type.
  text      - Signed text.
  signature - Signature.

  return - A key_return structure.

  If signature is NULL then its considered that the signature is
  attached to the document.
 */
KM_key_return_t* KM_signature_verify(KM_context_t context, 
  gchar* text, gchar* signature) {
  //no init == seg fault
  return pki[context].signature_verify(text, signature);
}

/*
  Gets information about a key.

  context - System type.
  key     - Text representation of the key.

  return - A key_return structure.
 */
KM_status_t KM_key_get_info(KM_context_t context, 
                              gchar* key, KM_key_return_t** pkr) {

  KM_key_return_t* kr;
  KM_status_t status;

  LG_log(lg_ctx, LG_FUNC,">KM_key_get_info: entered with pki type [%s]", 
                                KM_context_string(context));

  //no init == seg fault
  kr = pki[context].key_get_info(key);
  *pkr = kr;

  status = KM_key_return_get_status(kr);
  LG_log(lg_ctx, LG_FUNC,"<KM_key_get_info: exiting with status [%d]", 
                                status);
  return status;
}


/*
  Creates a new key_return structure.

  key_id       - Key Id.
  signature_ok - TRUE is signature is correct, FALSE if not.
  signed_text  - Signed text (detached from the signature).
  status       - Status of the operation.

  return - Structure representing the given parameters.
 */
km_key_return_t* km_key_return_new(
  gchar* key_id, gboolean signature_ok, gchar* signed_text, KM_status_t status) {
  km_key_return_t* key_ret;

  key_ret = g_malloc(sizeof(km_key_return_t));

  if (key_id) {
    key_ret->key_id = g_malloc(strlen(key_id)+1);
    strcpy(key_ret->key_id, key_id);
  }
  else {
    key_ret->key_id = NULL;
  }
  key_ret->signature_ok = signature_ok;
  key_ret->status = status;
  if (signed_text) {
    key_ret->signed_text = g_malloc(strlen(signed_text)+1);
    strcpy(key_ret->signed_text, signed_text);
  }
  else {
    key_ret->signed_text = NULL;
  }
  key_ret->hash = g_hash_table_new(g_str_hash, g_str_equal);

  return key_ret;
}

static gboolean km_hash_remove(gpointer key,
  gpointer value, gpointer user_data) {
  g_free(key);
  g_free(value);
  return TRUE;
}

/*
  Frees a key_return structure.

  kr - The structure to free.
 */
void KM_key_return_free(KM_key_return_t* kr) {
  km_key_return_t* key_ret;

  key_ret = (km_key_return_t*)kr;

  g_free(key_ret->key_id);
  g_free(key_ret->signed_text);
  g_hash_table_foreach_remove(key_ret->hash, km_hash_remove, NULL);
  g_hash_table_destroy(key_ret->hash);
  g_free(key_ret);
}

/*
  Returns the signed text (without any signature) from a key_return.

  key_info - Key return structure with the signed text.

  return - Signed text.
 */
const gchar* KM_key_return_get_signed_text(KM_key_return_t* key_info) {
  return ((km_key_return_t*)key_info)->signed_text;
}

/*
  Returns the Key Id from a key_return.

  key_info - Key return structure with the key id.

  return - Key Id.
 */
const gchar* KM_key_return_get_key_id(KM_key_return_t* key_info) {
  return ((km_key_return_t*)key_info)->key_id;
}

/*
  Returns the status from a key_return structure.

  key_info - Key return structure with the status.

  return - Status.
 */
KM_status_t KM_key_return_get_status(KM_key_return_t* key_info) {
  return ((km_key_return_t*)key_info)->status;
}

/*
  Sets the status in a key_return structure.

  key_info - Key return structure.
  status - value to set in structure

  return - none.
 */
void km_key_return_set_status(KM_key_return_t *key_info, int status) {
  ((km_key_return_t*)key_info)->status = status;
}

/*
  Returns the signature validity from a key_return.

  key_info - Key return structure with the signature validity.

  return - Signature validity.
 */
gboolean KM_key_return_get_signature_ok(KM_key_return_t* key_info) {
  return ((km_key_return_t*)key_info)->signature_ok;
}

/*
  Adds a parameter to the key_return hash table

  key_info - Key return structure
  key      - (Hash)Key
  value    - Value

 */
void km_key_return_add(km_key_return_t* kr,
  gchar* key, gchar* value) {
  g_hash_table_insert(kr->hash, g_strdup(key), g_strdup(value));
  LG_log(lg_ctx, LG_FUNC, ">km_key_return_add: entered with key [%s] value [%s]",
                               key, value);
  LG_log(lg_ctx, LG_FUNC, "<km_key_return_add: exiting");
}

/*
  Returns a parameter from the key_return hash table

  key_info - Key return structure
  key      - (Hash)Key

  return - Value (or NULL)
 */
const gchar* KM_key_return_get_parameter(KM_key_return_t* kr,
  gchar* key) {
  km_key_return_t* kkr;
 
  kkr = kr;
  return g_hash_table_lookup(kkr->hash, key);
}

/* Compares two certificates in ascii format.
   Receives certificate string
            certificate string
   Returns  EP return status value
*/

int KM_certif_match(char *obj_key_str, char *input_key_str)
{
  LG_log(lg_ctx, LG_FUNC, ">KM_certif_match: entered with certificate strings\n[%s]\n[%s]",
                               obj_key_str, input_key_str);

  if ( ! strcmp(obj_key_str, input_key_str) )
  {
    LG_log(lg_ctx, LG_FUNC, "<KM_certif_match: exiting with value %s", KM_return_string(KM_OK));
    return KM_OK;
  }
  else
  {
    LG_log(lg_ctx, LG_FUNC, "<KM_certif_match: exiting with value %s", KM_return_string(KM_FAIL));
    return KM_FAIL;
  }
}
