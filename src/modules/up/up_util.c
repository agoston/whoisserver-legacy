/***************************************
  $Id $

  UP_util.c

  Status: NOT REVIEWED, NOT TESTED

  Author(s):       Denis Walker

  ******************/ /******************
  Modification History:
        denis (20021021) Created.
  ******************/ /******************
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

#include "config.h"
#include <assert.h>

#include "rip.h"
#include "ep.h"
#include "rt.h"
#include "km.h"
#include "lg.h"
#include "lu_whois.h"
#include "dbupdate.h"
#include "up_pre_process.h"
#include "up_auto_nic.h"
#include "up_util.h"
#include "ca_defs.h"
#ifdef RDNSYES
#include "ns_util.h"
#else
#define ns_is_decompose(x,y,z,t) (0)
#define ns_remove_trailing_dot(x,y) (0)
#define ns_decompose_object(x,y,z,t,w) NULL
#endif
#include <glib.h>

/* Report an internal software error, send an ack if possoble and exit out of dbupdate
   Receives RT context
            LG context
            options structure
            error message string
            flag to surpress ack message
   Returns  none
*/

void UP_internal_error(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                     options_struct_t *options, char *err_msg, int no_ack)
{
  /* ER_perror(FAC_UP, err_type, err_msg); */
  LG_log(lg_ctx, LG_FUNC,">UP_internal_error: entered");

  LG_log(lg_ctx, LG_FATAL,"%s", err_msg);
  LG_log(lg_ctx, LG_PERROR,"%s", err_msg);

  RT_internal_error(rt_ctx);
  /* close input to the reporting context */
  RT_end(rt_ctx);

  /* Send ack message */
  /* If it is a mail message and the error occured before unfolding the input then
     we will not have the email address, so no ack will be sent, but it will still
     be logged.
     If it is not a mail message then an ack will be sent and logged */
  /* If the internal error occured in the ack code then the ack will be surpressed */
  if ( ! no_ack )
  {
    /* double negative, only process ack if it is NOT surpressed */
    NT_process_acknowledgement(rt_ctx, lg_ctx, options);
  }

  /************************* TIDY UP AND SHUTDOWN  ***************************/

  /* close down the report */
  RT_destroy(rt_ctx);

  /* free any remaining memory */
  UP_free_options(options);

  LG_log(lg_ctx, LG_FATAL,"UP_internal_error: exit(1)");
  /* close down the logging */
  LG_ctx_free(lg_ctx);
  /* close the state log file, but do not delete the file */
  close(options->statefd);

  /* report abnormal termination to the wrapper */
  exit(20);
}


/* Deletes entries in the AUTO_key_hash
   Receives key
            value
            NULL
   Returns  none
 */

void  up_free_auto_key(gpointer key, gpointer value, gpointer user_data)
{
  free((char *)key);
  free((char *)value);
}


/* free the memory allocated for data in the options structure
   Receives options structure
   Returns  none
*/

void UP_free_options(options_struct_t *options)
{
  /* free all the memory used by the options data structure */
  if (options->input_file_name)
    free(options->input_file_name);
  if (options->config_file_name)
    free(options->config_file_name);
  if (options->X509cert_file)
    free(options->X509cert_file);
  if (options->X509certificate)
    free(options->X509certificate);
  if (options->keywords)
    free(options->keywords);
  if (options->time_str)
    free(options->time_str);
  if (options->origin)
    free(options->origin);
  if (options->mail_hdr_data.from)
    free(options->mail_hdr_data.from);
  if (options->mail_hdr_data.cc)
    free(options->mail_hdr_data.cc);
  if (options->mail_hdr_data.subject)
    free(options->mail_hdr_data.subject);
  if (options->mail_hdr_data.date)
    free(options->mail_hdr_data.date);
  if (options->mail_hdr_data.replyto)
    free(options->mail_hdr_data.replyto);
  if (options->mail_hdr_data.msgid)
    free(options->mail_hdr_data.msgid);
/* The following 'free' causes problems, purify complains:
   "Attempting to free block at 0x309f08 in the heap, not obtained from malloc."
  if (options.mail_hdr_data.from_email)
    free(options.mail_hdr_data.from_email);
*/

  /* free the AUTO_key_hash */
  g_hash_table_foreach( options->AUTO_key_hash, (GHFunc)up_free_auto_key, NULL);
  g_hash_table_destroy(options->AUTO_key_hash);
}

/* removes the '\n's and '\r's at the end of the string, and returns it
   Receives string
   Returns  string
*/

char *UP_remove_EOLs(char *arg) {
	int last = strlen(arg)-1;
	while (last >= 0 && (arg[last] == '\n' || arg[last] == '\r')) {
		arg[last--] = 0;
	}

	return arg;
}



/* Creates a temp file name using the type specified in the name
   It is first called before the LG context is set up.
   Receives LG context
            type of file (eg input, state)
   Returns  temp file name string
*/

char *UP_get_temp_filename(LG_context_t *lg_ctx, char *type)
{
  char *tmpdir = NULL;
  char hostname[MAXHOSTNAMELEN];
  char *tmpfilename;
  int length;
  int pid;
  gulong pidlong; /* pid is sometimes long sometimes int */

  if ( lg_ctx )
    LG_log(lg_ctx, LG_FUNC,">UP_get_temp_filename: entered type=[%s]", type);

  gethostname(hostname, MAXHOSTNAMELEN);
  pid = getpid();
  tmpdir = ca_get_tmpdir;

  length = strlen(tmpdir) +  strlen(hostname) + strlen(type) + 64 + 6;
  tmpfilename = (char *)malloc(length);

  pidlong=pid;
  snprintf(tmpfilename, length, "%s/dbupdate.%s.%s.%ld.%ld.tmp", tmpdir, type, hostname, time(NULL), pidlong );
  free(tmpdir);

  if ( lg_ctx )
    LG_log(lg_ctx, LG_FUNC,"<UP_get_temp_filename: exiting tmpfilename [%s]", tmpfilename);
  return tmpfilename;
}



/* Adds the contents of the given file to the update log
   and to the state log for debugging
   Receives RT context
            LG context
            options structure
            input file handle
   Returns  none
*/

void UP_add_to_upd_log(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                            options_struct_t *options, FILE *infile)
{
  char *time_str;
  char datestr[10];
  char *updlogfile = NULL;
  char *updlog = NULL;
  char buf[1024];
  char *line = NULL;
  struct tm *tmstr;
  time_t now;
  time_t cur_time;
  FILE *log_file;
  FILE *cert_file;

  LG_log(lg_ctx, LG_FUNC,">UP_add_to_upd_log: entered");

  /* We need to get the a date string to construct the updlog file name */
  time(&now);
  tmstr = localtime(&now);
  strftime(datestr, 10, "%Y%m%d", tmstr);

  /* now that we have the date string, we can construct updlog file name */
  updlog = ca_get_updlog;
  updlogfile = (char *)malloc(strlen(updlog) + strlen(datestr) + 2);
  snprintf(updlogfile, strlen(updlog) + strlen(datestr) + 2,
           "%s.%s", updlog, datestr);


  if (( log_file = fopen(updlogfile, "a")) == NULL)
  {
    LG_log(lg_ctx, LG_FATAL, "UP_add_to_upd_log: Can't open upd log file [%s]", updlogfile);
    free(updlogfile);
    UP_internal_error(rt_ctx, lg_ctx, options, "UP_add_to_upd_log: Can't open upd log file\n", 0);
  }
  free(updlogfile);

  /* get time */
  cur_time = time(NULL);
  time_str = strdup(ctime(&cur_time));
  /* cut the '\n' at the end */
  time_str[strlen(time_str) - 1] = '\0';

  /* write the record seperater to the upd log */
  if (options->mail_input)
  {
    fprintf(log_file, "\n>>> time: %s MAIL UPDATE <<<\n\n", time_str);
  }
  else if (options->origin)
  {
    /* include the origin */
    fprintf(log_file, "\n>>> time: %s SYNC UPDATE (%s) <<<\n\n",
                       time_str, options->origin);
    /* add the keywords from the command options */
    fprintf(log_file, "keywords - %s\n\n",
                       options->keywords ? options->keywords : "None" );
  }
  else
  {
    fprintf(log_file, "\n>>> time: %s UPDATE <<<\n\n", time_str);
    /* add the keywords from the command options */
    fprintf(log_file, "keywords - %s\n\n",
                       options->keywords ? options->keywords : "None" );
  }
  free(time_str);

  LG_log(lg_ctx, LG_DEBUG, "UP_add_to_upd_log: Input data");
  LG_log(lg_ctx, LG_DEBUG, ">[");

  /* write the full update to the upd log and state file */
  while ( fgets(buf, 1023, infile) != NULL )
  {
    line = strdup(buf);
    line = UP_remove_EOLs(line);
    fprintf(log_file, "%s", buf);
    LG_log(lg_ctx, LG_DEBUG, "%s", line);
    free(line);
  }
  LG_log(lg_ctx, LG_DEBUG, "<]");

  if ( options->X509certificate )
  {
    /* we were passed a certificate on the command line, copy it to the log file */
    fprintf(log_file, "\nThe following certificate was passed on the command line\n\n");
    fprintf(log_file, "%s\n", options->X509certificate);
  }

  if ( options->X509cert_file )
  {
    /* we were passed a certificate in a file, copy it to the log file */

    if (( cert_file = fopen(options->X509cert_file, "r")) == NULL)
    {
      LG_log(lg_ctx, LG_FATAL, "UP_add_to_upd_log: Can't open X.509 certificate file [%s]", options->X509cert_file);
      UP_internal_error(rt_ctx, lg_ctx, options, "UP_add_to_upd_log: Can't open X.509 certificate file file\n", 0);
    }

    LG_log(lg_ctx, LG_DEBUG, "UP_add_to_upd_log: X.509 certificate data");
    LG_log(lg_ctx, LG_DEBUG, ">[");

    /* write the certificate to the upd log and state file */
    fprintf(log_file, "\nThe following certificate was passed in a file\n\n");
    while ( fgets(buf, 1023, cert_file) != NULL )
    {
      line = strdup(buf);
      line = UP_remove_EOLs(line);
      fprintf(log_file, "%s", buf);
      LG_log(lg_ctx, LG_DEBUG, "%s", line);
      free(line);
    }
    fprintf(log_file, "\n");
    LG_log(lg_ctx, LG_DEBUG, "<]");

    fclose(cert_file);
  }

  fclose(log_file);
  LG_log(lg_ctx, LG_FUNC,"<UP_add_to_upd_log: exiting");
}



/* Gets a GList of attr structures and checks if one of their
   (upper case)values starts with substr.
   Receives LG context
            list of attribute structures
            string to search for
   Returns  1 if found
            0 if NOT found
*/

