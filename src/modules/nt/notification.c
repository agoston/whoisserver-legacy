/***************************************
  $Revision: 1.1 $

  NT (Notifications) module

  Status: REVIEWED, NOT TESTED

  Author(s):    Denis Walker  ( re-write of functions by Engin Gunduz )

  ******************/ /******************
  Modification History:
        engin (06/07/2000) Created.
		denis (25/09/2001) Modified for new API
        denis (22/11/2002) partial re-write for re-structured dbupdate
  ******************/ /******************
  Copyright (c) 2000                              RIPE NCC
 
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



#include "rip.h"


/* Generates a unique file name and returns the full path of the filename 
   for storing the out going message to direct to sendmail.
   Creates the file and writes the outgoing email message to it. 
   May use PID or time or both to ensure uniqueness.
   Receives RT context
            LG context
            options structure
            To: mail address
            full email message including header
            redirect appendage for file name or NULL
   Returns  file path/name
*/
      
char *nt_filename_generate( RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                            options_struct_t *options, char *e_mail, 
                            char *out_mess, char *redirect )
{
  char *name = NULL;
  char *tmpdir = NULL;
  char *cleaned_email = NULL;
  int oloop, iloop;
  FILE *ntfy_file;
  
  LG_log(lg_ctx, LG_FUNC,">nt_filename_generate: entered, e-mail [%s] redirect [%s]\n", 
                              e_mail, redirect ? redirect : "NULL" );
       
  /* get the temp dir name */
  tmpdir = ca_get_tmpdir;
  tmpdir = g_strstrip(tmpdir);
  LG_log(lg_ctx, LG_DEBUG,"nt_filename_generate: tmpdir [%s]", tmpdir);

  /* clean up the email address, remove any : or / characters */
  cleaned_email = strdup(e_mail);
  iloop = 0;
  for (oloop=0; oloop<=strlen(e_mail); oloop++)  /* don't forget the \0 */
  {
    if ( e_mail[oloop] != ':' && e_mail[oloop] != '/' )
    {
      cleaned_email[iloop++] = e_mail[oloop];
    }
  }
  LG_log(lg_ctx, LG_DEBUG,"nt_filename_generate: cleaned_email [%s]", cleaned_email);

  /* allocate space for name.  32 should be enough for PID */
  if ( redirect )
  {
    name = (char*)malloc(strlen(tmpdir) + strlen(cleaned_email) + strlen("notify")
                           + strlen(redirect) +37 ); 
    sprintf(name, "%s/%s-%s.%i.%s", tmpdir, "notify", cleaned_email, (int)(getpid()), redirect );
  }
  else
  {
    name = (char*)malloc(strlen(tmpdir) + strlen(cleaned_email) + strlen("notify") +36 ); 
    sprintf(name, "%s/%s-%s.%i", tmpdir, "notify", cleaned_email, (int)(getpid()) );
  }
  free(tmpdir);
  free(cleaned_email);
  LG_log(lg_ctx, LG_DEBUG,"nt_filename_generate: file name [%s]", name);

  /* create the file */
  if (( ntfy_file = fopen(name, "w")) == NULL)
  {
    LG_log(lg_ctx, LG_FATAL,"nt_filename_generate: Can't open temp file for creating, %s", name);
    UP_internal_error(rt_ctx, lg_ctx, options, "nt_filename_generate: Can't open temp file for creating,\n",1);
  }

  /* write the out going message to the file */
  fprintf(ntfy_file, "%s", out_mess);

  /* close it */
  fclose(ntfy_file);

  LG_log(lg_ctx, LG_FUNC,"<nt_filename_generate: exiting\n");
  return name;
}



/* Generates a new RT context.
   Writes some initial information to this context.
   Receives LG context
            options structure
            email address for To field of outgoing email
            subject string for Subject field of outgoing email
   Returns  new RT context
*/
      
RT_context_t *nt_generate_new_context(LG_context_t *lg_ctx, 
                                       options_struct_t *options, 
                                       char *e_mail, char *subject)
{
  char *from_address = NULL;
  RT_context_t *new_rt_ctx;

  LG_log(lg_ctx, LG_FUNC,">nt_generate_new_context: entered with email [%s] subject [%s]\n", 
                               e_mail, subject ? subject : "NULL");
   
  /* create a new RT context */
  new_rt_ctx = RT_start();
  
  /* write outgoing mail header info to this ctx */
  from_address = g_strstrip(ca_get_humailbox);
//  RT_header(new_rt_ctx, e_mail, from_address, options->mail_hdr_data.subject);
  RT_header_to(new_rt_ctx, e_mail);
  RT_header_from(new_rt_ctx, from_address);
  RT_header_subject(new_rt_ctx, subject ? subject : options->mail_hdr_data.subject);
  free(from_address);
  RT_process_time(new_rt_ctx, options->time_str);
  
  if (options->mail_input)
  {
    /* write the details of the mail causing these changes to this ctx */
    RT_from_mail(new_rt_ctx, &(options->mail_hdr_data));
  }

  if (options->origin)
  {
    /* write the details of the origin causing these changes to this ctx */
    RT_notif_origin(new_rt_ctx, options->origin);
  }

  LG_log(lg_ctx, LG_FUNC,"<nt_generate_new_context: exiting\n");
  return new_rt_ctx;
}



/* Adds the e-mails in a linked list to the hash, generating appropriate RT contexts
   Receives LG context
            options structure
            email address list
            hash pointer (ntfy_hash or forw_hash)
   Returns  none
*/

void nt_add_list_to_hash(LG_context_t *lg_ctx, options_struct_t *options,
                             GList *e_mail_list, GHashTable *hash)
{
  char *e_mail;
  GList *temp = NULL;

  LG_log(lg_ctx, LG_FUNC,">nt_add_list_to_hash: entered\n");

  for (temp = e_mail_list; temp != NULL; temp = g_list_next(temp))
  {
    e_mail = (char *)(temp->data);

    if ( g_hash_table_lookup(hash, e_mail) == NULL )
    { 
      /* there is no such entry, so create it */
      LG_log(lg_ctx, LG_DEBUG,"nt_add_list_to_hash: email %s not in hash, create new context", e_mail);
      g_hash_table_insert(hash, strdup(e_mail), 
                             nt_generate_new_context(lg_ctx, options, e_mail, options->mail_hdr_data.subject));
    }
    else
      LG_log(lg_ctx, LG_DEBUG,"nt_add_list_to_hash: email %s already in hash", e_mail);
  }
  LG_log(lg_ctx, LG_FUNC,"<nt_add_list_to_hash: exiting\n");
}



/* Adds the message to the new context (taken from the hash) 
   for each email address in the list, then free the email address.
   Receives LG context
            list of email addresses
            hash pointer
            message
   Returns  none
*/

void nt_add_msg_to_list(LG_context_t *lg_ctx, GList *list, 
                                GHashTable *hash, char *msg)
{
  GList *email_item = NULL;
  RT_context_t *new_rt_ctx;

  LG_log(lg_ctx, LG_FUNC,">nt_add_msg_to_list: entered with message [\n%s]\n", msg);

  /* loop through the list of email addresses */
  for(email_item = list; email_item != NULL; email_item = g_list_next(email_item))
  {
    /* get the context pointer from the email address */
    new_rt_ctx = (RT_context_t *)g_hash_table_lookup( hash, ((char *)email_item->data) );
    /* add the message */
    RT_notif_add_msg(new_rt_ctx, msg, "");
    free(email_item->data);
  }
  LG_log(lg_ctx, LG_FUNC,"<nt_add_msg_to_list: exiting\n");
}



/* To be used with g_hash_table_foreach in nt_unify_list.
   Frees the 'key' and 'value' in the hash
   Receives key
            value
            NULL
   Returns  none
*/

void nt_free_list(char *key, char *value, void *nothing)
{
  if ( key != NULL )
    free(key);
  if ( value != NULL )
    free(value);
}



/* "unifies" a list in a case insensitive manner to remove duplicates
   Receives LG context
            email attibute list
   Returns  list of email addresses
*/

