/***************************************
  $Revision: 1.10 $

  UP auto nic handling

  Status: NOT REVIEWED, NOT TESTED

  Author(s):       Denis Walker ( re-write of functions by Engin Gunduz )

  ******************/ /******************
  Modification History:
        engin (15/12/2000) first Created.
        denis (31/08/2001) Modified for new parser API
        denis (11/11/2002) re-write for re-structured dbupdate
        engin (02/15/2004) Modified for organisation object
  ******************/ /******************
  Copyright (c) 2001                              RIPE NCC
 
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


#include <assert.h>
#include "rip.h"

#include "dbupdate.h"
#include "up_auto_nic.h"
#include "ns_util.h"
#include <glib.h>



/* Constructs an initials string from a given name (for NIC hdl generation) 
   The parser ensures that there are at least two words starting with a letter.
   Receives LG context
            person/role/org name
   Returns  initials string.
*/

char *up_find_initials(LG_context_t *lg_ctx, const char *person_role_org_name)
{
   char *temp, *pos;
   char *initials = NULL;
   int len, word_idx;
   char **vector;
   int count = 0;

   LG_log(lg_ctx, LG_FUNC,">find_initials: entered with name [%s]\n", person_role_org_name);

   temp = strdup(person_role_org_name);
   if ((pos = index(temp, '#')) != NULL)
   { /* delete the EOL comment */
     pos[0] = '\0';
   }

   vector = ut_g_strsplit_v1(temp, " ", 0);
   for (word_idx = 0; vector[word_idx] != NULL; word_idx++)
   {
     if ( (strlen(vector[word_idx]) > 0 ) && isalpha( (int)(vector[word_idx][0]) ) )
	 {
       /* this word starts with a letter */
       if ( count++ < 4 )
       {
         /* only take the first 4 initial letters */
         if (initials == NULL)
	     {
           initials = (char *)malloc(2);
           initials[0] = vector[word_idx][0]; 
		   initials[1] = '\0';
         }
	     else
	     {
           len = strlen(initials);
           initials = (char *)realloc(initials, len + 2 );
           initials[len] = vector[word_idx][0];
           initials[len + 1] = '\0';
         }
       }
     }
   }
   free(temp);
   g_strfreev(vector);
   /* We have to make sure that there are at least two letters in initials variable */

   /* If there were no initials found, assign 'AA'. */
   if(initials == NULL){
     initials = strdup("AA");
   }
   if(strlen(initials) == 0){
     initials = strdup("AA");
   }

   /* If there were only one letter in the initials list, add 'A' to make it two */
   if(strlen(initials) == 1){
     initials = (char *)realloc(initials, 3 );
     initials[1] = 'A';
     initials[2] = '\0';
   }
   LG_log(lg_ctx, LG_FUNC,"<find_initials: exiting with initials [%s]\n", initials);
   return initials;
}




/*  Gets the letter combination from the AUTO- nic-hdl if one has been specified,
    to be used in the automatically generated nic handle.
   Receives LG context
            AUTO- nic-hdl
   Returns  The letter combination in the AUTO- nic-handle, if specified.
            NULL otherwise
*/

char *up_get_combination_from_autonic(LG_context_t *lg_ctx, const char *autonic)
{
  GString *temp;
  char *str = NULL;
  char *pos;

  LG_log(lg_ctx, LG_FUNC,">up_get_combination_from_autonic: entered with autonic [%s]\n", autonic);

  temp = g_string_new(autonic);

  /* we don't really need this, because autonic variable is obtained via rpsl_attr_get_clean_value */
  /* EG 2003 12 23 */
  if ((pos = index(temp->str, '#')) != NULL)
  { 
    /* delete the EOL comment */
    pos[0] = '\0';
  }
  g_strstrip(temp->str);
  temp->len = strlen(temp->str);/* we directly played with temp->str, so adjust temp->len accordingly */

  /* make all upper case */
  temp = g_string_up(temp);
  /* delete 'AUTO-' part from the beginning */ 
  temp = g_string_erase(temp, 0, strlen("AUTO-")); 

  /* delete all digits from the beginning of the string */
  while (temp->len > 0 && ((temp->str)[0] >= '0' && (temp->str)[0] <= '9'))
  {
    temp = g_string_erase(temp, 0, 1);
  }

  if (temp->len < 2 )
  {
    /* should not happen as the parser rejects less than 2 chars as a syntax error 
       but I guess a double check does no harm */
    g_string_free(temp, TRUE);
    LG_log(lg_ctx, LG_FUNC,"<up_get_combination_from_autonic: exiting, less than 2 initials given, ignored\n");
    return NULL;
  }
  else
  {
    str = temp->str;
    g_string_free(temp, FALSE);
    if(strlen(str) > 4)
	{
      /* should not happen as the parser rejects more than 4 chars as a syntax error 
         but I guess a double check does no harm */
      str[4] = '\0'; 
    }
    LG_log(lg_ctx, LG_FUNC,"<up_get_combination_from_autonic: exiting with initials [%s]\n", str);
    return str;
  }
}