int UP_strstr_in_attr_list(LG_context_t *lg_ctx, GList *list, const char *substr)
{
  GList *item = NULL;
  char *word;

  LG_log(lg_ctx, LG_FUNC,">UP_strstr_in_attr_list: entered with search string [%s]", substr);

  for( item = list; item != NULL ; item = g_list_next(item) )
  {
   word = rpsl_attr_get_clean_value((rpsl_attr_t *)(item->data));
   g_strup(word);
   if (strstr(word, substr) == word)
   {
     free(word);
     LG_log(lg_ctx, LG_FUNC,"<UP_strstr_in_attr_list: exiting with value 1 (sub string found)");
     return 1;
   }
   free(word);
  }
  /* none of them matched, so return 0 */
  LG_log(lg_ctx, LG_FUNC,"<UP_strstr_in_attr_list: exiting with value 0 (no sub string found)");
  return 0;
}

/* Remove generated parent attribute(s)
   Receives LG context
            pointer to object string
            parsed object
   Returns  1 if any changes made
            0 otherwise
*/

int up_remove_parent(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                        char **object_str, rpsl_object_t *object)
{
  int retval = 0;
  char *value = NULL;
  int pos;
  GList *parent_list = NULL;

  LG_log(lg_ctx, LG_FUNC,">up_remove_parent: entered");

  /* find parent attributes */
  parent_list = rpsl_object_get_attr(object, "parent");
  if ( parent_list != NULL ) retval = 1;
  while ( parent_list != NULL )
  {
    /* remove first parent on the list */
	value = rpsl_attr_get_clean_value((rpsl_attr_t *)(parent_list->data));
	pos = rpsl_attr_get_ofs((rpsl_attr_t *)(parent_list->data));
	rpsl_object_remove_attr(object, pos, NULL);
    LG_log(lg_ctx, LG_DEBUG,"up_remove_parent: removed parent: [%s]", value);
	free(value);
    /* must regenerate parent list now as positions have changed after
       deleting an attribute */
    parent_list = rpsl_object_get_attr(object, "parent");
  }

  if ( retval )
  {
    /* regenerate object text string */
    *object_str = rpsl_object_get_text(object, 0);
   }

  LG_log(lg_ctx, LG_FUNC,"<up_remove_parent: exiting with value [%d]", retval);
  return retval;
}

/* Cleans out any unwanted data
   Receives LG context
            pointer to object string
   Returns  1 if any changes made
            0 otherwise
*/

char *up_clean_object(RT_context_t *rt_ctx, LG_context_t *lg_ctx, char **object_str)
{
  int retval = 0;
  char *retstr = NULL;
  const char *type = NULL;
  char *value = NULL;
  rpsl_object_t *object = NULL;
  GString *mess;

  LG_log(lg_ctx, LG_FUNC,">up_clean_object: entered with object [\n%s]", *object_str);
  mess = g_string_new(NULL);

  /* parse the object string */
  object = rpsl_object_init(*object_str);

  if ( mess->str ) retstr = strdup(mess->str);
  g_string_free(mess, 1);
  LG_log(lg_ctx, LG_FUNC,"<up_clean_object: exiting with msg [%s] object [\n%s]",
                                       retstr ? retstr : "NULL", *object_str);
  return retstr;
}


/* Gets the source data for each source from the list of config data for multiple sources.
   Gets the server name for each source and returns the list of servers and sources.
   ******* An assumption was made here that the LU_whois_init call made the connection to
           the server. This code is over complicated to 'use the same connection' and avoid
           having multiple connections to the same server. In actual fact the init function
           only returns the name of the server and a connection is made each time a lookup is
           performed. The complication has been left in case it is needed in the future.
   *******
   Receives RT context
            LG context
            options structure
            pointer to server list
            pointer to source list
            pointer to unique server list
   Returns  none
            but sets up above lists.
*/

void UP_connect_all_servers(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                                 options_struct_t *options,
                                 GList **server_list, GList **source_list,
                                 GList **unique_server_list)
{
  int src_idx;
  int num_sources = 0;
  char *source;
  char *query_host;
  int query_port;
  ca_updDbSource_t **upd_source_hdl;
  LU_server_t *server = NULL;
  GList *host_list = NULL;
  GList *port_list = NULL;
  GList *host_item = NULL;
  GList *port_item = NULL;
  GList *svr_item = NULL;

  LG_log(lg_ctx, LG_FUNC,">UP_connect_all_servers: entered");

  /* retrieve source variables for the multiple sources */
  /* upd_source_hdl is a pointer to an array of pointers to source data
     held in the ca module */
  LG_log(lg_ctx, LG_INFO,"UP_connect_all_servers: get sources from config file");
  upd_source_hdl = ca_get_UpdSourceHandle(CA_UPDSOURCE);

  if (upd_source_hdl[0] == NULL)
  {
    LG_log(lg_ctx, LG_FATAL,
        "UP_connect_all_servers: There must be at least one updateable source in the config file. Exiting");
    UP_internal_error(rt_ctx, lg_ctx, options,
        "UP_connect_all_servers: There must be at least one updateable source in the config file. Exiting", 0);
  }

  /* get the number of sources found */
  num_sources = ca_get_UpdSourceNum();
  LG_log(lg_ctx, LG_DEBUG, "UP_connect_all_servers: number of sources read from config file[%d]", num_sources);

  for ( src_idx=0; src_idx<num_sources; src_idx++ )
  {
    source = strdup(upd_source_hdl[src_idx]->name);
    query_host = strdup(upd_source_hdl[src_idx]->whoisd_host);
    query_port = upd_source_hdl[src_idx]->qryPort;
    LG_log(lg_ctx, LG_FUNC,"UP_connect_all_servers: source [%s]", source);
    LG_log(lg_ctx, LG_FUNC,"UP_connect_all_servers: query_host [%s]", query_host);
    LG_log(lg_ctx, LG_FUNC,"UP_connect_all_servers: query_port [%d]", query_port);

    server = NULL;
    host_item = host_list;
    port_item = port_list;
    svr_item = *server_list;
    while ( host_item && port_item && svr_item  )
    {
      if ( ! strcmp(query_host, host_item->data) && (gpointer)query_port == port_item->data )
      {
        /* we have already connected to this host/port
           so use the same connection */
        server = svr_item->data;
        break;
      }
      host_item = host_item->next;
      port_item = port_item->next;
      svr_item = svr_item->next;
    }

    if ( ! server )
    {
      /* this is a new server so connect to it */
      server = LU_whois_init(query_host, query_port, UPDATE_QUERY_TIMEOUT);
      *unique_server_list = g_list_append(*unique_server_list, server);
    }

    *source_list = g_list_append(*source_list, source);
    *server_list = g_list_append(*server_list, server);
    host_list = g_list_append(host_list, query_host);
    port_list = g_list_append(port_list, (gpointer)query_port);
  }

  /* clean up */
  host_item = host_list;
  while ( host_item  )
  {
    free(host_item->data);
    host_item = host_item->next;
  }
  g_list_free(host_list);
  g_list_free(port_list);


  LG_log(lg_ctx, LG_FUNC,"<UP_connect_all_servers: exiting");
}



/* takes the source string from an object and sets up the current source
   data from the list of config data for multiple sources
   Receives RT context
            LG context
            options structure
            source string
            source data structure pointer
   Returns  UP_OK for success and UP_FAIL for source not found
            source data in structure
*/

int up_set_current_source_data(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                                 options_struct_t *options,
                                 char *obj_source, source_data_t *source_data)
{
  int src_idx;
  int found = UP_FAIL;
  int num_sources = 0;
  ca_updDbSource_t **upd_source_hdl;

  LG_log(lg_ctx, LG_FUNC,">up_set_current_source_data: entered source [%s]", obj_source);

  /* retrieve source variables for the multiple sources */
  /* upd_source_hdl is a pointer to an array of pointers to source data
     held in the ca module */
  LG_log(lg_ctx, LG_INFO,"up_set_current_source_data: get sources from config file");
  upd_source_hdl = ca_get_UpdSourceHandle(CA_UPDSOURCE);

  if (upd_source_hdl[0] == NULL)
  {
    LG_log(lg_ctx, LG_FATAL,
        "up_set_current_source_data: There must be at least one updateable source in the config file. Exiting");
    UP_internal_error(rt_ctx, lg_ctx, options,
        "up_set_current_source_data: There must be at least one updateable source in the config file. Exiting", 0);
  }

  num_sources = ca_get_UpdSourceNum();
  LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: number of sources read from config file[%d]", num_sources);

  for ( src_idx=0; src_idx<num_sources; src_idx++ )
  {
    if ( ! strcasecmp( obj_source, upd_source_hdl[src_idx]->name) )
    {
      /* found the source in the list */
      found = UP_OK;
      LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: source found");

      if ( source_data->current_source )
        free(source_data->current_source);
      source_data->current_source = strdup(upd_source_hdl[src_idx]->name);
      source_data->update_host = upd_source_hdl[src_idx]->whoisd_host;
      if ( source_data->query_host )
        free(source_data->query_host);
      source_data->query_host = strdup(source_data->update_host);
      source_data->update_port = upd_source_hdl[src_idx]->updPort;
      source_data->query_port = upd_source_hdl[src_idx]->qryPort;
      source_data->DBhost = upd_source_hdl[src_idx]->updDb.host;
      source_data->DBport = upd_source_hdl[src_idx]->updDb.port;
      source_data->DBname = upd_source_hdl[src_idx]->updDb.dbName;
      source_data->DBuser = upd_source_hdl[src_idx]->updDb.user;
      source_data->DBpasswd = upd_source_hdl[src_idx]->updDb.password;

      /* log the config variables for debugging */
      LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: CURRENT SOURCE is: [%s]",
                                     source_data->current_source);
      LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: UPDATE_HOST is: [%s]",
                                     source_data->update_host);
      LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: UPDATE_PORT is: [%d]",
                                     source_data->update_port);
      LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: QUERY_HOST is: [%s]",
                                     source_data->query_host);
      LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: QUERY_PORT is: [%d]",
                                     source_data->query_port);
      LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: DBhost is: [%s]",
                                     source_data->DBhost);
      LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: DBport is: [%d]",
                                     source_data->DBport);
      LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: DBname is: [%s]",
                                     source_data->DBname);
      LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: DBuser is: [%s]",
                                     source_data->DBuser);
      LG_log(lg_ctx, LG_DEBUG, "up_set_current_source_data: DBpasswd is: [%s]",
                                     source_data->DBpasswd);
      break;
    }
  }

  if ( found == UP_FAIL )
  {
    RT_invalid_source(rt_ctx);
    LG_log(lg_ctx, LG_ERROR,"up_set_current_source_data: source in object not recognised [%s]", obj_source);
  }

  LG_log(lg_ctx, LG_FUNC,"<up_set_current_source_data: exiting with value %s", UP_ret2str(found));
  return found;
}



/* gets the source from the object and sets up the current source values
   Receives RT context
            LG context
            options structure
            parsed object
            ptr to source string
            source data structure pointer
   Returns  UP_OK for success and UP_FAIL for source not found
            source string
            source data in structure
*/

