#include <stdlib.h>
//#include "rip.h"
#include "rt.h"
#include "km.h"
#include "cr.h"
#include "ep.h"
#include "up_pre_process.h"

LG_context_t *ep_ctx;
RT_context_t *rt_ctx;
gchar *ep_temporary_directory = NULL;
gchar *ep_DN = NULL;
int ep_retval = EP_OK;

/*
  EP_init: initializes the system.
 */
void EP_init(LG_context_t* ctx, RT_context_t *rtctx, gchar* tmp_dir) {

  ep_ctx = ctx;
  rt_ctx = rtctx;
  ep_temporary_directory = g_strdup(tmp_dir);
//  ep_DN = g_strdup(DN);

  MM_init(ctx);
}

/*
  EP_end: terminates the system.
 */
void EP_end() {
  if ( ep_temporary_directory )
  {
    g_free(ep_temporary_directory);
  }
  if ( ep_DN )
  {
    g_free(ep_DN);
  }
}


/*
  Unfolds input.

  stream  - The input stream
  is_mail - is it mail?
  X509cert_file  file name or NULL
  X509certificate  certificate as string or NULL
  input_structure - pointer to the input structure to be filled in

  return  - success/error code

  This functions is the most important entry point of EP, its where
  the input is parsed.
 */
int EP_unfold(FILE* stream, gboolean is_mail, char *X509cert_file, 
                    char *X509certificate, ep_input_structure_t **input)
{
  int retval = EP_OK;
  GString *string_stream; /* no need to free - done in ep_authenticated_data_free */
  ep_authenticated_data_t *a_data;
  const gchar *ep_FP = NULL;
  KM_key_return_t *key_data;
  GList *key_id_item = NULL;
  GList *key_id_good = NULL;
  GList *key_id_bad = NULL;

  LG_log(ep_ctx, LG_FUNC, ">EP_unfold entered with parameters is mail %s X.509 cert file %s X.509 certificate %s",
                               is_mail ? "yes" : "no", X509cert_file ? "yes" : "no",
                               X509certificate ? "yes" : "no");
  retval = EP_stream_to_string(stream, NULL, &string_stream);
  if (retval == EP_FATAL)
  {
    LG_log(ep_ctx, LG_FATAL, "EP_unfold: stream error");
    LG_log(ep_ctx, LG_FUNC, "<EP_unfold exiting with value %s", EP_ret2str(retval));
    return retval;
  }

  a_data = ep_authenticated_data_new();
  a_data->exp_data->data_str = string_stream;
  a_data->exp_data->type = (is_mail) ? EP_MAIL : EP_TEXT;

  if ( X509cert_file || X509certificate )
  {
    /* This update came from web/sync updates or from LIR Portal via ssh*/
    /* Check the signature and get information from the certificate.
       Set the credential according to the check result */
    if ( X509cert_file )
    {
      LG_log(ep_ctx, LG_FATAL, "EP_unfold: web/sync update X.509 certificate file found");
    }
    else
    {
      LG_log(ep_ctx, LG_FATAL, "EP_unfold: LIR Portal update X.509 certificate found");
    }
    retval = ep_check_X509_sig(X509cert_file, X509certificate, NULL, &key_data, a_data->exp_data->type, NULL);

    if (retval == EP_FATAL)
    {
      LG_log(ep_ctx, LG_DEBUG, "EP_unfold: X.509 certificate error");
      LG_log(ep_ctx, LG_FUNC, "<EP_unfold exiting with value %s", EP_ret2str(retval));
      return retval;
    }
    else
    {
//      ep_DN = KM_key_return_get_parameter(key_data, KM_KO);
      ep_FP = KM_key_return_get_parameter(key_data, KM_FP);
      
      if ( retval == EP_OK )
      {
        /* now find the key-cert object to match this certificate */
        retval = ep_get_key_id(ep_FP, X509cert_file, X509certificate, &key_id_good, &key_id_bad);
        if (retval == EP_FATAL)
        {
          LG_log(ep_ctx, LG_FUNC, "<EP_unfold exiting with value %s", EP_ret2str(retval));
          return retval;
        }
      }
      
      /* create new credential(s) */
      a_data->credentials = NULL;
      for ( key_id_item = key_id_good; key_id_item != NULL ; key_id_item = g_list_next(key_id_item) )
      {
        LG_log(ep_ctx, LG_DEBUG, "EP_unfold: create validated credential");
        a_data->credentials = g_list_append(a_data->credentials, CR_credential_new(CR_X509, key_id_item->data, TRUE));
        free(key_id_item->data);
      }
      for ( key_id_item = key_id_bad; key_id_item != NULL ; key_id_item = g_list_next(key_id_item) )
      {
        LG_log(ep_ctx, LG_DEBUG, "EP_unfold: create invalidated credential");
        a_data->credentials = g_list_append(a_data->credentials, CR_credential_new(CR_X509, key_id_item->data ? key_id_item->data : "unknown ID", FALSE));
        if ( key_id_item->data )
        {
          free(key_id_item->data);
        }
      }
      if ( key_id_good )  g_list_free(key_id_good);
      if ( key_id_bad )  g_list_free(key_id_bad);
    }
  }
  else
  {
    /* this is not a web/sync update so credentials will be
       set up while exploding the data */
    a_data->credentials = NULL;
  }
  
  retval = ep_explode(a_data, input);
  ep_authenticated_data_free(a_data);

  LG_log(ep_ctx, LG_FUNC, "<EP_unfold exiting with value %s", EP_ret2str(retval));
  return retval;
}


