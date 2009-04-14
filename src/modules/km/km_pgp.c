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
#include <rip.h>
#include "km_internal.h"
#include <sys/file.h>
#include <sys/stat.h>

/*
  KM_pgp_trans_prepare NOT processed!
  Move errors
 */


#undef fclose
#undef close

static gchar* temporary_directory = NULL;
static gchar* gpg_path = NULL;
static LG_context_t* ctx;
static GList* my_sources = NULL;

void km_pgp_end() {
  GList* tmp_list;

  if (my_sources) {
    tmp_list = my_sources;
    while(tmp_list) {
      g_free(tmp_list->data);
      tmp_list = tmp_list->next;
    }
    g_list_free(my_sources);
    my_sources = NULL;
  }
  if (temporary_directory) {
    g_free(temporary_directory);
    temporary_directory = NULL;
  }
  if (gpg_path) {
    g_free(gpg_path);
    gpg_path = NULL;
  }
}

/*
  Initializes the PGP sub-system.

  return - the status.
 */
KM_status_t km_pgp_init(LG_context_t* _ctx, GList* servers,
                        GList* sources, gchar* tmp_dir, gchar* path) {
  ctx = _ctx;

  km_pgp_end();
  gpg_path = g_strdup(path);
  temporary_directory = g_strdup(tmp_dir);
  while (sources) {
    my_sources = g_list_append(my_sources, g_strdup((gchar*)sources->data));
    sources = sources->next;
  }
  KM_lu_init(servers, my_sources);

  return KM_OK;
}


/*
  Returns a char with the content of a file (should be text).

  file - The file handle.

  return - The file content as a NULL terminated string.
 */
static gchar* km_file_to_char(FILE *file)
{
    GString *g = g_string_sized_new(16384);
    gchar buf[4096];
    int len;

    do
    {
        len = fread(buf, 4096, 1, file);
        g_string_append_len(g, buf, len);
    }
    while (len < 4096);

    return g_string_free(g, FALSE);
}


/*
 * The low-lever signature verification. Contains all the dirty stuff.
 * Called from this file only.
 */
KM_key_return_t* km_pgp_signature_verify_low(gchar* text, gchar* signature, gchar *key_ring) {
  km_key_return_t* key_ret;
  FILE* general;
  gboolean valid;
  gchar* key_str;
#define LINE_LENGTH 400
  gchar key_id[LINE_LENGTH];
  gchar txt[LINE_LENGTH];
  GString* gpg_line;
  gchar out_file[LINE_LENGTH];
  gchar status_file[LINE_LENGTH];
  gchar signature_file[LINE_LENGTH];
  gchar in_file[LINE_LENGTH];
  gchar tmp[LINE_LENGTH];
  gchar* char_file;

  LG_log(ctx, LG_FUNC, ">Entering km_pgp_signature_verify_low");
  valid = FALSE;
  if (temporary_directory) {
    strcpy(tmp, temporary_directory);
  }
  else {
    strcpy(tmp, "/tmp");
  }
  sprintf(out_file,"%s/km_out_%d", tmp, (int)(getpid()) );
  sprintf(status_file,"%s/km_status_%d", tmp, (int)(getpid()) );
  sprintf(signature_file,"%s/km_sig_%d", tmp, (int)(getpid()) );
  sprintf(in_file,"%s/km_in_%d", tmp, (int)(getpid()) );

  general = fopen(in_file,"w");
  fprintf(general, "%s", text);
  fclose(general);
  if (signature) {
    general = fopen(signature_file,"w");
    fprintf(general, "%s", signature);
    fclose(general);
  }
  //awful
  gpg_line = g_string_new(gpg_path);
  g_string_append(gpg_line, " --no-default-keyring --no-secmem-warning ");
  if (key_ring) {
    g_string_append(gpg_line, "--keyring ");
    g_string_append(gpg_line, key_ring);
  }
  g_string_append(gpg_line, " -o ");
  g_string_append(gpg_line, out_file);
  if (signature) {
    g_string_append(gpg_line, " --verify ");
    g_string_append(gpg_line, signature_file);
  }
  else {
    g_string_append(gpg_line, " -d");
  }
  g_string_append(gpg_line, " ");
  g_string_append(gpg_line, in_file);
  g_string_append(gpg_line, " >");
  g_string_append(gpg_line, status_file);
  g_string_append(gpg_line, " 2>&1");
  LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify_low: command_line: %s", gpg_line->str);
  system(gpg_line->str);
  g_string_free(gpg_line, TRUE);
  /* Parsing gpg output */

  general = fopen(status_file, "r");
  while (fgets (txt, LINE_LENGTH - 1, general) != NULL) {
    LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify_low: gpg returns: %s", txt);
    if (strstr(txt, "Good signature") != NULL)
    {
      valid = TRUE;
      LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify_low: Good signature found");
    }

    if ((key_str = strstr(txt, "key ID")) != NULL) {
      key_str += 7;
      strcpy((char*)key_id, key_str);
      key_id[8] = 0;
      LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify_low: key_id [%s]", key_id);
    }
  }
  fclose(general);

  if (!signature) {
    LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify_low: not signature");
    general = fopen(out_file,"r");
    if (!general) {
      LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify_low: no out file, set FALSE cred with KM_INTERNAL status");
      key_ret = km_key_return_new(NULL, FALSE, NULL, KM_INTERNAL);
    }
    else {
      char_file = km_file_to_char(general);
      if ( valid == TRUE ) {
        LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify_low: set TRUE cred with KM_OK status");
      }
      else {
        LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify_low: set FALSE cred with KM_OK status");
      }
      key_ret = km_key_return_new((gchar*)key_id, valid, char_file, KM_OK);
      g_free(char_file);
      fclose(general);
    }
  }
  else {
    LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify_low: signature");
    if ( valid == TRUE ) {
      LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify_low: set TRUE cred with KM_OK status");
    }
    else {
      LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify_low: set FALSE cred with KM_OK status");
    }
    key_ret = km_key_return_new((gchar*)key_id, valid, text, KM_OK);
  }

  unlink(in_file);
  unlink(out_file);
  unlink(status_file);
  if (signature) {
    unlink(signature_file);
  }
  LG_log(ctx, LG_FUNC, "<Exiting km_pgp_signature_verify_low");
  return key_ret;
}