int up_get_source(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                            options_struct_t *options,
                            rpsl_object_t *object, char **obj_source,
                            source_data_t *source_data)
{
  int retval;
  const char *obj_dirty_source = NULL;
  GList *attr_list = NULL;

  LG_log(lg_ctx, LG_FUNC,">up_get_source: entered ");

  /* find source from object */
  attr_list = rpsl_object_get_attr(object, "source");
  if ( attr_list )
  {
    *obj_source = rpsl_attr_get_clean_value((rpsl_attr_t *)(attr_list->data));
  }

  if ( ! attr_list || ! (*obj_source) )
  {
    /* should only happen if the object has no source: attribute or the attribute has no value,
       but the parser should reject that case */
    LG_log(lg_ctx, LG_ERROR,"up_get_source: no source found");
    retval = UP_FAIL;
  }
  else
  {
    /* check if this is filtered whois output */
    /* get the 'not' clean value, including eol comments */
    obj_dirty_source = rpsl_attr_get_value((rpsl_attr_t *)(attr_list->data));
    if ( strstr(obj_dirty_source, "Filtered") != NULL )
    {
      /* this is filtered output and cannot be used for updates */
      RT_filtered_source(rt_ctx);
      retval = UP_FAIL;
    }
    else
    {
      /* set up current source data values */
      retval = up_set_current_source_data(rt_ctx, lg_ctx, options, (*obj_source), source_data);
    }

    if ( retval != UP_OK )
    {
      free(*obj_source);
      *obj_source = NULL;
    }
    rpsl_attr_delete_list(attr_list);
  }

  LG_log(lg_ctx, LG_FUNC,"<up_get_source: exiting returning %s", UP_ret2str(retval));
  return retval;
}



/*  Function to rewrite a line of text with only one blankspace between each word.
    remove any tab characters.
   Receives destination of string
            source of string
   Returns  sets new string in destination
*/

void up_string_pack(char *dest, const char *source)
{

/*
 * This while loop continues until the NULL character is copied into
 * the destination string.  If a tab character is copied into the
 * destination string, it is replaced with a blank-space character.
 *
 * Multiple blank-space and/or tab characters are skipped in the source
 * string until any other character is found.
 */

  while (1)
  {
    /* copy the next character, but replace tab with space */
    *dest = (*source == '\t') ? ' ' : *source;

    /* Exit if have copied the end of the string. */
    if (*dest == '\0')
      return;

    ++dest;
    if ( (*source == ' ') || (*source == '\t') )
    {
      /* the character just copied was a space or tab */
      ++source;
      /* skip past any sebsequent space or tabs until we find any other character */
      while ( (*source == ' ') || (*source == '\t') )
        ++source;
    }
    else
      ++source;
  }
}



/* replaces the erase_str occurences with insert_str in str
   Receives string
            erase string
            replace string
   Returns  new string
*/

char * up_replace_strings(char *str, const char *erase_str, const char *insert_str)
{
  GString *g_str;
  int pos;
  char *result_str;

  /* if erase_str is NULL return a copy of input string */
  if (erase_str == NULL)
    return strdup(str);

  g_str = g_string_new(str);

  /* replace erase_str with insert_str */
  while (strstr(g_str->str, erase_str) != NULL)
  {
    pos = strstr(g_str->str, erase_str) - g_str->str;
    g_str = g_string_erase(g_str, pos, strlen(erase_str));
    if(insert_str != NULL)
      g_str = g_string_insert(g_str, pos, insert_str);
  }

  /* save the result string */
  result_str = strdup(g_str->str);

  /* free the GString structure (NULL means 'don't free the char string') */
  g_string_free(g_str, 0);

  return result_str;
}

/* used by g_list_foreach to delete the objects in a list
   Receives object pointer
            NULL
   Returns  none
*/

void up_rpsl_object_delete(gpointer data, gpointer user_data)
{
    rpsl_object_delete((rpsl_object_t *)data);
}


/* checks if two objects are identical or not.
    Takes two objects, one as char *, the other as
	a parsed object,

    Algorithm is very simple: All strings of tabs and
    white spaces are collapsed into a single white space,
    and then the strings are compared (strcmp)
   Receives LG context
            old object string
            parsed object
   Returns  1 for identical and 0 for different
*/

int up_identical(LG_context_t *lg_ctx, const char *old_version, rpsl_object_t *object)
{
  char *arg[2];
  int str_idx;
  rpsl_object_t *object2;
  rpsl_error_t error;
  int result = 0;
  char *temp_str[2];
  char *temp;

  LG_log(lg_ctx, LG_FUNC,">up_identical: entered");

  arg[0] = strdup(old_version);
  object2 = rpsl_object_copy(object);
  rpsl_object_remove_attr_name(object2, "delete", &error);
  rpsl_object_remove_attr_name(object2, "override", &error);
  arg[1] = rpsl_object_get_text(object2,RPSL_STD_COLUMN);
  rpsl_object_delete(object2);

  for ( str_idx=0; str_idx<2; str_idx++ )
  {
    arg[str_idx] = g_strstrip(arg[str_idx]);

    temp_str[str_idx] = (char *)malloc(strlen(arg[str_idx]) + 1);
    up_string_pack(temp_str[str_idx], arg[str_idx]);

    /* if there are still \r's at the end of strings, remove them */
    if ((temp_str[str_idx][strlen(temp_str[str_idx]) - 1]) == '\r')
      temp_str[str_idx][strlen(temp_str[str_idx]) - 1] = '\0';

    /* there may be white spaces at the end of the strings now, remove them */
    if ((temp_str[str_idx][strlen(temp_str[str_idx]) - 1]) == ' ')
      temp_str[str_idx][strlen(temp_str[str_idx]) - 1] = '\0';

    /* remove the white space just before the EOL (since this is not taken care of by
       the up_string_pack func) */
    temp = up_replace_strings(temp_str[str_idx], " \n", "\n");
    free(temp_str[str_idx]);
    temp_str[str_idx] = temp;
    free(arg[str_idx]);
  }

  LG_log(lg_ctx, LG_DEBUG,"up_identical: length of object [%d] old object [%d]",
                  strlen(temp_str[1]), strlen(temp_str[0]) );
  LG_log(lg_ctx, LG_DEBUG,"up_identical: compressed object [\n%s]\n\ncompressed old object [\n%s]",
                  temp_str[1], temp_str[0] );
  result = strcmp(temp_str[0], temp_str[1]);

  free(temp_str[0]);
  free(temp_str[1]);

  LG_log(lg_ctx, LG_FUNC,"<up_identical: exiting with result [%s]",  result ? "not identical" : "identical");
  return ! result;
}



/* gets the object and old object (if exists) and determines the operation to be carried out
   checks for some operation dependent error conditions
   Receives RT context
            LG context
            options structure
            parsed object
            object string
            old object string
            old object
            pointer to operation
   Returns  UP_OK for success
            UP_FAIL for error
            operation
*/

int up_determine_operation(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                            options_struct_t *options, rpsl_object_t *object,
                            char *old_object_str, rpsl_object_t *old_obj,
                            int *operation)
{
  int retval = UP_OK;
  const char *type;

  LG_log(lg_ctx, LG_FUNC,">up_determine_operation: entered");

  /* determine operation */
  if ( rpsl_object_is_deleted(object) )
    *operation = UP_DELETE;
  else if ( old_object_str )
    *operation = UP_MODIFY;
  else
    *operation = UP_CREATE;
  LG_log(lg_ctx, LG_DEBUG,"up_determine_operation: operation is [%s]",
                                      UP_op2str(*operation));

  /* check for operation dependent errors */
  type = rpsl_object_get_class(object);
  if ( *operation == UP_DELETE && ! old_object_str )
  {
    /* this is a delete operation but the object does not exist in the database */
    LG_log(lg_ctx, LG_ERROR,"up_determine_operation: delete operation, but object does not exist in the database");
    RT_non_existent_object(rt_ctx);
    retval = UP_FAIL;
  }
  else if ( *operation == UP_DELETE && ! up_identical(lg_ctx, old_object_str, object) )
  {
    /* this is a delete operation but the object is not identical to the one that
       exists in the database */
    LG_log(lg_ctx, LG_ERROR,"up_determine_operation: object for deletion is not identical to the one in the database");
    RT_versions_dont_match(rt_ctx, old_obj);
    retval = UP_FAIL;
  }
  else if ( *operation == UP_MODIFY && up_identical(lg_ctx, old_object_str, object) )
  {
    /* this is a modify operation but the object is identical to the one that
       exists in the database */
    LG_log(lg_ctx, LG_DEBUG,"up_determine_operation: object for modify is identical to the one in the database, no operation actioned");
    RT_versions_match(rt_ctx);
    *operation = UP_NOOP_OP;
    retval = UP_NOOP;
  }

  if ( options->enforced_new && old_object_str )
  {
    /* the enforced new keyword was specified
    but this object already exists in the database */
    LG_log(lg_ctx, LG_ERROR,"up_determine_operation: enforced new set, but object already exists in the database");
    RT_enforcednew_exists(rt_ctx);
    /* set operation to create to generate correct summary in ack message */
    *operation = UP_CREATE;
    retval = UP_FAIL;
  }

  LG_log(lg_ctx, LG_FUNC,"<up_determine_operation: exiting with value %s", UP_ret2str(retval));
  return retval;
}



/* Performs external checks
   Receives RT context
            LG context
            parsed object
   Returns  UP_OK for success
            UP_FAIL for error
*/

int up_external_checks(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                            rpsl_object_t *object, char *object_str )
{
  int retval = UP_OK;

  /* currently there are no external checks to be done */

  return retval;
}



/* Prepares the object for commiting to the database.
   Some of the checks may alter the data in the parsed object.
   All checks are performed, even if some fail, then error reports
   can be made to include multiple errors if necessary. So the
   return value retval must |= calls to various functions.
   Receives RT context
            LG context
            options structure
            key_info pointer
            parsed object
            operation
            auto_key pointer
            object source
            int handle_auto_keys (Handle AUTO keys if handle_auto_keys==1)
            reason pointer to policy check fail string
            list of credentials
   Returns  UP_OK if successful
            UP_FAIL, UP_FWD otherwise
*/