/* Gets an object with an AUTO- key
   and  modifies the key value to be sent to the core server.
   For example, a "nic-hdl: AUTO-1" becomes "nic-hdl: HG*-RIPE 
   for a person with the initials HG
   Or a "key-cert: AUTO-1" becomes "key-cert: X509-*"
   or, "organisation: AUTO-1" becomes "organisation: ORG-HG*-RIPE"
   for an organisation object with the initials HG.
   Also, auto_nic_hdl is set to "AUTO-1" in these examples.
   auto_nic must be allocated enough memory before calling UP_replace_AUTO_key
   Receives RT context
            LG context
            options structure
            parsed object
            auto_key pointer
   Returns  UP_OK if successful
            UP_FAIL otherwise
*/
   
int UP_replace_AUTO_key(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                              options_struct_t *options, rpsl_object_t *preproc_obj,
                              char *auto_key, char *obj_source)
{
  int retval = UP_FAIL;
  char *person_role_name = NULL;
  char *org_name = NULL;
  char *initials = NULL;
  char *value, *new_value = NULL;
  const char *type;
  int pos;
  GList *attributes = NULL;
  rpsl_attr_t *attr;
  GList *class_attr;
  GList *org_name_attr;
  int list_idx;
  char *attr_list[] = { "nic-hdl",
                        "key-cert",
                        "organisation"
                      };   /* list of attributes that could contain and AUTO- key */

  LG_log(lg_ctx, LG_FUNC,">UP_replace_AUTO_key: entered\n");

  for ( list_idx=0; list_idx < sizeof(attr_list)/sizeof(char *); list_idx++ )
  {
    /* get a list with only one item */
    attributes = rpsl_object_get_attr(preproc_obj, attr_list[list_idx]); 
    if (attributes != NULL)
	{
      value = rpsl_attr_get_clean_value((rpsl_attr_t *)(attributes->data));
      LG_log(lg_ctx, LG_DEBUG,"UP_replace_AUTO_key: AUTO- key is [%s]", value);

      strcpy(auto_key, value);
      g_strdown(auto_key);
      pos = rpsl_attr_get_ofs((rpsl_attr_t *)(attributes->data));

      if ( ! strcmp(attr_list[list_idx], "nic-hdl") )
      {
        /* This is a nic-hdl AUTO- */
        /* if the letter combination is already specified, get it */
        /* use the original case sensitive value */
        initials = up_get_combination_from_autonic(lg_ctx, value);
        /* if the letter combination is not in the AUTO nichdl, obtain it from the name */
        if (initials == NULL)
        {
	      type = rpsl_object_get_class(preproc_obj);
	      class_attr = rpsl_object_get_attr(preproc_obj, type);
	      person_role_name = rpsl_attr_get_clean_value((rpsl_attr_t *)(class_attr->data));
	      rpsl_attr_delete_list(class_attr);
          initials = up_find_initials(lg_ctx, person_role_name);
	      free(person_role_name);
        }
        LG_log(lg_ctx, LG_DEBUG,"UP_replace_AUTO_key: initials [%s]", initials ? initials : "NULL");

        new_value = (char *)malloc(strlen(initials) + strlen(obj_source) + 3);
        strcpy(new_value, initials);
        strcat(new_value, "*-");
        strcat(new_value, obj_source);
        LG_log(lg_ctx, LG_DEBUG,"UP_replace_AUTO_key: new nic-hdl value [%s]", new_value);
        free(initials);
      }else if(! strcmp(attr_list[list_idx], "organisation") )
      {
        /* This is an organisation AUTO- */
        /* if the letter combination is already specified, get it */
        /* use the original case sensitive value */
        initials = up_get_combination_from_autonic(lg_ctx, value);
        /* if the letter combination is not in the AUTO nichdl, obtain it from the org-name */
        if (initials == NULL)
        {
	      org_name_attr = rpsl_object_get_attr(preproc_obj, "org-name");
	      org_name = rpsl_attr_get_clean_value((rpsl_attr_t *)(org_name_attr->data));
	      rpsl_attr_delete_list(org_name_attr);
          initials = up_find_initials(lg_ctx, org_name);
	      free(org_name);
        }
        LG_log(lg_ctx, LG_DEBUG,"UP_replace_AUTO_key: initials [%s]", initials ? initials : "NULL");

        new_value = (char *)malloc(strlen("ORG-") + strlen(initials) + strlen(obj_source) + 3);

        strcpy(new_value, "ORG-");
        strcat(new_value, initials);
        strcat(new_value, "*-");
        strcat(new_value, obj_source);
        LG_log(lg_ctx, LG_DEBUG,"UP_replace_AUTO_key: new nic-hdl value [%s]", new_value);
        free(initials);

      
      }
      else
      {
        /* This is a key-cert AUTO- */
        new_value = (char *)malloc(strlen("X509-*") + 1);
        strcpy(new_value, "X509-*");
        LG_log(lg_ctx, LG_DEBUG,"UP_replace_AUTO_key: new key-cert value [%s]", new_value);
      }

      /* now copy original attribute, replace value, remove old attr and add replacement */
      attr = rpsl_attr_copy((rpsl_attr_t *)(attributes->data));
      rpsl_attr_replace_value(attr, new_value);
      free(new_value);
      free(value);

      /* remove the attribute with the AUTO- */
      if ( pos == 0 )
      {
        rpsl_object_remove_first_attr(preproc_obj, NULL);
      }
      else
      {
        rpsl_object_remove_attr(preproc_obj, pos, NULL);
      }

      /* insert new attribute with new key value */
      if ( pos == 0 )
      {
        rpsl_object_add_first_attr(preproc_obj, attr, NULL);
      }
      else
      {
        rpsl_object_add_attr(preproc_obj, attr, pos, NULL);
      }

      rpsl_attr_delete_list(attributes);
      retval = UP_OK;
      break;
    }
  }
  
  LG_log(lg_ctx, LG_FUNC,"<UP_replace_AUTO_key: exiting with value [%s]\n", UP_ret2str(retval));
  return retval;
}



