/*
 * $Id: ns_rir.c,v 1.2.8.2 2006/08/02 12:49:02 katie Exp $
 */

#include "ns_rir.h"
#include "ns_util.h"
#include "ca_configFns.h"
#include "ca_defs.h"
#include "ca_dictionary.h"
#include "ca_macros.h"
#include <string.h>

/*
 * Read the delegations text file and construct a binary tree out of it
 */
static GTree *rdns_read_delegations_tree(LG_context_t * lg_ctx,
                                         char *file_name, int field_no)
{
  FILE *f;                      /* file descriptor that we read from */
  gchar line[200];              /* line buffer */
  gchar **items;                /* split line buffer */
  gchar *itemsint;              /* copy of the domain ending with ip6.int */
  GTree *result;                /* binary tree as the result */
  gchar *p;                     /* temporary placeholder */

  /* try to open the file */
  f = fopen(file_name, "r");
  if (f == NULL) {
    LG_log(lg_ctx, LG_FATAL, "rdns_read_delegations: can't open %s",
           file_name);
    return (NULL);
  }
  /* create binary tree */
  result = g_tree_new((GCompareFunc) g_strcasecmp);
  /* populate tree by reading the file */
  while (fgets(line, 160, f)) { /* a line could hardly be longer */
    if (strchr(line, '|') != NULL) {
      items = g_strsplit(g_strchomp(line), "|", 5);
      g_strdown(items[1]);
      p = strstr(items[1], "ip6.arpa");
      /* following case requires 2 insertions,
       * 1 for ip6.arpa and 1 for ip6.int */
      if ((p != NULL) && (g_strcasecmp(p, "ip6.arpa") == 0)) {
        itemsint = g_strdup(items[1]);
        p = strstr(itemsint, "arpa");   /* this can't be NULL */
        sprintf(p, "int");
        g_tree_insert(result, itemsint, g_strdup(items[field_no]));
      }
      g_tree_insert(result, g_strdup(items[1]), g_strdup(items[field_no]));
      g_strfreev(items);
    }
  }
  /* close file and return the binary tree as the result */
  fclose(f);
  return (result);
}

/*
 * Read the delegations tree for RIR information
 */
static GTree *rdns_read_delegations(LG_context_t * lg_ctx, char *file_name)
{
  return (rdns_read_delegations_tree(lg_ctx, file_name, 0));
}

/*
 * Read the delegations tree for ERX information
 */
static GTree *rdns_read_erx_delegations(LG_context_t * lg_ctx,
                                        char *file_name)
{
  return (rdns_read_delegations_tree(lg_ctx, file_name, 2));
}

static GTree *rdns_read_ds_delegations(LG_context_t * lg_ctx,
                                        char *file_name)
{
  return (rdns_read_delegations_tree(lg_ctx, file_name, 3));
}

/*
 * Internal function to traverse and free key/value pairs from the tree
 */
static GTraverseFunc int_rnds_done_delegations(gpointer key,
                                               gpointer value,
                                               gpointer data)
{
  g_free(key);
  g_free(value);
  return EXIT_SUCCESS;
}

/*
 * Cleans up the delegations binary tree
 */
void rdns_done_delegations(GTree * delegations)
{
  g_tree_traverse(delegations, *(GTraverseFunc) int_rnds_done_delegations,
                  G_IN_ORDER, delegations);
  g_tree_destroy(delegations);
}

/*
 * returns the registry to delegate the domain object
 */
gchar *rdns_who_delegates(LG_context_t * lg_ctx, GTree * delegations,
                          gchar * domain)
{
  gchar **domainv;              /* split domain by .'s to this string array */
  gchar *p;                     /* dummy pointer for disposable stuff */
  gchar **pp;                   /* dummy pointer for disposable stuff */
  gchar *domaintosearch;        /* top level domain to search in the tree */
  gchar *rir;                   /* result */
  gint digitcount;              /* how many pieces there are in the domain */
  gint currentsize;             /* iterator for pieces in the domain */

  /* verify input */
  g_strdown(domain);
  p = strstr(domain, "arpa");   /* check if it contains arpa */
  if (p != NULL) {
    if (g_strcasecmp(p, "arpa") != 0) { /* check if arpa is
                                           at the end of string */
      p = NULL;
    }
  }
  if (p == NULL) {
    p = strstr(domain, "int");  /* check if it contains int */
    if (p != NULL) {
      if (g_strcasecmp(p, "int") != 0) {        /* check if int is
                                                   at the end of string */
        p = NULL;
      }
    }
  }
  if (p == NULL) {
    LG_log(lg_ctx, LG_FATAL,
           "rdns_whos_delegates: %s not a valid domain object", domain);
    return "not found";
  }

  /* find how many digits domain has */
  domainv = g_strsplit(domain, ".", 100);       /* the last item is either
                                                   int or arpa */
  digitcount = -1;
  while ((g_strcasecmp(domainv[digitcount + 1], "int") != 0)
         && (g_strcasecmp(domainv[digitcount + 1], "arpa") != 0)) {
    digitcount++;
  }                             /* count number of digits */
  domainv[digitcount + 2] = NULL;       /* needed for g_strjoinv */

  /* check for match, top to down */
  currentsize = digitcount;
  rir = NULL;
  while ((currentsize > 0) && (rir == NULL)) {
    pp = (&domainv[currentsize - 1]);
    domaintosearch = g_strjoinv(".", pp);
    rir = g_tree_lookup(delegations, domaintosearch);
    LG_log(lg_ctx, LG_DEBUG, "looking for %s", domaintosearch);
    g_free(domaintosearch);
    currentsize--;
  }
  g_strfreev(domainv);

  /* return result */
  if (rir == NULL) {
    return "not found";         /* default return value */
  } else {
    return rir;
  }
}