/* Gets the key-id(s) for X.509 key-cert object(s) where the certificate
   in the certif: attributes matches the one presented with the input.
   Receives finger print
            X509cert_file  or NULL
            X509certificate  or NULL
            location of key_id_good list
            location of key_id_bad list
   Returns  EP return status value
*/

int ep_get_key_id(const gchar *ep_FP, char *X509cert_file, 
                     char *X509certificate, GList **key_id_good, GList **key_id_bad)
{
  int retval = EP_OK;
  LU_ret_t lu_retval;
  int matched = 0;
  char *obj_key_str;
  char *input_key_str;
//  char *tmp_cert;
  char *value;
  GList *key_cert_list = NULL;
  GList *kc_item;
  GList *attr;
  GString *return_string = NULL;

  LG_log(ep_ctx, LG_FUNC, ">ep_get_key_id entered");

  /* do an inverse lookup on the finger print of the certificate */
  lu_retval = KM_lu_inverse_lookup(ep_FP, &key_cert_list);
  if ( lu_retval == LU_ERROR )
  {
    retval = EP_FATAL;
    LG_log(ep_ctx, LG_FATAL, "ep_get_key_id: inverse lookup error");
    LG_log(ep_ctx, LG_FUNC, "<ep_get_key_id exiting with value %s", EP_ret2str(retval));
    return retval;
  }

  if ( X509cert_file )
  {
    /* certificate from sync updates in a file */
    retval =  EP_stream_to_string(NULL, X509cert_file, &return_string);
    if (retval == EP_FATAL)
    {
      LG_log(ep_ctx, LG_FATAL, "ep_get_key_id: stream error");
      LG_log(ep_ctx, LG_FUNC, "<ep_get_key_id exiting with value %s", EP_ret2str(retval));
      if ( return_string->str )
      {
        g_string_free(return_string, TRUE);
      }
      return retval;
    }
  }
  else
  {
    /* certificate data from LIR Portal in string */
    return_string = g_string_new(X509certificate);
//    if ( X509certificate[0] == '"' && X509certificate[strlen(X509certificate)-1] == '"' )
//    {
      /* the certificate string is enclosed in double quotes, remove them */
//      tmp_cert = strdup(X509certificate + 1);
//      tmp_cert[strlen(tmp_cert)-1] = '\0';
//    }
//    else tmp_cert = strdup(X509certificate);
//    g_string_append(return_string, "-----BEGIN CERTIFICATE-----\n");
//    g_string_append(return_string, tmp_cert);
//    g_string_append(return_string, "\n-----END CERTIFICATE-----\n");
//    free(tmp_cert);
  }
  
  /* UP_get_certif_data adds a newline to the end of each line.
     Certificates from LIR Portal may not have a newline at the end,
     so make sure they are comparable for the match below */
  if ( return_string->str[strlen(return_string->str)-1] != '\n' )
  {
    g_string_append(return_string, "\n");
  }
  input_key_str = return_string->str;

  for ( kc_item = key_cert_list; kc_item != NULL ; kc_item = g_list_next(kc_item) )
  {
    /* does this object match the certificate */
    obj_key_str = UP_get_certif_data(ep_ctx, (rpsl_object_t *)(kc_item->data));
    attr = rpsl_object_get_attr((rpsl_object_t *)kc_item->data, "key-cert");
    value = rpsl_attr_get_clean_value( (rpsl_attr_t *)(attr->data) );

    if ( KM_certif_match(obj_key_str, input_key_str) == KM_OK )
    {
      matched = 1;
      LG_log(ep_ctx, LG_DEBUG, "ep_get_key_id: matching key-cert object found");
      /* add key id to list */
      *key_id_good = g_list_append(*key_id_good, value);
      LG_log(ep_ctx, LG_DEBUG, "ep_get_key_id: good key id [%s]", value);
      retval = EP_OK;
    }
    else
    {
      LG_log(ep_ctx, LG_DEBUG, "ep_get_key_id: non matching key-cert object found");
      /* add key id to list */
      *key_id_bad = g_list_append(*key_id_bad, value);
      LG_log(ep_ctx, LG_DEBUG, "ep_get_key_id: bad key id [%s]", value);
      
    }
  }
  g_string_free(return_string, TRUE);
  
  if ( ! matched )
  {
    /* no matching key-cert object found */
    LG_log(ep_ctx, LG_DEBUG, "ep_get_key_id: no matching key-cert object found");
    retval = EP_FAIL;
  }

  LG_log(ep_ctx, LG_FUNC, "<ep_get_key_id exiting with value %s", EP_ret2str(retval));
  return retval;
}