GList *nt_unify_list(LG_context_t *lg_ctx, GList *attr_list)
{
  GHashTable *unification_hash;
  GList **address_list;
  GList *temp;
  GList *return_list = NULL;
  char *key, *value;
  
  LG_log(lg_ctx, LG_FUNC,">nt_unify_list: entered\n");

  /* allocate space for address_list */ 
  address_list = (GList **)malloc(sizeof(GList *));
  *address_list = NULL;

  /* initialize the hash to be used for unification process */
  unification_hash = g_hash_table_new(g_str_hash, g_str_equal);

  /* first put the list elements into a hash, to unify them */
  for (temp = attr_list; temp != NULL; temp = g_list_next(temp))
  {
    /* convert the email address into lowercase, for comparison reasons only */
    key = rpsl_attr_get_clean_value((rpsl_attr_t *)(temp->data));
    value = strdup(key);
    g_strdown(key);
    
    if (g_hash_table_lookup(unification_hash, key) == NULL)
	{ 
      /* if it is not already in the hash table, add to the hash and append to new list */
      LG_log(lg_ctx, LG_DEBUG,"nt_unify_list: add [%s] to hash", value);
      g_hash_table_insert(unification_hash, key, value);
      *address_list = g_list_insert_sorted( *address_list, strdup(value), (GCompareFunc)strcmp );
    }
	else
	{  
      /* it is a duplicate email address, don't append to new list */
	  free(key);
	  free(value);
	}
  }

  /* now, delete the elements in the hash */
  g_hash_table_foreach(unification_hash, (GHFunc)nt_free_list, NULL);

  g_hash_table_destroy(unification_hash);
  
  return_list = *address_list;
  free(address_list);
  LG_log(lg_ctx, LG_FUNC,"<nt_unify_list: exiting\n");
  return return_list;
}



/* Gets GLists of mnt-irt atributes from old and new object. Compares these
   lists and returns a list of diferences.
   Build up lists of mnt-irt attrs from the old object and the new object.
   Compare the two lists and flagged as matched in both lists any names that are common.
   Any items not flagged in either list must have just been added or deleted.
   Receives LG context
            old irt attibute list
            new irt attibute list
            option
              if option==1, return list contains only newly deleted mnt-irt
              if option==2, return list contains only newly added mnt-irt
              if option==3, return list contains both
   Returns  list of added and/or deleted mnt-irt attributes
*/

GList *nt_compare_lists(LG_context_t *lg_ctx, GList *old_irts,
                            GList *new_irts, int option)
{
  typedef struct irt_details
  {
    gchar *irt_name;
	rpsl_attr_t *irts;
	gint   matched;
  } irt_details_t;
  
  char *irt_name;
  GList *old_irt_details = NULL;
  GList *new_irt_details = NULL;
  GList *old_irts_item = NULL;
  GList *new_irts_item = NULL;
  GList *return_list = NULL;
  irt_details_t *irt_details;
  
  LG_log(lg_ctx, LG_FUNC,">nt_compare_lists: entered with option [%d]\n", option);

  /* collect data from the old_irts */
  for ( old_irts_item = old_irts; old_irts_item != NULL; old_irts_item = g_list_next(old_irts_item) )
  {
    irt_details = (irt_details_t *)malloc(sizeof(irt_details_t));
	/* get irt name from mnt-irt attr */
	irt_name = rpsl_attr_get_clean_value( (rpsl_attr_t *)(old_irts_item->data) );
	/* enter details into irt_details structure */
	irt_details->irts = (rpsl_attr_t *)(old_irts_item->data);
	irt_details->irt_name = irt_name;
	irt_details->matched = 0;
	/* append irt_details structure to old_irt_details list */
	old_irt_details = g_list_append(old_irt_details, irt_details);
  }
  
  /* collect data from the new_irts and compare with the old in the same loop */
  for ( new_irts_item = new_irts; new_irts_item != NULL; new_irts_item = g_list_next(new_irts_item) )
  {
    irt_details = (irt_details_t *)malloc(sizeof(irt_details_t));
	/* get irt name from attr */
	irt_name = rpsl_attr_get_clean_value( (rpsl_attr_t *)(new_irts_item->data) );
	/* enter details into irt_details structure */
	irt_details->irts = (rpsl_attr_t *)(new_irts_item->data);
	irt_details->irt_name = irt_name;
	irt_details->matched = 0;

	/* compare the name with the names from the old list  */
	for ( old_irts_item = old_irt_details; old_irts_item != NULL; old_irts_item = g_list_next(old_irts_item) )
    {
	  if ( ! strcmp(irt_name, ((irt_details_t *)(old_irts_item->data))->irt_name ) )
	  {
        /* this name appears in both lists */
	    irt_details->matched = 1;
		((irt_details_t *)(old_irts_item->data))->matched = 1;
		break;
	  }
	}

	/* append irt_details structure to new_irt_details list */
	new_irt_details = g_list_append(new_irt_details, irt_details);
  }
  
  /* we now want a list of mnt-irt taken from the old and new irt_details lists
     where the matched flag is _NOT_ set. These will only exist in one list
	 and have therefore just been added/deleted */
  /* if option==1, return list contains only newly deleted irts
     if option==2, return list contains only newly added irts
     if option==3, return list contains both */
  if ( option == 1 || option == 3 )
  {
    /* Find newly deleted mnt-irt attrs 
       These will be NOT flagged as matched in the old list */
    for ( old_irts_item = old_irt_details; old_irts_item != NULL; old_irts_item = g_list_next(old_irts_item) )
    {
      if ( ! ((irt_details_t *)(old_irts_item->data))->matched )
	  {
        /* this mnt-irt attr has been deleted */
        LG_log(lg_ctx, LG_DEBUG,"nt_compare_lists: adding deleted mnt-irt to return list [%s]", ((irt_details_t *)(old_irts_item->data))->irt_name);
	    return_list = g_list_append(return_list, ((irt_details_t *)(old_irts_item->data))->irts );
	  }
	  free ( ((irt_details_t *)(old_irts_item->data))->irt_name );
    }
  }
  g_list_free(old_irt_details);

  if ( option == 2 || option == 3 )
  {   
    /* Find newly added mnt-irt attrs 
       These will be NOT flagged as matched in the new list */
    for ( new_irts_item = new_irt_details; new_irts_item != NULL; new_irts_item = g_list_next(new_irts_item) )
    {
      if ( ! ((irt_details_t *)(new_irts_item->data))->matched )
	  {
        /* this mnt-irt attr has been added */
        LG_log(lg_ctx, LG_DEBUG,"nt_compare_lists: adding added mnt-irt to return list [%s]", ((irt_details_t *)(new_irts_item->data))->irt_name);
	    return_list = g_list_append(return_list, ((irt_details_t *)(new_irts_item->data))->irts );
	  }
	  free ( ((irt_details_t *)(new_irts_item->data))->irt_name );
    }
  }
  g_list_free(new_irt_details);
  
  LG_log(lg_ctx, LG_FUNC,"<nt_compare_lists: exiting\n");
  return return_list;
}



/* Gets a parsed object and returns a list of it's 'mnt-by' attributes.
   Receives parsed object
   Returns  list of mnt-by attributes
*/

GList *nt_get_mnt_by(rpsl_object_t *object)
{
  GList *list_of_mnt_attrs = NULL;

  list_of_mnt_attrs = rpsl_object_get_attr(object, "mnt-by");
  rpsl_attr_split_multiple(&list_of_mnt_attrs);

  return list_of_mnt_attrs; 
}



/* Gets a parsed object and returns a list of its 'mnt-irt:' attributes.
   Receives parsed object
   Returns  list of mnt-irt attributes
*/

GList *nt_get_mnt_irt(rpsl_object_t *object)
{
  GList *list_of_irt_attrs = NULL;

  list_of_irt_attrs = rpsl_object_get_attr(object, "mnt-irt");
  rpsl_attr_split_multiple(&list_of_irt_attrs);

  return list_of_irt_attrs; 
}



/* Gets a parsed object and returns a list of its 'org:' attributes.
   Receives parsed object
   Returns  list of org attributes
*/

GList *nt_get_org(rpsl_object_t *object)
{
  GList *list_of_org_attrs = NULL;

  list_of_org_attrs = rpsl_object_get_attr(object, "org");
  rpsl_attr_split_multiple(&list_of_org_attrs);

  return list_of_org_attrs; 
}



/* Gets a list of mntner/irt attributes, retrieves those mntners/irts from
   the database and extracts the 'nfy_type' attributes from the mntners/irts.
   Used to get the mnt-nfy, irt-nfy and upd-to attributes.
   Receives RT context
            LG context
            options structure
            source data structure
            list of mntner or irt attributes (mnt-by, mnt-irt) or mntner objects
            type of nfy attr required (mnt-nfy, irt-nfy)
            type of object (mntner or irt)
            type of list passed in (attributes or mntner objects)
   Returns  list of 'type' attributes (mnt-nfy or irt-nfy)
*/

