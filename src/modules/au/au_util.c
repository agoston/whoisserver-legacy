#include <string.h>

#include <glib.h>
#include <stdio.h>
#include "ca_configFns.h"
#include "ca_defs.h"
#include "ca_dictionary.h"
#include "ca_macros.h"


#include "cr.h"
#include "au_util.h"
#include "lu.h"
#include "rt_dbupdate.h"

#undef MIN
#undef MAX
#include "memwrap.h"

void au_override(AU_ret_t *ret_val, gboolean *override, au_plugin_callback_info_t *info) {
    char *override_pwd;
    gchar **override_pgp_keys, **override_pgp_key;
    size_t len;

    LG_log(au_context, LG_FUNC, ">au_override: entering");

    LG_log(au_context, LG_DEBUG, "au_override: ret_val start [%s]", AU_ret2str(*ret_val));

    /* only allow override if we do not have an error */
    if (*ret_val != AU_ERROR) {
        /* set default value */
        *override = FALSE;

        override_pwd = ca_get_overridecryptedpw;
        /* chop off any trailing newline */
        len = strlen(override_pwd);
        while ((len > 0) && (override_pwd[len - 1] == '\n')) {
            override_pwd[len - 1] = '\0';
            len--;
        }

        if (CR_credential_list_check(info->cred, CR_OVERRIDE, override_pwd, FALSE)) {
            *ret_val = AU_AUTHORISED;
            *override = TRUE;
        } else {
            /* check for PGP override if password override fails */
            char *overridepgp = ca_get_overridepgp;
            override_pgp_keys = g_strsplit(overridepgp, "\n", -1);

            for (override_pgp_key = override_pgp_keys; override_pgp_key && *override_pgp_key; override_pgp_key++) {
                LG_log(au_context, LG_DEBUG, "au_override: trying override_pgp_key [%s]", *override_pgp_key);
                if (CR_credential_list_check(info->cred, CR_PGP, *override_pgp_key, FALSE)) {
                    *ret_val = AU_AUTHORISED;
                    *override = TRUE;
                    LG_log(au_context, LG_DEBUG, "au_override: successfully authorized override_pgp_key [%s]", *override_pgp_key);
                    break;
                }
            }
            UT_free(overridepgp);
            g_strfreev(override_pgp_keys);
        }
        UT_free(override_pwd);
    }

    LG_log(au_context, LG_DEBUG, "au_override: ret_val final [%s]", AU_ret2str(*ret_val));
    LG_log(au_context, LG_FUNC, "<au_override: exiting");
}

AU_ret_t
au_check_authentications (GList *auth_attrs, GList *cred)
{
  AU_ret_t ret_val;
  GList *p;
  const gchar *auth;
  gboolean known_auth;
  CR_type auth_type = -1;
  const gchar *auth_val = NULL;

  LG_log(au_context, LG_FUNC, ">au_check_authentications: entering");

  /* default to unauthenticated, and set to authenticated if any match */
  ret_val = AU_UNAUTHORISED_CONT;

  /* loop through our attributes */
  for (p=auth_attrs; p != NULL; p = g_list_next(p))
  {
    auth = rpsl_attr_get_value(p->data);

    /* NONE is a special case and always succeeds */
    if (strcasecmp(auth, "NONE") == 0)
    {
      ret_val = AU_AUTHORISED;
      LG_log(au_context, LG_DEBUG,
             "au_check_authentications: NONE authenticated");
    }
    else
    {
      /* flag to verify we have a good authentication type */
      known_auth = TRUE;

      /* determine the authentication type and data from the attribute */
      if (strncasecmp(auth, "PGPKEY-", 7) == 0)
      {
        auth_type = CR_PGP;
        auth_val = auth+7;
      }
      else if (strncasecmp(auth, "MAIL-FROM ", 10) == 0)
      {
        auth_type = CR_FROM;
        auth_val = auth+10;
      }
      else if (strncasecmp(auth, "X509-", 5) == 0)
      {
        auth_type = CR_X509;
        auth_val = auth;
      }
      else if (strncasecmp(auth, "CRYPT-PW ", 9) == 0)
      {
        auth_type = CR_PASSWORD;
        auth_val = auth+9;
      }
      else if (strncasecmp(auth, "MD5-PW ", 7) == 0)
      {
        auth_type = CR_PASSWORD;
        auth_val = auth+7;
      }
      else
      {
        known_auth = FALSE;
        LG_log(au_context, LG_WARN,
               "au_check_authentications: unrecognised authentication %s",
               auth);
      }

      LG_log(au_context, LG_DEBUG,
               "au_check_authentications: auth_type %s", CR_type2str(auth_type));
      LG_log(au_context, LG_DEBUG,
               "au_check_authentications: auth_val %s", auth_val ? auth_val : "UNKNOWN");

      /* check the credential list for this authentication */
      if (known_auth &&
          CR_credential_list_check(cred, auth_type, auth_val, FALSE))
      {
        ret_val = AU_AUTHORISED;
        LG_log(au_context, LG_DEBUG,
               "au_check_authentications: %s authenticated", auth);
      }
      else
      {
        LG_log(au_context, LG_DEBUG,
               "au_check_authentications: %s NOT authenticated", auth);
      }
    }
  }

  /* log result */
  LG_log(au_context, LG_FUNC,
         "<au_check_authentications: exiting with value [%s]",
         AU_ret2str(ret_val));

  /* return result */
  return ret_val;
}

