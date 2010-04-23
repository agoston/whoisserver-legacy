#include "rip.h"
#include "ep.h"
#include <stdlib.h>
#include <glib.h>

/*
  Please see ep_mail_driver first
*/


/*
  text/plain process, uses text driver.
*/
GList *ep_plain_process(gchar *content, GList *credentials) {

  gchar **split;
  GList *return_list;

  LG_log(ep_ctx, LG_FUNC, ">ep_plain_process: entered");

  split = ut_g_strsplit_v1(content, "\n", -1);

  LG_log(ep_ctx, LG_DEBUG, "ep_plain_process: Calling ep_split_text");
  return_list = ep_split_text(split, credentials);

  g_strfreev(split);

  LG_log(ep_ctx, LG_FUNC, "<ep_plain_process: exiting");
  return return_list;
}

/*
  Processes multipart/mixed, in practice tries to
    process all parts.
*/
GList *ep_mixed_process(MM_content_t *content, GList *credentials) {

  GList *return_list;
  GList *parts;

  LG_log(ep_ctx, LG_FUNC, ">ep_mixed_process: entered");

  return_list = NULL;
  parts = content->parts;

  while (parts) {
    return_list = g_list_concat(return_list, ep_content_explode((MM_content_t*)parts->data, credentials));
    parts = parts->next;
  }

  LG_log(ep_ctx, LG_FUNC, "<ep_mixed_process: exiting");
  return return_list;
}

/*
  Processes multipart/alternative, in practice
    tries to process the 1st part only.
*/
GList *ep_alternative_process(MM_content_t *content, GList *credentials) {

  GList *return_list;
  GList *parts;

  LG_log(ep_ctx, LG_FUNC, ">ep_alternative_process: entered");

  return_list = NULL;
  parts = content->parts;

  if  (parts) {
    return_list = ep_content_explode((MM_content_t*)parts->data, credentials);
  }

  LG_log(ep_ctx, LG_FUNC, "<ep_alternative_process: exiting");
  return return_list;
}

/*
  Puts a mime part in 13+10 line ending format for pgp
    signature verification
*/
GString *ep_prepare_bulk(gchar *bulk) {

  GString *prepared_bulk;
  gint i;
  gchar prev;
  int len;

  len = strlen(bulk);
  prepared_bulk = g_string_new("");
  prev = 0;

  for (i=0;i<len;i++) {
    if (bulk[i]==10 && prev!=13) {
      g_string_append_c(prepared_bulk,13);
    }
    g_string_append_c(prepared_bulk, bulk[i]);
    prev = bulk[i];
  }

  return prepared_bulk;

}

/* Determine the type of signature from the bulk data supplied
   which includes the mime header for the signature part.
   
   Receives bulk_sig   pointer to signature part with mime header
   
   Returns  signature type
*/

EP_sig_type ep_get_sig_type(gchar *bulk_sig)
{
  int sig_type = EP_UNKNOWN;
  gchar **split;
  gchar **split_iter;
  gchar *current_line;

  LG_log(ep_ctx, LG_FUNC, ">ep_get_sig_type: entered");
  
  split = ut_g_strsplit_v1(bulk_sig, "\n", -1);
  split_iter = split;
  current_line = *split_iter;

  while (current_line != NULL) {

    g_strdown(current_line);
    if ( ! strncmp(current_line, "content-type:", 13)) {

      /* this line is the Content-Type:, find the signature type */
      LG_log(ep_ctx, LG_DEBUG, "ep_get_sig_type: Content-Type line [%s]", current_line);
      if ( strstr(current_line, "application/pgp-signature") )
      {
        sig_type = EP_PGP;
      }
      else if ( strstr(current_line, "application/x-pkcs7-signature") ||
                strstr(current_line, "application/pkcs7-signature") )
      {
        sig_type = EP_X509;
      }
      break;
    }
    split_iter++;
    current_line = *split_iter;
  }
  
  g_strfreev(split);

  LG_log(ep_ctx, LG_FUNC, "<ep_get_sig_type: exiting with signature type [%s]", 
                                EP_sig_type_ret2str(sig_type));
  return sig_type;
}