GList *nt_get_nfy_vector(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                              options_struct_t *options, source_data_t *source_data,
                              GList *attributes, char *nfy_type, char *obj_type,
                              int list_type)
{
  int retval = UP_OK;
  char *name = NULL;
  GList *list_of_nfy_attrs = NULL;
  GList *attr_item = NULL;
  GList *temp;
  rpsl_object_t *object = NULL;

  LG_log(lg_ctx, LG_FUNC,">nt_get_nfy_vector: entered with nfy_type [%s] list_type [%s]\n",
                      nfy_type, (list_type == ATTRIBUTE) ? "ATTRIBUTE" : "MNTNER_OBJ" );
  
  /* loop through the list of attributes (eg mnt-by, mnt-irt) or mntner objects */
  for ( attr_item = attributes; attr_item != NULL; attr_item = g_list_next(attr_item) )
  {
    if ( list_type == ATTRIBUTE )
    {
      /* get the name of the mntner/irt object from the attribute value */
	  name = rpsl_attr_get_clean_value( (rpsl_attr_t *)(attr_item->data) );
      LG_log(lg_ctx, LG_DEBUG,"nt_get_nfy_vector: Got a mntner/irt [%s]", name );

      /* get this mntner object from the database */
      retval = LU_lookup(source_data->current_server, &object, obj_type, name, source_data->current_source);
      if ( retval != LU_OKAY )
      {
        /* any lookup error is considered a fatal error */
        LG_log(lg_ctx, LG_FATAL,"nt_get_nfy_vector: lookup error retrieving mntner/irt [%s]", name);
        UP_internal_error(rt_ctx, lg_ctx, options, "nt_get_nfy_vector: lookup error retrieving a mntner/irt\n", 0);
      }
	  free(name);
    }
    else
      /* we already have the object in the list */
	  object = (rpsl_object_t *)(attr_item->data) ;

    if ( ! object )
    {
      /* mntner/irt not found in database */
      LG_log(lg_ctx, LG_WARN,"nt_get_nfy_vector: mntner/irt [%s] not found in database", name);
//        RT_object_not_found(rt_ctx, name);
    }
    else
    {
      /* get all the mnt-nfy/irt-nfy/upd-to attributes from this mntner/irt object */
      temp = rpsl_object_get_attr(object, nfy_type);
      rpsl_attr_split_multiple(&temp);
      LG_log(lg_ctx, LG_DEBUG,"nt_get_nfy_vector: found [%i] %s attributes", g_list_length(temp), nfy_type );

      /* append to list */
      list_of_nfy_attrs = g_list_concat(list_of_nfy_attrs, temp);
      if ( list_type == ATTRIBUTE )
        rpsl_object_delete(object);
    }
  }
  
  LG_log(lg_ctx, LG_FUNC,"nt_get_nfy_vector: exiting with total %s attributes [%i]\n", nfy_type, g_list_length(list_of_nfy_attrs) );
  return list_of_nfy_attrs; 
}



/* Gets old and new objects supplied, forms lists of any irt objects referenced 
   by these (mnt-irt). Returns a GList of irt-nfy attributes for any irt objects
   that heve been added or deleted with this update.
   Receives RT context
            LG context
            options structure
            source data structure
            old object
            new object
   Returns  list of irt-nfy attributes
*/

GList *nt_check_irtnfy(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                            options_struct_t *options, source_data_t *source_data,
                            rpsl_object_t *old_obj, rpsl_object_t *new_obj)
{
  GList *old_irt_attrs = NULL; 
  GList *new_irt_attrs = NULL; 
  GList *changed_irt_attrs = NULL; 
  GList *irt_nfy_attrs = NULL;

  LG_log(lg_ctx, LG_FUNC,">nt_check_irtnfy: entered\n");

  /* get the mnt-irt attributes from the old and new objects */
  if (old_obj != NULL)
    old_irt_attrs = nt_get_mnt_irt(old_obj);
  if (new_obj != NULL)
    new_irt_attrs = nt_get_mnt_irt(new_obj);

  if ( old_irt_attrs != NULL && new_irt_attrs!= NULL )
  {
    /* both old and new objects have some mnt-irt attributes */
    /* compare lists from the two objects and pick out any  additions and deletions */
    LG_log(lg_ctx, LG_DEBUG,"nt_check_irtnfy: getting list of changes to mnt-irt");
	changed_irt_attrs = nt_compare_lists(lg_ctx, old_irt_attrs, new_irt_attrs, 3);
    /* these mnt-irt have been added or deleted */
    LG_log(lg_ctx, LG_DEBUG,"nt_check_irtnfy: getting list of irt_nfy in changed mnt-irt");
	irt_nfy_attrs = nt_get_nfy_vector(rt_ctx, lg_ctx, options,
                                            source_data, changed_irt_attrs, "irt-nfy",
                                            "irt", ATTRIBUTE);
  }
  else if ( old_irt_attrs != NULL )
  {
    /* these mnt-irt have been deleted */
    LG_log(lg_ctx, LG_DEBUG,"nt_check_irtnfy: getting list of irt_nfy in deleted mnt-irt");
	irt_nfy_attrs =  nt_get_nfy_vector(rt_ctx, lg_ctx, options,
                                            source_data, old_irt_attrs, "irt-nfy",
                                            "irt", ATTRIBUTE);
  }
  else if ( new_irt_attrs != NULL )
  {
    /* these mnt-irt have been added */
    LG_log(lg_ctx, LG_DEBUG,"nt_check_irtnfy: getting list of irt_nfy in added mnt-irt");
	irt_nfy_attrs =  nt_get_nfy_vector(rt_ctx, lg_ctx, options,
                                            source_data, new_irt_attrs, "irt-nfy",
                                            "irt", ATTRIBUTE);
  }
  else
  {
    /* no irt objects referenced in either old or new object */
    LG_log(lg_ctx, LG_DEBUG,"nt_check_irtnfy: no irt objects referenced in either old or new object");
  }

  LG_log(lg_ctx, LG_FUNC,"<nt_check_irtnfy: exiting\n");
  return irt_nfy_attrs;
}



/* Gets old and new objects supplied, forms lists of any organisation objects referenced 
   by these ("org:"). Returns a GList of ref-nfy attributes for any organisation objects
   that heve been added or deleted with this update.
   Receives RT context
            LG context
            options structure
            source data structure
            old object
            new object
   Returns  list of ref-nfy attributes
*/

GList *nt_check_refnfy(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                            options_struct_t *options, source_data_t *source_data,
                            rpsl_object_t *old_obj, rpsl_object_t *new_obj)
{
  GList *old_org_attrs = NULL; 
  GList *new_org_attrs = NULL; 
  GList *changed_org_attrs = NULL; 
  GList *ref_nfy_attrs = NULL;

  LG_log(lg_ctx, LG_FUNC,">nt_check_refnfy: entered\n");

  /* get the org attributes from the old and new objects */
  if (old_obj != NULL) {
    old_org_attrs = nt_get_org(old_obj);
  }
  if (new_obj != NULL) {
    new_org_attrs = nt_get_org(new_obj);
  }

  if ( old_org_attrs != NULL && new_org_attrs != NULL )
  {
    /* both old and new objects have some "org:" attributes */
    /* compare lists from the two objects and pick out any  additions and deletions */
    LG_log(lg_ctx, LG_DEBUG,"nt_check_refnfy: getting list of changes to org");
    changed_org_attrs = nt_compare_lists(lg_ctx, old_org_attrs, new_org_attrs, 3);
    /* these "org:" have been added or deleted */
    LG_log(lg_ctx, LG_DEBUG,"nt_check_refnfy: getting list of ref-nfy in changed org");
    ref_nfy_attrs = nt_get_nfy_vector(rt_ctx, lg_ctx, options,
                                            source_data, changed_org_attrs, "ref-nfy",
                                            "organisation", ATTRIBUTE);
  }
  else if ( old_org_attrs != NULL )
  {
    /* these "org:" have been deleted */
    LG_log(lg_ctx, LG_DEBUG,"nt_check_refnfy: getting list of ref-nfy in deleted org");
    ref_nfy_attrs =  nt_get_nfy_vector(rt_ctx, lg_ctx, options,
                                            source_data, old_org_attrs, "ref-nfy",
                                            "organisation", ATTRIBUTE);
  }
  else if ( new_org_attrs != NULL )
  {
    /* these "org:" have been added */
    LG_log(lg_ctx, LG_DEBUG,"nt_check_refnfy: getting list of irt_nfy in added mnt-irt");
    ref_nfy_attrs =  nt_get_nfy_vector(rt_ctx, lg_ctx, options,
                                            source_data, new_org_attrs, "ref-nfy",
                                            "organisation", ATTRIBUTE);
  }
  else
  {
    /* no organisation objects referenced in either old or new object */
    LG_log(lg_ctx, LG_DEBUG,"nt_check_refnfy: no organisation objects referenced in either old or new object");
  }

  LG_log(lg_ctx, LG_FUNC,"<nt_check_refnfy: exiting\n");
  return ref_nfy_attrs;
}