AU_ret_t
au_mntner_authenticate(RT_context_t * ctx, const gchar * mntner_name, LU_server_t * lu,
	const gchar * source_name, GList * cred, rpsl_object_t ** mntner)
{
	AU_ret_t ret_val;
	GList *auth_attrs;

	LG_log(au_context, LG_FUNC, ">au_mntner_authenticate: entering");

	LG_log(au_context, LG_DEBUG, "au_mntner_authenticate: checking maintainer %s", mntner_name);

	/* try looking up the maintainer */
	if (LU_lookup(lu, mntner, "mntner", mntner_name, source_name) == LU_ERROR) {
		/* lookup failed */
		*mntner = NULL;
		LG_log(au_context, LG_WARN, "au_mntner_authenticate: error looking up mntner %s", mntner_name);
		ret_val = AU_ERROR;
	} else {
		/* lookup worked */
		if (*mntner == NULL) {
			/* no such maintainer (how did this get here?!?!?) */
			LG_log(au_context, LG_WARN, "au_mntner_authenticate: non-existant mntner %s", mntner_name);
			RT_non_exist_mntner(ctx, (char *)mntner_name);
			ret_val = AU_UNAUTHORISED_CONT;
		} else {
			/* mntner exists - check the "auth:" attributes */
			auth_attrs = rpsl_object_get_attr(*mntner, "auth");
			rpsl_attr_split_multiple(&auth_attrs);
			ret_val = au_check_authentications(auth_attrs, cred);
		}
	}

	LG_log(au_context, LG_FUNC, "<au_mntner_authenticate: exiting with value [%s]", AU_ret2str(ret_val));

	return ret_val;
}

static void au_rpsl_object_delete (gpointer data, gpointer user_data)
{
  rpsl_object_delete((rpsl_object_t *)data);
}

static gchar *
get_source (const rpsl_object_t *obj)
{
  GList *source_attr;
  gchar *source;

  source_attr = rpsl_object_get_attr(obj, "source");
  if (source_attr == NULL)
  {
    LG_log(au_context, LG_ERROR, "get_source: missing \"source:\" attribute");
    source = NULL;
  }
  else
  {
    source = rpsl_attr_get_clean_value(source_attr->data);
    rpsl_attr_delete_list(source_attr);
  }

  return source;
}