/* Replaces any refs to AUTO- keys with the assigned ones.
   All AUTO- refs are processed, even after an error, so that multiple erors can be
   reported if necessary.
   Receives RT context
            LG context
            options structure
            parsed object
            report_errors (if 1, errors are reported, if 0, errors are not reported)
   Returns  UP_OK if successful
            UP_FAIL otherwise
*/

int UP_replace_refs_to_AUTO_key(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                                         options_struct_t *options,
                                         rpsl_object_t *preproc_obj,
                                         int report_errors)
{
  int retval = UP_OK;
  rpsl_attr_t *attr;
  char *key = NULL;
  char *value;
  const GList *list_item = NULL;
  GList *attributes = NULL;
  int pos;
  int list_idx;
  char *attr_list[] = { "admin-c",
                        "tech-c",
                        "zone-c",
                        "author",
                        "auth",
                        "org"
                      };

  LG_log(lg_ctx, LG_FUNC,">UP_replace_refs_to_AUTO_key: entered\n");

  for ( list_idx=0; list_idx < sizeof(attr_list)/sizeof(char *); list_idx++ )
  {
    /* Take an item from the attr_list above and 
       get a list of all instances of this attribute in the object */
    attributes = rpsl_object_get_attr(preproc_obj, attr_list[list_idx]);
    rpsl_attr_split_multiple(&attributes);
    if (attributes != NULL)
    {
      /* Now loop through the list of all instances of of this attribute to see 
         if any of them contain a reference to an AUTO- key */
      list_item = attributes;
      while (list_item != NULL)
      {
	    value = rpsl_attr_get_clean_value((rpsl_attr_t *)(list_item->data));
	    g_strdown(value);
	    if ( strstr(value, "auto-") != NULL)
	    {
          /* this attribute has an auto- key */
	      pos = rpsl_attr_get_ofs((rpsl_attr_t *)(list_item->data));
          LG_log(lg_ctx, LG_DEBUG,"UP_replace_refs_to_AUTO_key: attribute [%s] has AUTO- key [%s]", attr_list[list_idx], value);

          if( (key = (char *)g_hash_table_lookup(options->AUTO_key_hash, value)) )
	      {
            /* we have this AUTO- key in the hash, so replace it. */
            LG_log(lg_ctx, LG_DEBUG,"UP_replace_refs_to_AUTO_key: actual key found [%s]", key);
		    /* create a new attribute with the auto key */
		    attr = rpsl_attr_copy((rpsl_attr_t *)(list_item->data));
            rpsl_attr_replace_value(attr, key);

		    /* remove the attribute with the auto- key */
		    rpsl_object_remove_attr(preproc_obj, pos, NULL);

		    /* insert new attribute with the key */
		    rpsl_object_add_attr(preproc_obj, attr, pos, NULL);

            /* replacing the attr destroys the list, so start the list again */
            attributes = rpsl_object_get_attr(preproc_obj, attr_list[list_idx]);
            rpsl_attr_split_multiple(&attributes);
            list_item = attributes; 
          }
	      else
	      {
            /* unknown auto- key referenced */
            LG_log(lg_ctx, LG_DEBUG,"UP_replace_refs_to_AUTO_key: Unknown AUTO- key referenced [%s]", value);
            if(report_errors)
            {
              RT_unknown_auto_key(rt_ctx, value);
            }
		    retval = UP_FAIL;
          }
	    }

	    free (value);
	    list_item = g_list_next(list_item);
      }
    }
    rpsl_attr_delete_list(attributes);
  }
	
  LG_log(lg_ctx, LG_FUNC,"<UP_replace_refs_to_AUTO_key: exiting with value [%s]\n", UP_ret2str(retval));
  return retval;
}