/* Gathers e-mail addresses to which we will send normal notification messages. It 
   takes old and new object strings, looks up maintainers and less specific inetnums/domains/routes 
   when necessary, finds the addresses (in mnt-nfy and notify attributes) and returns 
   a list of email addresses as strings. 
   Also now checks for irt-nfy in any irt objects that have been added or deleted 
   Receives RT context
            LG context
            options structure
            source data structure
            postproc object
            old object
            list of mntners used in auth
   Returns  list of email addresses
*/

GList *nt_gather_ntfy_addresses(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                                   options_struct_t *options, source_data_t *source_data,
                                   rpsl_object_t *new_obj, rpsl_object_t *old_obj,
                                   GList *mntner_used  )
{
  GList *temp_list = NULL;
  GList *address_list = NULL;
  GList *email_attrs = NULL;
  GList *attr_list = NULL; 
  rpsl_object_t *object = NULL;

  LG_log(lg_ctx, LG_FUNC,">nt_gather_ntfy_addresses: entered\n");

  if (old_obj != NULL && new_obj != NULL)
  { 
    /* it was a modify, use the old object */
    object = old_obj;
    LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: modify");
  }
  else if (old_obj == NULL && new_obj != NULL)
  { 
    /* it was a creation, use the new object */
    object = new_obj;
    LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: creation");
  }
  else if (old_obj != NULL && new_obj == NULL)
  { 
    /* it was a deletion, use the old object */
    object = old_obj;
    LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: deletion");
  }
  else
  {
    LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: no objects supplied\n" );
    LG_log(lg_ctx, LG_FUNC,"<nt_gather_ntfy_addresses: exiting with NULL list\n" );
    return address_list;
  }

  /* start with the 'notify' in the object itself */
  LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: get notify attrs from object itself");
  email_attrs = rpsl_object_get_attr(object, "notify");
  if ( email_attrs )
  {
    rpsl_attr_split_multiple(&email_attrs);
    LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: [%d] notify attrs found", g_list_length(email_attrs));
  }
  else
  {
    /* no notify attrs found in the object */
    LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: no notify attrs found");
  }

  /* now add the 'mnt-nfy' from any of the mntners in the object */
  LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: get mnt-nfy from any mntners in object");
//  mnt_attrs = nt_get_mnt_by(object);
  LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: [%d] mntners found", g_list_length(mntner_used));
  attr_list = nt_get_nfy_vector(rt_ctx, lg_ctx, options, source_data, mntner_used, "mnt-nfy", "mntner", MNTNER_OBJ);
  LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: [%d] mnt-nfy attrs found", g_list_length(attr_list));

  email_attrs = g_list_concat(email_attrs, attr_list);
  LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: [%d] attrs with email addresses found", g_list_length(email_attrs));

  /* Now add the 'irt-nfy'
     For a modify, from any of the irts in the old and new objects
     if they have just been added or deleted 
     For a creation, from any of the irts in the new object
     as they have just been added 
     For a deletion, from any of the mntners in the old object only
     as they have just been deleted */
  LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: get irt-nfy");
  attr_list = NULL;
  attr_list = nt_check_irtnfy(rt_ctx, lg_ctx, options, source_data, old_obj, new_obj);
  email_attrs = g_list_concat(email_attrs, attr_list);

  /* Do the same for "org:" attributes, and organisation objects' "ref-nfy:" attributes */
  LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: get ref-nfy");
  attr_list = NULL;
  attr_list = nt_check_refnfy(rt_ctx, lg_ctx, options, source_data, old_obj, new_obj);
  email_attrs = g_list_concat(email_attrs, attr_list);

  LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: [%d] attrs with email addresses found", g_list_length(email_attrs));

  if ( email_attrs )
  {
    /* if we have any emails, 
       we have to 'unify' the list here to remove duplicates, 
       address_list is now a list of malloc'd email address strings */
    address_list = nt_unify_list(lg_ctx, email_attrs);

    /* we have the emails so release the list of email attrs */
    rpsl_attr_delete_list( email_attrs );

    LG_log(lg_ctx, LG_DEBUG,"nt_gather_ntfy_addresses: notif email addresses" );
    for ( temp_list=address_list; temp_list!=NULL; temp_list=g_list_next(temp_list) )
    {
      LG_log(lg_ctx, LG_DEBUG,"[%s]", (char *)(temp_list->data) );
    }
    LG_log(lg_ctx, LG_DEBUG,"\n");
  }

  LG_log(lg_ctx, LG_FUNC,"<nt_gather_ntfy_addresses: exiting\n" );
  return address_list;
}



/* Gathers e-mail addresses to which we will forward messages. It takes
   an object, looks up maintainers in mnt-by, finds the mail addresses 
   (in upd-to attributes) and returns a list of them.
   Receives RT context
            LG context
            options structure
            source data structure
            object
            list of mntners used in auth
   Returns  list of email addresses
*/

GList *nt_gather_frwd_addresses(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                                   options_struct_t *options, source_data_t *source_data,
                                   rpsl_object_t *object,
                                   GList *mntner_used  )
{
//  char  *email;
//  GList *email_list = NULL;
  GList *updto_attrs = NULL;
//  GList *mail_attr = NULL;
  GList *temp_list = NULL;
  GList *address_list = NULL;

  LG_log(lg_ctx, LG_FUNC,">nt_gather_frwd_addresses: entered\n");

  /* get a list of the mnt-by attributes from the object */
  LG_log(lg_ctx, LG_DEBUG,"nt_gather_frwd_addresses: get upd-to");
//  mnt_attrs = nt_get_mnt_by(object);
  LG_log(lg_ctx, LG_DEBUG,"nt_gather_frwd_addresses: [%d] mntners found", g_list_length(mntner_used));
  /* get a list of upd-to attributes in these mntners */
  updto_attrs = nt_get_nfy_vector(rt_ctx, lg_ctx, options, source_data, mntner_used, "upd-to", "mntner", MNTNER_OBJ);
  LG_log(lg_ctx, LG_DEBUG,"nt_gather_frwd_addresses: [%d] upd-to attrs found", g_list_length(updto_attrs));

  /* now extract the email addresses from the values of these attributes */
  if ( updto_attrs  )
  {
    /* if we have any emails, we have to 'unify' the list here to remove duplicates, 
       address_list is now a list of malloc'd email address strings */
    address_list = nt_unify_list(lg_ctx, updto_attrs);

    /* we have the emails so release the list of email attrs */
    rpsl_attr_delete_list( updto_attrs );

    LG_log(lg_ctx, LG_DEBUG,"nt_gather_frwd_addresses: forward email addresses" );
    for ( temp_list=address_list; temp_list!=NULL; temp_list=g_list_next(temp_list) )
    {
      LG_log(lg_ctx, LG_DEBUG,"[%s]", (char *)(temp_list->data) );
    }
    LG_log(lg_ctx, LG_DEBUG,"\n");
  }

  LG_log(lg_ctx, LG_FUNC,"<nt_gather_frwd_addresses: exiting" );
  return address_list;
}



/* Gets the "From" line of the incoming mail message and extracts the address 
   Receives RT context
            LG context
            options structure
   Returns  extracted email address
*/