AU_ret_t
au_check_multiple_authentications (char *attr_to_check[],
                                   rpsl_object_t *obj,
                                   char *parent_text,
                                   au_plugin_callback_info_t *info)
{
  AU_ret_t ret_val;
  int num_error;
  char *key;
  const gchar *type;
  char *mnt_type;

  GList *authenticated;
  GList *unauthenticated;
  GList *mntners;
  GList *attr;
  gchar *value;
  gchar *source;

  rpsl_object_t *mntner;

  int i;

  LG_log(au_context, LG_FUNC, ">check_authentications: entering");

  num_error = 0;
  authenticated = NULL;
  unauthenticated = NULL;

  /* look through the list of attributes to use authentications in, and
     stop when we find the first one */

  mntners = NULL;
  for (i=0; (mntners == NULL) && (attr_to_check[i] != NULL); i++)
  {
    mntners = rpsl_object_get_attr(obj, attr_to_check[i]);
    mnt_type = strdup(attr_to_check[i]);
  }

  /* for now, if there are no maintainers, the creation is authorised.
     this is bogus, and should be changed to unauthorised, which could be
     done by simply removing these 7 lines
     */
  if (mntners == NULL)
  {
    mnt_type = NULL;
    LG_log(au_context, LG_DEBUG, "check_authentications: no mntners in object");
    LG_log(au_context, LG_FUNC, "<check_authentications: exiting with value [AU_AUTHORISED]");
    return AU_AUTHORISED;
  }

  /* find source */
  source = get_source(obj);
  if (source == NULL)
  {
    rpsl_attr_delete_list(mntners);
    LG_log(au_context, LG_FUNC, "<check_authentications: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }

  ret_val = AU_UNAUTHORISED_CONT;

  if (strcasecmp(mnt_type, "mnt-routes") == 0)
  {
    /* check if maintainer is applicable for this route, report possible errors */
    au_check_mnt_routes_prefix (&mntners, obj, parent_text, info);
    if (mntners == NULL)
    {
      /* no more valid mntners to check */
      mnt_type = NULL;
      LG_log(au_context, LG_DEBUG, "check_authentications: no valid mntners in object");
      LG_log(au_context, LG_FUNC, "<check_authentications: exiting with value [AU_UNAUTHORISED_CONT]");
      RT_no_mntners(info->ctx, strdup(rpsl_object_get_key_value(obj)), strdup(rpsl_object_get_class(obj)), parent_text);
      return AU_UNAUTHORISED_CONT;
    }
  }

  /* check the maintainers listed to see if any of them are authenticated */
  rpsl_attr_split_multiple(&mntners);

  /* make the mntner list unique */
  rpsl_attr_uniq_list(&mntners);

  for (attr = mntners; attr != NULL; attr = g_list_next(attr))
  {
    value = rpsl_attr_get_clean_value(attr->data);
    g_strup(value);
    LG_log(au_context, LG_DEBUG, "check_authentications: checking mntner %s",
           value);

    switch (au_mntner_authenticate(info->ctx, value, au_lookup, source, info->cred, &mntner))
    {
      case AU_AUTHORISED:
        LG_log(au_context, LG_DEBUG, "check_authentications: %s authenticated", value);
        ret_val = AU_AUTHORISED;
        authenticated = g_list_append(authenticated, mntner);
        LG_log(au_context, LG_DEBUG,"check_authentications: authenticated by [%d] mntners", g_list_length(authenticated));
        break;
      case AU_UNAUTHORISED_CONT:
        LG_log(au_context, LG_DEBUG, "check_authentications: %s not authenticated", value);
        if (mntner != NULL)
        {
          unauthenticated = g_list_append(unauthenticated, mntner);
          LG_log(au_context, LG_DEBUG,"check_authentications: not authenticated by [%d] mntners", g_list_length(unauthenticated));
        }
        break;
      case AU_ERROR:
        LG_log(au_context, LG_DEBUG, "check_authentications: %s caused error", value);
        num_error++;
        if (mntner != NULL)
        {
          unauthenticated = g_list_append(unauthenticated, mntner);
          LG_log(au_context, LG_DEBUG,"check_authentications: not authenticated by [%d] mntners", g_list_length(unauthenticated));
        }
        break;
      default:
        rpsl_attr_delete_list(mntners);
        LG_log(au_context, LG_FUNC, "<check_authentications: au return code not recognised, exiting with value [AU_ERROR]");
        return AU_ERROR;
        break;
    }
    UT_free(value);
  }
  rpsl_attr_delete_list(mntners);
  UT_free(source);

  /*********************************************************************/
  /* record maintainers used, attribute checked */
  /**** this must be done before these lists are concatenated below ****/
  key = rpsl_object_get_key_value(obj);
  type = rpsl_object_get_class(obj);
  RT_auth(info->ctx, key, strdup(type), mnt_type, authenticated, unauthenticated, parent_text);
  UT_free(key);
  /* UT_free(type); */
  UT_free(mnt_type);

  /* append any mntners checked to the list of mntner_used */
  info->mntner_used = g_list_concat(info->mntner_used, authenticated);
  info->mntner_used = g_list_concat(info->mntner_used, unauthenticated);
  /*********************************************************************/

  /* handle errors */
  if (num_error > 0)
  {
    ret_val = AU_ERROR;
    /* free list of mntners */
    g_list_foreach(info->mntner_used, au_rpsl_object_delete, NULL);
    info->mntner_used = NULL;
  }

  LG_log(au_context, LG_FUNC, "<check_authentications: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}



AU_ret_t
au_rir_auth_check(au_plugin_callback_info_t *info, const rpsl_object_t *obj, GList *rir_mntner_list)
{
  AU_ret_t ret_val = AU_ERROR;
  gchar *value;
  gchar *source;
  GList *attr;

  rpsl_object_t *mntner;

  LG_log(au_context, LG_FUNC, ">au_rir_auth_check: entering");

  /* find source */
  source = get_source(obj);
  if (source == NULL)
  {
    LG_log(au_context, LG_FUNC, "<au_rir_auth_check: exiting with value [AU_ERROR]");
    return AU_ERROR;
  }

  /* Check each of the mntners in the list */
  for (attr = rir_mntner_list; attr != NULL; attr = g_list_next(attr))
  {
    value = rpsl_attr_get_clean_value(attr->data);
    g_strup(value);
    LG_log(au_context, LG_DEBUG, "au_rir_auth_check: checking mntner %s",
           value);

    ret_val = au_mntner_authenticate(info->ctx, value, au_lookup, source, info->cred, &mntner);
    if (ret_val == AU_AUTHORISED)
    {
      LG_log(au_context, LG_DEBUG, "au_rir_auth_check: authorised by mntner %s",
               value);
      UT_free(value);
      break;
    }
    UT_free(value);
  }

  LG_log(au_context, LG_FUNC, "<au_rir_auth_check: exiting with value [%s]",
         AU_ret2str(ret_val));
  return ret_val;
}
