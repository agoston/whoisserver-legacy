#include <string.h>
#include "au_plugins.h"
#include "au_util.h"
#include "rt_dbupdate.h"
#include "ca_configFns.h"
#include "ca_defs.h"
#include "ca_dictionary.h"
#include "ca_macros.h"

#undef MIN
#undef MAX
#include "memwrap.h"

AU_ret_t object_creation(au_plugin_callback_info_t *info);
AU_ret_t org_creation(au_plugin_callback_info_t *info);
AU_ret_t object_modification(au_plugin_callback_info_t *info);
AU_ret_t org_allow(au_plugin_callback_info_t *info);


static const au_check_by_type_t organisation_plugins[] = 
{
  /* any object can have an "org:" attribute */

  /* object_type, creation_rule, deletion_rule, modification_rule */
  { "organisation", org_creation,     org_allow, object_modification },
  { "*",            object_creation,  org_allow, object_modification },
  { NULL }
};

/* things that we don't care about, authorise */
AU_ret_t
org_allow (au_plugin_callback_info_t *info)
{
  return AU_AUTHORISED;
}


gboolean
au_has_org_power_mnt (const rpsl_object_t *obj)
{
  char *org_power_mnt_str = NULL;
  char **org_power_mnt_list = NULL;
  int power_mnt_idx;

  GList *mntners;
  GList *p;

  gboolean org_power_mntner_found;

  LG_log(au_context, LG_FUNC, ">au_has_org_power_mnt: entering");

  /* get the list of mnt-by mntner names */
  mntners = rpsl_object_get_attr(obj, "mnt-by");
  rpsl_attr_split_multiple(&mntners);

  /* get the list of ORG_POWER_MNT mntner names */
  org_power_mnt_str = ca_get_org_power_mnt;
  /* split the alloc_mntner_str on space */
  org_power_mnt_list = ut_g_strsplit_v1(org_power_mnt_str, " ", 0);
  
  /* compare the two lists and look for a match */
  org_power_mntner_found = FALSE;
  for (power_mnt_idx = 0; org_power_mnt_list[power_mnt_idx] != NULL && !org_power_mntner_found; power_mnt_idx++)
  {
    g_strstrip(org_power_mnt_list[power_mnt_idx]);
    for (p=mntners; (p != NULL) && !org_power_mntner_found; p = g_list_next(p))
    {
      LG_log(au_context, LG_DEBUG, "au_has_org_power_mnt: comparing mnt-by: %s with POWER_ORG_MNT %s",
        rpsl_attr_get_value(p->data), org_power_mnt_list[power_mnt_idx]);
      if (strcasecmp(rpsl_attr_get_value(p->data), org_power_mnt_list[power_mnt_idx]) == 0)
      {
        org_power_mntner_found = TRUE;
      }
    }
  }
  rpsl_attr_delete_list(mntners);
  g_strfreev(org_power_mnt_list);

  LG_log(au_context, LG_FUNC, "<au_has_org_power_mnt: exiting with value [%s]",
    org_power_mntner_found ? "TRUE" : "FALSE");
  return org_power_mntner_found;
}


