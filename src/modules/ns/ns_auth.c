/*
 * $Id: ns_auth.c,v 1.4 2006/08/07 11:20:28 katie Exp $
 */

#include "rip.h"
#include <string.h>
#include "km.h"
#include "rt_dbupdate.h"
#include "au_plugins.h"
#include "syntax_api.h"
#include "au_util.h"
#include "au_rpsl.h"
#include "ns_rir.h"
#include "ns_perl.h"
#include "ns_util.h"
#include "ns_xml.h"
#include "lu.h"

#undef MIN
#undef MAX
#include "memwrap.h"

AU_ret_t rdns_creation(au_plugin_callback_info_t * info);
AU_ret_t rdns_deletion(au_plugin_callback_info_t * info);
AU_ret_t rdns_modification(au_plugin_callback_info_t * info);

static char *CHECK_MNT_DOMAINS_THEN_MNT_LOWER_THEN_MNT_BY[] =
    { "mnt-domains", "mnt-lower", "mnt-by", NULL };
static char *CHECK_MNT_LOWER_THEN_MNT_BY[] =
    { "mnt-lower", "mnt-by", NULL };
static char *CHECK_MNT_BY[] = { "mnt-by", NULL };

/* we deal only with domain objects in the auth part */
static const au_check_by_type_t rdns_plugins[] = {
  {"domain", rdns_creation, rdns_deletion, rdns_modification},
  {NULL}
};

/*
 * Deletion of a reverse dns object
 * We don't do delegation check for deletion
 */
