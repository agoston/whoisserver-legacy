#include "config.h"
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "au_plugins.h"
#include "au_util.h"

#include <glib.h>
#include <stdio.h>
#include "ca_configFns.h"
#include "ca_defs.h"
#include "ca_dictionary.h"
#include "ca_macros.h"

#undef MIN
#undef MAX
#include "memwrap.h"

AU_ret_t ripe_inetnum_checks(au_plugin_callback_info_t *info);
AU_ret_t ripe_inet6num_checks(au_plugin_callback_info_t *info);
AU_ret_t afrinic_inetnum_checks(au_plugin_callback_info_t *info);
AU_ret_t afrinic_inet6num_checks(au_plugin_callback_info_t *info);
AU_ret_t ripe_allow(au_plugin_callback_info_t *info);
AU_ret_t ripe_check_name_change(au_plugin_callback_info_t *info);

static const au_check_by_type_t ripe_plugins[] = 
{
#ifdef AFRINIC
  { "inetnum", afrinic_inetnum_checks, ripe_allow, afrinic_inetnum_checks },
  { "inet6num", afrinic_inet6num_checks, ripe_allow, afrinic_inet6num_checks },
#else
  { "inetnum", ripe_inetnum_checks, ripe_allow, ripe_inetnum_checks },
  { "inet6num", ripe_inet6num_checks, ripe_allow, ripe_inet6num_checks },
#endif
  { "person", ripe_allow, ripe_allow, ripe_check_name_change },
  { "role", ripe_allow, ripe_allow, ripe_check_name_change },
  { NULL }
};

/* things that we don't care about, authorise */
AU_ret_t
ripe_allow (au_plugin_callback_info_t *info)
{
  return AU_AUTHORISED;
}


/* extract the status value from an inetnum, and return in an allocated string */
static char *
get_status (rpsl_object_t *obj)
{
  GList *status_attr;
  char *status = NULL;

  LG_log(au_context, LG_FUNC, ">get_status: entering");
  status_attr = rpsl_object_get_attr(obj, "status");
  if (status_attr == NULL)
  {
    LG_log(au_context, LG_DEBUG, "get_status: no status");
  }
  else
  {
    status = rpsl_attr_get_clean_value(status_attr->data);
    g_strup(status);
  }
  rpsl_attr_delete_list(status_attr);

  LG_log(au_context, LG_FUNC, "<get_status: exiting with value [\"%s\"]", status ? status : "NULL");
  return status;
}

static gboolean
parent_status_is_valid (RT_context_t *ctx, const rpsl_object_t *obj, ...)
{
  va_list args;
  char *allowed_parent_type;

  char *key;

  GList *parents;
  rpsl_object_t *parent;
  char *parent_status;

  GList *all_parent_status;

  GString *status_types_msg;

  GList *removed_link;
  GList *p;

  LG_log(au_context, LG_FUNC, ">parent_status_is_valid: entering");

  /* get parent(s) */
  if (LU_get_parents(au_lookup, &parents, obj, NULL) != LU_OKAY)
  {
    /* error with lookup, reject */
    LG_log(au_context, LG_FUNC, "<parent_status_is_valid: exiting with value [FALSE]");
    return FALSE;
  }

  /* if no parents, reject */
  if (parents == NULL)
  {
    LG_log(au_context, LG_DEBUG, "parent_status_is_valid: no parent(s)");
    LG_log(au_context, LG_FUNC, "<parent_status_is_valid: exiting with value [FALSE]");
    return FALSE;
  }

  /* extract the status of all of the parents */
  all_parent_status = NULL;
  for (p=parents; p != NULL; p = g_list_next(p))
  {
    parent = p->data;
    parent_status = get_status(parent);
    key = rpsl_object_get_key_value(parent);
    if (parent_status == NULL)
    {
      LG_log(au_context, LG_DEBUG, "parent_status_is_valid: parent \"%s\" has no status", 
        key);
      /* add bogus value, which will make checks fail */
      parent_status = UT_strdup("");
    }
    else
    {
      LG_log(au_context, LG_DEBUG, "parent_status_is_valid: parent \"%s\" has status \"%s\"", 
        key, parent_status);
    }
    UT_free(key);
    all_parent_status = g_list_prepend(all_parent_status, parent_status);
  }

  /* remove each parent attribute that is allowed */
  va_start(args, obj);
  allowed_parent_type = va_arg(args, char*);
  status_types_msg = g_string_sized_new(64);
  while (allowed_parent_type != NULL)
  {
    /* build a string of allowed types */
    if (status_types_msg->len == 0) 
    {
      g_string_append(status_types_msg, allowed_parent_type);
    }
    else
    {
      g_string_sprintfa(status_types_msg, ", or %s", allowed_parent_type);
    }

    /* remove all status attributes that match this type */
    p = all_parent_status;
    while (p != NULL)
    {
      if (strcmp(p->data, allowed_parent_type) == 0)
      {
        LG_log(au_context, LG_DEBUG, "parent_status_is_valid: status \"%s\" okay",
          p->data);

        /* parent type matches, remove */
        removed_link = p;
        p = g_list_next(p);
        all_parent_status = g_list_remove_link(all_parent_status, removed_link);
        UT_free(removed_link->data);
        g_list_free(removed_link);
      }
      else
      {
        p = g_list_next(p);
      }
    }

    allowed_parent_type = va_arg(args, char*);
  }
  va_end(args);

  /* if all parent status are removed, we're okay */
  if (all_parent_status == NULL)
  {
    /* return okay */
    g_string_free(status_types_msg, TRUE);
    LG_log(au_context, LG_FUNC, "<parent_status_is_valid: exiting with value [TRUE]");
    return TRUE;
  }
  else
  {
    /* free memory */
    for (p = all_parent_status; p != NULL; p = g_list_next(p))
    {
      LG_log(au_context, LG_DEBUG, "parent_status_is_valid: status \"%s\" NOT okay",
        p->data);
      UT_free(p->data);
    }
    g_list_free(all_parent_status);

    /* note the failure */
    for (p = parents; p != NULL; p = g_list_next(p)) 
    {
        parent = p->data;
        key = rpsl_object_get_key_value(parent);
        RT_status_check_failed_parentwithoutstatus(ctx, key,
          status_types_msg->str);
        UT_free(key);
    }

    /* return error */
    g_string_free(status_types_msg, TRUE);
    LG_log(au_context, LG_FUNC, "<parent_status_is_valid: exiting with value [FALSE]");
    return FALSE;
  }
}