/*
  Verifies a signature.

  See KM_signature_verify for parameters.
 */
KM_key_return_t* km_pgp_signature_verify(gchar* text, gchar* signature) {

#define LINE_LENGTH 400
  KM_key_return_t* ret;
  gchar* key;
  gchar key_file[LINE_LENGTH];
  gchar nic[LINE_LENGTH];
  gchar tmp[LINE_LENGTH];
  KM_status_t ret_status;

  LG_log(ctx, LG_FUNC, ">Entering km_pgp_signature_verify");
  if (temporary_directory) {
    strcpy(tmp, temporary_directory);
  }
  else {
    strcpy(tmp, "/tmp");
  }
  LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify: call km_pgp_signature_verify_low with no key ring");
  ret = km_pgp_signature_verify_low(text, signature, NULL);
  ret_status = KM_key_return_get_status(ret);
  LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify: status [%s]", KM_return_string(ret_status));
  if (ret_status != KM_OK) {
    LG_log(ctx, LG_FUNC, "<Exiting km_pgp_signature_verify");
    return ret;
  }
  //printf("AAAAA%s\n", KM_key_return_get_key_id(ret));
  sprintf(nic, "PGPKEY-%s",  KM_key_return_get_key_id(ret));
  key = KM_lu_get(nic);
  if (key == NULL) {
    LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify: key-cert object %s not found", nic);
    LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify: set FALSE cred with KM_UNEXISTENT_KEY status");
    LG_log(ctx, LG_FUNC, "<Exiting km_pgp_signature_verify");
    return km_key_return_new(NULL, FALSE, NULL, KM_UNEXISTENT_KEY);
  }
  sprintf(key_file,"%s/km_key_%d", tmp, (int)(getpid()) );

  LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify: add key to key ring");
  ret = km_pgp_key_add_internal(key, key_file);
  if (KM_key_return_get_status(ret) == KM_OK) {
    LG_log(ctx, LG_DEBUG, "km_pgp_signature_verify: call km_pgp_signature_verify_low with key ring");
    ret = km_pgp_signature_verify_low(text, signature, key_file);
  }

  unlink(key_file);
  strcat(key_file, "~");
  unlink(key_file);
  LG_log(ctx, LG_FUNC, "<Exiting km_pgp_signature_verify");
  return ret;
}


/*
  Adds a key to a variable key ring.

  key      - the string representation of a key.
  key_ring - the file name (with path) of the key ring.

  return - A key_return with the result from the operation.

  The variable key ring is used for testing, ie, doing an operation
  on a safe key ring before doing it on the real one.
 */