AU_ret_t rdns_deletion(au_plugin_callback_info_t * info)
{
  AU_ret_t ret_val;
  rpsl_object_t *old_object;
  gboolean override;
  AU_ret_t parent_auth;         /* authentication from the parent */
  LU_ret_t lu_retval;           /* lookup return value */
  gchar *domain = NULL;         /* domain entries */
  gchar *source = NULL;         /* domain name of the object */
  GList *source_attrs;          /* used to retrieve source attribute */
  GList *inetnum_obj_list;      /* list of corresponding inetnums */
  rpsl_object_t *inetnum_obj;   /* corresponding inetnum object */

  LG_log(au_context, LG_FUNC, ">rdns_deletion: entering");
  /* retrieve the current version from database */
  if (LU_get_object(au_lookup, &old_object, info->obj, NULL) != LU_OKAY) {
    LG_log(au_context, LG_DEBUG,
           "rdns_deletion: error getting current version of object");
    ret_val = AU_ERROR;
  } else if (rpsl_object_get_attr(old_object, "mnt-by") == NULL) {
    /* success if it has no mnt-by's */
    LG_log(au_context, LG_DEBUG, "rdns_deletion: no maintainer, authorised");
    ret_val = AU_AUTHORISED;
  } else {
    /* check auth from mnt-by */
    ret_val = au_check_multiple_authentications
                    (CHECK_MNT_BY, old_object, "existing", info);

    if (ns_has_suffix(domain, "e164.arpa")) {
      /* do nothing, this is for completeness */
      /* e164.arpa domains do not have any reclaim functionality. */
    } else if (ret_val != AU_AUTHORISED) {
        /* check auth from corresponding inetnum */
        domain = rpsl_object_get_key_value(info->obj);
        source_attrs = rpsl_object_get_attr(info->obj, "source");
        if (source_attrs == NULL) {
          LG_log(au_context, LG_DEBUG, "rdns_deletion: can't retrieve source from object");
          ret_val = AU_ERROR;
        } else {
          source = rpsl_attr_get_clean_value(source_attrs->data);
          lu_retval =
            LU_get_inetnum_from_domain(au_lookup, domain, source,
                                   &inetnum_obj_list);
          if (inetnum_obj_list == NULL) {
            LG_log(au_context, LG_DEBUG, "rdns_deletion: %s has no corresponding inet(6)num", domain);
            RT_no_address_space(info->ctx);
            ret_val = AU_UNAUTHORISED_CONT;
          } else if (inetnum_obj_list->next != NULL) {
            LG_log(au_context, LG_DEBUG,
                   "rdns_deletion: %s has overlapping inet(6)num parents", domain);
            RT_rdns_overlap(info->ctx);
            ret_val = AU_UNAUTHORISED_CONT;
          } else {
            inetnum_obj = inetnum_obj_list->data;
            parent_auth =
              au_check_multiple_authentications
              (CHECK_MNT_DOMAINS_THEN_MNT_LOWER_THEN_MNT_BY, inetnum_obj, "parent", info);
            ret_val = parent_auth;
          }
        }
    }
  }

  /* override */
  au_override(&ret_val, &override, info);
  RT_rdns_auth_result(info->ctx, (ret_val == AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<rdns_deletion: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

/*
 * Modification of a reverse dns object
 * TODO implement reclaim
 */
AU_ret_t rdns_modification(au_plugin_callback_info_t * info)
{
  AU_ret_t ret_val = AU_UNAUTHORISED_CONT;
  AU_ret_t au_check_result;
  rpsl_object_t *old_object;
  gchar *domain = NULL;         /* domain entries */
  gchar **nservers;             /* array of nserver entries */
  gchar **ds_rdata;             /* array of ds-rdata entries */
  gboolean override;
  gchar *delcheck_conf_file;    /* conf file to use in delchecker */

  LG_log(au_context, LG_FUNC, ">rdns_modification: entering");

  /* Extract the domain name */
  domain = rpsl_object_get_key_value(info->obj);

  /* find the configuration file for delcheck */
  delcheck_conf_file = ns_find_delcheck_conf(au_context, domain);

  /* retrieve the current version from database */
  if (LU_get_object(au_lookup, &old_object, info->obj, NULL) != LU_OKAY) {
    LG_log(au_context, LG_DEBUG,
           "rdns_modification: error getting current version of object");
    ret_val = AU_ERROR;
  } else if (rpsl_object_get_attr(old_object, "mnt-by") != NULL) {
    /* check auth from existing object if it contains mnt-by */
    ret_val =
        au_check_multiple_authentications(CHECK_MNT_BY, old_object,
                                          "existing", info);
  } else {
    /* check auth from new object */
    ret_val =
        au_check_multiple_authentications(CHECK_MNT_BY, info->obj,
                                          "existing", info);
  }
  /* if no conf file, reject */
  if (delcheck_conf_file == NULL) {
    LG_log(au_context, LG_DEBUG,
             "rdns_modification: no delcheck conf file found - rdns size not accepted");
    RT_rdns_size_not_accepted(info->ctx);
    ret_val = AU_UNAUTHORISED_CONT;
  }
  else {
    if (ret_val == AU_AUTHORISED) {
      /* check if we're the related party */
      au_check_result = ns_find_rir(info, domain);
      if (au_check_result == AU_AUTHORISED) {
          /* find nservers */
          nservers =
              ns_nservers(info->obj, info->ctx, domain, &au_check_result);
          if (au_check_result != AU_AUTHORISED) {
            LG_log(au_context, LG_DEBUG,
                         "rdns_modification: no nservers found");
            ret_val = au_check_result;
          } else {
            /* find ds-rdata records */
            ds_rdata = ns_ds_rdata(info->obj, info->ctx, domain, &au_check_result);
            if (au_check_result != AU_AUTHORISED) {
              ret_val = au_check_result;
            } else {
              
              if (ds_rdata != NULL && ns_ds_accepted(domain) == FALSE) {
                 LG_log(au_context, LG_DEBUG,
                               "rdns_modification: ds not accepted");
                 RT_rdns_ds_not_accepted(info->ctx);
                 au_check_result = AU_UNAUTHORISED_CONT;
              }
              else {
                /* call delcheck */
                au_check_result = ns_domain_delcheck(info, domain, nservers, ds_rdata, delcheck_conf_file);
              }

              if (ds_rdata != NULL)  {
                g_strfreev(ds_rdata);
              }
              if (au_check_result != AU_AUTHORISED) {
                ret_val = au_check_result;
              }
            }
          }
          if (nservers != NULL) {
            g_strfreev(nservers);
          }
      } else {
        LG_log(au_context, LG_DEBUG,
                   "rdns_modification: not the right RIR");
        ret_val = AU_UNAUTHORISED_CONT;
      }
    }
  }

  /* override */
  au_override(&ret_val, &override, info);
  RT_rdns_auth_result(info->ctx, (ret_val == AU_AUTHORISED), override);

  LG_log(au_context, LG_FUNC, "<rdns_modification: exiting with value [%s]",
         AU_ret2str(ret_val));

  if (delcheck_conf_file != NULL) {
    g_free(delcheck_conf_file);
  }
  if (domain != NULL) {
    free(domain);
  }
  return ret_val;
}

/*
 * Flat authorisation for e164.arpa dns object 
 * similar to ns_hierarchical_creation, but there is no check
 * for corresponding inetnum object
 * also, we make sure that the parent object is 'e164.arpa' - no further
 * hierarchies are allowed.
*/

static AU_ret_t ns_flat_creation(au_plugin_callback_info_t * info,
                                 gchar * domain, gchar * source)
{
  AU_ret_t ret_val;             /* result of the function */
  GList *parents = NULL;        /* parent domains */
  rpsl_object_t *parent;        /* rpsl object for the parent */
  gchar *parent_key;            /* key of the parent object */

  LG_log(au_context, LG_FUNC, ">ns_flat_creation: entering");
  
  if (LU_get_parents(au_lookup, &parents, info->obj, NULL) != LU_OKAY) {
    /* error getting parent list */
    LG_log(au_context, LG_DEBUG, "ns_flat_creation: error getting parents of %s", domain);
    ret_val = AU_ERROR;
  } else if (parents == NULL) {
    /* there are no parents */
    LG_log(au_context, LG_DEBUG, "ns_flat_creation: %s has no parents", domain);
    RT_parent_not_exist(info->ctx);
    ret_val = AU_UNAUTHORISED_CONT;
  } else {
    /* one parent is enough */
    parent = parents->data;
    /* check whether parent is e164.arpa */
    parent_key = rpsl_object_get_key_value(parent);
    if (strcasecmp (parent_key, "e164.arpa") == 0 ) {
      ret_val = au_check_multiple_authentications (CHECK_MNT_LOWER_THEN_MNT_BY, parent, "parent", info);
    } else {
      RT_rdns_parentisnotenum(info->ctx);
      LG_log(au_context, LG_DEBUG, "ns_flat_creation: parent is not e164.arpa");
      ret_val = AU_UNAUTHORISED_CONT;
    }
  }
  
  LG_log(au_context, LG_FUNC, "<ns_flat_creation: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;

}
/*
 * Hierarchical authorisation of a reverse dns object
 * authorisation is only checked against address space
 */
static AU_ret_t ns_hierarchical_creation(au_plugin_callback_info_t * info,
                                         gchar * domain, gchar * source)
{
  AU_ret_t ret_val;             /* result of the function */
  GList *inetnum_obj_list;      /* list of corresponding inetnums */
  rpsl_object_t *inetnum_obj;   /* corresponding inetnum object */
  LU_ret_t lu_retval;           /* lookup return value */

  LG_log(au_context, LG_FUNC, ">ns_hierarchical_creation: entering");

  lu_retval =
      LU_get_inetnum_from_domain(au_lookup, domain, source, &inetnum_obj_list);
  if (lu_retval != LU_OKAY) {
    LG_log(au_context, LG_DEBUG, "ns_hierarchical_creation: error retrieving inet(6)num for %s", domain);
    ret_val = AU_ERROR;
  } else if (inetnum_obj_list == NULL) {
    LG_log(au_context, LG_DEBUG, "ns_hierarchical_creation: %s has no corresponding inet(6)num object", domain);
    RT_no_address_space(info->ctx);
    ret_val = AU_UNAUTHORISED_CONT;
  } else if (inetnum_obj_list->next != NULL) {
    LG_log(au_context, LG_DEBUG, "ns_hierarchical_creation: %s has overlapping inet(6)num parents", domain);
    RT_rdns_overlap(info->ctx);
    ret_val = AU_UNAUTHORISED_CONT;
  } else {
    inetnum_obj = inetnum_obj_list->data;
    /* check authorization from corresponding inet(6)num */
    ret_val = au_check_multiple_authentications
                     (CHECK_MNT_DOMAINS_THEN_MNT_LOWER_THEN_MNT_BY, inetnum_obj, "parent", info);
  }

  LG_log(au_context, LG_FUNC, "<ns_hierarchical_creation: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

/*
 * Creation of a reverse dns object
 */
AU_ret_t rdns_creation(au_plugin_callback_info_t * info)
{
  AU_ret_t ret_val = AU_AUTHORISED;     /* return value of this function */
  gchar *domain = NULL;         /* domain name of the object */
  gchar *source = NULL;         /* domain name of the object */
  gchar **nservers = NULL;      /* array of nserver entries */
  gchar **ds_rdata = NULL;      /* array of ds-rdata entries*/
  AU_ret_t au_check_result;     /* result returned from functions */
  GList *source_attrs;          /* used to retrieve source attribute */
  gboolean override;
  gchar *delcheck_conf_file;    /* conf file to use in delchecker */

  LG_log(au_context, LG_FUNC, ">rdns_creation: entering");

  /* Extract the domain name */
  domain = rpsl_object_get_key_value(info->obj);

  /* find the configuration file for delcheck */
  LG_log(au_context, LG_DEBUG, "rdns_creation: find the delcheck configuration");
  delcheck_conf_file = ns_find_delcheck_conf(au_context, domain);

  /* Extract the source attribute */
  source_attrs = rpsl_object_get_attr(info->obj, "source");
  if (source_attrs == NULL) {
    LG_log(au_context, LG_DEBUG, "rdns_creation: can't retrieve source from object");
    ret_val = AU_ERROR;
  } else {
    LG_log(au_context, LG_DEBUG, "rdns_creation: found the object source");
    source = rpsl_attr_get_clean_value(source_attrs->data);
    if (delcheck_conf_file == NULL) {   
      RT_rdns_size_not_accepted(info->ctx);
      ret_val = AU_UNAUTHORISED_CONT;
    } else {
      /* Extract the related nservers */
      LG_log(au_context, LG_DEBUG, "rdns_creation: extracting the nservers");
      nservers =
          ns_nservers(info->obj, info->ctx, domain, &au_check_result);
      if (au_check_result != AU_AUTHORISED) {
        ret_val = AU_UNAUTHORISED_CONT;
        LG_log(au_context, LG_DEBUG, "rdns_creation: problem extracting the nservers");
      } else {
        /* find ds-rdata records */
        LG_log(au_context, LG_DEBUG, "rdns_creation: checking DS rdata records");
        ds_rdata = ns_ds_rdata(info->obj, info->ctx, domain, &au_check_result);

        /* Check if we're the related party, fail if not */
        LG_log(au_context, LG_DEBUG, "rdns_creation: checking the RIR");
        au_check_result = ns_find_rir(info, domain);
        if (au_check_result != AU_AUTHORISED) {
          ret_val = au_check_result;
        } else {

          /* Check if the parent is us */
//          LG_log(au_context, LG_DEBUG, "rdns_creation: checking the parent");
//          au_check_result = ns_is_parent_ours(info, domain);
//          if (au_check_result != AU_AUTHORISED) {
//            ret_val = au_check_result;
//          } else {
            if (ns_is_e164_arpa(info)) {
              /* Check flat authorization */
              LG_log(au_context, LG_DEBUG, "rdns_creation: check the e164 flat authorisation");
              au_check_result = ns_flat_creation(info, domain, source);
            }
            else {
              /* Check hierarchical authorization */
              LG_log(au_context, LG_DEBUG, "rdns_creation: check hierarchical authorisation");
              au_check_result = ns_hierarchical_creation(info, domain, source);
            }
            if (au_check_result != AU_AUTHORISED) {
              ret_val = au_check_result;
            } else {

              if ( ds_rdata != NULL && ns_ds_accepted(domain) == FALSE) {
                  LG_log(au_context, LG_DEBUG, "rdns_creation: ds not accepted");
                  RT_rdns_ds_not_accepted(info->ctx);
                  au_check_result = AU_UNAUTHORISED_CONT;
              }
              else {
                /* call delcheck */
                LG_log(au_context, LG_DEBUG, "rdns_creation: calling delchecker");
                au_check_result =
                  ns_domain_delcheck(info, domain, nservers, ds_rdata,
                                     delcheck_conf_file);
              }
              if (au_check_result != AU_AUTHORISED) {
                ret_val = au_check_result;
              }
            }
//          }
        }
      }
    }
  }

  /* override */
  au_override(&ret_val, &override, info);
  RT_rdns_auth_result(info->ctx, (ret_val == AU_AUTHORISED), override);

  /* cleanup */
  if (source != NULL) {
    free(source);
  }
  if (domain != NULL) {
    free(domain);
  }
  if (nservers != NULL) {
    g_strfreev(nservers);
  }
  if (ds_rdata != NULL) {
    g_strfreev(ds_rdata);
  }
  if (delcheck_conf_file != NULL) {
    g_free(delcheck_conf_file);
  }
  rpsl_attr_delete_list(source_attrs);
  LG_log(au_context, LG_FUNC, "<rdns_creation: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}

/* 
  Entry point for rdns authorisation - invoke AU by-type support
  
  trans    - PG module transaction
  info     - AU plugin callback information

  return   - PG_OK means AU_AUTHORISED
             PG_ERROR_PROCEED means AU_UNAUTHORISED_CONT
             PG_ERROR_STOP means AU_UNAUTHORISED_END
             PG_ERROR_PLUGIN_NAME means AU_ERROR

  This is merely a dispatcher.
 */
PG_status_t au_rdns_check(PG_transaction_t * trans, gpointer * info)
{
  PG_status_t ret_val;
  au_plugin_callback_info_t *callback_info;
  AU_ret_t au_ret_val;
  const char *object_type;

  LG_log(au_context, LG_FUNC, ">au_rdns_check: entering");

  /* check if we really need to apply specific rules */
  callback_info = PG_global_get(trans);

  /* check object type */
  object_type = rpsl_object_get_class(callback_info->obj);

  if (!ns_is_rdns_suffix(callback_info)) {
    LG_log(au_context, LG_DEBUG, "au_rdns_check: not rdns");
    ret_val = AU_AUTHORISED;
  } else {
    LG_log(au_context, LG_DEBUG, "au_rdns_check: doing rdns checks");
    au_ret_val = AU_check_by_type(rdns_plugins, callback_info);
    switch (au_ret_val) {
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
      LG_log(au_context, LG_ERROR,
             "au_rdns_check: unknown return %d from AU_check_by_type()",
             au_ret_val);
    }
  }

  LG_log(au_context, LG_FUNC, "<au_rdns_check: exiting with value [%d]",
         ret_val);
  return ret_val;
}

gboolean au_rdns_init()
{
  return TRUE;
}