/* Replaces any refs to AUTO- keys with the assigned ones.
   Receives RT context
            LG context
            options structure
            object string
            report_errors (if 1, errors are reported, if 0, errors are not reported)
   Returns  UP_OK if successful
            UP_FAIL otherwise
*/

int UP_replace_refs_to_AUTO_key_from_string(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                                         options_struct_t *options,
                                         char **object_str,
                                         int report_errors)
{

  rpsl_object_t *object = NULL;
  char * object_str_copy = NULL;
  int ret_val;

  LG_log(lg_ctx, LG_FUNC,">UP_replace_refs_to_AUTO_key_from_string: entered\n");

  object_str_copy = strdup(*object_str);

  /* remove trailing dot from domain, if any */
  ns_remove_trailing_dot(lg_ctx,&object_str_copy);

  /* parse the object string */
  object = rpsl_object_init(object_str_copy);

  if ( rpsl_object_has_error(object, RPSL_ERRLVL_ERROR) )
  {
    LG_log(lg_ctx, LG_DEBUG,"UP_replace_refs_to_AUTO_key_from_string: object parsed with errors");
    /* The object has RPSL syntax error.  Do not bother to replace its AUTO references */
    free(object_str_copy);
    rpsl_object_delete(object);
    return UP_OK;
  }
  else
  {
    LG_log(lg_ctx, LG_DEBUG,"UP_replace_refs_to_AUTO_key_from_string: object parsed OK");
    free(object_str_copy);
    ret_val = UP_replace_refs_to_AUTO_key(rt_ctx, lg_ctx, options, object, report_errors);
    free(*object_str);
    *object_str = rpsl_object_get_text(object, 0);
    rpsl_object_delete(object);
    return ret_val;
  }

}



/* Replace the AUTO- key (if there is one) of the object with
   its assigned value
   Receives RT context
            LG context
            auto_key  (The key to be replaced)
            assigned key   (The value to insert)
            postproc object
   Returns  UP_OK if no error
            UP_FAIL otherwise
*/

