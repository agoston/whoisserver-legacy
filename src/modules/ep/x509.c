#include "rip.h"
#include "ep.h"
#include "dbupdate.h"



int ep_verify_X509_sig(char *mail_file, char **cert_file)
{
#define LINE_LENGTH 400

  int retval = EP_OK;
  char *opensslcmd = OPENSSL_CMD;
  FILE* general;
  GString* x509_line;
  gchar status_file[LINE_LENGTH];
  gchar irrelevant_file[LINE_LENGTH];
  gchar tmp[LINE_LENGTH];
  gchar txt[LINE_LENGTH];
 
  LG_log(ep_ctx, LG_DEBUG, ">ep_verify_X509_sig: entered ");

  if  (ep_temporary_directory)
  {
    strcpy(tmp, ep_temporary_directory);
  }
  else
  {
    strcpy(tmp, "/tmp");
  }
  sprintf(status_file,"%s/x509_status_%d", tmp, (int)getpid());
  *cert_file = g_malloc0( strlen(tmp) + strlen("/x509_cert_file.pem_") + 64 + 1);
  sprintf(*cert_file,"%s/x509_cert_file.pem_%d", tmp, (int)getpid());
  sprintf(irrelevant_file,"%s/irrelevant_text_%d", tmp, (int)getpid());
  
//  opensslcmd = ca_get_opensslcmd();

  /* Check if the signature is valid */
  x509_line = g_string_new(opensslcmd);

  g_string_append(x509_line, " smime -verify -in  ");
  g_string_append(x509_line, mail_file);
  g_string_append(x509_line, " -out ");
  g_string_append(x509_line, irrelevant_file);
  g_string_append(x509_line, " -signer ");
  g_string_append(x509_line, *cert_file);
  g_string_append(x509_line, " -noverify ");
  g_string_append(x509_line, " > ");
  g_string_append(x509_line, status_file);
  g_string_append(x509_line, " 2>&1");

  LG_log(ep_ctx, LG_DEBUG, "ep_verify_X509_sig: openssl cmd:\n[%s]", x509_line->str);
  system(x509_line->str);
  g_string_free(x509_line, TRUE);
  unlink(irrelevant_file);  /* not interested in the output */

  /* Parsing openssl output */
  if (( general = fopen(status_file, "r")) == NULL)
  {
    LG_log(ep_ctx, LG_FATAL, "ep_verify_X509_sig: Can't open status file [%s]", status_file);
    LG_log(ep_ctx, LG_DEBUG, "<ep_verify_X509_sig: exiting with value EP_FATAL");
    return EP_FATAL;
  }

  retval = EP_BAD_SIG;
  while (fgets (txt, LINE_LENGTH - 1, general) != NULL)
  {
    LG_log(ep_ctx, LG_DEBUG, "ep_verify_X509_sig: openssl return: [%s]", txt);
    g_strdown(txt);
    if ((strstr(txt, "verification successful")))
    {
      retval = EP_GOOD_SIG;
      break;
    }
  }
  fclose(general);
  unlink(status_file);

  LG_log(ep_ctx, LG_DEBUG, "<ep_verify_X509_sig: exiting with value %s", EP_verify_ret2str(retval));
  return retval;
}


/* Verifies the signature in an X.509 certificate and 
   extracts some information from the certificate.
   Receives certificate file name
            location of pointer to key structure
            update message type
            location of pointer to cert file name (used to return file name for mail messages)
   Returns  status
*/