/*
  Processes multipart/signed with X.509

*/
GList *ep_x509_signed_process(MM_content_t *content, GList *credentials) {
#define LINE_LENGTH 400

  int retval;
  GList *key_id_item = NULL;
  GList *key_id_good = NULL;
  GList *key_id_bad = NULL;
  char *X509cert_file = NULL;
  const gchar *ep_FP = NULL;
  gchar *bulk;
  gchar *signature;
  GList *returned_content;
  KM_key_return_t *key_data = NULL;
  GString *mess;

  LG_log(ep_ctx, LG_FUNC, ">ep_x509_signed_process: entered");

  //bulk = ((MM_content_t*)content->parts->data)->bulk_content;
  bulk = content->bulk_content;
  signature = ((MM_content_t*)content->parts->next->data)->bulk_content;

  /* reconstruct the signed message from the mime parts */
  /* note that the 'x-' is optional (old format) in 'x-pkcs7' */
    mess = g_string_new(NULL);
    mess = g_string_append(mess, "Content-Type: multipart/signed; protocol=\"application/x-pkcs7-signature\"; micalg=sha1; boundary=\"----bnd\"\n\n");
    mess = g_string_append(mess, "------bnd\n");
    mess = g_string_append(mess, bulk );
    mess = g_string_append(mess, "\n------bnd\n");
    mess = g_string_append(mess, signature );
    mess = g_string_append(mess, "\n------bnd--");
  
  /* not sure if this next step is needed for X.509 */
//  prepared_bulk = ep_prepare_bulk(bulk);

  /* check the signature */
  retval = ep_check_X509_sig(NULL, NULL, mess->str, &key_data, EP_MAIL, &X509cert_file);
  g_string_free(mess, 1);
  LG_log(ep_ctx, LG_DEBUG, "ep_x509_signed_process: cert_file name [%s]", X509cert_file);
  if ( retval == EP_FATAL )
  {
    ep_retval = EP_FATAL;
    if ( X509cert_file )
    {
      unlink(X509cert_file);
      g_free(X509cert_file);
    }
    LG_log(ep_ctx, LG_DEBUG, "ep_x509_signed_process: X.509 certificate error");
    LG_log(ep_ctx, LG_FUNC, "<ep_x509_signed_process: exiting with fatal error");
    return NULL;
  }
  else 
  {
//    ep_DN = KM_key_return_get_parameter(key_data, KM_KO);

    if ( retval == EP_OK )
    {
      /* now find the key-cert object to match this certificate */
      ep_FP = KM_key_return_get_parameter(key_data, KM_FP);
      retval = ep_get_key_id(ep_FP, X509cert_file, NULL, &key_id_good, &key_id_bad);
      unlink(X509cert_file);
      g_free(X509cert_file);
      X509cert_file = NULL;
      if (retval == EP_FATAL)
      {
        ep_retval = EP_FATAL;
        LG_log(ep_ctx, LG_FUNC, "<ep_x509_signed_process: exiting with fatal error");
        return NULL;
      }
    }
    else
    {
      unlink(X509cert_file);
      g_free(X509cert_file);
      X509cert_file = NULL;
    }

    /* create new credential(s) for this certificate */
    credentials = CR_credential_list_duplicate(credentials);
    for ( key_id_item = key_id_good; key_id_item != NULL ; key_id_item = g_list_next(key_id_item) )
    {
      LG_log(ep_ctx, LG_DEBUG, "ep_x509_signed_process: create validated credential");
      credentials = g_list_append(credentials, CR_credential_new(CR_X509, key_id_item->data, TRUE));
      free(key_id_item->data);
    }
    for ( key_id_item = key_id_bad; key_id_item != NULL ; key_id_item = g_list_next(key_id_item) )
    {
      LG_log(ep_ctx, LG_DEBUG, "ep_x509_signed_process: create invalidated credential");
      credentials = g_list_append(credentials, CR_credential_new(CR_X509, key_id_item->data ? key_id_item->data : "unknown ID", FALSE));
      if ( key_id_item->data )
      {
        free(key_id_item->data);
      }
    }
    if ( key_id_good )  g_list_free(key_id_good);
    if ( key_id_bad )  g_list_free(key_id_bad);
  }

  /* now process the signed content */
  returned_content =
    ep_content_explode((MM_content_t*)content->parts->data, credentials);

//  g_string_free(prepared_bulk, TRUE);
  CR_credential_list_free(credentials);
  if ( key_data )
  {
    KM_key_return_free(key_data);
  }

  LG_log(ep_ctx, LG_FUNC, "<ep_x509_signed_process: exiting");
  return returned_content;
}