int UP_put_assigned_key(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                           options_struct_t *options,  char *auto_key,
                           char *assigned_key, rpsl_object_t *postproc_obj)
{
  int retval = 0;
  char *value;
  int pos;
  rpsl_attr_t *attr;
  GList *attributes;
  int list_idx;
  char *attr_list[] = { "nic-hdl",
                        "key-cert",
                        "organisation"
                      };   /* list of attributes that could contain and AUTO- key */

  LG_log(lg_ctx, LG_FUNC,">UP_put_assigned_key: entered with assigned nic-hdl [%s]\n", 
                            (assigned_key && strlen(assigned_key)) ? assigned_key : "not set");

  for ( list_idx=0; list_idx < sizeof(attr_list)/sizeof(char *); list_idx++ )
  {
    /* get a list with only one item */
    attributes = rpsl_object_get_attr(postproc_obj, attr_list[list_idx]); 
    if ( attributes )
    {
      value = rpsl_attr_get_clean_value((rpsl_attr_t *)(attributes->data));

      /* replace the AUTO- key with the assigned value  */
      LG_log(lg_ctx, LG_DEBUG,"UP_put_assigned_key: replacing AUTO- key [%s]", value);

	  /* copy original attribute, replace value */
	  attr = rpsl_attr_copy((rpsl_attr_t *)(attributes->data));
	  rpsl_attr_replace_value(attr, assigned_key);
      pos = rpsl_attr_get_ofs((rpsl_attr_t *)(attributes->data));

      /* remove the attribute with the AUTO- */
      if ( pos == 0 )
      {
        rpsl_object_remove_first_attr(postproc_obj, NULL);
      }
      else
      {
        rpsl_object_remove_attr(postproc_obj, pos, NULL);
      }

      /* insert new attribute with new key value */
      if ( pos == 0 )
      {
        rpsl_object_add_first_attr(postproc_obj, attr, NULL);
      }
      else
      {
        rpsl_object_add_attr(postproc_obj, attr, pos, NULL);
      }


      free(value);
      rpsl_attr_delete_list(attributes);

      /* add the AUTO- key to the hash */
      g_hash_table_insert(options->AUTO_key_hash, strdup(auto_key), strdup(assigned_key));
      LG_log(lg_ctx, LG_DEBUG,"UP_put_assigned_key: AUTO_key_hash has %i pairs", 
                        g_hash_table_size(options->AUTO_key_hash));
      retval = 1;
      break;
    }
  }
  if ( ! retval )
  {
    /* this object does not have an AUTO- key to replace */
    LG_log(lg_ctx, LG_DEBUG,"UP_put_assigned_key: object does not have an AUTO- key");
  }
  
  retval = UP_OK;
  LG_log(lg_ctx, LG_FUNC,"<UP_put_assigned_key: exiting with value [%s]\n", UP_ret2str(retval));
  return retval;
}



/* Takes a parsed object and checks if certain keys contains an AUTO- value.
   There can only be one of these found in an object.
   Receives LG context
            parsed object
   Returns  1 if AUTO- key found
            0 otherwise
*/

int UP_has_AUTO_key(LG_context_t *lg_ctx, const rpsl_object_t *preproc_obj)
{
  int retval = 0;
  char *key;
  GList *attributes = NULL;
  int list_idx;
  char *attr_list[] = { "nic-hdl",
                        "key-cert",
                        "organisation"
                      };   /* list of attributes that could contain and AUTO- key */

  LG_log(lg_ctx, LG_FUNC,">UP_has_AUTO_key: entered\n");

  for ( list_idx=0; list_idx < sizeof(attr_list)/sizeof(char *); list_idx++ )
  {
    /* get a list with only one item */
    attributes = rpsl_object_get_attr(preproc_obj, attr_list[list_idx]); 
    if (attributes != NULL)
	{
      if (UP_strstr_in_attr_list(lg_ctx, attributes, "AUTO-") == 1)
	  { 
        /* it contains an AUTO- key */
        key = rpsl_attr_get_clean_value((rpsl_attr_t *)(attributes->data));
        LG_log(lg_ctx, LG_DEBUG,"UP_has_AUTO_key: %s key found", key);
        free(key);
        rpsl_attr_delete_list(attributes);
        retval = 1;
        break;
      }
    }
  }

  LG_log(lg_ctx, LG_FUNC,"<UP_has_AUTO_key: exiting, %s\n", 
                      retval ? "object contains AUTO- key" : "no AUTO- key found");
  return retval; 
}