/*
  Returns candidate keywords.

  input - Input structure.

  return - Candidate keywords.
 */
gchar* EP_get_candidate_keywords(ep_input_structure_t *input) {

  GString *result;
  gchar **tokens;
  int curr_token;
  gchar *return_result;

  if (!input->mail_info) {
    return "";
  }
  tokens = ut_g_strsplit_v1(input->mail_info->subject->str, " ", 0);
  result = g_string_new("");

  for (curr_token = 0; tokens[curr_token]; curr_token++) {
    if (tokens[curr_token][0] == 0) {
      continue;
    }
    if (curr_token) {
      g_string_append(result, ",");
    }
    g_string_append(result, tokens[curr_token]);
  }
 
  g_strfreev(tokens);
  return_result = result->str;
  g_string_free(result, FALSE);
  return return_result;
}

/*
  For each blob in an input structure apply a function

  input     - the input structure
  func      - the function
  user_data - extra data to be passed to the function
*/
void EP_blobs_foreach(ep_input_structure_t *input,
  ObjFunc func, gpointer user_data) {

  GList *blobs;

  blobs = input->flattened_authenticated_data;
  if (blobs == NULL) {
    return;
  }
  do {
    func(blobs->data, user_data);
  } while ( (blobs = g_list_next(blobs)) );
}

/*
  Returns the requested attribute of a mail_info.

  input  - the input structure

  field  - the field

  return - the address
*/
const gchar* EP_get_mail_hdr_field(ep_input_structure_t *input, gchar *field) {

  if (input->mail_info) {
    if (strcasecmp(field, "from") == 0) {
      return input->mail_info->from->str;
    }
    if (strcasecmp(field, "subject") == 0) {
      return input->mail_info->subject->str;
    }
    if (strcasecmp(field, "cc") == 0) {
      return input->mail_info->cc->str;
    }
    if (strcasecmp(field, "date") == 0) {
      return input->mail_info->date->str;
    }
    if (strcasecmp(field, "replyto") == 0) {
      return input->mail_info->reply_to->str;
    }
    if (strcasecmp(field, "message-id") == 0) {
      return input->mail_info->message_id->str;
    }
    return NULL;
  }
  else {
    return NULL;
  }
}

/*
  Returns a blob from a blob_credential.

  input  - the blob_credential

  return - the blob
*/
const gchar* EP_get_blob(ep_authenticated_data_t *input) {
  return input->exp_data->data_str->str;
}

/*
  Returns a credential from a blob_credential

  input  - the blob_credential

  return - the credential 
*/
GList* EP_get_credentials(ep_authenticated_data_t *input) {
  return input->credentials;
}

void ep_input_structure_remove_auth_data(gpointer data, gpointer user) {

  ep_authenticated_data_t *a_data;

  a_data = (ep_authenticated_data_t*) data;
  ep_authenticated_data_free(a_data);
}

void EP_input_structure_free(ep_input_structure_t *input) {

  if (input->mail_info) {
    ep_mail_info_free(input->mail_info);
  }
  g_list_foreach(input->flattened_authenticated_data, ep_input_structure_remove_auth_data, NULL);
  g_list_free(input->flattened_authenticated_data);
    
  g_free(input);

}

/*
  Frees a mail_info structure.

  mail_info - mail_info structure.
*/
void ep_mail_info_free(ep_mail_info_t* mail_info) {
  g_string_free(mail_info->from, TRUE);
  g_string_free(mail_info->subject, TRUE);
  g_string_free(mail_info->date, TRUE);
  g_string_free(mail_info->reply_to, TRUE);
  g_string_free(mail_info->cc, TRUE);
  g_string_free(mail_info->message_id, TRUE);
  g_free(mail_info);
}