char *nt_find_email_address(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                                options_struct_t *options)
{
  char *pos1 = NULL, *pos2 = NULL, *pos = NULL;
  char *temp = NULL;
  char *new_temp = NULL;
  char *part1 = NULL, *part2 = NULL;
  
  LG_log(lg_ctx, LG_FUNC,">nt_find_email_address entered with from_line [%s]", 
                        options->mail_hdr_data.from ? options->mail_hdr_data.from : "" );

  if (options->mail_hdr_data.from == NULL)
  {
    return NULL;
  }
  if (strstr(options->mail_hdr_data.from, "From:") != options->mail_hdr_data.from)
  {
    temp = strdup(options->mail_hdr_data.from);
  }
  else
  {
    temp = strdup(options->mail_hdr_data.from + strlen("From:"));
  }
  g_strstrip(temp);

  /* and now, we have to remove the parts in parantheses 
     this code assumes there are no nested parentheses */ 
  while ( strchr(temp, '(') != NULL && strchr(temp, ')') != NULL
          && strchr(temp, '(') < strchr(temp, ')') )
  {
    part1 = strdup(temp);
    /* terminate the string */
    pos = strchr(part1, '(');
    *pos = '\0';

    part2 = strdup(strchr(temp, ')') + 1);
    strcat(part1, part2);
    free(temp);
    temp = strdup(part1);
    free(part1);
    free(part2);
  }

  /* and now, we have to remove the parts in double quotes */ 
  while ( (pos = strchr(temp, '"')) != NULL && strchr(pos+1, '"') != NULL )
  {
    part1 = strdup(temp);
    /* terminate the string */
    pos = strchr(part1, '"');
    *pos = '\0';
    /* remove the first '"' from temp */
    pos = strchr(temp, '"');
    *pos = ' ';

    /* now find the second '"' */
    part2 = strdup(strchr(temp, '"') + 1);
    strcat(part1, part2);
    free(temp);
    temp = strdup(part1);
    free(part1);
    free(part2);
  }

  /* there should not be any parentheses or '"' now */
  if ( strchr(temp, '(') || strchr(temp, ')') || strchr(temp, '"') )
  {
    /* we have a malformed email address and we cannot continue */
    LG_log(lg_ctx, LG_FATAL,"nt_find_email_address: malformed email adress found [%s]", options->mail_hdr_data.from);
    UP_internal_error(rt_ctx, lg_ctx, options, "nt_find_email_address: malformed email adress found\n", 0);
  }
  

  /* unless the email address is malformed there should now only be one set of
     angle brackets <> in the string (or none at all) */
  pos1 = strchr(temp, '<');
  pos2 = strchr(temp, '>');
  if ( pos1 || pos2 )
  { 
    /* we have found at least one angle bracket */
    if ( ! (pos1 && pos2) || pos1 > pos2 || 
          strchr(pos1+1, '<') || strchr(pos2+1, '>') )
    {
      /* Either we don't have a matching pair of angle brackets
         or they are in the wrong order (> before <)
         or we have more than one of either angle bracket
         In any case it is a malformed email address and we cannot continue */
      LG_log(lg_ctx, LG_FATAL,"nt_find_email_address: malformed email adress found [%s]", options->mail_hdr_data.from);
      UP_internal_error(rt_ctx, lg_ctx, options, "nt_find_email_address: malformed email adress found\n", 0);
    }
    
    /* then the line contains something like '...<john@inter.net>...' */
    pos2 = strchr(temp, '>');
    *pos2 = '\0';
    new_temp = strdup(temp);
    free(temp);
    pos1 = strchr(new_temp, '<');
    temp = strdup(pos1 + 1);
    free(new_temp);
    LG_log(lg_ctx, LG_DEBUG,"nt_find_email_address email=[%s]", temp);
  }
  
  g_strstrip(temp); 
  LG_log(lg_ctx, LG_FUNC,"<nt_find_email_address exiting with email [%s]\n", temp);
  return temp;
}  




/* Gets old and new versions of the object, and creates new RT context
   when necessary, and then writes appropriate strings into those contexts
   Receives RT context
            LG context
            options structure
            source data structure
            postproc object
            old object
            list of mntners used in auth
   Returns  none
*/

void NT_write_all_ntfs(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                             options_struct_t *options, source_data_t *source_data,
                             rpsl_object_t *postproc_obj, rpsl_object_t *old_obj,
                             GList *mntner_used )
{ 
  char *e_mail_address = NULL;
  char *postproc_obj_str = NULL;
  char *old_obj_str = NULL;
  char buffer[DIFF_BUFFER_SIZE];
  char *diff_old = NULL;
  char *diff_new = NULL;
  char *diff_res = NULL;
  const char *class = NULL;
  char *pri_key = NULL;
  char *diff_command = DIFF_COMMAND;
  char *ptr = NULL;
  size_t data_cnt;
  GList *e_mail_list = NULL;
  GString *mess;
  GString *diff_command_line;
  GString *differences;
  FILE *diff_file;

  LG_log(lg_ctx, LG_FUNC,">NT_write_all_ntfs entered\n");
  LG_log(lg_ctx, LG_DEBUG,"NT_write_all_ntfs: mntner_used [%d]", g_list_length(mntner_used));

  if ( options->mail_input )
  {
  /*************** this may not be needed ************************/
    /* The update came via an email */
    /* From_address may contain also the name, like "Johnny Bravo <johnny@inter.net>",
       so extract the e-mail address from it */
    e_mail_address = nt_find_email_address(rt_ctx, lg_ctx, options);
    LG_log(lg_ctx, LG_DEBUG, "NT_write_all_ntfs: from_address=[%s], e_mail_address=[%s]", options->mail_hdr_data.from, e_mail_address);
    /* save shortened email address */
    options->mail_hdr_data.from_email = e_mail_address;
  }

  /* get a list of email addresses to send notifications to */
  e_mail_list = nt_gather_ntfy_addresses(rt_ctx, lg_ctx, options, source_data, 
                                               postproc_obj, old_obj, mntner_used );

  if ( e_mail_list )
  {
    /* add the emails from this list to the notif hash */
    nt_add_list_to_hash(lg_ctx, options, e_mail_list, options->ntfy_hash); 

    /* get the text versions of the objects */
    if ( postproc_obj )
      postproc_obj_str = rpsl_object_get_text(postproc_obj, RPSL_STD_COLUMN);
    if ( old_obj )
      old_obj_str = rpsl_object_get_text(old_obj, RPSL_STD_COLUMN);
    mess = g_string_new(NULL);

    if (old_obj != NULL && postproc_obj != NULL)
    { 
      /* it was a modification */   
      LG_log(lg_ctx, LG_DEBUG, "NT_write_all_ntfs: it was a modification");

      /* start the message */
      mess = g_string_append(mess, "---\nOBJECT BELOW MODIFIED:\n\n");

      if ( options->notif_diff )
      {
        /* find the difference */
        diff_old = UP_get_temp_filename(lg_ctx, "diff_old");
        diff_new = UP_get_temp_filename(lg_ctx, "diff_new");
        diff_res = UP_get_temp_filename(lg_ctx, "diff_res");

        /* write old and new objects to files */
        if (( diff_file = fopen(diff_old, "w")) == NULL)
        {
          LG_log(lg_ctx, LG_FATAL, "NT_write_all_ntfs: Can't open temp diff file [%s]", diff_old);
          UP_internal_error(rt_ctx, lg_ctx, options, "Can't open temp diff file\n", 0);
        }
        fprintf(diff_file, "%s", old_obj_str);
        fclose(diff_file);

        if (( diff_file = fopen(diff_new, "w")) == NULL)
        {
          LG_log(lg_ctx, LG_FATAL, "NT_write_all_ntfs: Can't open temp diff file [%s]", diff_new);
          UP_internal_error(rt_ctx, lg_ctx, options, "Can't open temp diff file\n", 0);
        }
        fprintf(diff_file, "%s", postproc_obj_str);
        fclose(diff_file);

        /* execute the diff and write to an output result file */
        diff_command_line = g_string_new(diff_command);
        diff_command_line = g_string_append(diff_command_line, " ");
        diff_command_line = g_string_append(diff_command_line, diff_old);
        diff_command_line = g_string_append(diff_command_line, " ");
        diff_command_line = g_string_append(diff_command_line, diff_new);
        diff_command_line = g_string_append(diff_command_line, " > ");
        diff_command_line = g_string_append(diff_command_line, diff_res); 
        LG_log(lg_ctx, LG_DEBUG,"NT_write_all_ntfs: diff_command_line [%s]", diff_command_line->str);
        system(diff_command_line->str);
        g_string_free(diff_command_line, 1);
        unlink(diff_old);
        unlink(diff_new);

        /* read the diff result */
        if (( diff_file = fopen(diff_res, "r")) == NULL)
        {
          LG_log(lg_ctx, LG_FATAL, "NT_write_all_ntfs: Can't open temp diff file [%s]", diff_res);
          UP_internal_error(rt_ctx, lg_ctx, options, "Can't open temp diff file\n", 0);
        }
        differences = g_string_new(NULL);
        while ( ! feof(diff_file) ) 
        {
          data_cnt = fread(buffer, 1, DIFF_READ_SIZE, diff_file);
          buffer[data_cnt] ='\0';
          differences = g_string_append(differences, buffer);
        }
        fclose(diff_file);
        /* change any --- line seperators to ===
           so as not to confuse the differences with object seperators */
        while ( (ptr = strstr(differences->str, "---")) && *(ptr+4) == '>' )
//        while ( (ptr = strstr(differences->str, "---\n>")) )
        {
          *ptr = '=';
          *(ptr+1) = '=';
          *(ptr+2) = '=';
        }
        LG_log(lg_ctx, LG_DEBUG,"NT_write_all_ntfs: differences [%s]", differences->str);
        unlink(diff_res);

        class = rpsl_object_get_class(old_obj);
        pri_key =  rpsl_object_get_key_value(old_obj);
        LG_log(lg_ctx, LG_DEBUG,"NT_write_all_ntfs: class [%s] pri_key [%s]", class, pri_key);

        /*insert the differences into the notif message */
        mess = g_string_append(mess, "Differences in [");
        mess = g_string_append(mess, class);
        mess = g_string_append(mess, "] ");
        mess = g_string_append(mess, pri_key);
        mess = g_string_append(mess, "\n\n");
        if ( ! strcmp(differences->str, "") || ! strcmp(differences->str, "\n") )
        {
          mess = g_string_append(mess, "Objects are identical.");
        }
        else mess = g_string_append(mess, differences->str);
        mess = g_string_append(mess, "\n\n");

        if ( pri_key ) free(pri_key);
        g_string_free(differences, 1);
      }

      /* build up the standard message part */
      mess = g_string_append(mess, old_obj_str);
      mess = g_string_append(mess, "\n\nREPLACED BY:\n\n");
      mess = g_string_append(mess, postproc_obj_str);
      mess = g_string_append(mess, "\n");
    }
    else if (old_obj == NULL && postproc_obj != NULL)
    { 
      /* it was a creation */
      LG_log(lg_ctx, LG_DEBUG, "NT_write_all_ntfs: it was a creation");

      /* build up the message */
      mess = g_string_append(mess, "---\nOBJECT BELOW CREATED:\n\n");
      mess = g_string_append(mess, postproc_obj_str);
      mess = g_string_append(mess, "\n");
    }
    else if (old_obj != NULL && postproc_obj == NULL)
    { 
      /* it was a deletion */
      LG_log(lg_ctx, LG_DEBUG, "NT_write_all_ntfs: it was a deletion");

      /* build up the message */
      mess = g_string_append(mess, "---\nOBJECT BELOW DELETED:\n\n");
      mess = g_string_append(mess, old_obj_str);
      mess = g_string_append(mess, "\n");
    }

    /* add this message to all the emails in the list (and free the emails in the list) */
    nt_add_msg_to_list(lg_ctx, e_mail_list, options->ntfy_hash, mess->str);

    /* free the g-string and the content */
    g_string_free(mess, 1);
    g_list_free(e_mail_list);
    if (postproc_obj_str)
      free(postproc_obj_str);
    if (old_obj_str)
      free(old_obj_str);
  }
  else
    LG_log(lg_ctx, LG_DEBUG, "NT_write_all_ntfs: no notifs to write");

  LG_log(lg_ctx, LG_FUNC,"<NT_write_all_ntfs exiting\n");
}



