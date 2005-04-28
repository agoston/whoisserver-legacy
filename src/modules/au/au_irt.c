#include "config.h"
#include <string.h>
#include "au_plugins.h"
#include "au_util.h"
#include "rt_dbupdate.h"

/*
#include <glib.h>
#include <stdio.h>
#include "ca_configFns.h"
#include "ca_defs.h"
#include "ca_dictionary.h"
#include "ca_macros.h"
*/

#undef MIN
#undef MAX
#include "memwrap.h"

AU_ret_t irt_creation(au_plugin_callback_info_t *info);
AU_ret_t irt_network_create(au_plugin_callback_info_t *info);
AU_ret_t irt_network_modify(au_plugin_callback_info_t *info);
AU_ret_t irt_allow(au_plugin_callback_info_t *info);


static const au_check_by_type_t irt_plugins[] = 
{
#ifndef AFRINIC
  { "irt", irt_creation, irt_allow, irt_allow },
  { "inetnum", irt_network_create, irt_allow, irt_network_modify },
  { "inet6num", irt_network_create, irt_allow, irt_network_modify },
#endif
  { NULL }
};

/* things that we don't care about, authorise */
AU_ret_t
irt_allow (au_plugin_callback_info_t *info)
{
  return AU_AUTHORISED;
}

