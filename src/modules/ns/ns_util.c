/*
 * $Id: ns_util.c,v 1.1 2004/12/27 16:38:43 can Exp $
 */

#include "ns_util.h"
#include "up_util.h"
#include "regex.h"

/*
 * Case insensitive strstr()
 */
static gchar *stristr(const gchar * haystack, const gchar * needle)
{
  /* use copies, to prevent string modification */
  gchar *haystack_copy = NULL, *needle_copy = NULL, *result_copy;
  gchar *result;

  if (haystack == NULL) {
    result = NULL;
  } else {
    haystack_copy = g_strdup(haystack);
    if (needle != NULL) {
      needle_copy = g_strdup(needle);
    }
    g_strdown(needle_copy);
    g_strdown(haystack_copy);
    result_copy = strstr(haystack_copy, needle_copy);
    if (result_copy == NULL) {
      result = NULL;
    } else {
      result = (gchar *) (haystack + (result_copy - haystack_copy));
    }
  }
  if (needle_copy != NULL) {
    g_free(needle_copy);
  }
  if (haystack_copy != NULL) {
    g_free(haystack_copy);
  }
  return (result);
}

/*
 * wraps the text, suitable for formatting
 */
gchar *ns_par(gchar * str)
{
  gchar *result;                /* resulting string */
  gchar *p, *pp, *resultp;      /* temporary iterators */
  gchar *prevnl;                /* pointer to last linefeed */
  gint nl_cnt;                  /* count of line feeds */

  result = g_strdup_printf("%s", str);

  /* remove leading/trailing whitespace */
  g_strstrip(result);

  /* remove repeating whitespace */
  p = result;
  while ((*p != 0) && ((*(p + 1)) != 0) && (p != NULL)
         && ((p + 1) != NULL)) {
    while ((isspace((int) *p) && (isspace((int) *(p + 1))))) {
      sprintf(p, p + 1);
      *p = ' ';
    }
    p++;
  }

  /* place linefeeds */
  p = result;
  prevnl = p;
  nl_cnt = 0;
  while ((*p != 0) && (p != NULL)) {
    if (*p == ' ') {
      pp = strchr((p + 1), ' ');
      if (pp == NULL) {
        pp = strchr(p + 1, 0);
      }
      if ((pp - prevnl) > 60) { /* 60 is the line width,
                                   it doesn't need configurability now */
        *p = '\n';
        nl_cnt++;
        prevnl = p + 1;
      }
    }
    p++;
  }

  /* indent */
  resultp = g_new0(gchar, strlen(result) + nl_cnt * 20 + 1);
  p = result;
  pp = strchr(result, '\n');
  while ((pp != NULL) && (result != NULL) && (*result != 0)) {
    *pp = 0;
    strcat(resultp, result);
    strcat(resultp, "\n            ");  /* 13 spaces is the indentation */
    result = pp + 1;
    pp = strchr(result, '\n');
  }
  strcat(resultp, result);
  g_free(p);

  return (resultp);
}

/*
 * Extract the related nservers
 */
gchar **ns_nservers(rpsl_object_t * obj, RT_context_t * ctx,
                    gchar * domain, AU_ret_t * result)
{
  GList *nserver_attrs;         /* nserver attributes from rpsl object */
  gint nserver_count = 1;       /* number of nservers in the object */
  gint i;                       /* generic temporary int */
  gchar *str;                   /* generic temporary string */
  gchar **nservers;             /* result */

  nserver_attrs = rpsl_object_get_attr(obj, "nserver");
  rpsl_attr_split_multiple(&nserver_attrs);
  if (nserver_attrs == NULL) {  /* we don't accept domains with no nservers */
    LG_log(au_context, LG_DEBUG, "object contains no nservers");
    RT_rdns_nonservers(ctx);
    *result = AU_UNAUTHORISED_CONT;
    nservers = NULL;
  } else {
    while ((nserver_attrs->next) != NULL) {
      nserver_count++;
      nserver_attrs = nserver_attrs->next;
    }
    nservers = g_new(char *, nserver_count + 1);
    i = 0;
    while ((nserver_attrs->prev) != NULL) {
      str =
          rpsl_attr_get_clean_value((rpsl_attr_t *) (nserver_attrs->data));
      nservers[i] = g_strdup(str);
      nservers[i + 1] = NULL;
      free(str);
      i++;
      nserver_attrs = nserver_attrs->prev;
    }
    str = rpsl_attr_get_clean_value((rpsl_attr_t *) (nserver_attrs->data));
    nservers[i] = g_strdup(str);
    nservers[i + 1] = NULL;
    free(str);
    rpsl_attr_delete_list(nserver_attrs);
    *result = AU_AUTHORISED;
  }
  return nservers;
}