int up_pre_process_object(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                           options_struct_t *options, key_info_t *key_info,
                           rpsl_object_t *preproc_obj,int operation,
                           char *auto_key, char *obj_source, LU_server_t *server,
                           int handle_auto_keys, char **reason,
                           GList *credentials)
{
  int retval = UP_OK;
  int key_status;
  const char *type = NULL;
  int str_idx, ctry_idx;
  char *country_str = NULL;
  char *countries[COUNTRY_LIST_SIZE];
  char **temp_vector;
  char *value;
  KM_context_t key_cert_type;
  KM_key_return_t *key_data = NULL;
  GList *overlap = NULL;
  GList *attr = NULL;

  LG_log(lg_ctx, LG_FUNC,">up_pre_process_object: entered");

  /* Perform all checks to report any errors.
     All functions called return 0 (UP_OK) for success, non zero for error.
     All function calls must do an |= on return value.
     For delete operations we omit some check. As long as the object is identical
     we will allow the deletion to reduce the legacy objects with wrong formats. */

  /* key-cert checks must be done first as one of the checks is for X.509 key-cert creations
     must have a name of the type AUTO-nnn but later operations replace the AUTO-nnn
     in X.509 key-cert objects */
  type = rpsl_object_get_class(preproc_obj);
  if ( ! strcasecmp(type, "key-cert") )
  {
    /* Special processing needed for key-cert objects */
    /* Determine the type of key-cert object */
    /* Get a list with only one item */
    attr = rpsl_object_get_attr(preproc_obj, type);
    value = rpsl_attr_get_clean_value((rpsl_attr_t *)(attr->data));
    g_strdown(value);
    if ( strstr(value, "pgpkey-") != NULL)
    {
      key_cert_type = KM_PGP;
    }
    else
    {
      key_cert_type = KM_X509;
    }
    rpsl_attr_delete_list(attr);
    LG_log(lg_ctx, LG_DEBUG,"up_pre_process_object: key-cert type [%s]",
                               KM_context_string(key_cert_type));
    if ( operation != UP_DELETE )
    {
      if ( operation == UP_CREATE && key_cert_type == KM_X509 )
      {
        /* Check key-cert name, must be AUTO-nnn */
        if ( strstr(value, "auto-") != value )
        {
          LG_log(lg_ctx, LG_DEBUG,"up_pre_process_object: invalid name [%s] for new X.509 key-cert object",
                          value);
          RT_keycert_auto(rt_ctx);
          retval |= UP_FAIL;
        }
      }
      /* Create the generated attributes for a
         create or modify key-cert object */
      if ( retval == UP_OK )
      {
        retval |= UP_generate_keycert_attrs(rt_ctx, lg_ctx, key_info, preproc_obj, key_cert_type);
      }
    }
    else if ( key_cert_type == KM_PGP )
    {
      /* we still need the key_id for delete */
      key_status = UP_get_key_data(rt_ctx, lg_ctx, key_info, &key_data, preproc_obj, key_cert_type);
      KM_key_return_free(key_data);
      if ( key_status != KM_OK )
      {
        retval |= UP_FAIL;
      }
    }
    free (value);
  }

  if ( operation != UP_DELETE )
  {
    /* get country details from config file */
    LG_log(lg_ctx, LG_INFO,"up_pre_process_object: get country codes from config file");
    country_str = ca_get_country;
    /* construct countries array from country string variable */
    temp_vector = ut_g_strsplit_v1(country_str, "\n", 0);
    for (str_idx=0, ctry_idx=0; temp_vector[str_idx] != NULL; str_idx++)
    {
      temp_vector[str_idx] = g_strstrip(temp_vector[str_idx]);
      if (strlen(temp_vector[str_idx]) > 0)
	  {
        countries[ctry_idx] = strdup(temp_vector[str_idx]);
        g_strup(countries[ctry_idx++]);
      }
    }
    countries[ctry_idx] = NULL; /* mark the end of array */
    g_strfreev(temp_vector);
    free(country_str);
    LG_log(lg_ctx, LG_DEBUG,"up_pre_process_object: number of countries [%d]", ctry_idx);

    retval |= UP_check_country_attr(rt_ctx, lg_ctx, preproc_obj, countries);

    retval |= UP_check_nicsuffixes(rt_ctx, lg_ctx, options, preproc_obj, countries);

    /* free the countries list */
    for (ctry_idx=0; countries[ctry_idx] != NULL; ctry_idx++)
    {
      free(countries[ctry_idx]);
    }

    retval |= UP_check_changed_attr(rt_ctx, lg_ctx, preproc_obj);

    retval |= UP_check_filter_set_object(rt_ctx, lg_ctx, preproc_obj);

    retval |= UP_check_peering_set_object(rt_ctx, lg_ctx, preproc_obj);

    retval |= UP_check_disallowmnt(rt_ctx, lg_ctx, preproc_obj);

    retval |= UP_check_org_attr(rt_ctx, lg_ctx, preproc_obj);

    retval |= UP_check_organisation(rt_ctx, lg_ctx, preproc_obj, operation);

    /* check for references to AUTO- keys */
    if (handle_auto_keys)
    {
      if ( retval==UP_OK && UP_has_ref_to_AUTO_key(lg_ctx, preproc_obj) )
      {
        /* replace references to auto keys with the already assigned keys */
        retval |= UP_replace_refs_to_AUTO_key(rt_ctx, lg_ctx, options, preproc_obj, 1);
      }
    }
  }

  type = rpsl_object_get_class(preproc_obj);
  if ( (! strcasecmp(type, "inetnum")) && (operation == UP_CREATE))
  {
    /* check overlapping inetnums */
    if ( LU_check_overlap(server, &overlap, preproc_obj, obj_source) != LU_OKAY )
    {
      /* any lookup error is considered a fatal error */
      LG_log(lg_ctx, LG_FATAL,"up_pre_process_object: lookup error");
      UP_internal_error(rt_ctx, lg_ctx, options, "up_pre_process_object: lookup error\n", 0);
    }
    if (overlap != NULL)
	{
      RT_overlap_inetnums(rt_ctx, overlap);
      retval |= UP_FAIL;
    }
  }

  /* MAKE THE POLICY CHECK THE LAST ONE IN THIS FUNCTION */
  /* stuff removed */
  /***** ADD ANY NEW CHECKS BEFORE THE POLICY CHECK ABOVE ******/

  LG_log(lg_ctx, LG_FUNC,"<up_pre_process_object: exiting");
  return retval;
}



/* Performs transactions on any external data repositories
   Currently none
   Receives RT context
            LG context
            parsed object
            operation
   Returns  UP_OK if successful
            UP_FAIL otherwise
*/

int up_external_transact(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                         rpsl_object_t *preproc_obj,
                         int operation)
{
  int retval = UP_OK;

  LG_log(lg_ctx, LG_FUNC,">up_external_transact: entered with operation [%s]", UP_op2str(operation));
  LG_log(lg_ctx, LG_FUNC,"<up_external_transact: exiting");

  return retval;
}



/* Commits to any external data repositories
   Currently none.
   Receives RT context
            LG context
            options structure
            parsed object
   Returns  UP_OK if successful
            UP_FAIL otherwise
*/

int up_external_commit(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                           options_struct_t *options,
                           rpsl_object_t *preproc_obj)
{
  int retval = UP_OK;

  return retval;
}



/* Rollback transactions for any external data repositories
   Currently none.
   Receives RT context
            LG context
            options structure
            parsed object
   Returns  UP_OK if successful
            UP_FAIL otherwise
*/

int up_external_rollback(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                           options_struct_t *options,
                           rpsl_object_t *preproc_obj)
{
  int retval = UP_OK;

  return retval;
}



/* Obtains a transaction ID for an object to be sent to ripupd
   Receives RT context
            LG context
            options structure
            source data structure
   Returns  new transaction id
*/

long up_get_transaction_id(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                           options_struct_t *options, source_data_t *source_data)
{
  SQ_connection_t *sql_connection;
  SQ_result_set_t *result;
  int error;
  long new_id;

  LG_log(lg_ctx, LG_FUNC,">up_get_transaction_id: entered");

  sql_connection = SQ_get_connection(source_data->DBhost, source_data->DBport,
                                        source_data->DBname, source_data->DBuser,
                                        source_data->DBpasswd);
  if (!sql_connection)
  {
    /* No SQL connection is fatal */
    LG_log(lg_ctx, LG_FATAL,"up_get_transaction_id: No SQL connection");
    UP_internal_error(rt_ctx, lg_ctx, options, "up_get_transaction_id: ERROR No SQL connection\n", 0);
  }
  error = SQ_execute_query(sql_connection, "INSERT INTO tid VALUES(NULL)", &result);
  if (error)
  {
    /* query fail is fatal */
    LG_log(lg_ctx, LG_FATAL,"up_get_transaction_id: Execute query failed [%s]", SQ_error(sql_connection));
    UP_internal_error(rt_ctx, lg_ctx, options, "up_get_transaction_id: ERROR Execute query failed\n", 0);
  }

  new_id = mysql_insert_id(sql_connection);

  SQ_close_connection(sql_connection);

  LG_log(lg_ctx, LG_FUNC,"<up_get_transaction_id: exiting with new id [%ld]", new_id);
  return new_id;
}



/* Extracts the error no from the string returned from RIPupd.
   Receives RT context
            LG context
            options structure
            result string from ripupd
            pointer to conection error flag
   Returns  error number
*/

int up_get_error_num(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                        options_struct_t *options,
                        const char *string, int *connect_err)
{
  char error_no[10];
  char **temp = NULL;
  int line_idx, char_idx, num_idx;
  int err = 0;

  LG_log(lg_ctx, LG_FUNC,">up_get_error_num: entered, ripupd_result [%s]",
                                string ? string : "");

  /* if the string is NULL or empty, fatal error */
  if (string == NULL || strlen(string) == 0)
  {
    LG_log(lg_ctx, LG_FATAL,"up_get_error_num: ripupd_result string is empty");
    UP_internal_error(rt_ctx, lg_ctx, options, "up_get_error_num: NULL return from ripupdate\n", 0);
  }

  /* split the string into lines */
  temp = ut_g_strsplit_v1((char *)string , "\n", 0);
  num_idx = 0;
  for (line_idx = 0; temp[line_idx] != NULL; line_idx++)
  {
    if ( strstr(temp[line_idx], "%ERROR") == temp[line_idx]  )
    {
      /* Skip past %ERROR and look for the number following */
      LG_log(lg_ctx, LG_DEBUG,"up_get_error_num: error number line found\n%s", temp[line_idx]);
      for ( char_idx=strlen("%ERROR"); temp[line_idx][char_idx] != '\0' && num_idx<9; char_idx++ )
      {
        if ( isdigit((int)temp[line_idx][char_idx]) )
        {
          /* save the digits */
          error_no[num_idx++] = temp[line_idx][char_idx];
        }
        else if ( num_idx )
        {
          /* first non digit AFTER the number */
          break;
        }
        else
        {
          if ( temp[line_idx][char_idx] == ':' )
          {
            /* a : before the number means this is a conection type error */
            *connect_err = 1;
          }
        }
      }
    }
  }
  g_strfreev(temp);

  if ( ! num_idx )
  {
    /* we have not found an error number */
    LG_log(lg_ctx, LG_FATAL,"up_get_error_num: ripupd_result string has no error number");
    UP_internal_error(rt_ctx, lg_ctx, options, "up_get_error_num: ripupd_result string has no error number\n", 0);
  }

  error_no[num_idx] = '\0';
  LG_log(lg_ctx, LG_DEBUG,"up_get_error_num: error number string %s", error_no);
  err = atoi(error_no);

  LG_log(lg_ctx, LG_FUNC,"<up_get_error_num: exiting with error num %d", err);
  return err;
}

