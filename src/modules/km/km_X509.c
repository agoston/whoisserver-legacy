/***************************************
  $Revision: 1.1 $

  X.509 module.

  Status: REVIEWED, TESTED

 Author(s):       Denis Walker

  ******************/ /******************
  Modification History:
        denis (02/01/2004) Created.
  ******************/ /******************
  Copyright (c) 2004               RIPE NCC
 
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
  KM_X509_trans_prepare NOT processed!
  Move errors
 */


#undef fclose
#undef close

static gchar *temporary_directory = NULL;
static gchar *openssl_path = NULL;
static LG_context_t *lg_ctx;
static GList *my_sources = NULL;

void km_X509_end();

/*
  Initializes the X509 sub-system.

   Receives LG context
            list of servers
            list of sources
            tmp directory
            path for openssl command
   Returns  status.
 */
 
KM_status_t km_X509_init(LG_context_t *_ctx, GList *servers,
                        GList *sources, gchar *tmp_dir, gchar *path)
{
  int retval = KM_OK;

  lg_ctx = _ctx;
  LG_log(lg_ctx, LG_FUNC,">km_X509_init: entered\n");

  /* clear static data */
  km_X509_end();

  /* set static data values */
  openssl_path = g_strdup(path);
  temporary_directory = g_strdup(tmp_dir);
  while (sources)
  {
    my_sources = g_list_append(my_sources, g_strdup((gchar *)sources->data));
    sources = sources->next;
  }
  KM_lu_init(servers, my_sources);

  LG_log(lg_ctx, LG_FUNC,"<km_X509_init: exiting with KM return status %s", KM_return_string(retval));
  return retval;
}

/*
  Clean up the X509 sub-system by removing the static data.

   Receives none
   Returns  none.
 */
 
void km_X509_end()
{
  /* GList *tmp_list; */
  
  if (my_sources) {
    g_list_free(my_sources);
    my_sources=NULL;
  }
  /*
  if (my_sources)
  {
    tmp_list = my_sources;
    while (tmp_list)
    {
      if (tmp_list->data != NULL) {
        g_free(tmp_list->data);
      }
      tmp_list = tmp_list->next;
    }
    g_list_free(my_sources);
    my_sources = NULL;
  }
  */
  
  if (temporary_directory)
  {
    g_free(temporary_directory);
    temporary_directory = NULL;
  }
  
  if (openssl_path)
  {
    g_free(openssl_path);
    openssl_path = NULL;
  }
}

/*
  Verifies a signature.

  See KM_signature_verify for parameters.
 */
KM_key_return_t *km_X509_signature_verify(gchar *text, gchar *signature) {

  KM_key_return_t *ret = NULL;

  return ret;
}


/*
  Extracts the finger print and owner from the certificate.

   Receives A structure with key data extracted from the certificate
            certificate data from the certif attributes
   Returns  none, but sets status value (KM_OK or KM_FAIL) in return structure.
*/
 