int ep_check_X509_sig( char *X509cert_file, char *X509certificate, char *message, 
                          KM_key_return_t **key_data, int type, char **cert_file )
{
#define LINE_LENGTH 400
  int retval = EP_OK;
//  char *tmp_cert;
  KM_status_t key_status;
  GString *signature;
  gchar msg_file[LINE_LENGTH];
  gchar tmp[LINE_LENGTH];
  FILE* general;

  LG_log(ep_ctx, LG_FUNC, ">EP_check_X509_sig: entered");

  /* for sync updates and LIR Portal the signature is verified by apache and the message
     received by dbupdate is not signed */
  if ( type == EP_MAIL && ! X509certificate )
  {
    /* this is a mail update so verify the signature */
    LG_log(ep_ctx, LG_DEBUG, "EP_check_X509_sig: this is a mail message, verify signature");
    
    if (ep_temporary_directory)
    {
      strcpy(tmp, ep_temporary_directory);
    }
    else
    {
      strcpy(tmp, "/tmp");
    }
    sprintf(msg_file,"%s/ep_msg_%d", tmp, (int)getpid());
    
    general = fopen(msg_file,"w");
    if ( ! general )
    {
      LG_log(ep_ctx, LG_DEBUG, "EP_check_X509_sig: failed to open message file [%s]", msg_file);
      LG_log(ep_ctx, LG_FUNC, "<EP_check_X509_sig: exiting with value %s", EP_ret2str(retval));
      return EP_FATAL;
    }
    LG_log(ep_ctx, LG_DEBUG, "EP_check_X509_sig: message [%s]", message);
    fprintf(general, "%s", message);
    fclose(general);

    retval = ep_verify_X509_sig(msg_file, cert_file);
    unlink(msg_file);
    
    if (retval == EP_GOOD_SIG)
    {
      retval = EP_OK;
    }
    else if (retval == EP_BAD_SIG)
    {
      retval = EP_FAIL;
    }

    if ( retval != EP_OK )
    {
      LG_log(ep_ctx, LG_FUNC, "<EP_check_X509_sig: exiting with value %s", EP_ret2str(retval));
      return retval;
    }
  }
  
  /* get the data from the certificate */
  LG_log(ep_ctx, LG_DEBUG, "EP_check_X509_sig: extract info from certificate");
  if ( X509cert_file )
  {
    /* this is a web update so read from X509cert_file */
    /* read the certificate from the file into a string */
    retval = EP_stream_to_string(NULL, X509cert_file, &signature);
    if (retval == EP_FATAL)
    {
      g_string_free(signature, TRUE);
      LG_log(ep_ctx, LG_FATAL, "EP_check_X509_sig: certificate file read error");
      LG_log(ep_ctx, LG_FUNC, "<EP_check_X509_sig: exiting with value %s", EP_ret2str(retval));
      return retval;
    }
  }
  else if ( X509certificate )
  {
printf("certificate pre-op\n[%s]\n", X509certificate);
//printf("X509certificate[0] [%c]  X509certificate[strlen(X509certificate)-1] [%c]\n", 
//        X509certificate[0], X509certificate[strlen(X509certificate)-1] );
    /* this is an LIR Portal update so read from cert string */
//    signature = g_string_new("");
    signature = g_string_new(X509certificate);
//    if ( X509certificate[0] == '"' && X509certificate[strlen(X509certificate)-1] == '"' )
//    {
      /* the certificate string is enclosed in double quotes, remove them */
//      tmp_cert = strdup(X509certificate + 1);
//      tmp_cert[strlen(tmp_cert)-1] = '\0';
//    }
//    else tmp_cert = strdup(X509certificate);
//printf("certificate post-op\n[%s]\n", tmp_cert);
//    g_string_append(signature, "-----BEGIN CERTIFICATE-----\n");
//    g_string_append(signature, tmp_cert);
//    g_string_append(signature, "\n-----END CERTIFICATE-----\n");
//    free(tmp_cert);
  }
  else
  {
    /* this is a mail update so read from cert_file */
    /* read the certificate from the file into a string */
    retval = EP_stream_to_string(NULL, *cert_file, &signature);
    if (retval == EP_FATAL)
    {
      g_string_free(signature, TRUE);
      LG_log(ep_ctx, LG_FATAL, "EP_check_X509_sig: certificate file read error");
      LG_log(ep_ctx, LG_FUNC, "<EP_check_X509_sig: exiting with value %s", EP_ret2str(retval));
      return retval;
    }
  }

  /* get the information we want into the structure */
  key_status = KM_key_get_info(KM_X509, signature->str, key_data);
  RT_report_key_info(rt_ctx, *key_data);
  if ( key_status == KM_OK )
  {
    retval = EP_OK;
  }
  else
  {
    retval = EP_FAIL;
  }

  g_string_free(signature, TRUE);
  LG_log(ep_ctx, LG_FUNC, "<EP_check_X509_sig: exiting with value %s", EP_ret2str(retval));
  return retval;
}