/* Interprets the result string coming from RIPupd
   Gets the error number. If not 0 (success) finds the error message(s) and passes them
   to RT without processing them.
   assigned_key is filled in if this is a person/role creation with AUTO nic hdl
   assigned_key must be allocated enough memory before up_interpret_ripudb_result is called
   If the caller does not expect a NIC hdl back, then assigned_key MUST be NULL

   Receives RT context
            LG context
            options structure
            result string from ripupd
            pointer to assigned_key
   Returns  UP_OK if successful
            UP_FAIL otherwise
            sets assigned_key
*/

int up_interpret_ripudb_result(RT_context_t *rt_ctx, LG_context_t *lg_ctx, options_struct_t *options,
        char *ripupd_result, char *assigned_key) {
    int retval = UP_OK;
    int err = 0;
    int line_idx;
    int connect_err = 0;
    char *pos = NULL;
    char *error_str = NULL;
    char **temp = NULL;
    char *temp_str = NULL;
    GString *mess;
    gchar **split_res;
    int split_num = 0;

    LG_log(lg_ctx, LG_FUNC, ">up_interpret_ripudb_result: entered, ripupd_result [%s]", ripupd_result);

    /* Get the error number from the return string.
     0 means no error in this context */
    err = up_get_error_num(rt_ctx, lg_ctx, options, ripupd_result, &connect_err);

    if (!err && assigned_key != NULL) {
        /* if the update was successful and the caller of the function expected
         to get an assigned key, get the key from the returned message */
        //    retval = UP_get_assigned_key(rt_ctx, lg_ctx, options, ripupd_result, assigned_key);
        retval = get_assigned_key(rt_ctx, lg_ctx, options, ripupd_result, assigned_key);
    } else if (err) {

        /* split string into something sensible & count the tokens */
        split_res = g_strsplit_set(ripupd_result, "[]:\"", 0);
        while (split_res[split_num])
            split_num++;

        /* According to the error no got from RIPupdate, construct an error string  */
        char temp_str[1024];
        switch (err) {
            case ERROR_U_MEM:
            case ERROR_U_DBS:
            case ERROR_U_BADOP:
            case ERROR_U_COP:
            case ERROR_U_NSUP:
            case ERROR_U_BUG:
                snprintf(temp_str, 1024, "***Error: Please contact database admin: Error no %i", err);
                LG_log(lg_ctx, LG_DEBUG, "up_interpret_ripudb_result: error string [%s]", temp_str);
                break;

            case ERROR_U_OBJ:
                /* if the object contains refs to unknown objects */
                if (strstr(ripupd_result, "dummy") != NULL) {
                    /* if the response from RIPupd contains "dummy not allowed" string */
                    snprintf(temp_str, 1024, "***Error: Unknown object referenced");
                } else if (strstr(ripupd_result, "reference cannot be resolved") != NULL) {
                    /* if the response from RIPupd contains a reference that cannot be resolved */
                    snprintf(temp_str, 1024, "***Error: Unknown object referenced %s", split_num > 4 ? split_res[4] : "");
                } else if (strstr(ripupd_result, "key-cert") != NULL) {
                    /* if the response from RIPupd contains "no key-cert object" string */
                    snprintf(temp_str, 1024, "***Error: Unknown key-cert object referenced");
                } else if (strstr(ripupd_result, "wrong prefix specified") != NULL) {
                    /* if the response from RIPupd contains "wrong prefix specified" string */
                    snprintf(temp_str, 1024, "***Error: Invalid prefix specified");
                } else if (strstr(ripupd_result, "Error with attribute") != NULL) {
                    /* if the response from RIPupd contains "Error with attribute" string */
                    snprintf(temp_str, 1024, "***Error: Error with attribute %s", split_num > 4 ? split_res[4] : "unknown");
                } else {
                    /* then, the object is referenced from other objects */
                    snprintf(temp_str, 1024, "***Error: Object is referenced from other objects");
                }
                LG_log(lg_ctx, LG_DEBUG, "up_interpret_ripudb_result: error string [%s]", temp_str);
                break;
            case ERROR_U_AUT:
                if (strstr(ripupd_result, "membership not allowed") != NULL) {
                    snprintf(temp_str, 1024, "***Error: Membership claim is not supported by mbrs-by-ref:\n"
                        "          attribute of the referenced set %s", split_num > 4 ? split_res[4] : "");
                }
                LG_log(lg_ctx, LG_DEBUG, "up_interpret_ripudb_result: error string [%s]", temp_str);
                break;
            default:
                snprintf(temp_str, 1024, "***Error: Please contact database admin: Error no %i", err);
                LG_log(lg_ctx, LG_DEBUG, "up_interpret_ripudb_result: error string [%s]", temp_str);
                break;
        }

        g_strfreev(split_res);
        RT_RIP_update_error(rt_ctx, temp_str);
        retval = UP_FAIL;
    }

    LG_log(lg_ctx, LG_FUNC, "<up_interpret_ripudb_result: exiting with value %s", UP_ret2str(retval));
    return retval;
}



/* Opens a socket to ripupd, sends the object,
   receives the reply and closes the socket.
   operation is the UP integer value. This is converted to the ripupdate string
   'ADD' ,'DEL' or 'UPD' by the macro UP_op2ripopstr(operation)
   Receives RT context
            LG context
            options structure
            source data structure
            parsed object
            operation
            pointer to result_string
   Returns  UP_OK if successful
            UP_FAIL otherwise
            sets result_string
*/

int up_send_object_db(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                           options_struct_t *options,
                           source_data_t *source_data,
                           rpsl_object_t *preproc_obj,
                           int operation, char **result_string )
{
  int retval = UP_OK;
  int sockfd, numbytes;
  char buf[DB_MAXDATASIZE + 1];
  char *to_be_sent = NULL;
  long tr_id;
  char *tr_id_str;
  sk_conn_st condat;
  rpsl_object_t *send_obj;
  rpsl_error_t return_error;

  LG_log(lg_ctx, LG_FUNC,">up_send_object_db: entered, operation [%s]", UP_op2str(operation));

  /* get the object text */
  if ( operation == UP_DELETE )
  {
    /* remove the DELETE: attribute */
    send_obj = rpsl_object_copy(preproc_obj);
    rpsl_object_remove_attr_name(send_obj,"delete",&return_error);
    to_be_sent = rpsl_object_get_text(send_obj,0);
    rpsl_object_delete(send_obj);
  }
  else
    to_be_sent = rpsl_object_get_text(preproc_obj,0);
  LG_log(lg_ctx, LG_DEBUG,"up_send_object_db: object to be sent [\n%s]", to_be_sent);

  /* get the transaction ID, to be sent to RIPupdate*/
  tr_id = up_get_transaction_id(rt_ctx, lg_ctx, options, source_data);
  /* convert it into a string */
  tr_id_str = (char *)malloc(64);
  sprintf(tr_id_str, "%ld", tr_id);
  LG_log(lg_ctx, LG_DEBUG,"up_send_object_db: transaction ID [%s]", tr_id_str);

  /* open a connection to ripupdate */
  retval = SK_connect(&sockfd, source_data->update_host,
                            source_data->update_port, RIP_TIMEOUT);
  if ( retval != SK_OK )
  {
    /* any error on connection to ripupdate is serious */
    LG_log(lg_ctx, LG_FATAL,"up_send_object_db: status return from socket connect [%d]", retval);
    UP_internal_error(rt_ctx, lg_ctx, options, "up_send_object_db: ERROR on socket connect to ripupdate\n", 0);
  }
  else
    retval = UP_OK;

  /* set up the connection data */
  SK_cd_make(&condat, sockfd, 0);

  /* send the data to ripupdate */
  if (SK_write(sockfd, UP_op2ripopstr(operation), strlen(UP_op2ripopstr(operation)), NULL, 0) == -1)
      goto UP_SEND_OBJECT_DB_ERROR;
  if (SK_write(sockfd, " ", strlen(" "), NULL, 0) == -1)
      goto UP_SEND_OBJECT_DB_ERROR;
  if (SK_write(sockfd, tr_id_str, strlen(tr_id_str), NULL, 0) == -1)
      goto UP_SEND_OBJECT_DB_ERROR;
  if (SK_write(sockfd, "\n\n" , 2, NULL, 0) == -1)
      goto UP_SEND_OBJECT_DB_ERROR;
  if (SK_write(sockfd, to_be_sent, strlen(to_be_sent), NULL, 0) == -1)
      goto UP_SEND_OBJECT_DB_ERROR;
  if (SK_write(sockfd, "\n\n", 2, NULL, 0)  == -1)
      goto UP_SEND_OBJECT_DB_ERROR;
  /* SK_write the ACK now */
  if (SK_write(sockfd, "ACK ",strlen("ACK "), NULL, 0)  == -1)
      goto UP_SEND_OBJECT_DB_ERROR;
  if (SK_write(sockfd, tr_id_str, strlen(tr_id_str), NULL, 0) == -1)
      goto UP_SEND_OBJECT_DB_ERROR;
  if (SK_write(sockfd, "\n\n", 2, NULL, 0)  == -1)
      goto UP_SEND_OBJECT_DB_ERROR;

  LG_log(lg_ctx, LG_INFO,"up_send_object_db: send complete");

  /* get the reply from ripupdate */
  while (( numbytes=SK_cd_gets(&condat, buf, DB_MAXDATASIZE) ) > 0)
  {
    buf[numbytes] = '\0';
    LG_log(lg_ctx, LG_DEBUG,"up_send_object_db: received from socket [\n%s]", buf);

    if (*result_string == NULL)
      *result_string = strdup(buf);
    else
    {
      *result_string = (char *)realloc(*result_string,
    				     strlen(*result_string) + strlen(buf) + 1);
      *result_string = strcat(*result_string, buf);
    }

    if (strstr(*result_string,"\n\n") != NULL)
    {
      /* if the result_string contains an empty line at the end, we will close */
      LG_log(lg_ctx, LG_DEBUG,"up_send_object_db: received blank line from socket, stop reading");
      break;
    };
  }

  SK_cd_free(&condat);
  SK_close(sockfd);
  free(to_be_sent);
  free(tr_id_str);

  if ( numbytes < 0 )
  {
    /* a read error from ripupdate is serious */
    LG_log(lg_ctx, LG_FATAL,"up_send_object_db: read error from ripupdate");
    LG_log(lg_ctx, LG_FATAL,"up_send_object_db: data received so far [%s]",
                             *result_string ? *result_string : "" );
    UP_internal_error(rt_ctx, lg_ctx, options, "up_send_object_db: ERROR reading from ripupdate\n", 0);
  }

  LG_log(lg_ctx, LG_FUNC,"<up_send_object_db: exiting with value %s", UP_ret2str(retval));
  return retval;

  /* write error exit */
  UP_SEND_OBJECT_DB_ERROR:
    /* a write error to ripupdate is serious */
    LG_log(lg_ctx, LG_FATAL,"up_send_object_db: write error to ripupdate");
    UP_internal_error(rt_ctx, lg_ctx, options, "up_send_object_db: ERROR writing to ripupdate\n", 0);
    return retval; /* just to remove warning */
}