/*
 * checks whether the suffix is rdns related
 */
static gboolean ns_check_suffix(rpsl_object_t * obj, gboolean with_dot)
{
  gchar *domain;                /* domain name */
  gchar *p;                     /* temporary pointer */
  gchar *ns_suffix_dotted;      /* add dot to ns_suffix */
  gboolean ret_val = FALSE;     /* result */
  gint i = 0;                   /* generic iterator */
  gchar **ns_suffix;            /* ns suffixes we recognize */

  ns_suffix = ut_g_strsplit_v1(ca_get_ns_suffix, "\n", 0);

  /* Extract the domain name */
  domain = rpsl_object_get_key_value(obj);

  /* check suffix */
  while (ns_suffix[i] != NULL) {
    if (with_dot) {
      ns_suffix_dotted = g_strdup_printf("%s.", ns_suffix[i]);
    } else {
      ns_suffix_dotted = g_strdup_printf("%s", ns_suffix[i]);
    }
    p = stristr(domain, ns_suffix_dotted);
    if ((p != NULL) && (strcasecmp(p, ns_suffix_dotted) == 0)) {
      ret_val = TRUE;
    }
    g_free(ns_suffix_dotted);
    i++;
  }

  free(domain);
  g_strfreev(ns_suffix);
  return ret_val;
}

/*
 * removes the trailing dot from string
 */
gboolean ns_remove_trailing_dot(LG_context_t * lg_ctx, gchar ** object_str)
{
  rpsl_object_t *object;        /* object itself */
  const rpsl_attr_t *domain_attr;       /* key attribute */
  gchar *domain;                /* extracted domain string */
  gchar *p;                     /* temporary pointer for iteration */
  const gchar *gp;              /* temporary pointer */
  gchar *new_object_str;        /* replacement object string */
  gboolean ret_val = FALSE;     /* our return value */
  gint space_count = 0;         /* leading spaces in the original attr */
  gchar *space_str;             /* copy of leading spaces */
  gchar *domain_with_space;     /* removed version without space */

  LG_log(lg_ctx, LG_DEBUG,
         "checking whether to remove trailing dot from domain attribute");
  if (*object_str == NULL) {
    LG_log(lg_ctx, LG_DEBUG, "NULL string in object_str");
    return FALSE;
  }
  object = rpsl_object_init(*object_str);

  /* Extract the domain name */
  domain = rpsl_object_get_key_value(object);

  if (!ns_check_suffix(object, TRUE)) {
    LG_log(lg_ctx, LG_DEBUG, "object is NOT rdns related");
    ret_val = FALSE;
  } else {
    LG_log(lg_ctx, LG_DEBUG, "object is rdns related");
    /* Remove . at the end if any */
    p = strrchr(domain, '.');
    if ((p != NULL) && (strcmp(p, ".") == 0)) {
      /* removing . */
      LG_log(lg_ctx, LG_DEBUG, "removing trailing . from %s", domain);
      *p = 0;

      /* copy leading whitespace - we assume domain starts with a number
       * disclaimer: any failure will make the object only uglier, there
       * are no other side effects */
      domain_attr = rpsl_object_get_attr_by_ofs(object, 0);
      gp = rpsl_attr_get_value(domain_attr);
      if (gp == NULL) {
        LG_log(lg_ctx, LG_DEBUG, "can't get value of domain attribute");
        ret_val = FALSE;
      } else {
        p = (gchar *) gp;
        while (!isalnum((int) *p)) {
          p++;
          space_count++;
        }
        space_str = g_strdup(gp);
        g_snprintf(space_str, space_count, "%s", gp);
        domain_with_space = g_strdup_printf(" %s%s", space_str, domain);
        g_free(space_str);
        rpsl_attr_replace_value((rpsl_attr_t *) domain_attr,
                                domain_with_space);
        g_free(domain_with_space);
        new_object_str = rpsl_object_get_text(object, 0);
        strcpy(*object_str, new_object_str);
        free(new_object_str);
        ret_val = TRUE;
      }
    } else {
      LG_log(lg_ctx, LG_DEBUG, "no trailing dot at the end of %s", domain);
    }
  }

  rpsl_object_delete(object);
  free(domain);
  return ret_val;
}