/* Gets old and new versions of the object, and creates new RT context
   when necessary, and then writes appropriate strings into those contexts
   Receives RT context
            LG context
            options structure
            source data structure
            postproc object
            old object
            list of mntners used in auth
   Returns  none
*/

void NT_write_all_frwds(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                             options_struct_t *options, source_data_t *source_data,
                             rpsl_object_t *postproc_obj, rpsl_object_t *old_obj,
                             GList *mntner_used )
{ 
  char *object_str = NULL;
  GList *e_mail_list = NULL;
  rpsl_object_t *object;
  GString *mess;

  LG_log(lg_ctx, LG_FUNC,">NT_write_all_frwds entered\n");

  object = old_obj == NULL ? postproc_obj : old_obj;
  /* get a list of email addresses to send forward messages to */
  e_mail_list = nt_gather_frwd_addresses(rt_ctx, lg_ctx, options, source_data, object, mntner_used);

  if ( e_mail_list )
  {
    /* add the emails from this list to the forw hash */
    nt_add_list_to_hash(lg_ctx, options, e_mail_list, options->forw_hash);

    /* get the text version of the object
       if the new object exists, use it, otherwise use the old object */
    if ( postproc_obj )
      object_str = rpsl_object_get_text(postproc_obj, RPSL_STD_COLUMN);
    else if ( old_obj )
      object_str = rpsl_object_get_text(old_obj, RPSL_STD_COLUMN);
    else
      object_str = strdup("");
    mess = g_string_new(NULL);

    if (old_obj != NULL && postproc_obj != NULL)
    { 
      /* it was a modify */
      LG_log(lg_ctx, LG_DEBUG, "NT_write_all_frwds: it was a modify");

      /* build up the message */
      mess = g_string_append(mess, "----\nMODIFY REQUESTED FOR:\n\n");
    }
    else if (old_obj == NULL && postproc_obj != NULL)
    { 
      /* it was a creation */
      LG_log(lg_ctx, LG_DEBUG, "NT_write_all_frwds: it was a creation");

      /* build up the message */
      mess = g_string_append(mess, "----\nCREATION REQUESTED FOR:\n\n");
    }
    else if (old_obj != NULL && postproc_obj == NULL)
    { 
      /* it was a deletion */
      LG_log(lg_ctx, LG_DEBUG, "NT_write_all_frwds: it was a deletion ");

      /* build up the message */
      mess = g_string_append(mess, "----\nDELETION REQUESTED FOR:\n\n");
    }
    mess = g_string_append(mess, object_str);
    mess = g_string_append(mess, "\n");

    /* add this message to all the emails in the list (and free the emails in the list) */
    nt_add_msg_to_list(lg_ctx, e_mail_list, options->forw_hash, mess->str);

    /* free the g-string and the content */
    g_string_free(mess, 1);
    g_list_free(e_mail_list);
    if (object_str)
      free(object_str);
  }
  else
    LG_log(lg_ctx, LG_DEBUG, "NT_write_all_frwds: no forwards to write");

  LG_log(lg_ctx, LG_FUNC,"<NT_write_all_frwds exiting\n");
}



/* Sends the creation forward message.
   Receives RT context
            LG context
            options structure
            out going message
            To address
            redirect heading
   Returns  none
*/

void nt_send_email( RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                             options_struct_t *options, char *out_mess, 
                             char *to_address, char *redirect_hdg)
{
  char *mail_command_line = NULL;
  char *mailcmd = NULL;
  char *defmail = NULL;
  char *humailbox = NULL;
  char *new_out = NULL;
  char *name = NULL;

  LG_log(lg_ctx, LG_FUNC,">nt_send_email entered: with To address [%s]\n",
                                to_address ? to_address : "NULL" );

  mailcmd = g_strstrip(ca_get_mailcmd);

  /* if we are not redirecting acks and notifs, send the message */
  if ( ! options->redirect )
  {
    LG_log(lg_ctx, LG_DEBUG,"nt_send_email: no re-direction");
    /* create a temp file and write the out going message to it */
    name = nt_filename_generate( rt_ctx, lg_ctx, options, 
                                     to_address ? to_address : "", 
                                     out_mess, NULL);
    LG_log(lg_ctx, LG_DEBUG,"nt_send_email: temp file name [%s]", name);

    if (to_address != NULL)
	{
      /* send the email taking input from the temp file */
      mail_command_line = (char *)malloc(strlen(mailcmd) + strlen(name) + 
                                        strlen(to_address) + 5);
      sprintf(mail_command_line, "%s %s < %s", mailcmd, to_address, name);
      LG_log(lg_ctx, LG_DEBUG,"nt_send_email: mail_command_line [%s]", mail_command_line);
      system(mail_command_line);

      free(mail_command_line);
    }
  }
  else
  {
    /* if we are redirecting acks and notifs, send the message to DEFMAIL  */

    /* get the default mail address to send the redirected message to */
    defmail = g_strstrip(ca_get_defmail);
    humailbox = g_strstrip(ca_get_humailbox);
    LG_log(lg_ctx, LG_DEBUG,"nt_send_email: re-direction, From [%s] To [%s]", 
                                    humailbox, defmail);
    
    /* prepend the out going message with a new mail header */
    new_out = malloc(strlen(out_mess) + strlen(humailbox) + 
                       strlen(defmail) + strlen(redirect_hdg) +
                       strlen("From: \nTo: \nSubject: Redirected  mail\n\n") +5);
    sprintf(new_out, "From: %s\nTo: %s\nSubject: Redirected %s mail\n\n%s",
                           humailbox, defmail, redirect_hdg, out_mess);

    /* create a temp file and write the out going message to it */
    name = nt_filename_generate( rt_ctx, lg_ctx, options, defmail, new_out, "redirect");
    LG_log(lg_ctx, LG_DEBUG,"nt_send_email: temp file name [%s]", name);

    /* send the email taking input from the temp file */
    mail_command_line = (char *)malloc(strlen(mailcmd) + strlen(defmail) + 
                                          strlen(name) + 5);
    sprintf(mail_command_line, "%s %s < %s", mailcmd, defmail, name);
    LG_log(lg_ctx, LG_DEBUG,"nt_send_email: mail_command_line [%s]", mail_command_line);
    system(mail_command_line);

    free(mail_command_line);
    free(new_out);
    free(defmail);
    free(humailbox);
  }
  free(mailcmd);
  LG_log(lg_ctx, LG_FUNC,"<nt_send_email exiting\n");
  unlink(name);
  free(name);
}