/* Commits to source data base
   Receives RT context
            LG context
            options structure
            source data structure
            parsed object
            operation
            pointer to assigned_key
   Returns  UP_OK if successful
            UP_FAIL otherwise
*/

int up_source_commit(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                       options_struct_t *options,
                       source_data_t *source_data,
                       rpsl_object_t *preproc_obj,
                       int operation, char *assigned_key)
{
  int retval = UP_OK;
  char *ripupd_result = NULL;

  LG_log(lg_ctx, LG_FUNC,">up_source_commit: entered with operation [%s]", UP_op2str(operation));
  LG_log(lg_ctx, LG_FUNC,"up_source_commit: the object is [%s]", rpsl_object_get_text(preproc_obj,0));

  /* send the object to the database */
  retval = up_send_object_db(rt_ctx, lg_ctx, options, source_data, preproc_obj,
                                       operation, &ripupd_result);

  /* interpret the results returned by ripupdate */
  if ( retval == UP_OK )
    retval = up_interpret_ripudb_result(rt_ctx, lg_ctx, options,
                                             ripupd_result, assigned_key);

  if ( ripupd_result )
    free(ripupd_result);

  /* if the commit was not successful then rollback any external transactions */
  if ( retval != UP_OK )
  {
    LG_log(lg_ctx, LG_DEBUG,"up_source_commit: commit failed, do external rollback");
    up_external_rollback(rt_ctx, lg_ctx, options, preproc_obj);
  }
  else
  {
    /* otherwise commit the external transactions */
    LG_log(lg_ctx, LG_DEBUG,"up_source_commit: commit succeeded, do external commit");
    up_external_commit(rt_ctx, lg_ctx, options, preproc_obj);
  }

  LG_log(lg_ctx, LG_FUNC,"<up_source_commit: exiting with value [%s]", UP_ret2str(retval));
  return retval;
}



/* Make any final changes to the object for reporting
   Receives RT context
            LG context
            options structure
            preproc object
            old object
            auto_key (if it exists otherwise NULL)
            assigned nic (if it exists otherwise NULL)
            operation
            list of mntners used in auth
   Returns  UP_OK for success
            UP_FAIL for any error
*/

int up_post_process_object(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             options_struct_t *options, source_data_t *source_data,
                             rpsl_object_t *preproc_obj, rpsl_object_t *old_obj,
                             char *auto_key, char *assigned_key, int operation,
                             GList *mntner_used )
{
  int retval = UP_OK;
  rpsl_object_t *postproc_obj = NULL;

  LG_log(lg_ctx, LG_FUNC,">up_post_process_object: entered with assigned_key [%s]",
                            (assigned_key && strlen(assigned_key)) ? assigned_key : "not set");

  postproc_obj = rpsl_object_copy(preproc_obj);

  /* check if this object has an AUTO- key and replace it with the assigned one */
  if ( assigned_key && strlen(assigned_key) )
    retval = UP_put_assigned_key(rt_ctx, lg_ctx, options, auto_key, assigned_key, postproc_obj);

  RT_postproc_obj(rt_ctx, postproc_obj);

  /* processing of this object is completed */
  /* prepare notif messages */
  LG_log(lg_ctx, LG_DEBUG,"up_post_process_object: object processing complete, prepare notif messages");
  if ( operation == UP_DELETE )
    NT_write_all_ntfs(rt_ctx, lg_ctx, options, source_data, NULL, old_obj, mntner_used);
  else
    NT_write_all_ntfs(rt_ctx, lg_ctx, options, source_data, postproc_obj, old_obj, mntner_used);

  rpsl_object_delete(postproc_obj);
  LG_log(lg_ctx, LG_FUNC,"<up_post_process_object: exiting with value [%s]", UP_ret2str(retval));
  return retval;
}


/* Gets an object string from the input submission with it's associated credentials.
   Processes the object.
   This is a single object.
   The associated credentials may be all or a sub set of the total credentials presented
   which are in scope for the submission containing this object.
   goto statements are used when error conditions are found to jump to the
   cleanup section at the end to avoid too many nested if..else blocks.
   Receives RT context
            LG context
            options structure
            object string
            list of credentials
            int handle_auto_keys (check AUTO keys if handle_auto_keys==1)
   Returns  one of the UP return_codes
*/