/* creation rules for a organisation object */
AU_ret_t 
org_creation (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val, org_type_check;
  gboolean override;
  GList *org_type_attr;
  char *org_type;

  LG_log(au_context, LG_FUNC, ">org_creation: entering");

  /* only the mntner that are listed in POWER_ORG_MNT config variable can
     create org objects with an "org-type:" other than 'NON-REGISTRY' */
  org_type_check = AU_UNAUTHORISED_CONT;
  org_type_attr = rpsl_object_get_attr(info->obj, "org-type");
  if (org_type_attr == NULL)
  {
    org_type_check = AU_ERROR;
    LG_log(au_context, LG_ERROR, 
           "object_creation: missing \"org-type:\" attribute");
  }
  else
  {
    org_type = rpsl_attr_get_clean_value(org_type_attr->data);
    g_strup(org_type); 
    if(strncmp(org_type, "NON-REGISTRY", strlen("NON-REGISTRY")) == 0){
      org_type_check = AU_AUTHORISED;
    }else{
      /* if the org-type is not 'NON-REGISTRY', then we need to
         compare the "mnt-by:" attributes to the ORG_POWER_MNT config variable */
      if(au_has_org_power_mnt(info->obj) == TRUE){
        org_type_check = AU_AUTHORISED;
      };
    }

  }

  if((org_type_check == AU_UNAUTHORISED_CONT) && 
     (ret_val != AU_ERROR) && (ret_val != AU_UNAUTHORISED_END)){
    ret_val = AU_UNAUTHORISED_CONT;
  }

  
  ret_val = org_type_check;

  /* override if necessary */
  au_override(&ret_val, &override, info);

  /* report result */
  RT_org_creation_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<org_creation: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

/* au_org_authenticate */

static AU_ret_t
au_org_authenticate (RT_context_t *ctx, const gchar *org_id, LU_server_t *lu,
                     const gchar *source_name, GList *cred,
                     rpsl_object_t **organisation)
{
  AU_ret_t ret_val;
  GList *mnt_ref_attrs;
  GList *p;
  rpsl_object_t *mntner;
  char *mntner_name;

  LG_log(au_context, LG_FUNC, ">au_org_authenticate: entering");

  LG_log(au_context, LG_DEBUG,
         "au_org_authenticate: checking org %s", org_id);

  /* try looking up the organisation */
  if (LU_lookup(lu, organisation, "organisation", org_id, source_name) == LU_ERROR)
  {
    /* lookup failed */
    LG_log(au_context, LG_WARN,
           "au_org_authenticate: error looking up organisation %s", org_id);
    ret_val = AU_ERROR;
  }
  else
  {
    /* lookup worked */
    if (*organisation == NULL)
    {
      /* no such org */
      LG_log(au_context, LG_WARN,
             "au_org_authenticate: non-existent organisation %s", org_id);
      RT_non_exist_org(ctx, org_id);
      ret_val = AU_UNAUTHORISED_CONT;
    }
    else
    {
      /* organisation exists - check the mntners in "mnt-ref:" attributes */
      mnt_ref_attrs = rpsl_object_get_attr(*organisation, "mnt-ref");
      rpsl_attr_split_multiple(&mnt_ref_attrs);
      ret_val = AU_UNAUTHORISED_CONT;
      /* loop through these maintainers */
     for (p=mnt_ref_attrs; p != NULL; p = g_list_next(p))
     {
        mntner_name = rpsl_attr_get_clean_value(p->data);
    
        if(au_mntner_authenticate(ctx, mntner_name, lu, 
                        source_name, cred, 
                        &mntner) == AU_AUTHORISED){
          ret_val = AU_AUTHORISED;
        }
     }
 
    }
  }

  LG_log(au_context, LG_FUNC,
         "<au_org_authenticate: exiting with value [%s]",
         AU_ret2str(ret_val));

  return ret_val;
}


/* object_creation function is the plug-in function to be called to check when an object
   is added to the database.  This function will check the auth using "org:" attributes
   of the object */
AU_ret_t
object_creation (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  gboolean override;

  GList *org;
  GList *p;

  GList *source_attr;
  char *source;

  char *key;
  char *type;
  char *value;
  int num_error;

  GList *org_used_ok = NULL;
  GList *org_used_fail = NULL;
  rpsl_object_t *organisation;


  LG_log(au_context, LG_FUNC, ">object_creation: entering");

  /* grab key */
  key = rpsl_object_get_key_value(info->obj);
  type = (char*)rpsl_object_get_class(info->obj);

  org = rpsl_object_get_attr(info->obj, "org");
  if (org == NULL)
  {
    ret_val = AU_AUTHORISED;
    LG_log(au_context, LG_DEBUG, "object_creation: no \"org:\"");
  }
  else
  {
    source_attr = rpsl_object_get_attr(info->obj, "source");
    if (source_attr == NULL)
    {
      ret_val = AU_ERROR;
      LG_log(au_context, LG_ERROR, 
             "object_creation: missing \"source:\" attribute");
    }
    else
    {
      source = rpsl_attr_get_clean_value(source_attr->data);

      ret_val = AU_AUTHORISED;
      num_error = 0;
      rpsl_attr_split_multiple(&org);
      for (p=org; p != NULL; p = g_list_next(p))
      {
        value = rpsl_attr_get_clean_value(p->data);
        g_strup(value);
        LG_log(au_context, LG_DEBUG, 
               "object_creation: checking \"org:\" for %s", value);

        switch (au_org_authenticate(info->ctx, value, au_lookup, source, info->cred, &organisation))
        {
          case AU_AUTHORISED:
            org_used_ok = g_list_append(org_used_ok, organisation);
            break;
          case AU_UNAUTHORISED_CONT:
            ret_val = AU_UNAUTHORISED_CONT;
            if (organisation != NULL) 
              org_used_fail = g_list_append(org_used_fail, organisation);
            break;
          case AU_ERROR:
            num_error++;
            if (organisation != NULL) 
              org_used_fail = g_list_append(org_used_fail, organisation);
            break;
          case AU_FWD:
            LG_log(au_context, LG_WARN,
              "object_creation: au_org_authenticate returned invalid value AU_FWD");
            ret_val = AU_ERROR;
            break;
          case AU_UNAUTHORISED_END:
            LG_log(au_context, LG_WARN,
              "object_creation: au_org_authenticate returned invalid value AU_UNAUTHORISED_END");
            ret_val = AU_ERROR;
            break;
        }

        UT_free(value);
      }

      if (num_error > 0)
      {
        ret_val = AU_ERROR;
      }

      UT_free(source);

      /* record maintainers used, attribute checked */
      RT_org_auth(info->ctx, key, type, "org", org_used_ok, org_used_fail);
    }
    rpsl_attr_delete_list(org);
  }

  /* override if necessary */
  au_override(&ret_val, &override, info);

  /* report result */
  RT_org_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override, FALSE);

  LG_log(au_context, LG_FUNC, "<object_creation: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

/* object_modification function is the plug-in function to be used using the "org:" attributes
   of the object when it is modified */
AU_ret_t
object_modification (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  gboolean override;

  GList *org;
  GList *p;

  GList *source_attr;
  char *source;

  char *key;
  char *type;

  rpsl_object_t *old_object;
  GList *old_org;
  GHashTable *old_org_hash;
  char *val;

  GList *org_used_ok = NULL;
  GList *org_used_fail = NULL;
  rpsl_object_t *organisation;
  int num_error;
  int organisation_not_exist;
  gboolean organisation_type_check_failed = FALSE;
  AU_ret_t org_type_check;
  GList *org_type_attr;
  char *org_type;


  LG_log(au_context, LG_FUNC, ">object_modify: entering");

  /* grab key */
  key = rpsl_object_get_key_value(info->obj);
  type = (char*)rpsl_object_get_class(info->obj);


  /* see if there are any "org:" on the new object */
  org = rpsl_object_get_attr(info->obj, "org");
  if (org == NULL)
  {
    LG_log(au_context, LG_DEBUG, "object_modify: no \"org:\"");
    ret_val = AU_AUTHORISED;
  }

  /* get the old version of the object */
  if (!LU_get_object(au_lookup, &old_object, info->obj, NULL)) 
  {
    rpsl_attr_delete_list(org);
    LG_log(au_context, LG_ERROR, 
           "object_modify: error looking up old version");
    LG_log(au_context, LG_FUNC,
           "<object_modify: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }

  /* get the source */
  source_attr = rpsl_object_get_attr(old_object, "source");
  if (source_attr == NULL)
  {
    rpsl_attr_delete_list(org);
    LG_log(au_context, LG_ERROR, 
           "object_modify: missing \"source:\" attribute");
    LG_log(au_context, LG_FUNC,
           "<object_modify: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }
  source = rpsl_attr_get_clean_value(source_attr->data);
  rpsl_attr_delete_list(source_attr);


  /* put the "org:" of the old version into a hash */
  old_org_hash = g_hash_table_new(g_str_hash, g_str_equal);
  old_org = rpsl_object_get_attr(old_object, "org");
  for (p=old_org; p != NULL; p = g_list_next(p))
  {
    val = rpsl_attr_get_clean_value(p->data);
    g_strup(val);
    g_hash_table_insert(old_org_hash, val, "dummy");
    UT_free(val);
  }
  rpsl_attr_delete_list(old_org);

  /* check the "org:" present on the new version but not the old */
  /* org authorisation is only required when an org is added to an object */
  ret_val = AU_AUTHORISED;
  num_error = 0;
  organisation_not_exist = 0;
  for (p=org; p != NULL; p = g_list_next(p))
  {
    val = rpsl_attr_get_clean_value(p->data);
    g_strup(val);
    if (g_hash_table_lookup(old_org_hash, val) == NULL)
    {
      /* this org has been added */
      LG_log(au_context, LG_DEBUG, "object_modify: %s added", val);

      switch (au_org_authenticate(info->ctx, val, au_lookup, source, info->cred, &organisation))
      {
        case AU_AUTHORISED:
          org_used_ok = g_list_append(org_used_ok, organisation);
          break;
        case AU_UNAUTHORISED_CONT:
          ret_val = AU_UNAUTHORISED_CONT;
          if (organisation != NULL) 
            org_used_fail = g_list_append(org_used_fail, organisation);
          break;
        case AU_ERROR:
          num_error++;
          if (organisation != NULL) 
            org_used_fail = g_list_append(org_used_fail, organisation);
          break;
        case AU_FWD:
          LG_log(au_context, LG_WARN,
            "object_creation: au_org_authenticate returned invalid value AU_FWD");
          ret_val = AU_ERROR;
          break;
        case AU_UNAUTHORISED_END:
          LG_log(au_context, LG_WARN,
            "object_creation: au_org_authenticate returned invalid value AU_UNAUTHORISED_END");
          ret_val = AU_ERROR;
          break;
      }

      if (organisation == NULL)
      {
        /* There is an org added and it was checked 
           but the organisation object does not exist. 
           We should still report this one */
        organisation_not_exist++;
      }
    }
    UT_free(val);
  }    



  /* only the mntner that are listed in ORG_POWER_MNT config variable can
     create org objects with an "org-type:" other than 'NON-REGISTRY' */
  if(strncmp(type, "organisation", strlen("organisation")) == 0)
  {
   org_type_check = AU_UNAUTHORISED_CONT;
   org_type_attr = rpsl_object_get_attr(info->obj, "org-type");
   if (org_type_attr == NULL)
   {
     org_type_check = AU_ERROR;
     LG_log(au_context, LG_ERROR, 
            "object_creation: missing \"org-type:\" attribute");
   }
   else
   {
     org_type = rpsl_attr_get_clean_value(org_type_attr->data);
     g_strup(org_type); 
     if(strncmp(org_type, "NON-REGISTRY", strlen("NON-REGISTRY")) == 0){
       org_type_check = AU_AUTHORISED;
     }else{
       /* if the org-type is not 'NON-REGISTRY', then we need to
          compare the "mnt-by:" attributes to the ORG_POWER_MNT config variable */
       if(au_has_org_power_mnt(info->obj) == TRUE){
         org_type_check = AU_AUTHORISED;
       };
     }

   }

   if(org_type_check != AU_AUTHORISED){
     organisation_type_check_failed = TRUE;
   }

   if((org_type_check == AU_UNAUTHORISED_CONT) && 
      (ret_val != AU_ERROR) && (ret_val != AU_UNAUTHORISED_END)){
     ret_val = AU_UNAUTHORISED_CONT;
   }
  }
  

  /* clean up */
  rpsl_attr_delete_list(org);
  g_hash_table_destroy(old_org_hash);
  UT_free(source);

  if (num_error > 0)
  {
    ret_val = AU_ERROR;
  }


  /* only report authorisation info and check for override
     if an org has been added */
  if ( org_used_ok || org_used_fail || organisation_not_exist || organisation_type_check_failed)
  {
    /* record organisations used, attribute checked */
    RT_org_auth(info->ctx, key, type, "org", org_used_ok, org_used_fail);

    /* override if necessary */
    au_override(&ret_val, &override, info);

    /* report result */
    RT_org_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override, organisation_type_check_failed);
  }

  LG_log(au_context, LG_FUNC, "<object_modify: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

/* 
  Entry point for organisation object authorisation - invoke AU by-type support
  
  trans    - PG module transaction
  info     - AU plugin callback information

  return   - PG_OK means AU_AUTHORISED
             PG_ERROR_PROCEED means AU_UNAUTHORISED_CONT
             PG_ERROR_STOP means AU_UNAUTHORISED_END
             PG_ERROR_PLUGIN_NAME means AU_ERROR

  This is merely a dispatcher.
 */
PG_status_t 
au_organisation_check (PG_transaction_t *trans, gpointer *info)
{
  PG_status_t ret_val;
  au_plugin_callback_info_t *callback_info;
  AU_ret_t au_ret_val;


  LG_log(au_context, LG_FUNC, ">au_organisation_check: entering");

  callback_info = PG_global_get(trans);
  au_ret_val = AU_check_by_type(organisation_plugins, callback_info);
  switch (au_ret_val) 
  {
    case AU_AUTHORISED:
      ret_val = PG_OK;
      break;
    case AU_UNAUTHORISED_CONT:
      ret_val = PG_ERROR_PROCEED;
      callback_info->ret_val = au_ret_val;
      break;
    case AU_FWD:
      ret_val = PG_ERROR_STOP;
      callback_info->ret_val = au_ret_val;
      break;
    case AU_UNAUTHORISED_END:
      ret_val = PG_ERROR_STOP;
      callback_info->ret_val = au_ret_val;
      break;
    case AU_ERROR:
      ret_val = PG_ERROR_PLUGIN_NAME;
      callback_info->ret_val = au_ret_val;
      break;
    default:
      ret_val = PG_ERROR_PLUGIN_NAME;
      callback_info->ret_val = AU_ERROR;
      LG_log(au_context, LG_ERROR, "au_organisation_check: unknown return %d from AU_check_by_type()",
         au_ret_val);
  }

  LG_log(au_context, LG_FUNC, "<au_organisation_check: exiting with value [%d]", 
         ret_val);
  return ret_val;
}

gboolean
au_organisation_init ()
{
  return TRUE;
}