/* Adds the notification message in out_mess to the file "logfilename.date".
   Receives RT context
            LG context
            options structure
            full email message including header
            log file name
   Returns  none
*/

void nt_log_message( RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                      options_struct_t *options, const char *out_mess, 
                      const char *logfilename, int type )
{
  char *logfile_date;
  char *date;
  char *time_str;
  char *msg_type;
  time_t cur_time;
  FILE *log_file;
  GString *sor;

  LG_log(lg_ctx, LG_FUNC,">nt_log_message: entered with log file, [%s]\n", logfilename);

  /* construct the "logfilename.date" string */
  logfile_date = (char *)malloc(strlen(logfilename) + 10);
  date = UP_get_current_date();
  snprintf(logfile_date, strlen(logfilename) + 10, "%s.%s", logfilename, date);
  free(date);

  LG_log(lg_ctx, LG_DEBUG,"nt_log_message: open log file, [%s]", logfile_date);
  if (( log_file = fopen(logfile_date, "a")) == NULL)
  {
    free(logfile_date);
    LG_log(lg_ctx, LG_FATAL,"nt_log_message: Can't open log file, %s", logfile_date);
    UP_internal_error(rt_ctx, lg_ctx, options, "nt_log_message: Can't open log file\n", 1);
  }
  free(logfile_date);

  /* get time */
  cur_time = time(NULL);
  time_str = strdup(ctime(&cur_time));
  /* cut the '\n' at the end */
  time_str[strlen(time_str) - 1] = '\0';

  /* write the record separater to the logfile */
  msg_type = options->mail_input ? " MAIL" : options->origin ? " SYNC" : "" ;

  sor = g_string_new(NULL);
  sor = g_string_append(sor, "\n>>> Time: ");
  sor = g_string_append(sor, time_str);
  sor = g_string_append(sor, msg_type);
  sor = g_string_append(sor, " UPDATE ");
  sor = g_string_append(sor, UP_notif2log_str(type) );
  if ( options->origin )
  {
    sor = g_string_append(sor, " (");
    sor = g_string_append(sor, options->origin ? options->origin : "NULL");
    sor = g_string_append(sor, ")");
  }
  sor = g_string_append(sor, options->redirect ? " (Redirected)" : "");
  sor = g_string_append(sor, " <<<\n\n");

  LG_log(lg_ctx, LG_DEBUG,"nt_log_message: write record separater to logfile\n%s", sor->str);
  fprintf(log_file, "%s", sor->str);

  /* free the g-string and the content */
  g_string_free(sor, 1);
  free(time_str);

  /* write the out going message to the log file */
  fprintf(log_file, "%s", out_mess);

  fclose(log_file);
  LG_log(lg_ctx, LG_FUNC,"<nt_log_message: exiting\n");
}



/* Sends the acknowledgement email and logs it to the system log.
   Receives RT context
            LG context
            options structure
   Returns  none
*/

void NT_process_acknowledgement(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                                  options_struct_t *options)
{
  char *logname = NULL;
  char *template = NULL;
  char *out_mess = NULL;
  char *to_address = NULL;
  char *redirect_hdg = "acknowledgement";

  LG_log(lg_ctx, LG_FUNC,">NT_process_acknowledgement: entered\n");

  /* get the log file name for ack messages */
  logname = ca_get_acklog;
  /* get the template for the ack message */
  template = ca_get_ack_template;
  LG_log(lg_ctx, LG_DEBUG,"NT_process_acknowledgement: using template [%s]", template);
  
  /* create the email to be sent from the RT template */
  out_mess = RT_get(rt_ctx, RT_UPD_INFO, template);
  LG_log(lg_ctx, LG_DEBUG,">NT_process_acknowledgement: out going message");
  LG_log(lg_ctx, LG_DEBUG,"[");
  LG_log(lg_ctx, LG_DEBUG,"%s", out_mess);
  LG_log(lg_ctx, LG_DEBUG,"<]");
// printf("notif: outmess[\n%s]\n", out_mess);

  /* send the acknowledgement message if it is a mail update and we have the To address */
  /* use the ReplyTo address if there is one, otherwise the From address */
  to_address = options->mail_hdr_data.replyto ? options->mail_hdr_data.replyto 
                                              : options->mail_hdr_data.from;
  if ( options->mail_input && to_address )
    nt_send_email(rt_ctx, lg_ctx, options, out_mess, to_address, redirect_hdg);

  /* if the update wasn't a mail update OR we have been asked explicitely
     to print out the ack to the stdout, print it */
  if ( ! options->mail_input || options->print )
    printf("%s", out_mess);

  /* log the acknowledgement message */
  nt_log_message(rt_ctx, lg_ctx, options, out_mess, logname, UP_ACK);
    
  /* clean up */
  free(out_mess);
  free(logname);
  free(template);
  LG_log(lg_ctx, LG_FUNC,"<NT_process_acknowledgement: exiting\n");
}



/* The function required for NT_process_notifications
   Gets a list of the keys and values from the hash.
   Receives email address
            RT context
            pointer to pointer to list of data structures
   Returns  none
*/

void  nt_gfunc_values(gpointer key, gpointer value, gpointer user_data)
{
  GList **plist;
  ctx_data_t *ctx_data = malloc(sizeof(ctx_data_t));

  plist = (GList **)user_data;

  ctx_data->key = (char *)key;
  ctx_data->value = (RT_context_t *)value;
  *plist = g_list_append(*plist, ctx_data);
}



/* Sends all the notification emails and logs then to the system log.
   Retrieves the messages from the set of RT contexts stored in the hash.
   type determines notif or frwd messages.
   Receives RT context
            LG context
            options structure
            type (notif or frwd)
   Returns  none
*/

void NT_process_notifications(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                                  options_struct_t *options, int type)
{
  char *logname = NULL;
  char *template = NULL;
  char *out_mess = NULL;
  char *email_addr = NULL;
  char *redirect_hdg = NULL;
  char *notif_hdg = "notification";
  char *forw_hdg = "forward";
  GList *ctx_list = NULL;
  GList *ctx_item = NULL;
  ctx_data_t *ctx_data = NULL;
  RT_context_t *notif_rt_ctx;
  GHashTable *hash;

  LG_log(lg_ctx, LG_FUNC,">NT_process_notifications: entered for type [%s]\n", UP_notif2str(type));

  if ( type == UP_NOTIF )
  {
    /* get the notify hash */
    hash = options->ntfy_hash;
    /* get the log file name for notify messages */
    logname = ca_get_notiflog;
    /* get the template for the notify message */
    template = ca_get_notif_template;
    redirect_hdg = notif_hdg;
  }
  else /* type == UP_FRWD */
  {
    /* get the forward hash */
    hash = options->forw_hash;
    /* get the log file name for forward messages */
    logname = ca_get_forwlog;
    /* get the template for the forward message */
    template = ca_get_frwd_template;
    redirect_hdg = forw_hdg;
  }
  LG_log(lg_ctx, LG_DEBUG,"NT_process_notifications: using template [%s]", template);

  /* get a list of the values (RT_ctx) and keys (email address) from the hash */
  g_hash_table_foreach( hash, (GHFunc)nt_gfunc_values, &ctx_list);
  
  /* loop through the list of RT contexts for the notification messages */
  for ( ctx_item = ctx_list; ctx_item != NULL; ctx_item = g_list_next(ctx_item) )
  {
    /* get the notif RT context and email address */
    ctx_data = (ctx_data_t *)ctx_item->data;
    notif_rt_ctx = (RT_context_t *)ctx_data->value;
    email_addr = (char *)ctx_data->key;

    /* close input to the notif reporting context */
    RT_end(notif_rt_ctx);

    /* create the email to be sent from the RT template */
    out_mess = RT_get(notif_rt_ctx, RT_UPD_INFO, template);
    LG_log(lg_ctx, LG_DEBUG,"NT_process_notifications: out going message [\n%s]", out_mess);

    /* send the notification message */
    nt_send_email(rt_ctx, lg_ctx, options, out_mess, email_addr, redirect_hdg);
    
    /* log the notification message */
    nt_log_message(rt_ctx, lg_ctx, options, out_mess, logname, type);
    
    /* clean up */
    free(out_mess);
    /* delete the notif RT context */
    RT_destroy(notif_rt_ctx );
    /* delete the hash entry */
    free(ctx_data->key);
    /* delete the list entry */
    free(ctx_data);
  }
  /* finally delete the list and hash */
  g_list_free(ctx_list);
  g_hash_table_destroy(hash);
  
  free(logname);
  free(template);
  LG_log(lg_ctx, LG_FUNC,"<NT_process_notifications: exiting\n");
}



