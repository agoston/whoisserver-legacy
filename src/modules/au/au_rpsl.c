/* XXX: a lot of duplicated code can probably be reduced */

#include <glib.h>

#undef MIN
#undef MAX

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "au_plugins.h"
#include "au_util.h"
#include "rt_dbupdate.h"
#include "iproutines.h"
#ifdef RDNSYES
#include "ns_util.h"
#else
#define ns_is_rdns_suffix(x) (-1)
#endif
#include "memwrap.h"

AU_ret_t generic_rpsl_create(au_plugin_callback_info_t *info);
AU_ret_t generic_rpsl_delete(au_plugin_callback_info_t *info);
AU_ret_t generic_rpsl_modify(au_plugin_callback_info_t *info);

AU_ret_t banned_create(au_plugin_callback_info_t *info);
AU_ret_t banned_delete(au_plugin_callback_info_t *info);
AU_ret_t banned_modify(au_plugin_callback_info_t *info);

AU_ret_t restricted_rpsl_create(au_plugin_callback_info_t *info);
AU_ret_t restricted_rpsl_delete(au_plugin_callback_info_t *info);
AU_ret_t restricted_rpsl_modify(au_plugin_callback_info_t *info);

AU_ret_t mntner_rpsl_create(au_plugin_callback_info_t *info);
AU_ret_t set_rpsl_create(au_plugin_callback_info_t *info);
AU_ret_t aut_num_rpsl_create(au_plugin_callback_info_t *info);
AU_ret_t hierarchical_rpsl_create(au_plugin_callback_info_t *info);
AU_ret_t route_rpsl_create(au_plugin_callback_info_t *info);
AU_ret_t route6_rpsl_create(au_plugin_callback_info_t *info);