/*
  Processes multipart/signed with pgp
*/
GList *ep_pgp_signed_process(MM_content_t *content, GList *credentials) {

  gchar *bulk;
  gchar *signature;
  GString *prepared_bulk;
  KM_key_return_t *signature_status;
  GList *returned_content;
  KM_status_t ret_status;

  LG_log(ep_ctx, LG_FUNC, ">ep_pgp_signed_process: entered");

  //bulk = ((MM_content_t*)content->parts->data)->bulk_content;
  bulk = content->bulk_content;
  signature = ((MM_content_t*)content->parts->next->data)->content;

  prepared_bulk = ep_prepare_bulk(bulk);
  /* check the signature */
  signature_status = KM_signature_verify(KM_PGP, prepared_bulk->str, signature);
  ret_status = KM_key_return_get_status(signature_status);
  LG_log(ep_ctx, LG_DEBUG, "ep_pgp_signed_process: sig verify status [%s]", KM_return_string(ret_status));

  /* create a new credential for this certificate */
  credentials = CR_credential_list_duplicate(credentials);
  if (ret_status == KM_OK) {
    LG_log(ep_ctx, LG_DEBUG, "ep_pgp_signed_process: create pgp cred");
    LG_log(ep_ctx, LG_DEBUG, "ep_pgp_signed_process: key-id [%s]", 
                                      KM_key_return_get_key_id(signature_status));
    LG_log(ep_ctx, LG_DEBUG, "ep_pgp_signed_process: sig OK [%s]", 
            KM_key_return_get_signature_ok(signature_status) == TRUE ? "TRUE" : "FALSE");
    credentials =
      g_list_append(credentials,
         CR_credential_new(CR_PGP,
                           KM_key_return_get_key_id(signature_status),
                           KM_key_return_get_signature_ok(signature_status)));
  }

  /* now process the signed content */
  returned_content =
    ep_content_explode((MM_content_t*)content->parts->data, credentials);

  g_string_free(prepared_bulk, TRUE);
  CR_credential_list_free(credentials);
  KM_key_return_free(signature_status);

  LG_log(ep_ctx, LG_FUNC, "<ep_pgp_signed_process: exiting");
  return returned_content;
}

/* check if number of parts are correct! */
/*
  Processes multipart/signed

*/
GList *ep_signed_process(MM_content_t *content, GList *credentials) {

  gchar *signature;
  gchar *bulk_sig;
  GList *returned_content = NULL;
  EP_sig_type sig_type;

  LG_log(ep_ctx, LG_FUNC, ">ep_signed_process: entered");

  signature = ((MM_content_t*)content->parts->next->data)->content;
  bulk_sig = ((MM_content_t*)content->parts->next->data)->bulk_content;

  /* Determine signature type 
     Use bulk_sig which contains the signature with mime header. */
  sig_type = ep_get_sig_type(bulk_sig);
  
  /* now process the signed message by signature type */
  if ( sig_type == EP_PGP ) {
  
    returned_content = ep_pgp_signed_process(content, credentials);
  }
  else if ( sig_type == EP_X509 ) {
  
    returned_content = ep_x509_signed_process(content, credentials);
  }
  else {
  
    /* the signature type is not recognised so process it as plain text 
       after exploding the signed content part */
    returned_content =
        ep_content_explode((MM_content_t*)content->parts->data, credentials);
    returned_content = g_list_concat(returned_content, 
                             ep_plain_process(signature, credentials));
  }

  LG_log(ep_ctx, LG_FUNC, "<ep_signed_process: exiting");
  return returned_content;
}

/*
  Processes a mail (embedded in another) message.

  content - content structure representing the... content.
  credentials - current list of credentials

  return - List of authenticated_data
 */