AU_ret_t 
irt_creation (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  gboolean override;

  LG_log(au_context, LG_FUNC, ">irt_creation: entering");

  /* anyone can do it */
  ret_val = AU_AUTHORISED;
  au_override(&ret_val, &override, info);

  LG_log(au_context, LG_FUNC, "<irt_creation: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

static AU_ret_t
au_irt_authenticate (RT_context_t *ctx, const gchar *irt_name, LU_server_t *lu,
                     const gchar *source_name, GList *cred,
                     rpsl_object_t **irt)
{
  AU_ret_t ret_val;
  GList *auth_attrs;

  LG_log(au_context, LG_FUNC, ">au_irt_authenticate: entering");

  LG_log(au_context, LG_DEBUG,
         "au_irt_authenticate: checking irt %s", irt_name);

  /* try looking up the irt */
  if (LU_lookup(lu, irt, "irt", irt_name, source_name) == LU_ERROR)
  {
    /* lookup failed */
    LG_log(au_context, LG_WARN,
           "au_irt_authenticate: error looking up irt %s", irt_name);
    ret_val = AU_ERROR;
  }
  else
  {
    /* lookup worked */
    if (*irt == NULL)
    {
      /* no such irt */
      LG_log(au_context, LG_WARN,
             "au_irt_authenticate: non-existant irt %s", irt_name);
      RT_non_exist_irt(ctx, (char *)irt_name);
      ret_val = AU_UNAUTHORISED_CONT;
    }
    else
    {
      /* irt exists - check the "auth:" attributes */
      auth_attrs = rpsl_object_get_attr(*irt, "auth");
      rpsl_attr_split_multiple(&auth_attrs);
      ret_val = au_check_authentications(auth_attrs, cred);
    }
  }

  LG_log(au_context, LG_FUNC,
         "<au_irt_authenticate: exiting with value [%s]",
         AU_ret2str(ret_val));

  return ret_val;
}

AU_ret_t
irt_network_create (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  gboolean override;

  GList *mnt_irt;
  GList *p;

  GList *source_attr;
  char *source;

  char *key;
  gchar *type;
  char *value;
  int num_error;

  GList *irt_used_ok = NULL;
  GList *irt_used_fail = NULL;
  rpsl_object_t *irt;

  LG_log(au_context, LG_FUNC, ">irt_network_create: entering");

  /* grab key */
  key = rpsl_object_get_key_value(info->obj);
  type = (char *)rpsl_object_get_class(info->obj);

  mnt_irt = rpsl_object_get_attr(info->obj, "mnt-irt");
  if (mnt_irt == NULL)
  {
    ret_val = AU_AUTHORISED;
    LG_log(au_context, LG_DEBUG, "irt_network_create: no \"mnt-irt:\"");
  }
  else
  {
    source_attr = rpsl_object_get_attr(info->obj, "source");
    if (source_attr == NULL)
    {
      ret_val = AU_ERROR;
      LG_log(au_context, LG_ERROR, 
             "irt_network_create: missing \"source:\" attribute");
    }
    else
    {
      source = rpsl_attr_get_clean_value(source_attr->data);

      ret_val = AU_AUTHORISED;
      num_error = 0;
      rpsl_attr_split_multiple(&mnt_irt);
      for (p=mnt_irt; p != NULL && ret_val != AU_ERROR; p = g_list_next(p))
      {
        value = rpsl_attr_get_clean_value(p->data);
        g_strup(value);
        LG_log(au_context, LG_DEBUG, 
               "irt_network_create: checking \"mnt-irt:\" for %s", value);

        switch (au_irt_authenticate(info->ctx, value, au_lookup, source, info->cred, &irt))
        {
          case AU_AUTHORISED:
            irt_used_ok = g_list_append(irt_used_ok, irt);
            break;
          case AU_UNAUTHORISED_CONT:
            ret_val = AU_UNAUTHORISED_CONT;
            if (irt != NULL) 
              irt_used_fail = g_list_append(irt_used_fail, irt);
            break;
          case AU_ERROR:
            num_error++;
            if (irt != NULL) 
              irt_used_fail = g_list_append(irt_used_fail, irt);
            break;
          default:
            LG_log(au_context, LG_FUNC, "irt_network_create: au return code not recognised, exiting with value [AU_ERROR]");
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
      RT_irt_auth(info->ctx, key, type, "mnt-irt", irt_used_ok, irt_used_fail);
    }
    rpsl_attr_delete_list(mnt_irt);
  }

  /* override if necessary */
  au_override(&ret_val, &override, info);

  /* report result */
  RT_irt_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<irt_network_create: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

AU_ret_t
irt_network_modify (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  gboolean override;

  GList *mnt_irt;
  GList *p;

  GList *source_attr;
  char *source;

  char *key;
  gchar *type;

  rpsl_object_t *old_object;
  GList *old_mnt_irt;
  GHashTable *old_mnt_irt_hash;
  char *val;

  GList *irt_used_ok = NULL;
  GList *irt_used_fail = NULL;
  rpsl_object_t *irt;
  int num_error;
  int irt_not_exist;

  LG_log(au_context, LG_FUNC, ">irt_network_modify: entering");

  /* grab key */
  key = rpsl_object_get_key_value(info->obj);
  type = (char *)rpsl_object_get_class(info->obj);

  /* see if there are any "mnt-irt:" on the new object */
  mnt_irt = rpsl_object_get_attr(info->obj, "mnt-irt");
  if (mnt_irt == NULL)
  {
    LG_log(au_context, LG_DEBUG, "irt_network_modify: no \"mnt-irt:\"");
    LG_log(au_context, LG_FUNC,
           "<irt_network_modify: exiting with value [AU_AUTHORISED]");
    return AU_AUTHORISED;
  }

  /* get the old version of the object */
  if (!LU_get_object(au_lookup, &old_object, info->obj, NULL)) 
  {
    rpsl_attr_delete_list(mnt_irt);
    LG_log(au_context, LG_ERROR, 
           "irt_network_modify: error looking up old version");
    LG_log(au_context, LG_FUNC,
           "<irt_network_modify: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }
  
  /* get the source */
  source_attr = rpsl_object_get_attr(old_object, "source");
  if (source_attr == NULL)
  {
    rpsl_attr_delete_list(mnt_irt);
    LG_log(au_context, LG_ERROR, 
           "irt_network_create: missing \"source:\" attribute");
    LG_log(au_context, LG_FUNC,
           "<irt_network_modify: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }
  source = rpsl_attr_get_clean_value(source_attr->data);
  rpsl_attr_delete_list(source_attr);


  /* put the "mnt-irt:" of the old version into a hash */
  old_mnt_irt_hash = g_hash_table_new(g_str_hash, g_str_equal);
  old_mnt_irt = rpsl_object_get_attr(old_object, "mnt-irt");
  for (p=old_mnt_irt; p != NULL; p = g_list_next(p))
  {
    val = rpsl_attr_get_clean_value(p->data);
    g_strup(val);
    g_hash_table_insert(old_mnt_irt_hash, val, "dummy");
  }
  rpsl_attr_delete_list(old_mnt_irt);

  /* check the "mnt-irt:" present on the new version but not the old */
  /* mnt-irt authorisation is only required when an mnt-irt is added to an object */
  ret_val = AU_AUTHORISED;
  num_error = 0;
  irt_not_exist = 0;
  for (p=mnt_irt; p != NULL && ret_val != AU_ERROR; p = g_list_next(p))
  {
    val = rpsl_attr_get_clean_value(p->data);
    g_strup(val);
    if (g_hash_table_lookup(old_mnt_irt_hash, val) == NULL)
    {
      /* this mnt-irt has been added */
      LG_log(au_context, LG_DEBUG, "irt_network_modify: %s added", val);

      switch (au_irt_authenticate(info->ctx, val, au_lookup, source, info->cred, &irt))
      {
        case AU_AUTHORISED:
          irt_used_ok = g_list_append(irt_used_ok, irt);
          break;
        case AU_UNAUTHORISED_CONT:
          ret_val = AU_UNAUTHORISED_CONT;
          if (irt != NULL) 
            irt_used_fail = g_list_append(irt_used_fail, irt);
          break;
        case AU_ERROR:
          num_error++;
          if (irt != NULL) 
            irt_used_fail = g_list_append(irt_used_fail, irt);
          break;
          default:
            LG_log(au_context, LG_FUNC, "irt_network_modify: au return code not recognised, exiting with value [AU_ERROR]");
            ret_val = AU_ERROR;
            break;
      }
      if (irt == NULL)
      {
        /* There is an mnt-irt added and it was checked 
           but the irt object does not exist. 
           We should still report this one */
        irt_not_exist++;
      }
    }
    UT_free(val);
  }    

  rpsl_attr_delete_list(mnt_irt);
  g_hash_table_destroy(old_mnt_irt_hash);
  UT_free(source);

  if (num_error > 0)
  {
    ret_val = AU_ERROR;
  }

  /* only report authorisation info and check for override
     if an mnt-irt has been added */
  if ( irt_used_ok || irt_used_fail || irt_not_exist )
  {
    /* record maintainers used, attribute checked */
    RT_irt_auth(info->ctx, key, type, "mnt-irt", irt_used_ok, irt_used_fail);

    /* override if necessary */
    au_override(&ret_val, &override, info);

    /* report result */
    RT_irt_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);
  }

  LG_log(au_context, LG_FUNC, "<irt_network_modify: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

/* 
  Entry point for irt authorisation - invoke AU by-type support
  
  trans    - PG module transaction
  info     - AU plugin callback information

  return   - PG_OK means AU_AUTHORISED
             PG_ERROR_PROCEED means AU_UNAUTHORISED_CONT
             PG_ERROR_STOP means AU_UNAUTHORISED_END
             PG_ERROR_PLUGIN_NAME means AU_ERROR

  This is merely a dispatcher.
 */
PG_status_t 
au_irt_check (PG_transaction_t *trans, gpointer *info)
{
  PG_status_t ret_val;
  au_plugin_callback_info_t *callback_info;
  AU_ret_t au_ret_val;

  LG_log(au_context, LG_FUNC, ">au_irt_check: entering");

  callback_info = PG_global_get(trans);
  au_ret_val = AU_check_by_type(irt_plugins, callback_info);
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
      LG_log(au_context, LG_ERROR, "au_irt_check: unknown return %d from AU_check_by_type()",
         au_ret_val);
  }

  LG_log(au_context, LG_FUNC, "<au_irt_check: exiting with value [%d]", 
         ret_val);
  return ret_val;
}

gboolean
au_irt_init ()
{
  return TRUE;
}