void km_X509_key_get_fingerprint(gchar *out_dir, km_key_return_t *kr, gchar *key)
{
#define LINE_LENGTH 400
  FILE *general;
  gchar *key_str;
  char txt[LINE_LENGTH];
  GString *openssl_line;
  gchar in_file[LINE_LENGTH];
  gchar status_file[LINE_LENGTH];
  gchar finger_print[LINE_LENGTH];
  gchar subject[LINE_LENGTH];
  gchar err_str[LINE_LENGTH];
  int km_status = KM_OK;

  LG_log(lg_ctx, LG_FUNC,">km_X509_key_get_fingerprint: entered\n");

  finger_print[0] = 0;
  subject[0] = 0;

  sprintf(in_file,"%s/km_in_%d", out_dir, (int)(getpid()) );
  sprintf(status_file,"%s/km_status_%d", out_dir, (int)(getpid()) );
  LG_log(lg_ctx, LG_DEBUG,">km_X509_key_get_fingerprint: input file [%s]", in_file);
  LG_log(lg_ctx, LG_DEBUG,">km_X509_key_get_fingerprint: status file [%s]", status_file);

  /* write the certificate to the input file */
  general = fopen(in_file, "w");
  fprintf(general, "%s", key);
  fclose(general);

  /* get the fingerprint and subject from the certificate */
  openssl_line = g_string_new(openssl_path);
  g_string_append(openssl_line, " x509 -inform PEM -in ");
  g_string_append(openssl_line, in_file);
  g_string_append(openssl_line, " -fingerprint -subject -noout ");
  g_string_append(openssl_line, " > ");
  g_string_append(openssl_line, status_file);
  g_string_append(openssl_line, " 2> ");
  g_string_append(openssl_line, status_file);

  LG_log(lg_ctx, LG_DEBUG,">km_X509_key_get_fingerprint: openssl cmd [%s]", openssl_line->str);
  system(openssl_line->str);
  g_string_free(openssl_line, TRUE);

  /* Parsing openssl output for fingerpront and subject values.
     May also find error conditions */

  general = fopen(status_file, "r");
  err_str[0] = '\0';
  while (fgets (txt, LINE_LENGTH - 1, general) != NULL)
  {
    //printf("%s\n", txt);
    if ((key_str = strstr(txt, "Fingerprint=")) != NULL )
    {
      strcpy(finger_print, key_str + 12);
      finger_print[strlen(finger_print)-1] = 0;
      LG_log(lg_ctx, LG_DEBUG,">km_X509_key_get_fingerprint: finger_print [%s]", finger_print);
    }
    if ((key_str = strstr(txt, "subject=")) !=NULL)
    {
      strcpy(subject, key_str + 8);
      subject[strlen(subject)-1] = 0;
      LG_log(lg_ctx, LG_DEBUG,">km_X509_key_get_fingerprint: subject [%s]", subject);
    }
    if ((key_str = strstr(txt, "no start line")) !=NULL)
    {
      strcpy(err_str, "no start line");
      LG_log(lg_ctx, LG_DEBUG,">km_X509_key_get_fingerprint: [%s]", err_str);
    }
    if ((key_str = strstr(txt, "bad end line")) !=NULL)
    {
      strcpy(err_str, "bad end line");
      LG_log(lg_ctx, LG_DEBUG,">km_X509_key_get_fingerprint: [%s]", err_str);
    }
    if ((key_str = strstr(txt, "bad base64 decode")) !=NULL)
    {
      strcpy(err_str, "bad base64 decode");
      LG_log(lg_ctx, LG_DEBUG,">km_X509_key_get_fingerprint: [%s]", err_str);
    }
  }
  fclose(general);
  
  /* report if any errors found */
  if ( err_str[0] )
  {
    RT_cert_error(rt_ctx, err_str);
  }

  /* dump the tmp files */
  unlink(in_file);
  unlink(status_file);

  /* Add values to the structure hash.
     The error status handling is a bit messy as it is trying to
     combine new X.509 errors with existing pgp errors.
     pgp only allowed one error, X.509 can have multiple errors. */
  if (finger_print[0])
  {
    km_key_return_add(kr, KM_FP, finger_print);
  }
  else
  {
    if ( km_status == KM_OK )
    {
      km_status = KM_NO_FP;
    }
    else
    {
      km_status |= KM_NO_FP;
    }
    LG_log(lg_ctx, LG_DEBUG,">km_X509_key_get_fingerprint: no fingerprint, status [%d]", km_status);
  }
  if (subject[0])
  {
    km_key_return_add(kr, KM_KO, subject);
  } 
  else
  {
    if ( km_status == KM_OK )
    {
      km_status = KM_NO_SJ;
    }
    else
    {
      km_status |= KM_NO_SJ;
    }
    LG_log(lg_ctx, LG_DEBUG,">km_X509_key_get_fingerprint: no subject, status [%d]", km_status);
  }
  
  /* update the status */
  km_key_return_set_status(kr, km_status);
  LG_log(lg_ctx, LG_FUNC,"<km_X509_key_get_fingerprint: exiting with status [%d]\n", km_status);
}

/*
  Extracts key information from the certificate.

   Receives certificate data from the certif attributes
   Returns  A structure with key data extracted from the certificate.
*/
 
KM_key_return_t *km_X509_get_info(gchar *key)
{
#define LINE_LENGTH 400
  KM_key_return_t *key_return;
  gchar tmp[LINE_LENGTH];
  gchar out_dir[LINE_LENGTH];

  LG_log(lg_ctx, LG_FUNC,">km_X509_get_info: entered with key [\n%s\n]", key);

  if (temporary_directory) 
  {
    strcpy(tmp, temporary_directory);
  }
  else
  {
    strcpy(tmp, "/tmp");
  }

  sprintf(out_dir,"%s/km_dir_%d", tmp, (int)(getpid()) );
  mkdir(out_dir, 0700);
  LG_log(lg_ctx, LG_DEBUG,"km_X509_get_info: out_dir\n", out_dir);

  /* create a new structure */
  key_return = km_key_return_new(NULL, FALSE, NULL, KM_OK);
  /* get the data from the certificate into the structure hash */
  km_X509_key_get_fingerprint(out_dir, key_return, key);

  rmdir(out_dir);
  LG_log(lg_ctx, LG_FUNC,"<km_X509_get_info: exiting\n");
  return key_return;
}


/*
  Registers the X509 driver.

   Receives logging context
   Returns  A structure with PKI function pointers.
*/

PKI km_X509_register(LG_context_t *lg_ctx)
{
  PKI pki;

  LG_log(lg_ctx, LG_FUNC,">km_X509_register: entered\n");

  pki.init = &km_X509_init;
  pki.end = &km_X509_end;
  pki.signature_verify = &km_X509_signature_verify;
  pki.key_get_info = &km_X509_get_info;

  LG_log(lg_ctx, LG_FUNC,"<km_X509_register: exiting\n");
  return pki;
}