/*
 * checks whether the suffix is rdns related
 */
gboolean ns_is_rdns_suffix(au_plugin_callback_info_t * info)
{
  if (ns_check_suffix(info->obj, FALSE)) {
    LG_log(au_context, LG_DEBUG, "object is rdns related");
    return TRUE;
  } else {
    LG_log(au_context, LG_DEBUG, "object is NOT rdns related");
    return FALSE;
  }
}

/*
 * checks whether the parent object has our name servers
 */
AU_ret_t ns_is_parent_ours(au_plugin_callback_info_t * info,
                           gchar * domain)
{
  AU_ret_t ret_val;             /* our return value */
  GList *parents;               /* list of parent domains */
  rpsl_object_t *parent;        /* one parent object */
  gchar *parent_key;            /* domain attribute of key */
  gchar **nservers;             /* list of name servers */
  gint i, j;                    /* temp iterators */
  gboolean found;               /* flag to set if the prefix is found */
  gchar **ns_ours;              /* our name servers */

  ns_ours = ut_g_strsplit_v1(ca_get_ns_nservers, "\n", -1);

  if (LU_get_parents(au_lookup, &parents, info->obj, NULL) != LU_OKAY) {
    /* error getting parent list */
    LG_log(au_context, LG_DEBUG, "error getting parents of %s", domain);
    ret_val = AU_ERROR;
  } else if (parents == NULL) {
    /* there are no parents */
    LG_log(au_context, LG_DEBUG, "%s has no parents", domain);
    RT_parent_not_exist(info->ctx);
    ret_val = AU_UNAUTHORISED_CONT;
  } else {
    /* one parent is enough */
    parent = parents->data;
    parent_key = rpsl_object_get_key_value(parent);
    nservers = ns_nservers(parent, info->ctx, parent_key, &ret_val);
    LG_log(au_context, LG_DEBUG, "parent is %s with nservers %s\n",
           parent_key, nservers ? g_strjoinv(",",
                                             nservers) : "NONE FOUND");
    i = 0;
    found = FALSE;
    if (nservers != NULL) {
      while (nservers[i] != NULL) {
        j = 0;
        while (ns_ours[j] != NULL) {
          if (strcasecmp(ns_ours[j], nservers[i]) == 0) {
            found = TRUE;
          }
          j++;
        }
        i++;
      }
      g_strfreev(nservers);
    }
    if (found) {
      RT_rdns_parenthasourns(info->ctx);
      ret_val = AU_AUTHORISED;
    } else {
      RT_rdns_notdelegated(info->ctx);
      ret_val = AU_UNAUTHORISED_CONT;
    }
  }

  g_strfreev(ns_ours);
  return ret_val;
}

/*
 * returns true if the object must be decomposed
 */
gboolean ns_is_decompose(LG_context_t * lg_ctx, const gchar * object_str,
                         gint * range_start, gint * range_end)
{
  gboolean ret_val = FALSE;     /* we assume we won't */
  rpsl_object_t *object;        /* rpsl object itself */
  gchar *domain;                /* key value */
  regex_t preg;                 /* compiled regular expression */
  regmatch_t pmatch[10];        /* info about matches */
  gint reg_retval;              /* return value from regs */
  gchar errstr[200];            /* error string */
  gchar temp_num[100];          /* temp holder for str to num */

  object = rpsl_object_init(object_str);
  domain = rpsl_object_get_key_value(object);
  LG_log(lg_ctx, LG_DEBUG, "checking decomposition against %s", domain);
  if ((reg_retval =
       regcomp(&preg,
               "^([0-9]+)\\-([0-9]+)\\.[0-9]+\\.[0-9]+\\.in\\-addr\\.arpa\\.?$",
               REG_EXTENDED)) != 0) {
    regerror(reg_retval, &preg, errstr, 100);
    LG_log(lg_ctx, LG_DEBUG,
           "compiling regular expression in ns_is_decompose: %s", errstr);
  } else if ((reg_retval = regexec(&preg, domain, 3, pmatch, 0)) != 0) {
    regerror(reg_retval, &preg, errstr, 100);
    LG_log(lg_ctx, LG_DEBUG,
           "executing regular expression in ns_is_decompose: %s", errstr);
  } else {
    g_snprintf(temp_num, (pmatch[1].rm_eo - pmatch[1].rm_so + 2), "%s",
               domain + pmatch[1].rm_so);
    *range_start = atoi(temp_num);
    g_snprintf(temp_num, (pmatch[2].rm_eo - pmatch[2].rm_so + 2), "%s",
               domain + pmatch[2].rm_so);
    *range_end = atoi(temp_num);
    ret_val = TRUE;
  }
  rpsl_object_delete(object);
  free(domain);
  regfree(&preg);

  return ret_val;
}