static const au_check_by_type_t rpsl_plugins[] = 
{
  /* 3.1.2 restricted classes */
  { "as-block", restricted_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
#ifdef AFRINIC
  { "organisation", restricted_rpsl_create, restricted_rpsl_delete, restricted_rpsl_modify },
#endif
  /* mntner */
  { "mntner", mntner_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  /* 3.1.3 hierarchical name classes */
  { "as-set", set_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  { "filter-set", set_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  { "peering-set", set_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  { "route-set", set_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  { "rtr-set", set_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  { "domain", hierarchical_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  /* 3.1.4 aut-num */
  { "aut-num", aut_num_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  /* 3.1.6 inetnum and inet6num */
  { "inetnum", hierarchical_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  { "inet6num", hierarchical_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  /* 3.1.7 route */
#ifdef AFRINIC
  { "route", banned_create, banned_delete, banned_modify },
  /* RPSLng rule */
  { "route6", banned_create, banned_delete, banned_modify },
#elif
  { "route", route_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  /* RPSLng rule */
  { "route6", route6_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
#endif
#ifdef AFRINIC
  /* irt */
  { "irt", banned_create, banned_delete, banned_modify },
#endif
  /* 3.1.1 simple rules */
  { "*", generic_rpsl_create, generic_rpsl_delete, generic_rpsl_modify },
  { NULL }
};

static void au_rpsl_object_delete (gpointer data, gpointer user_data)
{
    rpsl_object_delete((rpsl_object_t *)data);
}

static char *CHECK_MNT_BY[]                 = { "mnt-by", NULL };
static char *CHECK_MNT_LOWER[]              = { "mnt-lower", NULL };
static char *CHECK_MNT_LOWER_THEN_MNT_BY[]  = { "mnt-lower", "mnt-by", NULL };
static char *CHECK_MNT_ROUTES_THEN_MNT_BY[] = { "mnt-routes", "mnt-by", NULL };
static char *CHECK_MNT_ROUTES_THEN_MNT_LOWER_THEN_MNT_BY[] = { "mnt-routes", 
                                                               "mnt-lower",
                                                               "mnt-by", 
                                                               NULL };

AU_ret_t
banned_operation (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  gboolean override;
  char *type = NULL;

  LG_log(au_context, LG_FUNC, ">banned_operation: entering");

  au_override(&ret_val, &override, info);
  ret_val = AU_UNAUTHORISED_CONT;

  type = strdup( rpsl_object_get_class(info->obj) );
  RT_banned_operation(info->ctx, type);
  UT_free(type);

  /* report result */
  RT_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<banned_operation: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

AU_ret_t
banned_create (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;

  LG_log(au_context, LG_FUNC, ">banned_create: entering");

  ret_val = banned_operation(info);
  
  LG_log(au_context, LG_FUNC, "<banned_create: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

AU_ret_t
banned_delete (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;

  LG_log(au_context, LG_FUNC, ">banned_delete: entering");

  ret_val = banned_operation(info);

  LG_log(au_context, LG_FUNC, "<banned_delete: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

AU_ret_t
banned_modify (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;

  LG_log(au_context, LG_FUNC, ">banned_modify: entering");

  ret_val = banned_operation(info);

  LG_log(au_context, LG_FUNC, "<banned_modify: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

AU_ret_t
restricted_rpsl_operation (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  gboolean override;
  char *type = NULL;
  char *key = NULL;
  char *mnt_type = NULL;

  LG_log(au_context, LG_FUNC, ">restricted_rpsl_operation: entering");

  /* only override works*/
  ret_val = AU_FWD;
  au_override(&ret_val, &override, info);

  if ( db_test_mode )
  {
    LG_log(au_context, LG_DEBUG, "restricted_rpsl_operation: test mode - authorised");
    ret_val = AU_AUTHORISED;
  }

  type = strdup( rpsl_object_get_class(info->obj) );
  key = strdup( rpsl_object_get_key_value(info->obj) );
  if ( override == TRUE )
  {
    LG_log(au_context, LG_DEBUG, "restricted_rpsl_operation: override - forward");
    mnt_type = g_malloc0(strlen("override") + 1);
    strcat(mnt_type, "override");
    RT_auth(info->ctx, key, type, mnt_type, NULL, NULL, "");
    g_free(mnt_type);
  }
  else if ( db_test_mode )
  {
    mnt_type = g_malloc0(strlen("test mode") + 1);
    strcat(mnt_type, "test mode");
    RT_auth(info->ctx, key, type, mnt_type, NULL, NULL, "");
    g_free(mnt_type);
  }
  UT_free(key);
  UT_free(type);
  /* report result */
  RT_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<restricted_rpsl_operation: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

AU_ret_t
restricted_rpsl_create (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;

  LG_log(au_context, LG_FUNC, ">restricted_rpsl_create: entering");

  ret_val = restricted_rpsl_operation(info);
  
  LG_log(au_context, LG_FUNC, "<restricted_rpsl_create: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

AU_ret_t
restricted_rpsl_delete (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;

  LG_log(au_context, LG_FUNC, ">restricted_rpsl_delete: entering");

  ret_val = restricted_rpsl_operation(info);

  LG_log(au_context, LG_FUNC, "<restricted_rpsl_delete: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

AU_ret_t
restricted_rpsl_modify (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;

  LG_log(au_context, LG_FUNC, ">restricted_rpsl_modify: entering");

  ret_val = restricted_rpsl_operation(info);

  LG_log(au_context, LG_FUNC, "<restricted_rpsl_modify: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

AU_ret_t
mntner_rpsl_create (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  gboolean override;
  char *type = NULL;
  char *key = NULL;
  char *mnt_type = NULL;

  LG_log(au_context, LG_FUNC, ">mntner_rpsl_create: entering");

  /* Currently all mntner objects are authorised without any checks. 
     This is a transient situation to remove mntners from the list of
     manually created objects by ripe-dbm.
     Authorisation INFO will be "default" in the ack message.
     At a later date the mntner's own authorisation will be checked. */

  ret_val = AU_AUTHORISED;
  au_override(&ret_val, &override, info);

  type = strdup( rpsl_object_get_class(info->obj) );
  key = strdup( rpsl_object_get_key_value(info->obj) );

  mnt_type = g_malloc0(strlen("default") + 1);
  strcat(mnt_type, "default");
//  RT_auth(info->ctx, key, type, mnt_type, NULL, NULL, "");
  g_free(mnt_type);

  UT_free(key);
  UT_free(type);

  /* report result */
  RT_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<mntner_rpsl_create: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

AU_ret_t
set_rpsl_create (au_plugin_callback_info_t *info)
{
  AU_ret_t parent_auth;
  AU_ret_t child_auth;
  AU_ret_t ret_val;
  gboolean override;

  const gchar *object_type;

  GList *attr;
  gchar *set_name;

  GList *parents;
  rpsl_object_t *parent;
  char *parent_key;

  LG_log(au_context, LG_FUNC, ">set_rpsl_create: entering");

  /* get set name */
  object_type = rpsl_object_get_class(info->obj);
  attr = rpsl_object_get_attr(info->obj, object_type);
  assert(attr != NULL);
  set_name = rpsl_attr_get_clean_value(attr->data);
  rpsl_attr_delete_list(attr);

  LG_log(au_context, LG_DEBUG, "set_rpsl_create: set is [%s]", set_name);

  /* check for parent authorisation */
  if (strchr(set_name, ':') == NULL) 
  {
    /* not hierarchical, implicit parental authorisation */
    parent_auth = AU_AUTHORISED;
    LG_log(au_context, LG_DEBUG, "set_rpsl_create: non-hierarchical set");
  } 
  else 
  {
    /* hierarchical, get parent */
    if (LU_get_parents(au_lookup, &parents, info->obj, NULL) != LU_OKAY) 
    {
      parent_auth = AU_ERROR;
    } 
    else if (parents == NULL) 
    {
      /* parent does not exist */
      parent_auth = AU_UNAUTHORISED_CONT;
      RT_parent_not_exist(info->ctx);
      LG_log(au_context, LG_DEBUG, "set_rpsl_create: no parent, invalid name");
    } 
    else 
    {
      assert(g_list_next(parents) == NULL);  /* must only be a single parent */

      parent = parents->data;
      parent_key = rpsl_object_get_key_value(parent);
      LG_log(au_context, LG_DEBUG, "set_rpsl_create: parent is [%s]", parent_key);
      UT_free(parent_key);

      parent_auth = au_check_multiple_authentications(CHECK_MNT_LOWER_THEN_MNT_BY, 
                                          parent, "parent", info);
    }
  }

  LG_log(au_context, LG_DEBUG, "set_rpsl_create: parent_auth is [%s]",
         AU_ret2str(parent_auth));

  UT_free(set_name);

  /* now check the "mnt-by:" in the object itself */
  child_auth = au_check_multiple_authentications(CHECK_MNT_BY, 
                                     info->obj, "", info);

  LG_log(au_context, LG_DEBUG, "set_rpsl_create: child_auth is [%s]",
         AU_ret2str(child_auth));

  /* determine authorisation based on the parent and child */
  if ((parent_auth == AU_ERROR) || (child_auth == AU_ERROR)) 
  {
    ret_val = AU_ERROR;
  } 
  else if ((parent_auth == AU_AUTHORISED) && (child_auth == AU_AUTHORISED)) 
  {
    ret_val = AU_AUTHORISED;
  } 
  else 
  {
    ret_val = AU_UNAUTHORISED_CONT;
  }

  /* check for override condition */
  au_override(&ret_val, &override, info);

  /* return maintainers, or free maintainers */
  LG_log(au_context, LG_DEBUG,"set_rpsl_create: [%d] mntners used", g_list_length(info->mntner_used));
  if (ret_val == AU_ERROR) 
  {
      g_list_foreach(info->mntner_used, au_rpsl_object_delete, NULL);
      info->mntner_used = NULL;
  }

  /* report result */
  RT_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<set_rpsl_create: exiting with value [%s]", 
         AU_ret2str(ret_val));

  return ret_val;
}

AU_ret_t
hierarchical_rpsl_create (au_plugin_callback_info_t *info)
{
  AU_ret_t parent_auth;
  AU_ret_t child_auth;
  AU_ret_t ret_val;
  gboolean override;

  char *key;

  GList *parents;
  rpsl_object_t *parent;
  char *parent_key;
  const rpsl_attr_t *first_attr;

  LG_log(au_context, LG_FUNC, ">hierarchical_rpsl_create: entering");

  key = rpsl_object_get_key_value(info->obj);
  LG_log(au_context, LG_DEBUG, "hierarchical_rpsl_create: key is [%s]", key);
  UT_free(key);

  /* this is ugly, needs definite fix TODO */
  /* check auth from corresponding inetnum */
  first_attr = rpsl_object_get_attr_by_ofs(info->obj,0);
  if ((strcmp(first_attr->name,"domain")==0)&&(ns_is_rdns_suffix(info))) {
    LG_log(au_context,LG_DEBUG,"hierarchical_rpsl_create: domain object encountered, skipping");
    ret_val = AU_AUTHORISED;
    override = FALSE;
    RT_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);
    return ret_val;
  }

  /* check for parent authorisation */
  if (LU_get_parents(au_lookup, &parents, info->obj, NULL) != LU_OKAY) 
  {
    /* error finding parent */
    parent_auth = AU_ERROR;
  }
  else if (parents == NULL)
  {
    /* no parent */
    parent_auth = AU_UNAUTHORISED_CONT;
    RT_parent_not_exist(info->ctx);
    LG_log(au_context, LG_DEBUG, "hierarchical_rpsl_create: no parent found");
  }
  else if (g_list_next(parents) != NULL)
  {
    /* multiple parents - e.g. overlapping inetnum objects */
    /* we will still do the auth checks on all the parents so that
       the lists of mntners (authenticated, unauthenticated) are built
       up for the notifications to be sent to */

    LG_log(au_context, LG_DEBUG, "hierarchical_rpsl_create: multiple parents found");
    RT_multiple_parents(info->ctx, parents);

    while ( parents != NULL)
    {
      parent = parents->data;
      parent_key = rpsl_object_get_key_value(parent);
      LG_log(au_context, LG_DEBUG, "hierarchical_rpsl_create: parent is [%s]", parent_key);
      UT_free(parent_key);

      parent_auth = au_check_multiple_authentications(CHECK_MNT_LOWER_THEN_MNT_BY, 
                                          parent, "parent", info);
      
      parents = g_list_next(parents);
    }
    
    /* set parent_auth to AU_UNAUTHORISED_CONT, regardless of the result */
    parent_auth = AU_UNAUTHORISED_CONT;
  }
  else
  {
    parent = parents->data;
    parent_key = rpsl_object_get_key_value(parent);
    LG_log(au_context, LG_DEBUG, "hierarchical_rpsl_create: parent is [%s]", parent_key);
    UT_free(parent_key);

    parent_auth = au_check_multiple_authentications(CHECK_MNT_LOWER_THEN_MNT_BY, 
                                          parent, "parent", info);
  }

  LG_log(au_context, LG_DEBUG, "hierarchical_rpsl_create: parent_auth is [%s]",
         AU_ret2str(parent_auth));

  /* now check the "mnt-by:" in the object itself */
  child_auth = au_check_multiple_authentications(CHECK_MNT_BY, 
                                     info->obj, "", info);

  LG_log(au_context, LG_DEBUG, "hierarchical_rpsl_create: child_auth is [%s]",
         AU_ret2str(child_auth));

  /* determine authorisation based on the parent and child */
  if ((parent_auth == AU_ERROR) || (child_auth == AU_ERROR)) 
  {
    ret_val = AU_ERROR;
  } 
  else if ((parent_auth == AU_AUTHORISED) && (child_auth == AU_AUTHORISED)) 
  {
    ret_val = AU_AUTHORISED;
  } 
  else 
  {
    ret_val = AU_UNAUTHORISED_CONT;
  }

  au_override(&ret_val, &override, info);

  /* return maintainers, or free maintainers */
  LG_log(au_context, LG_DEBUG,"hierarchical_rpsl_create: [%d] mntners used", g_list_length(info->mntner_used));
  if (ret_val == AU_ERROR) 
  {
      g_list_foreach(info->mntner_used, au_rpsl_object_delete, NULL);
      info->mntner_used = NULL;
  }

  /* report result */
  RT_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<hierarchical_rpsl_create: exiting with value [%s]", 
         AU_ret2str(ret_val));

  return ret_val;
}

AU_ret_t
route_rpsl_create (au_plugin_callback_info_t *info)
{
  AU_ret_t parent_auth;
  AU_ret_t aut_num_auth;
  AU_ret_t child_auth;
  AU_ret_t ret_val;
  gboolean override;
  int ip_retval;

  char *key;
  const char *type;
  int exact = FALSE;

  GList *parents;
  GList *p;
  const rpsl_attr_t *attr;
  const char *attr_name;
  rpsl_object_t *parent;
  rpsl_object_t *aut_num;
  char *parent_key;
  char *aut_num_key;
  char *parent_text;
  char *attr_value;
  char *parent_attr_value;

  ip_prefix_t prefptr;
  ip_range_t rangptr;
  char range_str[50];

  LG_log(au_context, LG_FUNC, ">route_rpsl_create: entering");

  key = rpsl_object_get_key_value(info->obj);
  LG_log(au_context, LG_DEBUG, "route_rpsl_create: key is [%s]", key);
  UT_free(key);

  /* check for parent authorisation */
  if (LU_get_parents(au_lookup, &parents, info->obj, NULL) != LU_OKAY) 
  {
    parent_auth = AU_ERROR;
    aut_num_auth = AU_ERROR;
  }
  else 
  {
    parent = NULL;
    aut_num = NULL;
    for (p=parents; p != NULL; p = g_list_next(p))
    {
      attr = rpsl_object_get_attr_by_ofs(p->data, 0);
      attr_name = rpsl_attr_get_name(attr);

      /* parent is route if exists, else inetnum */
      if (strcmp(attr_name, "route") == 0)
      {
        parent = p->data;
      }
      else if (strcmp(attr_name, "inetnum") == 0)
      {
        if (parent == NULL)
        {
          parent = p->data;
        }
      }
      else if (strcmp(attr_name, "aut-num") == 0)
      {
        aut_num = p->data;
      }
      else
      {
        LG_log(au_context, LG_ERROR, "route_rpsl_create: unknown parent type %s",
               attr_name);
      }
    }

    if (parent == NULL)
    {
      parent_auth = AU_UNAUTHORISED_CONT;
      RT_parent_not_exist(info->ctx);
      LG_log(au_context, LG_DEBUG, "route_rpsl_create: no IP parent found");
    }
    else
    {
      parent_key = rpsl_object_get_key_value(parent);
      type = rpsl_object_get_class(parent);
      LG_log(au_context, LG_DEBUG, "route_rpsl_create: parent is [%s] %s", type, parent_key);
      UT_free(parent_key);

      attr = rpsl_object_get_attr_by_ofs(info->obj, 0);
      assert(attr != NULL);
      attr_value = rpsl_attr_get_clean_value(attr);
      attr = rpsl_object_get_attr_by_ofs(parent, 0);
      assert(attr != NULL);
      parent_attr_value = rpsl_attr_get_clean_value(attr);

      /* check for exact match route prefix or inetnum */
      if ( ! strcmp(type, "route") )
      {
        if ( ! strcmp( attr_value, parent_attr_value ) )
          exact = TRUE;
      }
      else if ( ! strcmp(type, "inetnum") )
      {
        ip_retval = IP_pref_t2b(&prefptr, attr_value, IP_PLAIN);
        if ( ip_retval == IP_OK )
        {
          ip_retval = IP_pref_2_rang(&rangptr, &prefptr);
          if ( ip_retval == IP_OK )
          {
            ip_retval = IP_rang_b2a(&rangptr, range_str, 49);
            if ( ip_retval == IP_OK )
            {
              if ( ! strcmp( range_str, parent_attr_value ) )
                exact = TRUE;
            }
          }
        }
      }
      if ( ! exact )
      {
        /* the parent is a less specific route/inetnum */
        parent_text = "parent";

        parent_auth = au_check_multiple_authentications(CHECK_MNT_ROUTES_THEN_MNT_LOWER_THEN_MNT_BY,
                                            parent, parent_text, info);
      }
      else
      {
        /* the parent is an exact match route/inetnum */
        parent_text = "exact match";

        parent_auth = au_check_multiple_authentications(CHECK_MNT_ROUTES_THEN_MNT_BY,
                                            parent, parent_text, info);
      }
      free(attr_value);
      free(parent_attr_value);
    }

    if (aut_num == NULL)
    {
      aut_num_auth = AU_UNAUTHORISED_CONT;
      LG_log(au_context, LG_DEBUG, "route_rpsl_create: no aut-num found");
      RT_origin_not_exist(info->ctx);
    }
    else
    {
      aut_num_key = rpsl_object_get_key_value(aut_num);
      LG_log(au_context, LG_DEBUG, "route_rpsl_create: aut-num is [%s]", aut_num_key);
      UT_free(aut_num_key);

      aut_num_auth = au_check_multiple_authentications(CHECK_MNT_ROUTES_THEN_MNT_BY,
                                           aut_num, "origin", info);
    }
  }

  LG_log(au_context, LG_DEBUG, "route_rpsl_create: parent_auth is [%s]",
         AU_ret2str(parent_auth));
  LG_log(au_context, LG_DEBUG, "route_rpsl_create: aut_num_auth is [%s]",
         AU_ret2str(aut_num_auth));

  /* now check the "mnt-by:" in the object itself */
  child_auth = au_check_multiple_authentications(CHECK_MNT_BY, 
                                     info->obj, "", info);

  LG_log(au_context, LG_DEBUG, "route_rpsl_create: child_auth is [%s]",
         AU_ret2str(child_auth));

  /* determine authorisation based on the parent and child */
  if ((parent_auth == AU_ERROR) || 
      (aut_num_auth == AU_ERROR) ||
      (child_auth == AU_ERROR)) 
  {
    ret_val = AU_ERROR;
  } 
  else if ((parent_auth == AU_AUTHORISED) && 
           (aut_num_auth == AU_AUTHORISED) &&
           (child_auth == AU_AUTHORISED)) 
  {
    ret_val = AU_AUTHORISED;
  } 
  else 
  {
    ret_val = AU_UNAUTHORISED_CONT;
  }

  au_override(&ret_val, &override, info);

  /* return maintainers, or free maintainers */
  LG_log(au_context, LG_DEBUG,"route_rpsl_create: [%d] mntners used", g_list_length(info->mntner_used));
  if (ret_val == AU_ERROR) 
  {
      g_list_foreach(info->mntner_used, au_rpsl_object_delete, NULL);
      info->mntner_used = NULL;
  }

  /* report result */
  RT_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<route_rpsl_create: exiting with value [%s]", 
         AU_ret2str(ret_val));

  return ret_val;
}

AU_ret_t
aut_num_rpsl_create (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  gboolean override;
  AU_ret_t parent_auth;
  AU_ret_t child_auth;

  char *child_key;

  GList *parents;
  rpsl_object_t *parent;
  char *parent_key;

  LG_log(au_context, LG_FUNC, ">aut_num_rpsl_create: entering");

  child_key = rpsl_object_get_key_value(info->obj);
  LG_log(au_context, LG_DEBUG, "aut_num_rpsl_create: aut-num is [%s]", child_key);
  UT_free(child_key);

  /* get the key */
  if (LU_get_parents(au_lookup, &parents, info->obj, NULL) != LU_OKAY) 
  {
    parent_auth = AU_ERROR;
  } 
  else if (parents == NULL)
  {
    LG_log(au_context, LG_DEBUG, "aut_num_rpsl_create: no as-block");
    parent_auth = AU_UNAUTHORISED_CONT;
    RT_parent_not_exist(info->ctx);
  }
  else 
  {
    assert(g_list_next(parents) == NULL);  /* must only be a single parent */
    
    parent = parents->data;
    parent_key = rpsl_object_get_key_value(parent);
    LG_log(au_context, LG_DEBUG, "aut_num_rpsl_create: encompassing as-block is [%s]",
           parent_key);
    UT_free(parent_key);

    parent_auth = au_check_multiple_authentications(CHECK_MNT_LOWER_THEN_MNT_BY, 
                                          parent, "parent", info);
  }

  LG_log(au_context, LG_DEBUG, "aut_num_rpsl_create: parent_auth is [%s]",
         AU_ret2str(parent_auth));

  /* now check the "mnt-by:" in the object itself */
  child_auth = au_check_multiple_authentications(CHECK_MNT_BY, 
                                     info->obj, "", info);

  LG_log(au_context, LG_DEBUG, "aut_num_rpsl_create: child_auth is [%s]",
         AU_ret2str(child_auth));

  /* determine authorisation based on the parent and child */
  if ((parent_auth == AU_ERROR) || (child_auth == AU_ERROR)) 
  {
    ret_val = AU_ERROR;
  } 
  else if ((parent_auth == AU_AUTHORISED) && (child_auth == AU_AUTHORISED)) 
  {
    ret_val = AU_AUTHORISED;
  } 
  else 
  {
    ret_val = AU_UNAUTHORISED_CONT;
  }

  au_override(&ret_val, &override, info);

  /* return maintainers, or free maintainers */
  LG_log(au_context, LG_DEBUG,"aut_num_rpsl_create: [%d] mntners used", g_list_length(info->mntner_used));
  if (ret_val == AU_ERROR) 
  {
      g_list_foreach(info->mntner_used, au_rpsl_object_delete, NULL);
      info->mntner_used = NULL;
  }

  /* report result */
  RT_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<aut_num_rpsl_create: exiting with value [%s]", 
         AU_ret2str(ret_val));

  return ret_val;
}


AU_ret_t
route6_rpsl_create (au_plugin_callback_info_t *info)
{
  AU_ret_t parent_auth;
  AU_ret_t aut_num_auth;
  AU_ret_t child_auth;
  AU_ret_t ret_val;
  gboolean override;

  char *key;
  const char *type;
  int exact = FALSE;

  GList *parents;
  GList *p;
  const rpsl_attr_t *attr;
  const char *attr_name;
  rpsl_object_t *parent;
  rpsl_object_t *aut_num;
  char *parent_key;
  char *aut_num_key;
  char *parent_text;
  char *attr_value;
  char *parent_attr_value;


  LG_log(au_context, LG_FUNC, ">route6_rpsl_create: entering");

  key = rpsl_object_get_key_value(info->obj);
  LG_log(au_context, LG_DEBUG, "route6_rpsl_create: key is [%s]", key);
  UT_free(key);

  /* check for parent authorisation */
  if (LU_get_parents(au_lookup, &parents, info->obj, NULL) != LU_OKAY) 
  {
    parent_auth = AU_ERROR;
    aut_num_auth = AU_ERROR;
  }
  else 
  {
    parent = NULL;
    aut_num = NULL;
    for (p=parents; p != NULL; p = g_list_next(p))
    {
      attr = rpsl_object_get_attr_by_ofs(p->data, 0);
      attr_name = rpsl_attr_get_name(attr);

      /* parent is route6 if exists, else inet6num */
      if (strcmp(attr_name, "route6") == 0)
      {
        parent = p->data;
      }
      else if (strcmp(attr_name, "inet6num") == 0)
      {
        if (parent == NULL)
        {
          parent = p->data;
        }
      }
      else if (strcmp(attr_name, "aut-num") == 0)
      {
        aut_num = p->data;
      }
      else
      {
        LG_log(au_context, LG_ERROR, "route6_rpsl_create: unknown parent type %s",
               attr_name);
      }
    }

    if (parent == NULL)
    {
      parent_auth = AU_UNAUTHORISED_CONT;
      RT_parent_not_exist(info->ctx);
      LG_log(au_context, LG_DEBUG, "route6_rpsl_create: no IP parent found");
    }
    else
    {
      parent_key = rpsl_object_get_key_value(parent);
      type = rpsl_object_get_class(parent);
      LG_log(au_context, LG_DEBUG, "route6_rpsl_create: parent is [%s] %s", type, parent_key);
      UT_free(parent_key);

      attr = rpsl_object_get_attr_by_ofs(info->obj, 0);
      assert(attr != NULL);
      attr_value = rpsl_attr_get_clean_value(attr);
      attr = rpsl_object_get_attr_by_ofs(parent, 0);
      assert(attr != NULL);
      parent_attr_value = rpsl_attr_get_clean_value(attr);

      /* check for exact match route prefix or inetnum */
      if ( ! strcmp(type, "route6") )
      {
        if ( ! strcmp( attr_value, parent_attr_value ) )
        {
          exact = TRUE;
        }
      }
      else if ( ! strcmp(type, "inet6num") )
      {
        if ( ! strcmp( attr_value, parent_attr_value ) )
        {
          exact = TRUE;
        }
      }
      if ( ! exact )
      {
        /* the parent is a less specific route6/inet6num */
        parent_text = "parent";

        parent_auth = au_check_multiple_authentications(CHECK_MNT_ROUTES_THEN_MNT_LOWER_THEN_MNT_BY,
                                            parent, parent_text, info);
      }
      else
      {
        /* the parent is an exact match route6/inet6num */
        parent_text = "exact match";

        parent_auth = au_check_multiple_authentications(CHECK_MNT_ROUTES_THEN_MNT_BY,
                                            parent, parent_text, info);
      }
      free(attr_value);
      free(parent_attr_value);
    }

    if (aut_num == NULL)
    {
      aut_num_auth = AU_UNAUTHORISED_CONT;
      LG_log(au_context, LG_DEBUG, "route6_rpsl_create: no aut-num found");
      RT_origin_not_exist(info->ctx);
    }
    else
    {
      aut_num_key = rpsl_object_get_key_value(aut_num);
      LG_log(au_context, LG_DEBUG, "route6_rpsl_create: aut-num is [%s]", aut_num_key);
      UT_free(aut_num_key);

      aut_num_auth = au_check_multiple_authentications(CHECK_MNT_ROUTES_THEN_MNT_BY,
                                           aut_num, "origin", info);
    }
  }

  LG_log(au_context, LG_DEBUG, "route6_rpsl_create: parent_auth is [%s]",
         AU_ret2str(parent_auth));
  LG_log(au_context, LG_DEBUG, "route6_rpsl_create: aut_num_auth is [%s]",
         AU_ret2str(aut_num_auth));

  /* now check the "mnt-by:" in the object itself */
  child_auth = au_check_multiple_authentications(CHECK_MNT_BY, 
                                     info->obj, "", info);

  LG_log(au_context, LG_DEBUG, "route6_rpsl_create: child_auth is [%s]",
         AU_ret2str(child_auth));

  /* determine authorisation based on the parent and child */
  if ((parent_auth == AU_ERROR) || 
      (aut_num_auth == AU_ERROR) ||
      (child_auth == AU_ERROR)) 
  {
    ret_val = AU_ERROR;
  } 
  else if ((parent_auth == AU_AUTHORISED) && 
           (aut_num_auth == AU_AUTHORISED) &&
           (child_auth == AU_AUTHORISED)) 
  {
    ret_val = AU_AUTHORISED;
  } 
  else 
  {
    ret_val = AU_UNAUTHORISED_CONT;
  }

  au_override(&ret_val, &override, info);

  /* return maintainers, or free maintainers */
  LG_log(au_context, LG_DEBUG,"route6_rpsl_create: [%d] mntners used", g_list_length(info->mntner_used));
  if (ret_val == AU_ERROR) 
  {
      g_list_foreach(info->mntner_used, au_rpsl_object_delete, NULL);
      info->mntner_used = NULL;
  }

  /* report result */
  RT_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<route6_rpsl_create: exiting with value [%s]", 
         AU_ret2str(ret_val));

  return ret_val;
}


AU_ret_t 
generic_rpsl_create (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  gboolean override;
  const rpsl_attr_t *first_attr;

  LG_log(au_context, LG_FUNC, ">generic_rpsl_create: entering");

  /* this is ugly, needs definite fix TODO */
  /* check auth from corresponding inetnum */
  first_attr = rpsl_object_get_attr_by_ofs(info->obj,0);
  if ((strcmp(first_attr->name,"domain")==0)&&(ns_is_rdns_suffix(info))) {
    LG_log(au_context,LG_DEBUG,"generic_rpsl_create: domain object encountered, skipping");
    ret_val = AU_AUTHORISED;
    override = FALSE;
  } else {
    /* get the maintainers listed in the object, if any */
    ret_val = au_check_multiple_authentications(CHECK_MNT_BY, 
                                  info->obj, "", info);

    /* override if necessary */
    au_override(&ret_val, &override, info);

    LG_log(au_context, LG_DEBUG,"generic_rpsl_create: [%d] mntners used", g_list_length(info->mntner_used));
  }

  /* report result */
  RT_auth_result(info->ctx, (ret_val == AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<generic_rpsl_create: exiting with value [%s]", 
         AU_ret2str(ret_val));

  return ret_val;
}

AU_ret_t 
generic_rpsl_delete (au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  gboolean override;
  const rpsl_attr_t *first_attr;

  LG_log(au_context, LG_FUNC, ">generic_rpsl_delete: entering");

  /* this is ugly, needs definite fix TODO */
  /* check auth from corresponding inetnum */
  first_attr = rpsl_object_get_attr_by_ofs(info->obj,0);
  if ((strcmp(first_attr->name,"domain")==0)&&(ns_is_rdns_suffix(info))) {
    LG_log(au_context,LG_DEBUG,"generic_rpsl_delete: domain object encountered, skipping");
    ret_val = AU_AUTHORISED;
    override = FALSE;
  } else {

    /* get the maintainers listed in the object, if any */
    ret_val = au_check_multiple_authentications(CHECK_MNT_BY, 
                                  info->obj, "", info);

    au_override(&ret_val, &override, info);

    LG_log(au_context, LG_DEBUG,"generic_rpsl_delete: [%d] mntners used", g_list_length(info->mntner_used));
  }

  /* report result */
  RT_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<generic_rpsl_delete: exiting with value [%s]", 
       AU_ret2str(ret_val));

  return ret_val;
}

/* 
  Authentication check for generic RPSL modifies.
  
  info     - AU plugin callback information

  return   - AU_AUTHORISED, AU_UNAUTHORISED_CONT, or AU_ERROR

  This implements the semantics for changing an object.
 */
AU_ret_t 
generic_rpsl_modify (au_plugin_callback_info_t *info)
{
  rpsl_object_t *old_object;
  rpsl_object_t *object_to_use;

  AU_ret_t ret_val;
  gboolean override;

  LG_log(au_context, LG_FUNC, ">generic_rpsl_modify: entering");

  /* get the old version of the object */
  if (!LU_get_object(au_lookup, &old_object, info->obj, NULL)) 
  {
    LG_log(au_context, LG_ERROR, "generic_rpsl_modify: error looking up old version");
    LG_log(au_context, LG_FUNC, "<generic_rpsl_modify: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }
  if (old_object == NULL) 
  {
    LG_log(au_context, LG_ERROR, "generic_rpsl_modify: no old version");
    LG_log(au_context, LG_FUNC, "<generic_rpsl_modify: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }

  /* find which objects maintainers to use */
  if ( rpsl_object_get_attr(old_object, "mnt-by") ) 
  {
    /* if the old version has a "mnt-by:" attribute, use that */
    LG_log(au_context, LG_DEBUG, "generic_rpsl_modify: using \"mnt-by:\" of old object");
    object_to_use = old_object;
  } 
  else 
  {
    /* otherwise use the "mnt-by:" in the new object, if any */
    LG_log(au_context, LG_DEBUG, "generic_rpsl_modify: using \"mnt-by:\" of new object");
    object_to_use = info->obj;
  }

  ret_val = au_check_multiple_authentications(CHECK_MNT_BY, 
                                  object_to_use, "", info);

  /* memory cleanup */
  rpsl_object_delete(old_object);

  /* override if necessary */
  au_override(&ret_val, &override, info);

  /* report result */
  RT_auth_result(info->ctx, (ret_val==AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<generic_rpsl_modify: exiting with value [%s]",
         AU_ret2str(ret_val));

  return ret_val;
}


/* 
  Entry point for RPSL authentication - invoke AU by-type support
  
  trans    - PG module transaction
  info     - AU plugin callback information

  return   - PG_OK means AU_AUTHORISED
             PG_ERROR_PROCEED means AU_UNAUTHORISED_CONT
             PG_ERROR_STOP means AU_UNAUTHORISED_END
             PG_ERROR_PLUGIN_NAME means AU_ERROR

  This is merely a dispatcher.
 */
PG_status_t 
au_rpsl_check (PG_transaction_t *trans, gpointer *info)
{
  PG_status_t ret_val;
  au_plugin_callback_info_t *callback_info;
  AU_ret_t au_ret_val;
//  char *type;
//  int mntner_create;

  LG_log(au_context, LG_FUNC, ">au_rpsl_check: entering");

  callback_info = PG_global_get(trans);

  RT_start_detailed_report(callback_info->ctx);

  au_ret_val = AU_check_by_type(rpsl_plugins, callback_info);
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
      callback_info->ret_val = au_ret_val;
      LG_log(au_context, LG_ERROR, "au_rpsl_check: unknown return %d from AU_check_by_type()",
        au_ret_val);
  }

  /* This is a hack. RT_end_detailed_report will exit(1) if called
     after AU finds an error. It is not clear how to fix this in RT
     so we avoid calling this function. An internal error is generated
     by dbupdate if AU exits with AU_ERROR, so we get away with it. */

//  g_strdown( (type = g_strdup( rpsl_object_get_class(callback_info->obj) )) );
//  mntner_create = ( ! strcmp( type, "mntner" ) ) && (callback_info->type == AU_CREATE);
//  g_free(type);
//  if ( au_ret_val != AU_ERROR && ! mntner_create )
  if ( au_ret_val != AU_ERROR )
  {
    RT_end_detailed_report(callback_info->ctx);
  }

  LG_log(au_context, LG_FUNC, "<au_rpsl_check: exiting with value [%d]", 
         ret_val);
  return ret_val;
}

AU_ret_t au_check_mnt_routes_prefix (GList **mntners, rpsl_object_t *parent, char *parent_text, au_plugin_callback_info_t *info)
{
  GList *p;
  GList *q;
  GList *removed_link;
  GList *prefix_list;
  GList *invalid_list;
  GList *partial_invalid_list;
  GList *failed_mntners;
  int found;
  ip_prefix_t prefix;
  const gchar *route_text;
  const rpsl_attr_t *route;
  AU_ret_t ret_val = AU_UNAUTHORISED_CONT;
  
  p = *mntners;

  invalid_list = NULL;
  failed_mntners = NULL;

  LG_log(au_context, LG_INFO, ">au_check_mnt_routes_prefix: entering ");

  /* convert the prefix to internal representation */
  route = rpsl_object_get_attr_by_ofs(info->obj, 0);
  route_text = rpsl_attr_get_clean_value(route);
  IP_pref_t2b (&prefix, route_text, IP_PLAIN);

  while (p != NULL)
  {
    LG_log(au_context, LG_INFO, "au_check_mnt_routes_prefix: checking mnt-routes \"%s\"", rpsl_attr_get_clean_value(p->data));

    /* check if there is a list of prefixes in mnt-routes attribute */
    prefix_list = NULL;
    partial_invalid_list = NULL;
    found = 0;

    rpsl_mnt_routes_clean_split(p->data, &prefix_list, &partial_invalid_list);

    if (partial_invalid_list != NULL)
    {
      /* report error and delete this maintainer from the list */
      LG_log(au_context, LG_INFO, "au_check_mnt_routes_prefix: mntner [%s] excluded", 
                                   rpsl_attr_get_clean_value(p->data));
      LG_log(au_context, LG_INFO, "au_check_mnt_routes_prefix: mnt-routes contains invalid prefix(es)");

      /* prepare for RT reporting */
      invalid_list = g_list_concat(invalid_list, partial_invalid_list);
      failed_mntners = g_list_append(failed_mntners, p->data);

      /* remove the mntner from the list */
      removed_link = p;
      p = g_list_next(p);
      *mntners = g_list_remove_link(*mntners, removed_link);
      continue;
    }

    if (prefix_list != NULL)
    {
      /* check if the prefix is contained in prefix range list */
      q = prefix_list;
      while ((q != NULL) && (found != 1)) 
      {
        if (IP_pref_in_prefrang(&prefix, q->data))
        {
          found = 1;
        }
        else 
        {
          q = g_list_next(q);
        }
      }

      if (found != 1)
      {
        /* mntner is not applicate for this prefix */
        LG_log(au_context, LG_INFO, "au_check_mnt_routes_prefix: mntner [%s] excluded", rpsl_attr_get_clean_value(p->data));
        removed_link = p;
        p = g_list_next(p);
        *mntners = g_list_remove_link(*mntners, removed_link);
        UT_free(removed_link->data);
        g_list_free(removed_link);
      }
      else 
      {
        p = g_list_next(p);
      }
    }
    else 
    {
      p = g_list_next(p);
    }
  }  

  /* report errors */

  if ((invalid_list != NULL) && (failed_mntners != NULL))
  {
    LG_log(au_context, LG_INFO, "au_check_mnt_routes_prefix: adding errors to report");
    RT_invalid_mnt_routes(info->ctx, rpsl_object_get_key_value(parent), rpsl_object_get_class(parent), 
                          parent_text, failed_mntners, invalid_list);
    ret_val |= AU_UNAUTHORISED_CONT;   
  }
  
  LG_log(au_context, LG_INFO, "<au_check_mnt_routes_prefix: exiting with value [%s]", AU_ret2str(ret_val));
  return ret_val;
}

gboolean
au_rpsl_init ()
{
  return TRUE;
}