/* Takes a parsed object and checks if this object contains
   a reference to an AUTO- generated object key.
   Receives LG context
            parsed object
   Returns  1 if reference found
            0 if NO reference found
*/

int UP_has_ref_to_AUTO_key(LG_context_t *lg_ctx, rpsl_object_t *object)
{
  GList *attributes = NULL;
  int list_idx;
  char *attr_list_person_role_organisation[] = { "admin-c",
                        "tech-c",
                        "zone-c",
                        "author",
                        "auth",
                        "org"
                      };


  LG_log(lg_ctx, LG_FUNC,">UP_has_ref_to_AUTO_key: entered\n");

  if ( ! rpsl_object_is_deleted(object) )
  {
    for ( list_idx=0; list_idx < sizeof(attr_list_person_role_organisation)/sizeof(char *); list_idx++ )
    {
      attributes = rpsl_object_get_attr(object, attr_list_person_role_organisation[list_idx]);
      rpsl_attr_split_multiple(&attributes);
      if (attributes != NULL)
	  {
        if (UP_strstr_in_attr_list(lg_ctx, attributes, "AUTO-") == 1)
	    { 
          /* it contains a ref to AUTO- key */
          rpsl_attr_delete_list(attributes);
          LG_log(lg_ctx, LG_FUNC,"<UP_has_ref_to_AUTO_key: exiting with value 1 (%s has AUTO-)", attr_list_person_role_organisation[list_idx]);
          return 1;
        }
      }
      rpsl_attr_delete_list(attributes);
    }
    LG_log(lg_ctx, LG_FUNC,"<UP_has_ref_to_AUTO_key: exiting with value 0 (no refs found)\n");
    return 0;        
  }
  else
  {  
    /* it is to be deleted so it doesn't matter if 
       it contains refs to AUTO keys. */
    LG_log(lg_ctx, LG_FUNC,"<UP_has_ref_to_AUTO_key: exiting with value 0 (to be deleted)\n");
    return 0;        
  }
}

/* Takes an object as char* and checks if this object contains
   a reference to an AUTO- generated object key.
   Receives LG context
            parsed object
   Returns  1 if reference found
            0 if NO reference found
*/

int UP_has_ref_to_AUTO_key_from_string(LG_context_t *lg_ctx, char *object_str)
{
  rpsl_object_t *object = NULL;
  char * object_str_copy = NULL;
  int ret_val;

  LG_log(lg_ctx, LG_FUNC,">UP_has_ref_to_AUTO_key_from_string: entered\n");

  object_str_copy = strdup(object_str);

  /* remove trailing dot from domain, if any */
  ns_remove_trailing_dot(lg_ctx,&object_str_copy);

  /* parse the object string */
  object = rpsl_object_init(object_str_copy);

  if ( rpsl_object_has_error(object, RPSL_ERRLVL_ERROR) )
  {
    LG_log(lg_ctx, LG_DEBUG,"UP_has_ref_to_AUTO_key_from_string: object parsed with errors");
    /* The object has RPSL syntax error.  Do not bother to look if it has AUTO references */
    free(object_str_copy);
    rpsl_object_delete(object);
    return 0;
  }
  else
  {
    LG_log(lg_ctx, LG_DEBUG,"UP_has_ref_to_AUTO_key_from_string: object parsed OK");
    free(object_str_copy);
    ret_val = UP_has_ref_to_AUTO_key(lg_ctx, object);
    rpsl_object_delete(object);
    return ret_val;
  }


}


/**************************************************************************************/    
/*                     TEMPORARY CODE TO USE EXISTING RIPUPD                          */
  
/* Gets assigned key from the string that is returned from RIPupdate */

