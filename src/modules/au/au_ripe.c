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
AU_ret_t ripe_allow(au_plugin_callback_info_t *info);
AU_ret_t ripe_check_name_change(au_plugin_callback_info_t *info);

static const au_check_by_type_t ripe_plugins[] =
{
  { "inetnum", ripe_inetnum_checks, ripe_allow, ripe_inetnum_checks },
  { "inet6num", ripe_inet6num_checks, ripe_allow, ripe_inet6num_checks },
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
    rpsl_attr_delete_list(status_attr);
  }

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

/* This function is only called if new status is AGGREGATED-BY-LIR */

static gboolean
grand_parent_status_is_valid (RT_context_t *ctx, const rpsl_object_t *obj)
{
    GList *parents;
    char *parent_status;
    const char *type = NULL;
    GList *pkey = NULL;
    char *pvalue = NULL;
    char *ass_size = NULL;
    char *prefix = NULL;
    GList *ass_size_attrs = NULL;

    LG_log(au_context, LG_FUNC, ">grand_parent_status_is_valid: entering");

    /* get parent(s) */
    if (LU_get_parents(au_lookup, &parents, obj, NULL) != LU_OKAY)
    {
      /* error with lookup, reject */
      LG_log(au_context, LG_FUNC, "<grand_parent_status_is_valid: exiting with value [FALSE]");
      return FALSE;
    }

    /* if no parents, reject */
    if (parents == NULL)
    {
      LG_log(au_context, LG_DEBUG, "grand_parent_status_is_valid: no parent(s)");
      LG_log(au_context, LG_FUNC, "<grand_parent_status_is_valid: exiting with value [FALSE]");
      return FALSE;
    }

    parent_status = get_status(parents->data);
    if ( parent_status && ! strcmp(parent_status, "AGGREGATED-BY-LIR") )
    {
      /* if parent_status is AGGREGATED-BY-LIR, grand parent status cannot be AGGREGATED-BY-LIR */
      /* check status of parent of parent object */
      if ( ! parent_status_is_valid(ctx, parents->data, "ALLOCATED-BY-RIR", "ALLOCATED-BY-LIR", NULL) )
      {
        LG_log(au_context, LG_FUNC, "<grand_parent_status_is_valid: exiting with value [FALSE]");
        RT_invalid_grandparent_status(ctx);
        return FALSE;
      }
      /* check if prefix length is equal to parent assignment size.
         This may not be the logical place to do this check, but we
         have all the pieces available here to do it.
      */
      type = rpsl_object_get_class(obj);
      pkey = rpsl_object_get_attr(obj, type);
      pvalue = rpsl_attr_get_clean_value((rpsl_attr_t *)(pkey->data));
      prefix = strchr(pvalue, '/');
      if ( prefix && prefix++ != '\0' )
      {
          /* get parents assignment size */
          ass_size_attrs = rpsl_object_get_attr(parents->data, "assignment-size");
          ass_size = rpsl_attr_get_clean_value((rpsl_attr_t *)(ass_size_attrs->data));
          if ( strcmp(prefix, ass_size) )
          {
            LG_log(au_context, LG_DEBUG, "grand_parent_status_is_valid: prefix != aggregated parent assignment size");
            LG_log(au_context, LG_FUNC, "<grand_parent_status_is_valid: exiting with value [FALSE]");
            RT_invalid_more_specific_prefix_size(ctx, ass_size);
            return FALSE;
          }
      }
    }

    LG_log(au_context, LG_FUNC, "<grand_parent_status_is_valid: exiting with value [TRUE]");
    return TRUE;
}

/* Find all the mntners in the mnt-by attributes of the object.
   Get a list of the RIR mntners from the config file.
   Compare the lists.
   If we find a match then this object does have an RIR mntner.
   Then check if an RIR mntner has authorised this update.
   Return AU_AURTHORISED or not
 */