int up_process_object(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                           options_struct_t *options, char *object_str,
                           GList *credentials, int handle_auto_keys)
{
  int retval = UP_OK;
  char *retstr = NULL;
  int au_retval;
  int operation = 0;
  int parsed_ok = 0;
  int inetnum_key_converted = 0;

  char *old_object_str = NULL;
  char *auto_key = NULL;
  char *assigned_key = NULL;
  char *obj_source = NULL;
  char *gpgcmd = NULL;
  char *opensslcmd = NULL;
  char *tmp_dir = NULL;
  const char *object_class;
  char *key_value = NULL;
  char *value;
  char *reason = NULL;
  rpsl_object_t *object = NULL;
  rpsl_object_t *old_object = NULL;
  rpsl_object_t *preproc_obj = NULL;
  LU_server_t *current_server = NULL;
  GList *mntner_used = NULL;
  GList *obj_source_list = NULL;
  GList *current_server_list = NULL;
  GList *attr = NULL;
  gboolean is_dot_removed;
  gchar *domain_key;

  source_data_t source_data = {NULL, NULL, NULL, 0, 0, NULL, 0, NULL, NULL, NULL, NULL};
  key_info_t    key_info    = {NULL, NULL};

  LG_log(lg_ctx, LG_FUNC,">up_process_object: entered with object [\n%s]", object_str);

  /* initialise the lookup module */
  LU_init(lg_ctx);

  /* remove trailing dot from domain, if any */
  is_dot_removed = ns_remove_trailing_dot(lg_ctx, &object_str);

  /* Clean out any unwanted data
     This must be done before parsing the object as it may
     remove some attributes. */
  retstr = up_clean_object(rt_ctx, lg_ctx, &object_str);

  /* parse the object string */
  object = rpsl_object_init(object_str);

  RT_set_object(rt_ctx, object);

  if ( retstr && strcmp(retstr, "") )  RT_clean_object(rt_ctx, retstr);
  if ( retstr ) free(retstr);

  if ( is_dot_removed )
  {
    domain_key = rpsl_object_get_key_value(object);
    RT_rdns_trailingdotremoved(rt_ctx,domain_key);
    free(domain_key);
    rpsl_object_delete(object);
    object = rpsl_object_init(object_str);
  }

  if ( rpsl_object_has_error(object, RPSL_ERRLVL_ERROR) )
  {
    LG_log(lg_ctx, LG_DEBUG,"up_process_object: object parsed with errors");
    RT_syntax_error(rt_ctx);
    parsed_ok = 0;
  }
  else
  {
    LG_log(lg_ctx, LG_DEBUG,"up_process_object: object parsed OK");
    RT_syntax_ok(rt_ctx);
    parsed_ok = 1;
  }

  /* if it did NOT parse OK and it is NOT a delete then this is an error.
     if it did not parse ok and it is a delete it MAY be ok if it is a
     legacy object */
  if ( ! parsed_ok && ! rpsl_object_is_deleted(object) )
  {
    LG_log(lg_ctx, LG_DEBUG,"up_process_object: parse errors and not deletion");
    retval = UP_FAIL;
    operation = UP_SYNTAX_ERR;
    goto up_process_object_exit;
  }

  object_class = rpsl_object_get_class(object);
  key_value = rpsl_object_get_key_value(object);

  /* If it is a delete of an object with syntax errors,
     check that the class attribute exists and does not have a null value
     and the primary key exists and is not null */
  if ( ! parsed_ok )
  {
    if ( key_value == NULL || ! strcmp(key_value, "") )
    {
      /* primary key attribute has no value, cannot continue with this object */
      LG_log(lg_ctx, LG_DEBUG,"up_process_object: deletion, no primary key attribute value");
      retval = UP_FAIL;
      operation = UP_SYNTAX_ERR;
    }
    else if ( object_class == NULL )
    {
      /* no object class attribute, cannot continue with this object */
      LG_log(lg_ctx, LG_DEBUG,"up_process_object: deletion, no class attribute");
      retval = UP_FAIL;
      operation = UP_SYNTAX_ERR;
    }
    else
    {
      attr = rpsl_object_get_attr(object, object_class);
      value = rpsl_attr_get_clean_value((rpsl_attr_t *)(attr->data));
      if ( ! strcmp(value, "") )
      {
        /* class attribute has no value, cannot continue with this object */
        LG_log(lg_ctx, LG_DEBUG,"up_process_object: deletion, no class attribute value");
        retval = UP_FAIL;
        operation = UP_SYNTAX_ERR;
      }
    }
  }

  if ( retval != UP_OK ) goto up_process_object_exit;

  /* Convert prefixes into ranges on inetnum objects. */
  if( object_class != NULL && ! strcasecmp(object_class, "inetnum") )
  {
    /*
       The slash notation expansion is performed on the original object submitted.
       This modified object becomes the 'new original' object.
       The modified object will be used in all further operations and reporting.
     */
    retval = up_convert_inetnum_prefix(rt_ctx, lg_ctx, object, &inetnum_key_converted);
  }

  if ( retval != UP_OK ) goto up_process_object_exit;

  if ( object_class != NULL && ! strcasecmp(object_class, "as-block")
         && up_check_as_block(rt_ctx, lg_ctx, key_value) != UP_OK )
  {
    retval = UP_FAIL;
    operation = UP_SYNTAX_ERR;
  }

  if ( retval != UP_OK ) goto up_process_object_exit;

  /* Normalise nserver attribute in domain objects: lowercase IP,
  *    * remove trailing dot in hostname. */
  if( object_class != NULL && ! strcasecmp(object_class, "domain") )
    {
      /*
         The cleanup is performed on the original object submitted.
         This modified object becomes the 'new original' object.
         The modified object will be used in all further operations and reporting.
       */
      retval = up_normalise_nserver(rt_ctx, lg_ctx, object);
    }

  if ( retval != UP_OK ) goto up_process_object_exit;

  /* find source from object and set current source */
  retval = up_get_source(rt_ctx, lg_ctx, options, object, &obj_source, &source_data);
  if ( retval != UP_OK ) goto up_process_object_exit;

  /* set up current server for lookups */
  current_server = LU_whois_init(source_data.query_host, source_data.query_port, UPDATE_QUERY_TIMEOUT);
  source_data.current_server = current_server;

  /* initialise key management library */
  gpgcmd = ca_get_gpgcmd;
//  opensslcmd = ca_get_opensslcmd;
  opensslcmd = strdup(OPENSSL_CMD);
  tmp_dir = ca_get_tmpdir;
  LG_log(lg_ctx, LG_INFO,"up_process_object: initialise key management with gpgcmd %s", gpgcmd ? gpgcmd : "NULL");
  LG_log(lg_ctx, LG_INFO,"up_process_object: initialise key management with opensslcmd %s", opensslcmd ? opensslcmd : "NULL");
  LG_log(lg_ctx, LG_INFO,"up_process_object: initialise key management with tmp_dir %s", tmp_dir ? tmp_dir : "NULL");
  obj_source_list = g_list_append(NULL, obj_source);
  current_server_list = g_list_append(NULL, current_server);
  KM_init(KM_PGP, rt_ctx, lg_ctx, current_server_list,  obj_source_list, tmp_dir, gpgcmd );
  KM_init(KM_X509, rt_ctx, lg_ctx, current_server_list,  obj_source_list, tmp_dir, opensslcmd );
  g_list_free(obj_source_list);

  /* get old object, if there is one */
  retval = LU_get_object(current_server, &old_object, object, obj_source);
  if ( retval != LU_OKAY )
  {
    /* any lookup error is considered a fatal error */
    LG_log(lg_ctx, LG_FATAL,"up_process_object: lookup error");
    UP_internal_error(rt_ctx, lg_ctx, options, "up_process_object: lookup error\n", 0);
  }

  if ( old_object )
  {
    old_object_str = rpsl_object_get_text(old_object, RPSL_STD_COLUMN);
  }

  /* determine the operation to be carried out on this object */
  retval = up_determine_operation(rt_ctx, lg_ctx, options, object,
                                   old_object_str, old_object, &operation);
  if ( retval != UP_OK ) goto up_process_object_exit;

  /* Check if we have converted a CIDR key into range earlier.
     This is only allowed for a creation. */
  if(inetnum_key_converted == 1 && operation != UP_CREATE)
  {
    RT_slash_not_allowed(rt_ctx);
    retval = UP_FAIL;
    goto up_process_object_exit;
  }

  preproc_obj = rpsl_object_copy(object);

  /* perform a set of pre-processing operations and checks on the object */
  /* set up storage location for the auto code sent to the core server  */
  auto_key = (char *)calloc(AUTO_KEY_LENGTH, sizeof(char));
  retval = up_pre_process_object(rt_ctx, lg_ctx, options, &key_info, preproc_obj,
                                    operation, auto_key, obj_source, current_server,
                                    handle_auto_keys, &reason, credentials);

  if ( retval != UP_OK )
  {
    LG_log(lg_ctx, LG_DEBUG,"up_process_object: pre-processing failed");
    if ( retval & UP_FWD )
    {
      LG_log(lg_ctx, LG_FATAL,"up_process_object: policy checks failed");
      LG_log(lg_ctx, LG_FATAL,"up_process_object: %s", reason);
      /* send message to hostmaster */
      NT_forw_policy_fail(rt_ctx, lg_ctx, options, op2fwd_op_str(operation),
                             object, reason, credentials);
      retval = UP_FWD;
      operation = UP_FWD_OP_POLICY;
      goto up_process_object_exit;   /* finished processing this object */
    }
    /* else check auth before reporting pre processing errors */
  }

  /* auth checks */
  LG_log(lg_ctx, LG_DEBUG,"up_process_object: checking for auth errors");
  AU_set_lookup(current_server);
  au_retval = AU_check_auth(preproc_obj, credentials, op2au_op(operation),
                            rt_ctx, &mntner_used);

  if (au_retval != AU_AUTHORISED)
  {
    LG_log(lg_ctx, LG_DEBUG,"up_process_object: check auth returned %s", AU_ret2str(au_retval));
    if ( au_retval == AU_FWD )
    {
      /* This was an irt (RIPE) or as-block creation request with no override.
         If we are not in test mode and no pre-processing errors were found,
         forward it to <HUMAILBOX>  */
      if ( retval == UP_OK && ! options->test_mode )
      {
        LG_log(lg_ctx, LG_DEBUG,"up_process_object: send forward create message");
        NT_forw_create_req(rt_ctx, lg_ctx, options, op2fwd_op_str(operation),
                              object, credentials);
        retval = UP_FWD;
        operation = op2fwd_op(operation);
      }
    }
    /* otherwise auth failed */
    else if ( au_retval == AU_UNAUTHORISED_CONT )
    {
      /* The auth failed on this object, but we are able to ack this object
         and safely continue to process other objects in this update message */
      /* Prepare forward messages */
      LG_log(lg_ctx, LG_DEBUG,"up_process_object: prepare forward messages");
      if ( operation == UP_DELETE )
      {
        NT_write_all_frwds(rt_ctx, lg_ctx, options, &source_data, NULL, old_object, mntner_used);
      }
      else
      {
        NT_write_all_frwds(rt_ctx, lg_ctx, options, &source_data, object, old_object, mntner_used);
      }
      retval = UP_FAIL;
    }
    else
    {
      /* The auth failed in a way that is not possible to continue to
         process any other objects in this message.
         Report a fatal error */
      retval = UP_FATAL;
      LG_log(lg_ctx, LG_FATAL,"up_process_object: AU_check_auth fatal error");
      if (mntner_used)
      {
        g_list_foreach(mntner_used, up_rpsl_object_delete, NULL);
        g_list_free(mntner_used);
      }
      UP_internal_error(rt_ctx, lg_ctx, options, "up_process_object: AU_check_auth fatal error\n", 0);
    }
  }
  /* if either pre-processing or auth failed, or processing is complete (UP_FWD) */
  if ( retval != UP_OK )
    goto up_process_object_exit;

  if (UP_has_AUTO_key(lg_ctx, preproc_obj) ){
      /* the object has an AUTO- nic hdl */
      LG_log(lg_ctx, LG_DEBUG,"up_process_object: object contains AUTO- nic handles");

      /* replace auto nic handle with special code for ripupdate */
      retval = UP_replace_AUTO_key(rt_ctx, lg_ctx, options, preproc_obj,
                                           auto_key, obj_source);

  }

  /* there was a problem with AUTO-nic replacement */
  if ( retval != UP_OK ) goto up_process_object_exit;

  /* external checks */
  retval = up_external_checks(rt_ctx, lg_ctx, preproc_obj, object_str);
  if ( retval != UP_OK ) goto up_process_object_exit;

  /* if an AUTO- key has been found set up storage location
     for the actual key to be created by the core server */
  if ( strlen(auto_key) )
  {
    assigned_key = (char *)calloc(AUTO_KEY_LENGTH, sizeof(char));
  }

  /* perform transactions to external data repositories */
  retval = up_external_transact(rt_ctx, lg_ctx, preproc_obj, operation);
  if ( retval != UP_OK ) goto up_process_object_exit;

  /* Commit to source database */
  retval = up_source_commit(rt_ctx, lg_ctx, options, &source_data, preproc_obj,
                                     operation, assigned_key);
  if ( retval != UP_OK ) goto up_process_object_exit;

  /* post process */
  retval =  up_post_process_object(rt_ctx, lg_ctx, options, &source_data,
                                     preproc_obj, old_object, auto_key, assigned_key,
                                     operation, mntner_used );

  /* tidyup before returning retval */
  up_process_object_exit:

    /* keep a tally */
    if (retval == UP_OK || retval==UP_FWD) options->count_successful++;
	else if (retval == UP_NOOP)  options->count_noop++;
    else  options->count_unsuccessful++;

    RT_unset_object(rt_ctx, op2rt_upd_op(operation),
                      (retval==UP_OK || retval==UP_NOOP || retval==UP_FWD) ? 1 : 0 );
    rpsl_object_delete(object);
    /* cleanup LU module */
    if ( current_server_list )
    {
      g_list_free(current_server_list);
    }
    if ( current_server )
    {
      LU_cleanup(current_server);
    }
    if ( old_object )
    {
      rpsl_object_delete(old_object);
    }
    if ( old_object_str )
    {
      free(old_object_str);
    }
    if ( preproc_obj )
    {
      rpsl_object_delete(preproc_obj);
    }
    if ( auto_key )
    {
      free(auto_key);
    }
    if ( assigned_key )
    {
      free(assigned_key);
    }
    if ( obj_source )
    {
      free(obj_source);
    }
    if ( key_info.key )
    {
      free(key_info.key);
    }
    if ( key_info.key_id )
    {
      free(key_info.key_id);
    }
    if ( source_data.current_source )
    {
      free(source_data.current_source);
    }
    if ( source_data.query_host )
    {
      free(source_data.query_host);
    }
    if (mntner_used)
    {
      g_list_foreach(mntner_used, up_rpsl_object_delete, NULL);
      g_list_free(mntner_used);
    }
    if ( gpgcmd )
    {
      free(gpgcmd);
    }
    if ( opensslcmd )
    {
      free(opensslcmd);
    }
    if ( key_value )
    {
      free(key_value);
    }

    LG_log(lg_ctx, LG_FUNC,"<up_process_object: exiting with value %s", UP_ret2str(retval));
    return retval;
}


/* Gets a submission from the input with it's associated credentials.
   A submission is one 'blob' of data from the input stream after
   un-wrapping the mime parts.
   The associated credentials may be all or a sub set of the total credentials
   presented which are in scope for this submission.
   Receives RT context
            LG context
            options structure
            submission
   Returns  one of the UP return_codes
*/