GList* ep_message_process(MM_content_t* content, GList* credentials) {

  MM_mail_info_t* mail;
  GList* return_list;

  LG_log(ep_ctx, LG_FUNC, ">ep_message_process: entered");

  mail = MM_extract_mail_info(content->content);
  
  credentials = CR_credential_list_duplicate(credentials);
  //credentials = g_list_append(credentials, CR_credential_new(CR_FROM, mail->header->from, TRUE));
  return_list = ep_content_explode(mail->content, credentials);

  CR_credential_list_free(credentials);
  MM_mail_info_free(mail);

  LG_log(ep_ctx, LG_FUNC, "<ep_message_process: exiting");
  return return_list; 
}

/*
  Calls the appropriate function for the content type
*/
GList *ep_content_explode(MM_content_t *content, GList *credentials) {

  GList *ret_list;

  LG_log(ep_ctx, LG_FUNC, ">ep_content_explode: entered");

  if (!content) {
    ret_list = NULL;
  }
  else {
    switch (content->type) {

      case MM_PLAIN:
        ret_list = ep_plain_process(content->content, credentials);
        break;
      case MM_MULTIPART_MIXED:
        ret_list = ep_mixed_process(content, credentials);
        break;
      case MM_MULTIPART_ALTERNATIVE:
        ret_list = ep_alternative_process(content, credentials);
        break;
      case MM_MULTIPART_SIGNED:
        ret_list = ep_signed_process(content, credentials);
        break;
      case MM_MESSAGE:
        ret_list = ep_message_process(content, credentials);
        break;
      default:
        LG_log(ep_ctx, LG_DEBUG, "ep_content_explode: unknown content type");
        ret_list = NULL;
        break;
    }
  }
  
  LG_log(ep_ctx, LG_FUNC, "<ep_content_explode: exiting");
  return ret_list;
}


/*
  This is the entry point for the mail driver

  This code can be recursive. There is a contract regarding the
  credential list: if a function wants to use it (attach it to another
  structure then it has to copy it completely - list plus credentials).

  In practice it simply calls ep_content explode appending a mail from
  credential.
*/
ep_input_structure_t *ep_mail_driver(ep_authenticated_data_t *a_data) {

  ep_input_structure_t *input;
  MM_mail_info_t *mail;
  GList *credentials;
//MM_content_t *tmp;

// printf("ep_mail_driver: mail->data [\n%s\n]\n", a_data->exp_data->data_str->str);
  LG_log(ep_ctx, LG_FUNC, ">ep_mail_driver: entered");

  /* Create MM mailer structure */
  mail = MM_extract_mail_info(a_data->exp_data->data_str->str);
  if ( ep_retval == EP_FATAL )
  {
    LG_log(ep_ctx, LG_FUNC, "<ep_mail_driver: exiting with status [EP_FATAL]");
    return NULL;
  }
/*printf("ep_mail_driver: mail->content->content [\n%s\n]\n", mail->content->content ? mail->content->content : "NULL");
printf("ep_mail_driver: mail->content->bulk_content [\n%s\n]\n", mail->content->bulk_content ? mail->content->bulk_content : "NULL");
if ( mail->content->parts && g_list_next(mail->content->parts) )
{
tmp = (MM_content_t *)(g_list_next(mail->content->parts)->data);
printf("ep_mail_driver: next(mail->content->parts->data)->content [\n%s\n]\n", tmp->content);
} */

  /* Create input structure */
  input = ep_input_structure_new();
  input->mail_info = ep_mail_info_new(mail->header);
  input->is_mail = TRUE;

  credentials =
    g_list_append(a_data->credentials,
                  CR_credential_new(CR_FROM, mail->header->from ?
                                    mail->header->from : "", TRUE));

  LG_log(ep_ctx, LG_DEBUG, "ep_mail_driver: Exploding data");
  input->flattened_authenticated_data =
    ep_content_explode(mail->content, credentials);
  MM_mail_info_free(mail);

  LG_log(ep_ctx, LG_FUNC, "<ep_mail_driver: exiting");
  return input; 

}