int get_assigned_key(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                              options_struct_t *options,
                              char *string, char *key )
{
   int retval = UP_OK;
   char **temp = NULL;
   int i=1;

  LG_log(lg_ctx, LG_FUNC,">get_assigned_key: entered with string [%s]\n", string);

  /* split the string into lines */
  temp = ut_g_strsplit_v1(string , "\n", 0);

  /* look for assigned key */
  /* in the second line RIPupdate returns for example "I[65][EK3-RIPE]" We
     need to extract EK3-RIPE part */
  key = strncpy(key, rindex(temp[i],'[') + 1 ,  
                             rindex(temp[i],']') - rindex(temp[i],'[') - 1);
  key[rindex(temp[i],']') - rindex(temp[i],'[') - 1] = '\0';

  LG_log(lg_ctx, LG_DEBUG,"get_assigned_key: key returned [%s]\n", key);

  g_strfreev(temp);

  LG_log(lg_ctx, LG_FUNC,"<get_assigned_key: exiting with value [%s]\n", UP_ret2str(retval));
  return retval;
}
/**************************************************************************************/    



/* Gets assigned key from the string that is returned from the core server 
   Receives RT context
            LG context
            options structure
            result string from ripupd
            pointer to assigned_key
   Returns  UP_OK if successful
            UP_FAIL otherwise
            sets value in key
*/

int UP_get_assigned_key(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                              options_struct_t *options,
                              char *string, char *key )
{
  int retval = UP_OK;
  int line_idx, char_idx;
  int num_idx = 0;
  int attr_type = 0;
  char attr_type_str[10];
  char *pos = NULL;
  char **temp = NULL;

  LG_log(lg_ctx, LG_FUNC,">UP_get_assigned_key: entered with string [%s]\n", string);

  /* Find the line starting with 'I' followed by the attr_type [<A_NH>] */
  /* split the string into lines */
  temp = ut_g_strsplit_v1(string , "\n", 0);
  for (line_idx = 0; temp[line_idx] != NULL; line_idx++)
  {
    if ( strstr(temp[line_idx], "I") == temp[line_idx] )
    {
      /* Found a line starting with 'I'.
         Check for the attr_type A_NH */
      for ( char_idx=1; temp[line_idx][char_idx] != '\0' && num_idx<9; char_idx++ )
      {
        if ( isdigit((int)temp[line_idx][char_idx]) )
        {
          /* save the digits */
          attr_type_str[num_idx++] = temp[line_idx][char_idx];
        }
        else if ( num_idx )
        {
          /* first non digit AFTER the number */
          break;
        }
      }
      assert(num_idx);  /* should always find a number */
      attr_type_str[num_idx] = '\0';
      attr_type = atoi(attr_type_str);
      if ( attr_type == A_NH || attr_type == A_OG ) /* "nic-hdl:" or "organistion:" attribute */
      {
        /* look for assigned key 
           core server returns for example "I[<A_NH>]:EK3-RIPE" 
           We need to extract EK3-RIPE part */
        temp[line_idx] = g_strchomp(temp[line_idx]); 
        pos = strchr(temp[line_idx], ':');
        if ( pos )
        {
          strcpy(key, pos+1);
          LG_log(lg_ctx, LG_DEBUG,"UP_get_assigned_key:assigned key [%s]", key);
        }
        else
        {
          /* there is no key, or the colon was missing, fatal error,
             this should not happen, but just in case */
          LG_log(lg_ctx, LG_FATAL, "UP_get_assigned_key: no :key returned from core server [%s]", temp[line_idx]);
          UP_internal_error(rt_ctx, lg_ctx, options, "UP_get_assigned_key: no :key returned from core server\n", 0);
        }
        break;
      }
    }
  }
  g_strfreev(temp);
  
  if ( attr_type != A_NH  && attr_type != A_OG )/* "nic-hdl:" or "organisation:" attribute */
  {
    /* no key found, fatal error */
    LG_log(lg_ctx, LG_FATAL,"UP_get_assigned_key: no key attribute type returned from core server");
    UP_internal_error(rt_ctx, lg_ctx, options, "UP_get_assigned_key: no key attribute type returned from core server\n", 0);
  }

  LG_log(lg_ctx, LG_FUNC,"<UP_get_assigned_key: exiting with value [%s]\n", UP_ret2str(retval));
  return retval;
}