AU_ret_t
has_rir_mntner (au_plugin_callback_info_t *info, const rpsl_object_t *obj)
{
  AU_ret_t ret_val;
  char *alloc_mntner_str;
  char **alloc_mntner_list = NULL;
  int alloc_idx;

  GList *mntners;
  GList *rir_mntner_list = NULL;
  GList *p;

  gboolean rir_mntner_found;

  LG_log(au_context, LG_FUNC, ">has_rir_mntner: entering");

  /* get the list of mnt-by mntner names */
  mntners = rpsl_object_get_attr(obj, "mnt-by");
  rpsl_attr_split_multiple(&mntners);

  /* make the mntner list unique */
  rpsl_attr_uniq_list(&mntners);

  /* get the list of ALLOCMNT mntner names */
  alloc_mntner_str = ca_get_allocmnt;
  assert(alloc_mntner_str != NULL);  /* there should always be at least one */
  /* split the alloc_mntner_str on comma */
  alloc_mntner_list = g_strsplit_set(alloc_mntner_str, ",\n", 0);

  /* compare all elements of the two lists and look for a match */
  rir_mntner_found = FALSE;
  for (alloc_idx = 0; alloc_mntner_list[alloc_idx] != NULL; alloc_idx++)
  {
    g_strstrip(alloc_mntner_list[alloc_idx]);
    for (p=mntners; p != NULL; p = g_list_next(p))
    {
      LG_log(au_context, LG_DEBUG, "has_rir_mntner: comparing mnt-by: %s with ALLOCMNT %s",
        rpsl_attr_get_value(p->data), alloc_mntner_list[alloc_idx]);
      if (strcasecmp(rpsl_attr_get_value(p->data), alloc_mntner_list[alloc_idx]) == 0)
      {
        LG_log(au_context, LG_DEBUG, "has_rir_mntner: valid rir mntner found");
        rir_mntner_found = TRUE;
        rir_mntner_list = g_list_prepend(rir_mntner_list, p->data);
      }
    }
  }

  ret_val = AU_UNAUTHORISED_CONT; /* the default */
  if (rir_mntner_found)
  {
    /* now check the authorisation is by one of the rir mntners */
    ret_val = au_rir_auth_check(info, obj, rir_mntner_list);
  }
  /* rir_mntner_list elements are pointers to elements of mntners,
     so only free one of them */
  rpsl_attr_delete_list(mntners);
  g_strfreev(alloc_mntner_list);

  /* return the result */
  LG_log(au_context, LG_FUNC, "<has_rir_mntner: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
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

    /* changing from ASSIGNED ANYCAST not allowed */
    if ((strcmp(new_status, "ASSIGNED ANYCAST") != 0) &&
             (strcmp(old_status, "ASSIGNED ANYCAST") == 0))
    {
      RT_status_check_failed_modify(info->ctx, "ASSIGNED ANYCAST");
      ret_val = AU_UNAUTHORISED_CONT;
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: trying to modify status from ASSIGNED ANYCAST");
    }
    /* changing to ALLOCATED */
    else if ((strncmp(new_status, "ALLOCATED ", 10) == 0) &&
        (strncmp(old_status, "ALLOCATED ", 10) != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: ALLOCATED");
      if ( (ret_val=has_rir_mntner(info, info->obj)) != AU_AUTHORISED )
      {
        RT_status_check_failed_allocated(info->ctx);
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
    else if ((strcmp(new_status, "ASSIGNED ANYCAST") == 0) &&
             (strcmp(old_status, "ASSIGNED ANYCAST") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: changing to ASSIGNED ANYCAST");

      if ( strcmp(old_status, "") != 0 )
      {
        /* ASSIGNED ANYCAST can only be set on object creation,
        not on modification */
        ret_val = AU_UNAUTHORISED_CONT;
        RT_status_check_failed_modify(info->ctx, "ASSIGNED ANYCAST");
        LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: trying to modify status to ASSIGNED ANYCAST");
      }
      else
      {
        if ( (ret_val=has_rir_mntner(info, info->obj)) != AU_AUTHORISED)
        {
          RT_status_check_failed_anycast_rir(info->ctx);
          LG_log(au_context, LG_DEBUG, "ripe_inetnum_checks: status ASSIGNED ANYCAST can only be set by hostmaster");
        }
        else
        {
          parent_status_ok = parent_status_is_valid(info->ctx, info->obj,
                             "ALLOCATED PI", "ALLOCATED UNSPECIFIED", NULL);

          if (parent_status_ok)
          {
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
          "ALLOCATED UNSPECIFIED", "ALLOCATED PA", "LIR-PARTITIONED PA", "SUB-ALLOCATED PA", "ASSIGNED PA", "EARLY-REGISTRATION", NULL);

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
          "ALLOCATED UNSPECIFIED", "ALLOCATED PI", "LIR-PARTITIONED PI", "SUB-ALLOCATED PI", "ASSIGNED PI", "EARLY-REGISTRATION", NULL);

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

/**
 * \brief IPv6 Address Assignment Business Logic Checks
 *
 * Check that the status is only set on creation (ASSIGNED ANYCAST or ASSIGNED PI);
 * Check that the object is only created by the hostmaster;
 * Check that the parent status is ALLOCATED-BY-RIR;
 *
 * \param *au_context Logging context
 * \param *old_status Status string on the old object
 * \param *new_status Status string on the new object
 * \param *rt_context Reporting context
 * \param *object The new object
 * \param *status The object status value to set
 *
 * \returns AU_AUTHORISED (default) in case the desired change is allowed by the business rules.
 * \returns AU_UNAUTHORISED_CONT in case the desired change is against some of those business rules.
 *
 */
AU_ret_t
au_v6_assigned_check ( RT_context_t * rt_context, LG_context_t *au_context, char* old_status, 
                       char* new_status, rpsl_object_t * object, char* status, au_plugin_callback_info_t *info )
{
  AU_ret_t ret_val = AU_UNAUTHORISED_CONT;
  gboolean parent_status_ok;

  LG_log(au_context, LG_FUNC, ">au_v6_assigned_check: entered.");
  LG_log(au_context, LG_DEBUG, "au_v6_assigned_check: changing to %s", status );

  if ( strcmp(old_status, "") != 0 )
  {
    /* status can only be set on object creation, not on modification */
    RT_status_check_failed_modify( rt_context, status );
    LG_log(au_context, LG_DEBUG, "au_v6_assigned_check: trying to modify status to %s", status );
  }
  else
  {
    if ( (ret_val=has_rir_mntner(info, object)) != AU_AUTHORISED )
    {
      RT_status_check_failed_rir(rt_context, status);
      LG_log(au_context, LG_DEBUG, "au_v6_assigned_check: status %s can only be set by hostmaster", status);
    }
    else
    {
      parent_status_ok = parent_status_is_valid(rt_context, object, "ALLOCATED-BY-RIR", NULL);

      if (parent_status_ok)
      {
        LG_log(au_context, LG_ERROR, "au_v6_assigned_check: parent status check ok");
      }
      else
      {
        /* note that the RT logging is done by parent_status_is_valid() */
        ret_val = AU_UNAUTHORISED_CONT;
        LG_log(au_context, LG_ERROR, "au_v6_assigned_check: parent status check failed");
      }
    }
  }

  LG_log(au_context, LG_FUNC, "<au_v6_assigned_check: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

/** 
 * \brief Run several business logic tests over inet6num objects
 *
 * Tests currently implemented include: 
 * 1. There are no lookup errors while trying to lookup the previous version of this object (doesn't matter if the object exists or not);
 * 2. There is a non-blank "status:" attribute on the new version of the object;
 * 3. If the old object "status:" is "ASSIGNED ANYCAST", don't allow it to be changed;
 * 4. If the "status:" is changed to "ALLOCATED-BY-RIR", enforce that the object:
 * 4.1. ... has the RIR maintainer in it;
 * 4.2. ... 
 *
 * XXX: FINISH DOCUMENTATION
 *
 * \param *info The plugin callback information structure.
 * \return AU_ERROR in case of lookup error while searching for the previous version of the object;
 * \return AU_UNAUTHORISED_CONT if any business logic test is not satisfied.
 *
 */
AU_ret_t
ripe_inet6num_checks (au_plugin_callback_info_t *info)
{
  rpsl_object_t *old_object;

  char *old_status;
  char *new_status;

  gboolean parent_status_ok;

  AU_ret_t ret_val = AU_UNAUTHORISED_CONT;
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
      if (!old_status)
      {
        old_status = UT_strdup("");
      }
    }

    /// XXX: This must be transformed in a case statement, so we can get rid of the if-else-if chain

    /* changing from ASSIGNED ANYCAST not allowed */
    if ((strcmp(new_status, "ASSIGNED ANYCAST") != 0) &&
             (strcmp(old_status, "ASSIGNED ANYCAST") == 0))
    {
      RT_status_check_failed_modify(info->ctx, "ASSIGNED ANYCAST");
      LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: trying to modify status from ASSIGNED ANYCAST");
    }
    /* changing from ASSIGNED PI not allowed */
    else if ((strcmp(new_status, "ASSIGNED PI") != 0) &&
             (strcmp(old_status, "ASSIGNED PI") == 0))
    {
      RT_status_check_failed_modify(info->ctx, "ASSIGNED PI");
      LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: trying to modify status from ASSIGNED PI");
    }
    /* changing from AGGREGATED-BY-LIR not allowed */
    else if ((strcmp(new_status, "AGGREGATED-BY-LIR") != 0) &&
             (strcmp(old_status, "AGGREGATED-BY-LIR") == 0))
    {
      RT_status_check_failed_modify(info->ctx, "AGGREGATED-BY-LIR");
      LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: trying to modify status from AGGREGATED-BY-LIR");
    }
    /* changing to ASSIGNED ANYCAST */
    else if ((strcmp(new_status, "ASSIGNED ANYCAST") == 0) &&
             (strcmp(old_status, "ASSIGNED ANYCAST") != 0))
    {
      ret_val = au_v6_assigned_check( info->ctx, au_context, old_status, new_status, info->obj, "ASSIGNED ANYCAST", info );
    }
    /* changing to ASSIGNED PI */
    else if ((strcmp(new_status, "ASSIGNED PI") == 0) &&
             (strcmp(old_status, "ASSIGNED PI") != 0))
    {
      ret_val = au_v6_assigned_check( info->ctx, au_context, old_status, new_status, info->obj, "ASSIGNED PI", info );
    }
    /* changing to ALLOCATED-BY-RIR */
    else if ((strcmp(new_status, "ALLOCATED-BY-RIR") == 0) &&
        (strcmp(old_status, "ALLOCATED-BY-RIR") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: ALLOCATED-BY-RIR");
      if ( (ret_val=has_rir_mntner(info, info->obj)) !=  AU_AUTHORISED )
      {
        RT_status_check_failed_allocbyrir(info->ctx);
        LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: status ALLOCATED-BY-RIR can only be set by hostmaster");
      }
      else
      {
        LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: has RIPE NCC maintainer");
        parent_status_ok = parent_status_is_valid(info->ctx, info->obj,
          "ALLOCATED-BY-RIR", NULL);
        if ( ! parent_status_ok )
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
    }
    /* changing to AGGREGATED-BY-LIR */
    else if ((strcmp(new_status, "AGGREGATED-BY-LIR") == 0) &&
             (strcmp(old_status, "AGGREGATED-BY-LIR") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: AGGREGATED-BY-LIR");
      if ( strcmp(old_status, "") )
      {
          LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: modify to AGGREGATED-BY-LIR not allowed");
          RT_status_check_failed_modify(info->ctx, "AGGREGATED-BY-LIR");
      }
      else
      {
          parent_status_ok = parent_status_is_valid(info->ctx, info->obj,
            "ALLOCATED-BY-RIR", "ALLOCATED-BY-LIR", "AGGREGATED-BY-LIR", NULL);
          if (parent_status_ok && grand_parent_status_is_valid(info->ctx, info->obj) )
          {
              ret_val = AU_AUTHORISED;
          }
      }
    }
    /* changing to ASSIGNED */
    else if ((strcmp(new_status, "ASSIGNED") == 0) &&
             (strcmp(old_status, "ASSIGNED") != 0))
    {
      LG_log(au_context, LG_DEBUG, "ripe_inet6num_checks: ASSIGNED");
      parent_status_ok = parent_status_is_valid(info->ctx, info->obj,
        "ALLOCATED-BY-RIR", "ALLOCATED-BY-LIR", "AGGREGATED-BY-LIR", NULL);
      if (parent_status_ok)
      {
        ret_val = AU_AUTHORISED;
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