gboolean
has_rir_mntner (const rpsl_object_t *obj)
{
  char *alloc_mntner_str;
  char **alloc_mntner_list = NULL;
  int alloc_idx;

  GList *mntners;
  GList *p;

  gboolean rir_mntner_found;

  LG_log(au_context, LG_FUNC, ">has_rir_mntner: entering");

  /* get the list of mnt-by mntner names */
  mntners = rpsl_object_get_attr(obj, "mnt-by");
  rpsl_attr_split_multiple(&mntners);

  /* get the list of ALLOCMNT mntner names */
  alloc_mntner_str = ca_get_allocmnt;
  assert(alloc_mntner_str != NULL);  /* there should always be at least one */
  /* split the alloc_mntner_str on comma */
  alloc_mntner_list = ut_g_strsplit_v1(alloc_mntner_str, ",", 0);
  
  /* compare the two lists and look for a match */
  rir_mntner_found = FALSE;
  for (alloc_idx = 0; alloc_mntner_list[alloc_idx] != NULL && !rir_mntner_found; alloc_idx++)
  {
    g_strstrip(alloc_mntner_list[alloc_idx]);
    for (p=mntners; (p != NULL) && !rir_mntner_found; p = g_list_next(p))
    {
      LG_log(au_context, LG_DEBUG, "has_rir_mntner: comparing mnt-by: %s with ALLOCMNT %s",
        rpsl_attr_get_value(p->data), alloc_mntner_list[alloc_idx]);
      if (strcasecmp(rpsl_attr_get_value(p->data), alloc_mntner_list[alloc_idx]) == 0)
      {
        rir_mntner_found = TRUE;
      }
    }
  }
  rpsl_attr_delete_list(mntners);
  g_strfreev(alloc_mntner_list);

  LG_log(au_context, LG_FUNC, "<has_rir_mntner: exiting with value [%s]",
            rir_mntner_found ? "TRUE" : "FALSE");
  return rir_mntner_found;
}