/*
 * decomposes objects and processes them one by one
 */
gchar **ns_decompose_object(LG_context_t * lg_ctx, RT_context_t * rt_ctx,
                            gchar * object_str, gint range_start,
                            gint range_end)
{
  gchar *domain;                /* key value */
  rpsl_object_t *object;        /* object */
  const rpsl_attr_t *domain_attr;       /* domain attribute pointer */
  int reg_retval;               /* return value from regs */
  regex_t preg;                 /* compiled regular expression */
  regmatch_t pmatch[10];        /* info about matches */
  gchar errstr[200];            /* error string */
  gchar domain_new[1000];       /* temp holder for domain string */
  gchar domain_suffix[1000];    /* temp holder for domain suffix */
  gchar **ret_val;              /* return value */
  gchar *new_object_str;        /* modified object */
  gint i;                       /* generic iterator */

  LG_log(lg_ctx, LG_FUNC, ">Entering ns_process_object_decomposed");
  object = rpsl_object_init(object_str);
  domain = rpsl_object_get_key_value(object);
  LG_log(lg_ctx, LG_DEBUG, "decomposing %s", domain);
  rpsl_object_delete(object);
  free(domain);

  /* compile regex */
  if ((reg_retval =
       regcomp(&preg,
               "^[0-9]+\\-[0-9]+\\.([0-9]+\\.[0-9]+\\.in\\-addr\\.arpa.*)$",
               REG_EXTENDED)) != 0) {
    regerror(reg_retval, &preg, errstr, 100);
    LG_log(lg_ctx, LG_DEBUG,
           "compiling regex in ns_process_object_decomposed: %s", errstr);
    ret_val = NULL;
  } else {

    /* get the domain suffix, the constant part */
    object = rpsl_object_init(object_str);
    domain = rpsl_object_get_key_value(object);
    if ((reg_retval = regexec(&preg, domain, 3, pmatch, 0)) != 0) {
      regerror(reg_retval, &preg, errstr, 100);
      LG_log(lg_ctx, LG_DEBUG,
             "executing regex in ns_process_object_decomposed: %s",
             errstr);
    }
    g_snprintf(domain_suffix, (pmatch[1].rm_eo - pmatch[1].rm_so + 2),
               "%s", domain + pmatch[1].rm_so);
    rpsl_object_delete(object);

    /* check if we have a sane range */
    if ((range_start > range_end) || (range_start < 0)
        || (range_start > 255) || (range_end < 0) || (range_end > 255)) {
      ret_val = NULL;
      LG_log(lg_ctx, LG_DEBUG, "range %d - %d can't be decomposed",
             range_start, range_end);
      RT_rdns_invalid_range(rt_ctx, object_str);
    } else {

      /* create ranges */
      ret_val = g_new(gchar *, range_end - range_start + 1);
      i = 0;
      while (range_start <= range_end) {
        object = rpsl_object_init(object_str);
        domain_attr = rpsl_object_get_attr_by_ofs(object, 0);
        sprintf(domain_new, "%d.%s", range_start, domain_suffix);
        rpsl_attr_replace_value((rpsl_attr_t *) domain_attr, domain_new);
        new_object_str = rpsl_object_get_text(object, 1);
        LG_log(lg_ctx, LG_DEBUG, "sending %s to up_process_object",
               domain_new);
        ret_val[i] = new_object_str;
        ret_val[i + 1] = NULL;
        range_start++;
        i++;
        rpsl_object_delete(object);
      }
    }

    free(domain);
  }
  regfree(&preg);
  LG_log(lg_ctx, LG_FUNC, "<Exiting ns_process_object_decomposed");
  return ret_val;
}