/*
  Explodes a piece of input.

  a_data   - authenticated data to explode
  input_structure - location of pointer to input_structure

  return - EP status value
*/
int ep_explode(ep_authenticated_data_t *a_data, 
                              ep_input_structure_t **input_structure) {

  LG_log(ep_ctx, LG_FUNC, ">ep_explode: entered");

  /* Because ep_text_driver and ep_mail_driver involve lots of recursive 
     function calls and basically all they do is a mass of string handling
     it is difficult to pass a return status value around.
     So instead try a global return status */
  ep_retval = EP_OK;
  switch (a_data->exp_data->type) {

    case EP_TEXT:
      LG_log(ep_ctx, LG_DEBUG, "ep_explode: text message");
      *input_structure = ep_text_driver(a_data);
      break;

    case EP_MAIL:
      LG_log(ep_ctx, LG_DEBUG, "ep_explode: mail message");
      *input_structure = ep_mail_driver(a_data);
      break;
    
    default:
      /* should never happen */
      LG_log(ep_ctx, LG_FATAL, "ep_explode: unknown message type");
      ep_retval = EP_FATAL;
      break;
  }

  LG_log(ep_ctx, LG_FUNC, "<ep_explode: exiting with value %s", EP_ret2str(ep_retval));
  return ep_retval;
}

/*
  EP_stream_to_string - Converts a file to a GString
  If stream is NOT NULL it reads the data from it
  otherwise it opens the file name first.

  receives  stream - FILE stream (or NULL)
            file_name - name of stream file (or NULL)
            location of return string pointer

  return - a GString with the whole file contents

  The file is supposedly a text string (without 0s).
*/
int EP_stream_to_string(FILE *stream, char *file_name, GString **return_string) {
#define ESTS_BUFFER_SIZE 1025
#define ESTS_READ_SIZE 1024
  int retval = EP_OK;
  char buffer[ESTS_BUFFER_SIZE];
  FILE *input;

  LG_log(ep_ctx, LG_DEBUG, ">EP_stream_to_string entered ");

  *return_string = g_string_new("");
  if ( stream )
  {
    input = stream;
  }
  else
  {
    if ( file_name == NULL || (input = fopen(file_name, "r")) == NULL )
    {
      LG_log(ep_ctx, LG_FATAL, "<EP_stream_to_string: Can't open stream file [%s]", 
                                 file_name ? file_name : "NULL");
      retval =  EP_FATAL;
      return retval;
    }
  }
  
  while (!feof(input)) {
    memset(buffer, 0, ESTS_BUFFER_SIZE);
    fread(buffer, 1, ESTS_READ_SIZE, input);
    if ( ferror(input) )
    {
      retval =  EP_FATAL;
      break;
    }
    g_string_append(*return_string, buffer);
  }
  
  if ( ! stream )
  {
    fclose(input);
  }

  LG_log(ep_ctx, LG_DEBUG, "<EP_stream_to_string exiting with status [%s] string [\n%s\n]", 
                                       EP_ret2str(retval), (*return_string)->str );
  return retval;
}

/*
  Creates a new input_structure.

  return - input_structure.
 */
ep_input_structure_t* ep_input_structure_new() {
  ep_input_structure_t * input;

  input = g_malloc0(sizeof(ep_input_structure_t));
  input->mail_info = NULL;
  input->flattened_authenticated_data = NULL;

  return input;
}

/*
  Creates a new authenticated_data structure.

  return - authenticated_data structure.
*/
ep_authenticated_data_t* ep_authenticated_data_new() {
  ep_authenticated_data_t* a_data;

  a_data = g_malloc0(sizeof(ep_authenticated_data_t));
  a_data->exp_data = ep_exploded_data_new();
  a_data->credentials = NULL;

  return a_data;
}

/*
  Frees an authenticated_data structure.

  data - authenticated_data structure.
 */
void ep_authenticated_data_free(ep_authenticated_data_t *a_data) {

  ep_exploded_data_free(a_data->exp_data);
  CR_credential_list_free(a_data->credentials);
  g_free(a_data);
}

/*
  Creates a new exploded_data structure.

  return - exploded_data structure.
*/
ep_exploded_data_t *ep_exploded_data_new() {

  ep_exploded_data_t *exp_data;

  exp_data = g_malloc0(sizeof(ep_exploded_data_t));
  exp_data->data_str = g_string_new("");

  return exp_data;
}

/*
  Frees an exploded_data structure.

  data - expolded_data structure.
 */
void ep_exploded_data_free(ep_exploded_data_t *exp_data) {

  g_string_free(exp_data->data_str, TRUE);
  g_free(exp_data);
}

/*
  Creates a new mail_info structure.

  header    - pointer to mail header info

  return - New mail_info structure.
 */
  
ep_mail_info_t *ep_mail_info_new(MM_header_t *header) {

  ep_mail_info_t *mail_info;

  mail_info = g_malloc0(sizeof(ep_mail_info_t));
  mail_info->from = g_string_new(header->from ? header->from : "");
  mail_info->subject = g_string_new(header->subject ? header->subject : "");
  mail_info->date = g_string_new(header->date ? header->date : "");
  mail_info->reply_to = g_string_new(header->reply_to ? header->reply_to : "");
  mail_info->cc = g_string_new(header->cc ? header->cc : "");
  mail_info->message_id = g_string_new(header->message_id ? header->message_id : "");

  return mail_info;
}