AU_ret_t 
ripe_inetnum_checks (au_plugin_callback_info_t *info)
{
  rpsl_object_t *old_object;

  char *old_status;
  char *new_status;

  gboolean parent_status_ok;

  AU_ret_t ret_val;
  gboolean override;

  LG_log(au_context, LG_FUNC, ">ripe_inetnum_checks: entering");

  if (!LU_get_object(au_lookup, &old_object, info->obj, NULL))
  {
    LG_log(au_context, LG_ERROR, "ripe_inetnum_checks: error looking up old version");
    LG_log(au_context, LG_FUNC, "<ripe_inetnum_checks: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }
  if (old_object == NULL) 
  {
    LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: no old version, new");
  }

  new_status = get_status(info->obj);
  if (new_status == NULL)
  {
    ret_val = AU_UNAUTHORISED_CONT;
    LG_log(au_context, LG_ERROR, "ripe_inetnum_checks: no \"status:\" on updated inetnum");
    RT_status_check_failed_missingstatus(info->ctx);
  }
  else
  {
    /* get old status */
    /* use "" to simplify our comparisons if the old object had no status */
    if (old_object == NULL)
    {
      old_status = UT_strdup("");
    }
    else
    {
      old_status = get_status(old_object);
      if (old_status == NULL)
      {
        old_status = UT_strdup("");
      }
    }

    /* changing from ASSIGNED-ANYCAST not allowed */
    if ((strcmp(new_status, "ASSIGNED-ANYCAST") != 0) &&
             (strcmp(old_status, "ASSIGNED-ANYCAST") == 0))
    {
      RT_status_check_failed_anycast_modify(info->ctx);
      ret_val = AU_UNAUTHORISED_CONT;
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: trying to modify status from ASSIGNED-ANYCAST");
    }
    /* changing to ALLOCATED */
    else if ((strncmp(new_status, "ALLOCATED ", 10) == 0) &&
        (strncmp(old_status, "ALLOCATED ", 10) != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: ALLOCATED");
      if (has_rir_mntner(info->obj))
      {
        ret_val = AU_AUTHORISED;
      }
      else
      {
        RT_status_check_failed_allocated(info->ctx);
        ret_val = AU_UNAUTHORISED_CONT;
        LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: status ALLOCATED can only be set by hostmaster");
      }
    }
    /* changing to LIR-PARTITIONED */
    else if ((strncmp(new_status, "LIR-PARTITIONED ", 16) == 0) &&
             (strncmp(old_status, "LIR-PARTITIONED ", 16) != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: changing to LIR-PARTITIONED");

      if (strcmp(new_status, "LIR-PARTITIONED PA") == 0)
      {
        parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
          "LIR-PARTITIONED PA", "ALLOCATED PA", "ALLOCATED UNSPECIFIED", "SUB-ALLOCATED PA", NULL);
      }
      else if (strcmp(new_status, "LIR-PARTITIONED PI") == 0)
      {
        parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
          "LIR-PARTITIONED PI", "ALLOCATED PI", "ALLOCATED UNSPECIFIED", NULL);
      }
      else
      {
        parent_status_ok = FALSE;
        LG_log(au_context, LG_ERROR, "ripe_inetnum_checks: bogus LIR-PARTITIONED status \"%s\"", 
          new_status);
      }

      if (parent_status_ok)
      {
        ret_val = AU_AUTHORISED;
      }
      else
      {
        /* note that the RT logging is done by parent_status_is_valid() */
        ret_val = AU_UNAUTHORISED_CONT;
      }
      
    }
    /* changing to SUB-ALLOCATED PA */
    else if ((strcmp(new_status, "SUB-ALLOCATED PA") == 0) &&
             (strcmp(old_status, "SUB-ALLOCATED PA") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: changing to SUB-ALLOCATED PA");

      parent_status_ok = parent_status_is_valid(info->ctx, info->obj,
          "LIR-PARTITIONED PA", "ALLOCATED PA", "SUB-ALLOCATED PA", NULL);

      if (parent_status_ok)
      {
        ret_val = AU_AUTHORISED;
      }
      else
      {
        /* note that the RT logging is done by parent_status_is_valid() */
        ret_val = AU_UNAUTHORISED_CONT;
      }

    }
    /* changing to ASSIGNED ANYCAST */
    else if ((strcmp(new_status, "ASSIGNED-ANYCAST") == 0) &&
             (strcmp(old_status, "ASSIGNED-ANYCAST") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: changing to ASSIGNED-ANYCAST");

      if ( strcmp(old_status, "") != 0 )
      {
        /* ASSIGNED ANYCAST can only be set on object creation, 
	   not on modification */
        RT_status_check_failed_anycast_modify(info->ctx);
        ret_val = AU_UNAUTHORISED_CONT;
        LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: trying to modify status to ASSIGNED-ANYCAST");
      }
      else
      {
	if ( ! has_rir_mntner(info->obj))
	{
          RT_status_check_failed_anycast_rir(info->ctx);
          ret_val = AU_UNAUTHORISED_CONT;
          LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: status ASSIGNED-ANYCAST can only be set by hostmaster");
	}
	else
	{
	  parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
                             "ALLOCATED PI", "ALLOCATED UNSPECIFIED", NULL);

	  if (parent_status_ok)
	  {
            ret_val = AU_AUTHORISED;
            LG_log(au_context, LG_ERROR, "ripe_inetnum_checks: parent status check ok");
	  }
	  else
	  {
            /* note that the RT logging is done by parent_status_is_valid() */
            ret_val = AU_UNAUTHORISED_CONT;
            LG_log(au_context, LG_ERROR, "ripe_inetnum_checks: parent status check failed");
	  }
	}
      }
    }
    /* changing to EARLY-REGISTRATION */
    else if ((strcmp(new_status, "EARLY-REGISTRATION") == 0) &&
             (strcmp(old_status, "EARLY-REGISTRATION") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: changing to EARLY-REGISTRATION");
      ret_val = AU_UNAUTHORISED_CONT;

      RT_status_check_failed_earlyregistration(info->ctx);
    }
    /* changing to NOT-SET */
    else if ((strcmp(new_status, "NOT-SET") == 0) &&
             (strcmp(old_status, "NOT-SET") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: changing to NOT-SET");
      ret_val = AU_UNAUTHORISED_CONT;

      RT_status_check_failed_notset(info->ctx);
    }
    else if ((strcmp(new_status, "ASSIGNED PA") == 0) &&
             (strcmp(old_status, "ASSIGNED PA") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: changing to ASSIGNED PA");

      parent_status_ok = parent_status_is_valid(info->ctx, info->obj,
          "ALLOCATED UNSPECIFIED", "ALLOCATED PA", "LIR-PARTITIONED PA", "SUB-ALLOCATED PA", "ASSIGNED PA", NULL);

      if (parent_status_ok)
      {
        ret_val = AU_AUTHORISED;
      }
      else
      {
        /* note that the RT logging is done by parent_status_is_valid() */
        ret_val = AU_UNAUTHORISED_CONT;
      }

    }
    else if ((strcmp(new_status, "ASSIGNED PI") == 0) &&
             (strcmp(old_status, "ASSIGNED PI") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: changing to ASSIGNED PI");

      parent_status_ok = parent_status_is_valid(info->ctx, info->obj,
          "ALLOCATED UNSPECIFIED", "ALLOCATED PI", "LIR-PARTITIONED PI", "SUB-ALLOCATED PI", "ASSIGNED PI", NULL);

      if (parent_status_ok)
      {
        ret_val = AU_AUTHORISED;
      }
      else
      {
        /* note that the RT logging is done by parent_status_is_valid() */
        ret_val = AU_UNAUTHORISED_CONT;
      }

    }
    else
    {
      ret_val = AU_AUTHORISED;
      LG_log(au_context, LG_DEBUG, 
        "ripe_inetnum_checks: no special handling changing status \"%s\" to \"%s\"",
        old_status, new_status);
    }

    UT_free(old_status);
    UT_free(new_status);
  }

  /* done with original object now */
  if (old_object != NULL)
  {
      rpsl_object_delete(old_object);
  }

  /* override if necessary */
  au_override(&ret_val, &override, info);

  LG_log(au_context, LG_FUNC, "<ripe_inetnum_checks: exiting with value [%s]", 
         AU_ret2str(ret_val));

  return ret_val;
}

AU_ret_t 
ripe_inet6num_checks (au_plugin_callback_info_t *info)
{
  rpsl_object_t *old_object;

  char *old_status;
  char *new_status;

  gboolean parent_status_ok;

  AU_ret_t ret_val;
  gboolean override;

  LG_log(au_context, LG_FUNC, ">ripe_inet6num_checks: entering");

  if (!LU_get_object(au_lookup, &old_object, info->obj, NULL))
  {
    LG_log(au_context, LG_ERROR, "ripe_inet6num_checks: error looking up old version");
    LG_log(au_context, LG_FUNC, "<ripe_inet6num_checks: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }
  if (old_object == NULL) 
  {
    LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: no old version, new");
  }

  new_status = get_status(info->obj);
  if (new_status == NULL)
  {
    ret_val = AU_UNAUTHORISED_CONT;
    LG_log(au_context, LG_ERROR, "ripe_inet6num_checks: no \"status:\" on updated inet6num");
    RT_status_check_failed_missingstatus(info->ctx);
  }
  else
  {
    /* get old status */
    /* use "" to simplify our comparisons if the old object had no status */
    if (old_object == NULL)
    {
      old_status = UT_strdup("");
    }
    else
    {
      old_status = get_status(old_object);
      if (old_status == NULL)
      {
        old_status = UT_strdup("");
      }
    }

    /* changing from ASSIGNED-ANYCAST not allowed */
    if ((strcmp(new_status, "ASSIGNED-ANYCAST") != 0) &&
             (strcmp(old_status, "ASSIGNED-ANYCAST") == 0))
    {
      RT_status_check_failed_anycast_modify(info->ctx);
      ret_val = AU_UNAUTHORISED_CONT;
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: trying to modify status from ASSIGNED-ANYCAST");
    }
    /* changing to ALLOCATED-BY-RIR */
    else if ((strcmp(new_status, "ALLOCATED-BY-RIR") == 0) &&
        (strcmp(old_status, "ALLOCATED-BY-RIR") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: ALLOCATED-BY-RIR");
      if ( ! has_rir_mntner(info->obj) )
      {
        RT_status_check_failed_allocbyrir(info->ctx);
        ret_val = AU_UNAUTHORISED_CONT;
        LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: status ALLOCATED-BY-RIR can only be set by hostmaster");
      }
      else
      {
        LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: has RIPE NCC maintainer");
        parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
          "ALLOCATED-BY-RIR", NULL);
        if (parent_status_ok)
        {
          ret_val = AU_AUTHORISED;
        }
        else
        {
          /* note that the RT logging is done by parent_status_is_valid() */
          ret_val = AU_UNAUTHORISED_CONT;
        }
      }
    }
    /* changing to ALLOCATED-BY-LIR */
    else if ((strcmp(new_status, "ALLOCATED-BY-LIR") == 0) &&
             (strcmp(old_status, "ALLOCATED-BY-LIR") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: ALLOCATED-BY-LIR");
      parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
        "ALLOCATED-BY-RIR", "ALLOCATED-BY-LIR", NULL);
      if (parent_status_ok)
      {
        ret_val = AU_AUTHORISED;
      }
      else
      {
        /* note that the RT logging is done by parent_status_is_valid() */
        ret_val = AU_UNAUTHORISED_CONT;
      }
    }
    else if ((strcmp(new_status, "ASSIGNED-ANYCAST") == 0) &&
             (strcmp(old_status, "ASSIGNED-ANYCAST") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: changing to ASSIGNED-ANYCAST");

      if ( strcmp(old_status, "") != 0 )
      {
        /* ASSIGNED ANYCAST can only be set on object creation, 
	   not on modification */
        RT_status_check_failed_anycast_modify(info->ctx);
        ret_val = AU_UNAUTHORISED_CONT;
        LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: trying to modify status to ASSIGNED-ANYCAST");
      }
      else
      {
	if ( ! has_rir_mntner(info->obj))
	{
          RT_status_check_failed_anycast_rir(info->ctx);
          ret_val = AU_UNAUTHORISED_CONT;
          LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: status ASSIGNED-ANYCAST can only be set by hostmaster");
	}
	else
	{
	  parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
                             "ALLOCATED-BY-RIR", NULL);

	  if (parent_status_ok)
	  {
            ret_val = AU_AUTHORISED;
            LG_log(au_context, LG_ERROR, "ripe_inet6num_checks: parent status check ok");
	  }
	  else
	  {
            /* note that the RT logging is done by parent_status_is_valid() */
            ret_val = AU_UNAUTHORISED_CONT;
            LG_log(au_context, LG_ERROR, "ripe_inet6num_checks: parent status check failed");
	  }
	}
      }
    }
    /* changing to ASSIGNED */
    else if ((strcmp(new_status, "ASSIGNED") == 0) &&
             (strcmp(old_status, "ASSIGNED") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: ASSIGNED");
      parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
        "ALLOCATED-BY-RIR", "ALLOCATED-BY-LIR", NULL);
      if (parent_status_ok)
      {
        ret_val = AU_AUTHORISED;
      }
      else
      {
        /* note that the RT logging is done by parent_status_is_valid() */
        ret_val = AU_UNAUTHORISED_CONT;
      }
    }
    /* no change */
    else
    {
      ret_val = AU_AUTHORISED;
      LG_log(au_context, LG_DEBUG, 
        "ripe_inet6num_checks: no special handling changing status \"%s\" to \"%s\"",
        old_status, new_status);
    }

    UT_free(old_status);
    UT_free(new_status);
  }

  /* done with original object now */
  if (old_object != NULL)
  {
      rpsl_object_delete(old_object);
  }

  /* override if necessary */
  au_override(&ret_val, &override, info);

  LG_log(au_context, LG_FUNC, "<ripe_inet6num_checks: exiting with value [%s]", 
         AU_ret2str(ret_val));

  return ret_val;
}


AU_ret_t 
afrinic_inetnum_checks (au_plugin_callback_info_t *info)
{
  rpsl_object_t *old_object;

  char *old_status;
  char *new_status;

  gboolean parent_status_ok;

  AU_ret_t ret_val;
  gboolean override;

  LG_log(au_context, LG_FUNC, ">afrinic_inetnum_checks: entering");

  if (!LU_get_object(au_lookup, &old_object, info->obj, NULL))
  {
    LG_log(au_context, LG_ERROR, "afrinic_inetnum_checks: error looking up old version");
    LG_log(au_context, LG_FUNC, "<afrinic_inetnum_checks: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }
  if (old_object == NULL) 
  {
    LG_log(au_context, LG_DEBUG, "afrinic_inetnum_checks: no old version, new");
  }

  new_status = get_status(info->obj);
  if (new_status == NULL)
  {
    ret_val = AU_UNAUTHORISED_CONT;
    LG_log(au_context, LG_ERROR, "afrinic_inetnum_checks: no \"status:\" on updated inetnum");

    RT_status_check_failed_missingstatus(info->ctx);
  }
  else
  {
    /* get old status */
    /* use "" to simplify our comparisons if the old object had no status */
    if (old_object == NULL)
    {
      old_status = UT_strdup("");
    }
    else
    {
      old_status = get_status(old_object);
      if (old_status == NULL)
      {
        old_status = UT_strdup("");
      }
    }

    /* changing to ALLOCATED */
    if ((strncmp(new_status, "ALLOCATED ", 10) == 0) &&
        (strncmp(old_status, "ALLOCATED ", 10) != 0))
    {
      LG_log(au_context, LG_DEBUG, "afrinic_inetnum_checks: status changing to ALLOCATED");
      if (has_rir_mntner(info->obj))
      {
        if (strcmp(new_status, "ALLOCATED PA") == 0)
        {
          parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
                              "ALLOCATED PA", "ALLOCATED UNSPECIFIED", NULL);
        }
        else if (strcmp(new_status, "ALLOCATED PI") == 0)
        {
          parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
                              "ALLOCATED PI", "ALLOCATED UNSPECIFIED", NULL);
        }
        else if (strcmp(new_status, "ALLOCATED UNSPECIFIED") == 0)
        {
          parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
                              "ALLOCATED UNSPECIFIED", NULL);
        }

        if (parent_status_ok)
        {
          ret_val = AU_AUTHORISED;
        }
        else
        {
          /* note that the RT logging is done by parent_status_is_valid() */
          ret_val = AU_UNAUTHORISED_CONT;
        }
      }
      else
      {
        RT_status_check_failed_allocated(info->ctx);
        ret_val = AU_UNAUTHORISED_CONT;
      }
    }
    /* changing to SUB-ALLOCATED PA */
    else if ((strcmp(new_status, "SUB-ALLOCATED PA") == 0) &&
             (strcmp(old_status, "SUB-ALLOCATED PA") != 0))
    {
      LG_log(au_context, LG_DEBUG, "afrinic_inetnum_checks: status changing to SUB-ALLOCATED PA");

      parent_status_ok = parent_status_is_valid(info->ctx, info->obj,
                                     "ALLOCATED UNSPECIFIED", "ALLOCATED PA", NULL);

      if (parent_status_ok)
      {
        ret_val = AU_AUTHORISED;
      }
      else
      {
        /* note that the RT logging is done by parent_status_is_valid() */
        ret_val = AU_UNAUTHORISED_CONT;
      }
    }
    /* changing to ASSIGNED */
    else if ((strncmp(new_status, "ASSIGNED ", 9) == 0) &&
             (strncmp(old_status, "ASSIGNED ", 9) != 0))
    {
      LG_log(au_context, LG_DEBUG, "afrinic_inetnum_checks: status changing to ASSIGNED");

      if (strcmp(new_status, "ASSIGNED PA") == 0)
      {
        parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
                        "ALLOCATED PA", "ALLOCATED UNSPECIFIED", "SUB-ALLOCATED PA", "ASSIGNED PA", NULL);
      }
      else if (strcmp(new_status, "ASSIGNED PI") == 0)
      {
        parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
                            "ALLOCATED PI", "ALLOCATED UNSPECIFIED", "ASSIGNED PI", NULL);
      }

      if (parent_status_ok)
      {
        ret_val = AU_AUTHORISED;
      }
      else
      {
        /* note that the RT logging is done by parent_status_is_valid() */
        ret_val = AU_UNAUTHORISED_CONT;
      }
    }
    /* changing to EARLY-REGISTRATION */
    else if ((strcmp(new_status, "EARLY-REGISTRATION") == 0) &&
             (strcmp(old_status, "EARLY-REGISTRATION") != 0))
    {
      LG_log(au_context, LG_DEBUG, "afrinic_inetnum_checks: status changing to EARLY-REGISTRATION");
      ret_val = AU_UNAUTHORISED_CONT;

      RT_status_check_failed_earlyregistration(info->ctx);
    }
    /* changing to NOT-SET */
    else if ((strcmp(new_status, "NOT-SET") == 0) &&
             (strcmp(old_status, "NOT-SET") != 0))
    {
      LG_log(au_context, LG_DEBUG, "afrinic_inetnum_checks: status changing to NOT-SET");
      ret_val = AU_UNAUTHORISED_CONT;

      RT_status_check_failed_notset(info->ctx);
    }
    else
    {
      ret_val = AU_AUTHORISED;
      LG_log(au_context, LG_DEBUG, 
        "afrinic_inetnum_checks: no special handling changing status \"%s\" to \"%s\"",
        old_status, new_status);
    }

    UT_free(old_status);
    UT_free(new_status);
  }

  /* done with original object now */
  if (old_object != NULL)
  {
      rpsl_object_delete(old_object);
  }

  /* override if necessary */
  au_override(&ret_val, &override, info);

  LG_log(au_context, LG_FUNC, "<afrinic_inetnum_checks: exiting with value [%s]", 
         AU_ret2str(ret_val));

  return ret_val;
}

AU_ret_t 
afrinic_inet6num_checks (au_plugin_callback_info_t *info)
{
  rpsl_object_t *old_object;

  char *old_status;
  char *new_status;

  gboolean parent_status_ok;

  AU_ret_t ret_val;
  gboolean override;

  LG_log(au_context, LG_FUNC, ">afrinic_inet6num_checks: entering");

  if (!LU_get_object(au_lookup, &old_object, info->obj, NULL))
  {
    LG_log(au_context, LG_ERROR, "afrinic_inet6num_checks: error looking up old version");
    LG_log(au_context, LG_FUNC, "<afrinic_inet6num_checks: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }
  if (old_object == NULL) 
  {
    LG_log(au_context, LG_DEBUG, "afrinic_inet6num_checks: no old version, new");
  }

  new_status = get_status(info->obj);
  if (new_status == NULL)
  {
    ret_val = AU_UNAUTHORISED_CONT;
    LG_log(au_context, LG_ERROR, "afrinic_inet6num_checks: no \"status:\" on updated inet6num");
    RT_status_check_failed_missingstatus(info->ctx);
  }
  else
  {
    /* get old status */
    /* use "" to simplify our comparisons if the old object had no status */
    if (old_object == NULL)
    {
      old_status = UT_strdup("");
    }
    else
    {
      old_status = get_status(old_object);
      if (old_status == NULL)
      {
        old_status = UT_strdup("");
      }
    }

    /* changing to ALLOCATED-BY-RIR */
    if ((strcmp(new_status, "ALLOCATED-BY-RIR") == 0) &&
        (strcmp(old_status, "ALLOCATED-BY-RIR") != 0))
    {
      LG_log(au_context, LG_DEBUG, "afrinic_inet6num_checks: changing status to ALLOCATED-BY-RIR");
      if (has_rir_mntner(info->obj))
      {
        LG_log(au_context, LG_DEBUG, "afrinic_inet6num_checks: has AfriNic maintainer");
        parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
                                                   "ALLOCATED-BY-RIR", NULL);
        if (parent_status_ok)
        {
          ret_val = AU_AUTHORISED;
        }
        else
        {
          /* note that the RT logging is done by parent_status_is_valid() */
          ret_val = AU_UNAUTHORISED_CONT;
        }
      }
      else
      {
        RT_status_check_failed_allocbyrir(info->ctx);
        ret_val = AU_UNAUTHORISED_CONT;
      }
    }
    /* changing to SUB-ALLOCATED PA */
    else if ((strcmp(new_status, "SUB-ALLOCATED PA") == 0) &&
             (strcmp(old_status, "SUB-ALLOCATED PA") != 0))
    {
      LG_log(au_context, LG_DEBUG, "afrinic_inet6num_checks: changing status to SUB-ALLOCATED PA");
      parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
                                                     "ALLOCATED-BY-RIR", NULL);
      if (parent_status_ok)
      {
        ret_val = AU_AUTHORISED;
      }
      else
      {
        /* note that the RT logging is done by parent_status_is_valid() */
        ret_val = AU_UNAUTHORISED_CONT;
      }
    }
    /* changing to ASSIGNED PA */
    else if ((strcmp(new_status, "ASSIGNED PA") == 0) &&
             (strcmp(old_status, "ASSIGNED PA") != 0))
    {
      LG_log(au_context, LG_DEBUG, "afrinic_inet6num_checks: changing status to ASSIGNED PA");
      parent_status_ok = parent_status_is_valid(info->ctx, info->obj, 
                                      "ALLOCATED-BY-RIR", "SUB-ALLOCATED PA", NULL);
      if (parent_status_ok)
      {
        ret_val = AU_AUTHORISED;
      }
      else
      {
        /* note that the RT logging is done by parent_status_is_valid() */
        ret_val = AU_UNAUTHORISED_CONT;
      }
    }
    /* no change */
    else
    {
      ret_val = AU_AUTHORISED;
      LG_log(au_context, LG_DEBUG, 
        "afrinic_inet6num_checks: no special handling changing status \"%s\" to \"%s\"",
        old_status, new_status);
    }

    UT_free(old_status);
    UT_free(new_status);
  }

  /* done with original object now */
  if (old_object != NULL)
  {
      rpsl_object_delete(old_object);
  }

  /* override if necessary */
  au_override(&ret_val, &override, info);

  LG_log(au_context, LG_FUNC, "<afrinic_inet6num_checks: exiting with value [%s]", 
         AU_ret2str(ret_val));

  return ret_val;
}

/*
 * ripe_check_name_change() makes checks on private (person and role) objects,
 * and authorises the update only if the name attribute (person: or role:) is 
 * unchanged.  Otherwise, the function returns AU_UNAUTHORISED_CONT which
 * prevents the object from being modified.
 *
 * The override scheme bypasses this check.
 *
 */
AU_ret_t 
ripe_check_name_change(au_plugin_callback_info_t *info) 
{

  AU_ret_t ret_val = AU_AUTHORISED;

  GList *attr_list = NULL;
  gboolean override;
  rpsl_object_t *old_object;

  const char *type = NULL;
  gchar *old_name;
  gchar *new_name;
  
  LG_log(au_context, LG_FUNC, ">ripe_check_name_change: entering");

  au_override(&ret_val, &override, info);

  if ( override == TRUE )
  {   
    LG_log(au_context, LG_DEBUG, "ripe_check_name_change: override used");   
    LG_log(au_context, LG_FUNC, "<ripe_check_name_change: exiting with value [%s]", AU_ret2str(ret_val));   
    return ret_val;
  }
  
  type = rpsl_object_get_class(info->obj);

  if(type == NULL)
  {
    LG_log(au_context, LG_FUNC, "<ripe_check_name_change: exiting with value [%s]", AU_ret2str(ret_val));
    ret_val = AU_ERROR;
    return ret_val;
  }


  attr_list = rpsl_object_get_attr(info->obj, type);

  if (attr_list == NULL)
  {
    LG_log(au_context, LG_ERROR, "ripe_check_name_change: cannot fetch new attribute list");
    ret_val = AU_ERROR;
    return ret_val;
  }

  new_name = g_strdup(rpsl_attr_get_clean_value((rpsl_attr_t *)(attr_list->data)));
  rpsl_attr_delete_list(attr_list);

  if (!LU_get_object(au_lookup, &old_object, info->obj, NULL))
  {
    LG_log(au_context, LG_ERROR, "ripe_check_name_change: error looking up old version");
    ret_val = AU_ERROR;
    LG_log(au_context, LG_FUNC, "<ripe_check_name_change: exiting with value [%s]", AU_ret2str(ret_val));
    ret_val = AU_ERROR;
    g_free(new_name);
    return ret_val;
  }

  attr_list = rpsl_object_get_attr(old_object, type);

  if ( attr_list  == NULL)
  {
    LG_log(au_context, LG_ERROR, "ripe_check_name_change: cannot fetch old attribute list");
    ret_val = AU_ERROR;
    LG_log(au_context, LG_FUNC, "<ripe_check_name_change: exiting with value [%s]", AU_ret2str(ret_val));
    g_free(new_name);
    return ret_val;
  }

  old_name = g_strdup(rpsl_attr_get_clean_value((rpsl_attr_t *)(attr_list->data)));
  rpsl_attr_delete_list(attr_list);

  if(strcasecmp(old_name, new_name))
  {
    RT_name_changed(info->ctx);
    ret_val = AU_UNAUTHORISED_CONT;
  }

  g_free(new_name);
  g_free(old_name);

  LG_log(au_context, LG_FUNC, "<ripe_check_name_change: exiting with value [%s]", 
         AU_ret2str(ret_val));


  return ret_val;
}




/* 
  Entry point for RIPE authorisation - invoke AU by-type support
  
  trans    - PG module transaction
  info     - AU plugin callback information

  return   - PG_OK means AU_AUTHORISED
             PG_ERROR_PROCEED means AU_UNAUTHORISED_CONT
             PG_ERROR_STOP means AU_UNAUTHORISED_END
             PG_ERROR_PLUGIN_NAME means AU_ERROR

  This is merely a dispatcher.
 */
PG_status_t 
au_ripe_check (PG_transaction_t *trans, gpointer *info)
{
  PG_status_t ret_val;
  au_plugin_callback_info_t *callback_info;
  AU_ret_t au_ret_val;

  LG_log(au_context, LG_FUNC, ">au_ripe_check: entering");

  callback_info = PG_global_get(trans);
  au_ret_val = AU_check_by_type(ripe_plugins, callback_info);
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
      ret_val = PG_ERROR_PROCEED;
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
      LG_log(au_context, LG_ERROR, "au_ripe_check: unknown return %d from AU_check_by_type()",
        au_ret_val);
  }

  LG_log(au_context, LG_FUNC, "<au_ripe_check: exiting with value [%d]", 
         ret_val);
  return ret_val;
}

gboolean
au_ripe_init ()
{
  return TRUE;
}