int UP_process_submission(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                           options_struct_t *options,
                           ep_authenticated_data_t *submission )
{
  int retval = UP_OK;
  char **split_lines = NULL;
  char *line = NULL;
  char *object_str = NULL;
  gchar **object_strings;
  int i;
  int line_idx, line_cnt;
  int valid_first_line = 0;
  int source_line_found = 0;
  int in_object = 0;
  int not_object = 0;
  char *new_data_str = NULL;
  const gchar *data_str = NULL;
  const GList *error_list = NULL;
  const GList *error_list_item = NULL;
  gint elevel, ecode;
  gchar *edescr = NULL;
  GList *credentials;
  GSList *list_of_AUTO_refd_objects = NULL;   /* for objects referencing an AUTO key */
  GSList *second_object_list = NULL; /* needed when dealing with references to AUTO keys */
  GSList *item;
  rpsl_object_t *object = NULL;
  guint previous_number_of_objects;
  guint current_number_of_objects;
  gint range_start; /*required for decomposition */
  gint range_end; /* required for decomposition */

  LG_log(lg_ctx, LG_FUNC,">UP_process_submission: entered");

  /* get the data and credentials from this submission */
  LG_log(lg_ctx, LG_INFO, "UP_process_submission: get submission data");
  data_str = EP_get_blob(submission);
  LG_log(lg_ctx, LG_DEBUG, "UP_process_submission: submission data [\n%s]", data_str);
  LG_log(lg_ctx, LG_INFO, "UP_process_submission: get submission credentials");
  credentials = EP_get_credentials(submission);
  /* add an extra blank line to the end of the data,
     just in case the last object was not followed by a blank line */
  new_data_str = strdup(data_str);
  new_data_str = realloc(new_data_str, strlen(new_data_str) + 2);
  strcat(new_data_str, "\n");

  /* split the new_data_str on \n */
  split_lines = ut_g_strsplit_v1(new_data_str, "\n", 0);

  /* process the lines in the submission */
  LG_log(lg_ctx, LG_INFO, "UP_process_submission: start to process the submission data");
  for (line_idx=0; split_lines[line_idx] != NULL; line_idx++)
  {
    line = split_lines[line_idx];  /* just to make the next few lines easier to read */
    /* remove '\n's and '\r' first */
    line = UP_remove_EOLs(line);
    /* remove trailing white space */
    line = g_strchomp(line);

    /* build up an object string */
    /* Whilst doing this check if it looks like an object --
       look for the first line starting with an alphabetic char and containing a colon before a white space
       look for a line starting with 'source:', but not the first line */

    if (strlen(line) != 0)
    {
      /* add this line to the current object string */
      if (object_str == NULL)
      {
        /* this is the first line of the current object string */
        /* check if it starts with an alphabetic char and
           contains a colon (:) before any white spaces */
        if ( (*line >= 'a' && *line <= 'z') || (*line >= 'A' && *line <= 'Z') )
        {
          for ( line_cnt=0; line_cnt < strlen(line); line_cnt++ )
          {
            if ( line[line_cnt] == ' ' || line[line_cnt] == '\t'
                   || line[line_cnt] == '\r' )
              break;
            if ( line[line_cnt] == ':' )
            {
              LG_log(lg_ctx, LG_DEBUG, "UP_process_submission: valid_first_line = 1");
              valid_first_line = 1;
              break;
            }
          }
        }
        in_object = 1;
        object_str = (char *)malloc(strlen(line) + 2);
        strcpy(object_str, line);
        strcat(object_str, "\n"); /* add EOL again (we removed it before) */
      }
      else
      {
        object_str = (char *)realloc(object_str, strlen(object_str) + strlen(line) + 2);
        strcat(object_str, line);
        strcat(object_str, "\n"); /* add EOL again (we removed it before) */
		g_strdown(line);
        if ( strstr(line, "source:") == line )
        {
          LG_log(lg_ctx, LG_DEBUG, "UP_process_submission: source_line_found = 1");
          source_line_found = 1;
        }
      }
    }
    else
    {
      /* blank line found, end of object */
      if ( in_object )      /* allow for multiple blank lines */
      {
// if ( exit_cnt++ == 2 ) UP_internal_error(rt_ctx, lg_ctx, options, "test exit\n", 0);
        LG_log(lg_ctx, LG_INFO, "UP_process_submission: end of object");
        LG_log(lg_ctx, LG_DEBUG, "UP_process_submission: object string [\n%s]", object_str);
        /* has it passed the test for looking like a real object */
        if ( valid_first_line && source_line_found )
        {
          not_object = 0;
          LG_log(lg_ctx, LG_DEBUG, "UP_process_submission: looks like an object");
          /* parse the object string */
	      object = rpsl_object_init(object_str);
          error_list = rpsl_object_errors(object);
          if (error_list)
          {
            LG_log(lg_ctx, LG_DEBUG,"UP_process_submission: errors found on initial object parse");
            for ( error_list_item = error_list; error_list_item != NULL; error_list_item = g_list_next(error_list_item) )
            {
              elevel = ((rpsl_error_t *)(error_list_item->data))->level;
              ecode = ((rpsl_error_t *)(error_list_item->data))->code;
              edescr = strdup(((rpsl_error_t *)(error_list_item->data))->descr);

              LG_log(lg_ctx, LG_DEBUG,"UP_process_submission: level [%d]  code [%d]  [%s]", elevel, ecode, edescr);
              if ( ecode == RPSL_ERR_ONLYCOMMENTS || ecode == RPSL_ERR_BADCLASS
                        ||ecode == RPSL_ERR_UNKNOWNCLASS )
              {
                /* does not look like an object after all */
                not_object = 1;
                LG_log(lg_ctx, LG_DEBUG,"UP_process_submission: This does not look like an object after all");
              }
              free(edescr);
            }
          }

          if ( not_object )
          {
            /* this does not look like an object */
            LG_log(lg_ctx, LG_DEBUG, "UP_process_submission: unparsable input");
            RT_unparsable_input(rt_ctx, object_str);
          }
          else
          {
            LG_log(lg_ctx, LG_DEBUG, "UP_process_submission: recognised object");
            if ( ! rpsl_object_has_error(object, RPSL_ERRLVL_ERROR) && UP_has_ref_to_AUTO_key(lg_ctx, object) )
            {
		      /* object has a reference to an auto- key and no syntax errors
		         so defer the processing to allow the auto- key to be created first */
              /* put this object string on the list of objects referencing AUTO keys */
              LG_log(lg_ctx, LG_DEBUG, "UP_process_submission: put object on list_of_AUTO_refd_objects");
              list_of_AUTO_refd_objects = g_slist_append(list_of_AUTO_refd_objects, strdup(object_str));
            }
            else
            {
              /* check if decomposition is needed */
              LG_log(lg_ctx,LG_DEBUG,
                  "UP_process_submission: checking if the object should be decomposed");
              if (ns_is_decompose(lg_ctx,object_str,&range_start,&range_end))
              {
                LG_log(lg_ctx,LG_DEBUG,
                    "UP_process_submission: object will be decomposed");
                object_strings=ns_decompose_object(lg_ctx,rt_ctx,object_str,
                                                   range_start,range_end);
              }
              else
              {
                object_strings=g_new(gchar *,3);
                object_strings[0]=object_str;
                object_strings[1]=NULL;
              }

              if (object_strings==NULL)
              {
                RT_unparsable_input(rt_ctx, object_str);
                object_strings=g_new(gchar *,2);
                object_strings[0]=NULL;
                retval |= UP_FAIL;
              }
              /* process this object
                 OR the return value so we can keep track of any failures */
              i=0;
              while (object_strings[i]!=NULL)
              {
                retval |= up_process_object(rt_ctx, lg_ctx,
                                            options,
                                            object_strings[i], credentials, 0);
                i++;
                /* do not continue to process objects after a fatal error */
                if (retval & UP_FATAL)
                {
                  retval = UP_FATAL;  /* so that UP_ret2str macro works */
                  /* free up memory */
                  if ( list_of_AUTO_refd_objects )
                  {
                    for ( item=list_of_AUTO_refd_objects; item != NULL; item=g_slist_next(item) )
                      free((char *)(item->data));
                    g_slist_free(list_of_AUTO_refd_objects);
                  }
                  free(object_str);
                  g_strfreev(split_lines);
                  free(new_data_str);
                  LG_log(lg_ctx, LG_FUNC,"<UP_process_submission: exiting with value %s", UP_ret2str(retval));
                  return retval;
                }
              }

              if (object_strings[i]!=NULL)
              {
                g_strfreev(object_strings);
              }
            }
          }
        }
        else
        {
          /* this does not look like an object */
          LG_log(lg_ctx, LG_DEBUG, "UP_process_submission: does not look like an object");
          RT_unparsable_input(rt_ctx, object_str);
        }

        /* reset flags */
        free(object_str);
        object_str = NULL;
        valid_first_line = 0;
        source_line_found = 0;
        in_object = 0;
      }
    }
  }
  /* end of submission */
  LG_log(lg_ctx, LG_INFO, "UP_process_submission: end of submission");
  g_strfreev(split_lines);
  free(new_data_str);

  /* now process the list of objects referencing AUTO keys */
  if ( list_of_AUTO_refd_objects )
  {
    LG_log(lg_ctx, LG_DEBUG, "UP_process_submission: process list_of_AUTO_refd_objects");
    second_object_list = NULL;
    current_number_of_objects = g_slist_length(list_of_AUTO_refd_objects);

    /* Set previous_number_of_objects one higher than current_number_of_objects
       to guarantee the first iteration in the while loop */
    previous_number_of_objects = current_number_of_objects + 1;

    while ( previous_number_of_objects > current_number_of_objects )
    {
      for ( item=list_of_AUTO_refd_objects; item != NULL; item=g_slist_next(item) )
      {
        object_str = (char *)(item->data);
        LG_log(lg_ctx, LG_DEBUG, "UP_process_submission: object string [\n%s]", object_str);
        /* Now, replace references to AUTO keys, and then if there's no reference to AUTO
           keys left, process the object.  If there are still references, then put that
           into another list */
        UP_replace_refs_to_AUTO_key_from_string(rt_ctx, lg_ctx, options, &object_str, 0);
        if ( UP_has_ref_to_AUTO_key_from_string(lg_ctx, object_str) == 0 )
        {
          /* process this object
             OR the return value so that we can keep track of any failures */
          retval |= up_process_object(rt_ctx, lg_ctx, options,
                                       object_str, credentials, 0);

          /* do not continue to process objects after a fatal error */
          if (retval & UP_FATAL)
          {
            retval = UP_FATAL;  /* so that UP_ret2str macro works */
            /* free up memory */
            for ( item=item; item != NULL; item=g_slist_next(item) )
            {
              free((char *)(item->data));
            }
            g_slist_free(list_of_AUTO_refd_objects);
            LG_log(lg_ctx, LG_FUNC,"<UP_process_submission: exiting with value %s", UP_ret2str(retval));
            return retval;
          }

          free(object_str);
        }
        else
        {
          /* There are still references to AUTO keys */
          second_object_list = g_slist_append(second_object_list, strdup(object_str));
        }
      }

      previous_number_of_objects = current_number_of_objects;
      current_number_of_objects = g_slist_length(second_object_list);
      /* free up memory */
      g_slist_free(list_of_AUTO_refd_objects);
      list_of_AUTO_refd_objects = second_object_list;
      second_object_list = NULL;
    }

    /* Now, if we still have objects to be processed, go ahead and process them.
      They certainly have unresolved AUTO keys, but we need to return relevant errors
      to the user */
    if ( list_of_AUTO_refd_objects )
    {
      RT_circular_dependency(rt_ctx);
      for ( item=list_of_AUTO_refd_objects; item != NULL; item=g_slist_next(item) )
      {
        object_str = (char *)(item->data);
        retval |= up_process_object(rt_ctx, lg_ctx, options,
                                     object_str, credentials, 1);

        /* do not continue to process objects after a fatal error */
        if (retval & UP_FATAL)
        {
          retval = UP_FATAL;  /* so that UP_ret2str macro works */
          /* free up memory */
          for ( item=item; item != NULL; item=g_slist_next(item) )
          {
            free((char *)(item->data));
          }
          g_slist_free(list_of_AUTO_refd_objects);
          LG_log(lg_ctx, LG_FUNC,"<UP_process_submission: exiting with value %s", UP_ret2str(retval));
          return retval;
        }
      }
    }
  }

  LG_log(lg_ctx, LG_FUNC,"<UP_process_submission: exiting with value %s", UP_ret2str(retval));
  return retval;
}