/* Forwards the as-block and irt creation requests to <HUMAILBOX>
   Receives RT context
            LG context
            options structure
            operation
            object to be created
            list of credentials
   Returns  none
*/

void NT_forw_create_req(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                             options_struct_t *options, char *operation,
                             rpsl_object_t *object, GList *credentials)
{
  char *object_str = NULL;
  char *to_address = NULL;
  char *template = NULL;
  char *out_mess = NULL;
  char *forwlog = NULL;
  const char *type = NULL;
  char *redirect_hdg = "forward create";
  char *pkey = NULL;
  RT_context_t *new_rt_ctx;
  GString *mess;
  GString *subject;

  LG_log(lg_ctx, LG_FUNC,">NT_forw_create_req: entered\n");

  /* get the text version of the object */
  object_str = rpsl_object_get_text(object, RPSL_STD_COLUMN);
  type = rpsl_object_get_class(object);
  pkey = rpsl_object_get_key_value(object);
  /* build up the subject line */
  subject = g_string_new(NULL);
  if ( ! strcmp(type, "mntner") )
    subject = g_string_append(subject, "Maintainer" );
  else if ( ! strcmp(type, "irt") )
    subject = g_string_append(subject, "IRT" );
  else if ( ! strcmp(type, "as-block") )
    subject = g_string_append(subject, "AS-BLOCK" );
  else
    subject = g_string_append(subject, "UNKNOWN" );
  subject = g_string_append(subject, " creation request: " );
  subject = g_string_append(subject, pkey );

  /* get the address to send the request to */
  to_address = g_strstrip(ca_get_humailbox);
  LG_log(lg_ctx, LG_DEBUG,"NT_forw_create_req: To address [%s]", to_address);

  /* set up new reporting context */
  new_rt_ctx = nt_generate_new_context(lg_ctx, options, to_address, subject->str);
//  RT_set_object(new_rt_ctx, object);

  /* build up the message */
  mess = g_string_new(NULL);
  mess = g_string_append(mess, object_str);
//  mess = g_string_append(mess, "\n\n");

  /* add the message to the new reporting context */
  LG_log(lg_ctx, LG_DEBUG,"NT_forw_create_req: message [\n%s]", mess->str);
  RT_notif_add_msg(new_rt_ctx, mess->str, operation);

  /* get the template for the forw_create_req message */
  template = ca_get_forw_create_template;
  LG_log(lg_ctx, LG_DEBUG,"NT_forw_create_req: using template [%s]", template);

  /* pass full input and credentials to the new_rt_ctx,
     then close input to the new reporting context */
//  RT_unset_object(new_rt_ctx, op2rt_upd_op(UP_CREATE), 1);
  RT_credentials(new_rt_ctx, credentials);
  RT_full_input(new_rt_ctx, options->input_file_name);
  RT_end(new_rt_ctx);

  /* create the email to be sent from the RT template */
  out_mess = RT_get(new_rt_ctx, RT_UPD_INFO, template);
  LG_log(lg_ctx, LG_DEBUG,"NT_forw_create_req: out going message [\n%s]", out_mess);

  /* send it */ 
  nt_send_email(rt_ctx, lg_ctx, options, out_mess, to_address, redirect_hdg);

  /* log it */
  forwlog = ca_get_forwlog;
  nt_log_message(rt_ctx, lg_ctx, options, out_mess, forwlog, UP_FRWD_CREATE);
  
  /* close down the new report */
  RT_destroy(new_rt_ctx);

  /* free the g-string and the content */
  g_string_free(mess, 1);
  g_string_free(subject, 1);
  /* free the mem */ 
  free(object_str);
  free(to_address);
  free(template);
  free(forwlog);
  free(out_mess);
  LG_log(lg_ctx, LG_FUNC,"<NT_forw_create_req exiting\n");
}


/* Forwards any policy check failures to <HUMAILBOX>
   Receives RT context
            LG context
            options structure
            operation
            object
            reason
            list of credentials
   Returns  none
*/

void NT_forw_policy_fail(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                             options_struct_t *options, char *operation,
                             rpsl_object_t *object, char *reason, 
                             GList *credentials)
{
  char *object_str = NULL;
  char *to_address = NULL;
  char *template = NULL;
  char *out_mess = NULL;
  char *forwlog = NULL;
  const char *type = NULL;
  char *redirect_hdg = "forward create";
  char *pkey = NULL;
  RT_context_t *new_rt_ctx;
  GString *mess;
  GString *subject;
  GString *policy_reason;

  LG_log(lg_ctx, LG_FUNC,">NT_forw_policy_fail: entered\n");

  /* get the text version of the object */
  object_str = rpsl_object_get_text(object, RPSL_STD_COLUMN);
  type = rpsl_object_get_class(object);
  pkey = rpsl_object_get_key_value(object);
  /* build up the subject line */
  subject = g_string_new(NULL);
  subject = g_string_append(subject, "Policy check fail forward message: " );
  subject = g_string_append(subject, pkey );

  /* get the address to send the request to */
  to_address = g_strstrip(ca_get_humailbox);
  LG_log(lg_ctx, LG_DEBUG,"NT_forw_policy_fail: To address [%s]", to_address);

  /* set up new reporting context */
  new_rt_ctx = nt_generate_new_context(lg_ctx, options, to_address, subject->str);
//  RT_set_object(new_rt_ctx, object);

  /* build up the message */
  mess = g_string_new(NULL);
  mess = g_string_append(mess, object_str);
  mess = g_string_append(mess, "\n");
/* Dirty rotten hack!!!!
   This reason should appear magically in the template output
   from the call to RT_policy_fail
   BUT......the magic failed me :(
   So I have had to append it to the end of the object text */
  mess = g_string_append(mess, "***Info: ");
  mess = g_string_append(mess, reason);
  mess = g_string_append(mess, "\n");

  policy_reason = g_string_new(NULL);
  policy_reason = g_string_append(policy_reason, reason);

  /* add the message to the new reporting context */
  LG_log(lg_ctx, LG_DEBUG,"NT_forw_policy_fail: message [\n%s]", mess->str);
  RT_notif_add_msg(new_rt_ctx, mess->str, operation);
//   RT_policy_fail(new_rt_ctx, policy_reason->str);

  /* get the template for the forw_create_req message */
//  template = ca_get_forw_policy_template;
  template = ca_get_forw_create_template;
  LG_log(lg_ctx, LG_DEBUG,"NT_forw_policy_fail: using template [%s]", template);

  /* pass full input and credentials to the new_rt_ctx,
     then close input to the new reporting context */
//  RT_unset_object(new_rt_ctx, op2rt_upd_op(UP_CREATE), 1);
  RT_credentials(new_rt_ctx, credentials);
  RT_full_input(new_rt_ctx, options->input_file_name);
  RT_end(new_rt_ctx);

  /* create the email to be sent from the RT template */
  out_mess = RT_get(new_rt_ctx, RT_UPD_INFO, template);
  LG_log(lg_ctx, LG_DEBUG,"NT_forw_policy_fail: out going message [\n%s]", out_mess);

  /* send it */ 
  nt_send_email(rt_ctx, lg_ctx, options, out_mess, to_address, redirect_hdg);

  /* log it */
  forwlog = ca_get_forwlog;
  nt_log_message(rt_ctx, lg_ctx, options, out_mess, forwlog, UP_FRWD_POLICY);
  
  /* close down the new report */
  RT_destroy(new_rt_ctx);

  /* free the g-string and the content */
  g_string_free(mess, 1);
  g_string_free(subject, 1);
  g_string_free(policy_reason, 1);
  /* free the mem */ 
  free(object_str);
  free(to_address);
  free(template);
  free(forwlog);
  free(out_mess);
  LG_log(lg_ctx, LG_FUNC,"<NT_forw_policy_fail exiting\n");
}