KM_key_return_t* km_pgp_key_add_internal(gchar* key, gchar *key_ring) {
#define LINE_LENGTH 400

  km_key_return_t* key_ret;
  FILE* general;
  gboolean valid;
  gchar key_id[LINE_LENGTH];
  gchar txt[LINE_LENGTH];
  GString* gpg_line;
  gchar out_file[LINE_LENGTH];
  gchar secret_key_ring[LINE_LENGTH];
  gchar status_file[LINE_LENGTH];
  gchar in_file[LINE_LENGTH];
  gchar tmp[LINE_LENGTH];
  int imported;
  int unchanged;
  int secret;

  imported = 0;
  unchanged = 0;
  secret = 0;
  valid = FALSE;
  if (temporary_directory) {
    strcpy(tmp, temporary_directory);
  }
  else {
    strcpy(tmp, "/tmp");
  }
  sprintf(secret_key_ring,"%s/km_sec_%d.gpg", tmp, (int)(getpid()) );
  sprintf(out_file,"%s/km_out_%d", tmp, (int)(getpid()) );
  sprintf(status_file,"%s/km_status_%d", tmp, (int)(getpid()) );
  sprintf(in_file,"%s/km_in_%d", tmp, (int)(getpid()) );

  general = fopen(in_file,"w");
  fprintf(general, "%s", key);
  fclose(general);
  //awful
  gpg_line = g_string_new(gpg_path);
  g_string_append(gpg_line, " --no-default-keyring --no-secmem-warning ");
  g_string_append(gpg_line, " --secret-keyring ");
  g_string_append(gpg_line, secret_key_ring);
  g_string_append(gpg_line, " --keyring ");
  g_string_append(gpg_line, key_ring);
  g_string_append(gpg_line, " --import ");
  g_string_append(gpg_line, in_file);
  g_string_append(gpg_line, " > ");
  g_string_append(gpg_line, status_file);
  g_string_append(gpg_line, " 2> ");
  g_string_append(gpg_line, status_file);
  LG_log(ctx, LG_DEBUG, "%s\n", gpg_line->str);
  system(gpg_line->str);
  g_string_free(gpg_line, TRUE);
  /* Parsing gpg output */

  general = fopen(status_file, "r");
  while (fgets (txt, LINE_LENGTH - 1, general) != NULL) {
    LG_log(ctx, LG_DEBUG, "%s", txt);
    if (strstr(txt, "public key") != NULL) {
      if ((strstr(txt, "unchanged") != NULL) ||
          (strstr(txt, "not changed") != NULL)) {
        snprintf(key_id, LINE_LENGTH - 1, "%8s\n", txt + 9);
        key_id[8] = 0;
        unchanged++;
      }
      if(strstr(txt, "imported") != NULL) {
        snprintf(key_id, LINE_LENGTH -1, "%8s\n", txt + 9);
        key_id[8] = 0;
        imported++;
      }
    }
    if (strstr(txt, "secret key") != NULL ||
        strstr(txt, "failed to create temporary file") != NULL ) {
        //this falied stuff has to do with using /dev/null its OK
      secret++;
    }
  }
  fclose(general);

  unlink(in_file);
  unlink(out_file);
  unlink(status_file);
  unlink(secret_key_ring);

  //If there is more than 1 key...
  if (secret) {
    key_ret = km_key_return_new(key_id, FALSE, NULL, KM_SECRET_KEY);
  }
  else if (unchanged) {
    key_ret = km_key_return_new(key_id, FALSE, NULL, KM_KEY_EXISTS);
  }
  else if ((unchanged+imported+secret) > 1) {
    key_ret = km_key_return_new(NULL, FALSE, NULL, KM_MULTIPLE_KEYS);
  }
  else if (imported) {
    key_ret = km_key_return_new(key_id, FALSE, NULL, KM_OK);
  }
  else {
    key_ret = km_key_return_new(NULL, FALSE, NULL, KM_NO_KEY);
  }

  return key_ret;
}