/*
 * Check if we're the related party - fail if not
 */
AU_ret_t ns_find_rir(au_plugin_callback_info_t * info, gchar * domain)
{
  GTree *delegations_tree;      /* delegations tree */
  gchar *majorityRIR;           /* which rir is related */
  gchar *ns_rir;                /* which rir we're working for */

  if (ns_is_e164_arpa(info)) {
    return AU_AUTHORISED;
  }

  ns_rir = g_strdup(ca_get_ns_rir);
  g_strchomp(ns_rir);           /* do we need this for every conf var? */
  LG_log(au_context, LG_DEBUG, "reading delegations file");
  delegations_tree =
      rdns_read_delegations(au_context, ca_get_ns_delegationsfile);
  if (delegations_tree == NULL) {       /* this is internal software error */
    return AU_UNAUTHORISED_END;
  }
  majorityRIR =
      g_strdup(rdns_who_delegates(au_context, delegations_tree, domain));
  rdns_done_delegations(delegations_tree);
  LG_log(au_context, LG_DEBUG, "majority rir: %s", majorityRIR);
  if (strcmp(majorityRIR, ns_rir) == 0) {
    LG_log(au_context, LG_DEBUG, "this is our delegation");
  } else {
    LG_log(au_context, LG_DEBUG, "this is a delegation for rir: %s",
           majorityRIR);
    if (strcmp(majorityRIR, "not found") != 0) {
      RT_rdns_useotherrir(info->ctx, majorityRIR);
    } else {
      RT_rdns_cannotregister(info->ctx);
    }
    return AU_UNAUTHORISED_CONT;
  }
  g_free(majorityRIR);
  g_free(ns_rir);
  return AU_AUTHORISED;
}

/*
 * Check if the domain is an erx range, return false if not
 */
gboolean ns_is_erx(gchar * domain)
{
  gboolean ret_val;             /* return value */
  GTree *erx_tree;              /* erx tree */
  gchar *erx_val;               /* erx value in the tree */

  LG_log(au_context, LG_DEBUG, "reading delegations file for erx");
  erx_tree =
      rdns_read_erx_delegations(au_context, ca_get_ns_delegationsfile);
  if (erx_tree == NULL) {
    LG_log(au_context, LG_DEBUG, "can't populate erx delegations tree");
    ret_val = FALSE;            /* failsafe, we assume it's not erx */
  } else {
    erx_val =
        erx_val =
        g_strdup(rdns_who_delegates(au_context, erx_tree, domain));
    LG_log(au_context, LG_DEBUG, "erx value: %s", erx_val);
    if (erx_val[0] != '1') {
      LG_log(au_context, LG_DEBUG, "not ERX range");
      ret_val = FALSE;
    } else {
      LG_log(au_context, LG_DEBUG, "ERX range");
      ret_val = TRUE;
    }
    g_free(erx_val);
  }
  rdns_done_delegations(erx_tree);
  return ret_val;
}
gboolean ns_ds_accepted(gchar * domain)
{
  gboolean ret_val;             /* return value */
  GTree *ds_tree;              /* ds tree */
  gchar *ds_val;               /* ds value in the tree */

  /* e164.arpa -> DS not accepted */
  /* this will have to be changed when e164.arpa is signed. */
  if (ns_has_e164_arpa_suffix(domain) == TRUE) {
    LG_log(au_context, LG_DEBUG, "NOT reading delegations file: domain is e164.arpa related");
    LG_log(au_context, LG_DEBUG, "DS record not allowed.");
    return FALSE;
  }

  LG_log(au_context, LG_DEBUG, "reading delegations file for ds");
  ds_tree =
      rdns_read_ds_delegations(au_context, ca_get_ns_delegationsfile);
  if (ds_tree == NULL) {
    LG_log(au_context, LG_DEBUG, "can't populate ds delegations tree");
    ret_val = FALSE;            /* failsafe, we assume it's not ds */
  } else {
    ds_val = g_strdup(rdns_who_delegates(au_context, ds_tree, domain));
    LG_log(au_context, LG_DEBUG, "ds value: %s", ds_val);
    if (ds_val[0] != '1') {
      LG_log(au_context, LG_DEBUG, "DS is not accepted");
      ret_val = FALSE;
    } else {
      LG_log(au_context, LG_DEBUG, "DS is accepted");
      ret_val = TRUE;
    }
    g_free(ds_val);
  }
  rdns_done_delegations(ds_tree);
  return ret_val;
}