void km_pgp_key_get_fingerprint(km_key_return_t* kr, gchar *key_ring) {
#define LINE_LENGTH 400

  FILE* general;
  gboolean valid;
  gchar* key_str;
  gchar txt[LINE_LENGTH];
  GString* gpg_line;
  gchar out_file[LINE_LENGTH];
  gchar status_file[LINE_LENGTH];
  gchar tmp[LINE_LENGTH];
  gchar finger_print[LINE_LENGTH];
  gchar key_owner[LINE_LENGTH];
  int imported;
  int unchanged;
  int secret;

  imported = 0;
  unchanged = 0;
  secret = 0;
  finger_print[0] = 0;
  key_owner[0] = 0;
  valid = FALSE;
  if (temporary_directory) {
    strcpy(tmp, temporary_directory);
  }
  else {
    strcpy(tmp, "/tmp");
  }
  sprintf(out_file,"%s/km_out_%d", tmp, (int)(getpid()) );
  sprintf(status_file,"%s/km_status_%d", tmp, (int)(getpid()) );

  //awful
  gpg_line = g_string_new(gpg_path);
  g_string_append(gpg_line, " --no-default-keyring --no-secmem-warning ");
  g_string_append(gpg_line, "--keyring ");
  g_string_append(gpg_line, key_ring);
  g_string_append(gpg_line, " --fingerprint ");
  g_string_append(gpg_line, KM_key_return_get_key_id(kr) );
  g_string_append(gpg_line, " > ");
  g_string_append(gpg_line, status_file);
  g_string_append(gpg_line, " 2> ");
  g_string_append(gpg_line, status_file);

  //printf("%s\n", gpg_line->str);
  system(gpg_line->str);
  g_string_free(gpg_line, TRUE);
  /* Parsing gpg output */

  general = fopen(status_file, "r");
  while (fgets (txt, LINE_LENGTH - 1, general) != NULL) {
    //printf("%s\n", txt);
    if ((key_str = strstr(txt, "pub "/*"Key fingerprint ="*/)) == txt /*!= NULL*/) {
      strcpy(key_owner, key_str + 30);
      key_owner[strlen(key_owner)-1] = 0;
    }
    if ((key_str = strstr(txt, "Key fingerprint =")) !=NULL) {
      strcpy(finger_print, key_str + 18);
      finger_print[strlen(finger_print)-1] = 0;
    }
  }
  fclose(general);

  unlink(out_file);
  unlink(status_file);

  if (finger_print[0]) {
    km_key_return_add(kr, "finger_print", finger_print);
  }
  if (key_owner[0]) {
    km_key_return_add(kr, "key_owner", key_owner);
  }
}

KM_key_return_t* km_pgp_add_check(gchar* key, gboolean get_information) {
#define LINE_LENGTH 400
  gchar tmp[LINE_LENGTH];
  gchar out_dir[LINE_LENGTH];
  gchar out_ring[LINE_LENGTH];
  KM_key_return_t* key_return;

  if (temporary_directory) {
    strcpy(tmp, temporary_directory);
  }
  else {
    strcpy(tmp, "/tmp");
  }

  sprintf(out_dir,"%s/km_dir_%d", tmp, (int)(getpid()) );
  mkdir(out_dir, 0700);
  //sprintf(out_ring,"%s/km_dir_%d/pubring.gpg", tmp, (int)(getpid()) );
  sprintf(out_ring, "%s/pubring.gpg", out_dir);
  key_return = km_pgp_key_add_internal(key, out_ring);
  if (get_information && KM_key_return_get_key_id(key_return)) {
    km_pgp_key_get_fingerprint(key_return, out_ring);
  }
  unlink(out_ring);
  sprintf(out_ring, "%s/pubring.gpg~", out_dir);
  unlink(out_ring);
  rmdir(out_dir);

  return key_return;
}

/*
  Reports on a key.

  See KM_key_add for parameters.
 */
KM_key_return_t* km_pgp_get_info(gchar* key) {
#define LINE_LENGTH 400
  KM_key_return_t* key_return;

  key_return = km_pgp_add_check(key, TRUE);

  return key_return;
}


/*
  Registers the PGP driver.

  return - A structure with PKI function pointers.
*/
PKI km_pgp_register() {
  PKI pki;

  pki.init = &km_pgp_init;
  pki.end = &km_pgp_end;
  pki.signature_verify = &km_pgp_signature_verify;
  //pki.key_remove = &km_pgp_key_remove;
  pki.key_get_info = &km_pgp_get_info;
  //pki.key_add = &km_pgp_key_add;
  //pki.key_update = &km_pgp_key_update;
  //pki.commit = &km_pgp_commit;
  //pki.rollback = &km_pgp_rollback;

  return pki;
}
