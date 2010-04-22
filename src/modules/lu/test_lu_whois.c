#include <pthread.h>
#include <stdio.h>
#include "lg.h"
#include "sk.h"
#include "memwrap.h"
#include "lu_whois.h"
#include "ut.h"

const char *person =
    "person: Shane Kerr\n"
    "address: Singel 258\n"
    "phone: +31 20 535 4444\n"
    "nic-hdl: SWK1-RIPE\n"
    "changed: shane@ripe.net\n"
    "source: RIPE\n";

const char *arpa_domain =
    "domain: 17.1.0.193.in-addr.arpa\n"
    "descr: bogus in-addr.arpa domain\n"
    "admin-c: SWK1-RIPE\n"
    "tech-c: SWK1-RIPE\n"
    "zone-c: SWK1-RIPE\n"
    "changed: shane@ripe.net\n"
    "source: RIPE\n";

const char *referral_domain =
    "domain: something.somewhere.de\n"
    "descr: bogus de domain\n"
    "admin-c: SWK1-RIPE\n"
    "tech-c: SWK1-RIPE\n"
    "zone-c: SWK1-RIPE\n"
    "changed: shane@ripe.net\n"
    "source: RIPE\n";

const char *route =
    "route: 193.0.0.0/32\n"
    "descr: blah\n"
    "origin: AS3333\n"
    "mnt-by: FOO-MNT\n"
    "changed: shane@ripe.net\n"
    "source: RIPE\n";

const char *parentless_route =
    "route: 255.254.253.252/32\n"
    "descr: blah\n"
    "origin: AS1\n"
    "mnt-by: FOO-MNT\n"
    "changed: shane@ripe.net\n"
    "source: RIPE\n";

const char *inetnum = 
     "inetnum:      192.255.255.255 - 192.255.255.255\n"
     "netname:      RIPE-NCC\n"
     "descr:        RIPE Network Coordination Centre\n"
     "descr:        Amsterdam, Netherlands\n"
     "remarks:      Used for RIPE NCC infrastructure.\n"
     "country:      NL\n"
     "admin-c:      AMR68-RIPE\n"
     "admin-c:      RDK-RIPE\n"
     "tech-c:       OPS4-RIPE\n"
     "status:       ASSIGNED PI\n"
     "mnt-by:       RIPE-NCC-MNT\n"
     "mnt-lower:    RIPE-NCC-MNT\n"
     "changed:      mally@ripe.net 20030522\n"
     "source:       RIPE\n";

const char *inverse_person_role =
    "role:         American Registry for Internet Numbers\n"
    "address:      ARIN, see http://www.arin.net\n"
    "admin-c:      ARIN1-RIPE\n"
    "tech-c:       ARIN1-RIPE\n"
    "nic-hdl:      ARIN1-RIPE\n"
    "remarks:      For more information on ARIN assigned blocks, connect\n"
    "remarks:      to ARIN's whois database, whois.arin.net.\n"
    "mnt-by:       RIPE-NCC-MNT\n"
    "changed:      ripe-dbm@ripe.net   20010508\n"
    "source:       RIPE\n";


int
main ()
{
  LU_server_t *whois;
  rpsl_object_t *obj;
  rpsl_object_t *dbase_obj;
  GList *parents;
  GList *overlap;
  GList *p;
  gchar *text;
  LG_appender_t *app;
  LG_context_t *ctx;
  LG_context_t *null_ctx;
  GList *types = NULL;
  GList *objects;
  GList *inverse_keys = NULL;

  /* set up for logging */
  app = LG_app_get_file_info_dump(stderr);
  LG_app_set_level(app, LG_ALL);
  LG_app_set_formatter(app, LG_frm_dbupdate_prepared());
  ctx = LG_ctx_new();
  LG_ctx_add_appender(ctx, app);
  LU_init(ctx);

  /* NULL logging for these, since we don't care */
  null_ctx = LG_ctx_new();
  SK_init(null_ctx);
  UT_init(null_ctx);

  /* test response with server error */
  LG_log(ctx, LG_INFO, "test response with server error");
  whois = LU_whois_init("bogusserver.ripe.net", 70000, 1);
  if (LU_lookup(whois, &obj, "person", "SWK1-RIPE", "RIPE") != LU_ERROR)
  {
    fprintf(stderr, "%s:%d No error with bogus server!\n", __FILE__, __LINE__);
    return 1;
  }
  /* test cleanup */
  LU_cleanup(whois);

  /* test query for existing record */
  LG_log(ctx, LG_INFO, "test query for existing record");
  whois = LU_whois_init("joshua.ripe.net", 43, 30);
  if (LU_lookup(whois, &obj, "person", "swk1-ripe", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up person\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(obj, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  rpsl_object_delete(obj);

  /* test query for nonexistant record (i.e. bad nic-hdl) */
  LG_log(ctx, LG_INFO, "test query for nonexistant record (i.e. bad nic-hdl)");
  if (LU_lookup(whois, &obj, "person", "shane kerr", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up person\n", __FILE__, __LINE__);
    return 1;
  }
  if (obj != NULL) 
  {
    fprintf(stderr, "%s:%d Found bogus person\n", __FILE__, __LINE__);
    return 1;
  }
  LU_cleanup(whois);

  /* test query with infinite timeout */
  LG_log(ctx, LG_INFO, "test query with infinite timeout");
  whois = LU_whois_init("joshua.ripe.net", 43, 0);
  if (LU_lookup(whois, &obj, "person", "SWK1-RIPE", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up person\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(obj, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  rpsl_object_delete(obj);
  LU_cleanup(whois);

  /* test query with real timeout */
  LG_log(ctx, LG_INFO, "test query with real timeout");
  whois = LU_whois_init("joshua.ripe.net", 43, 1);
  if (LU_lookup(whois, &obj, "person", "a", "RIPE") != LU_ERROR)
  {
    fprintf(stderr, "%s:%d No timeout with lookup\n", __FILE__, __LINE__);
    return 1;
  }
  LU_cleanup(whois);

  /* test query that returns an error */
  LG_log(ctx, LG_INFO, "test query that returns an error");
  whois = LU_whois_init("joshua.ripe.net", 43, 600);
  if (LU_lookup(whois, &obj, "person", "^", "RIPE") != LU_ERROR)
  {
    fprintf(stderr, "%s:%d No error with lookup\n", __FILE__, __LINE__);
    return 1;
  }

  /* test query that has lots of objects to search through */
/*
  LG_log(ctx, LG_INFO, "test query that has lots of objects to search through");
  if (LU_lookup(whois, &obj, "person", "mike", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up person\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(obj, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  rpsl_object_delete(obj);
*/

  /* check for overlaps */
  LG_log(ctx, LG_INFO, "checking for overlapping inetnums");
  obj = rpsl_object_init(inetnum);
  if (LU_check_overlap(whois, &overlap, obj, "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error overlap check\n", __FILE__, __LINE__);
    return 1;
  }
  if (overlap != NULL) {
    fprintf (stderr, "Overlaps found:\n");
  }
  for (p=overlap; p!= NULL; p=g_list_next(p))
   {
     text = rpsl_object_get_text(p->data, RPSL_STD_COLUMN);
     puts(text);
     UT_free(text);
     rpsl_object_delete(p->data);
   }
  g_list_free(overlap);

  exit(0);
  /* test route query */
  LG_log(ctx, LG_INFO, "test route query");
  if (LU_lookup(whois, &obj, "route", "193.0.0.0/21AS3333", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up route\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(obj, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  rpsl_object_delete(obj);

  /* route query for route with multiple origins */
/*
  LG_log(ctx, LG_INFO, "test route query for route with multiple origins");
  if (LU_lookup(whois, &obj, "route", "193.5.174.0/24AS1836", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up route\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(obj, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
*/

  /* inverse lookup the objects */
  LG_log(ctx, LG_INFO, "inverse lookup the objects");
  obj = rpsl_object_init(inverse_person_role);
  inverse_keys = g_list_append(inverse_keys, "admin-c");
  inverse_keys = g_list_append(inverse_keys, "tech-c");
  types =  g_list_append(types, "role");
  types =  g_list_append(types, "inetnum");
  if (LU_inverse_lookup(whois, &objects, types, rpsl_object_get_key_value(obj), inverse_keys, "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error inverse lookup \n", __FILE__, __LINE__);
    return 1;
  }
  if (objects != NULL) {
  for (p=objects; p!= NULL; p=g_list_next(p))
   {
     text = rpsl_object_get_text(p->data, RPSL_STD_COLUMN);
     puts(text);
     UT_free(text);
     rpsl_object_delete(p->data);
   }
  g_list_free(objects);
  }
  if (types != NULL) g_list_free(types);
  g_list_free(inverse_keys);

  /* lookup the object, specify source */
  LG_log(ctx, LG_INFO, "lookup the object, specify source");
  obj = rpsl_object_init(person);
  if (LU_get_object(whois, &dbase_obj, obj, "RIPE") != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error looking up object\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(dbase_obj, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  rpsl_object_delete(dbase_obj);

  /* lookup the object, get source from object */
  LG_log(ctx, LG_INFO, "lookup the object, get source from object");
  if (LU_get_object(whois, &dbase_obj, obj, NULL) != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error looking up object\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(dbase_obj, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  rpsl_object_delete(obj);
  obj = dbase_obj;

  /* lookup "parent" of non-hierarchical object, specify source */
  LG_log(ctx, LG_INFO, "lookup \"parent\" of non-hierarchical object, specify source");
  if (LU_get_parents(whois, &parents, obj, "RIPE") != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error looking up object\n", __FILE__, __LINE__);
    return 1;
  }
  if (parents != NULL)
  {
    fprintf(stderr, "%s:%d Found parents in object\n", __FILE__, __LINE__);
    return 1;
  }

  /* lookup "parent" of non-hierarchical object, source in object */
  LG_log(ctx, LG_INFO, "lookup \"parent\" of non-hierarchical object, source in object");
  if (LU_get_parents(whois, &parents, obj, NULL) != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error looking up object\n", __FILE__, __LINE__);
    return 1;
  }
  if (parents != NULL)
  {
    fprintf(stderr, "%s:%d Found parents in object\n", __FILE__, __LINE__);
    return 1;
  }
  rpsl_object_delete(obj);

  /* lookup parent of as-set, aut-num */
  LG_log(ctx, LG_INFO, "lookup lookup parent of as-set, aut-num");
  if (LU_lookup(whois, &obj, "as-set", "AS3320:AS-DTAG", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up as-set\n", __FILE__, __LINE__);
    return 1;
  }
  if (obj == NULL) 
  {
    fprintf(stderr, "%s:%d no as-set AS3320:AS-DTAG\n", __FILE__, __LINE__);
    return 1;
  }
  if (LU_get_parents(whois, &parents, obj, NULL) != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error getting as-set parents\n", __FILE__, __LINE__);
    return 1;
  }
  if (parents == NULL)
  {
    fprintf(stderr, "%s:%d No parents found\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(parents->data, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  LU_free_parents(parents);
  rpsl_object_delete(obj);

  /* lookup parent of as-set, as-set (plus specify source) */
  LG_log(ctx, LG_INFO, "lookup lookup parent of as-set, as-set (plus specify source)");
  if (LU_lookup(whois, &obj, "as-set", "AS8308:AS-Customers:AS12968", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up as-set\n", __FILE__, __LINE__);
    return 1;
  }
  if (obj == NULL) 
  {
    fprintf(stderr, "%s:%d no as-set AS8308:AS-Customers:AS12968\n", __FILE__, __LINE__);
    return 1;
  }
  if (LU_get_parents(whois, &parents, obj, "RIPE") != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error getting as-set parents\n", __FILE__, __LINE__);
    return 1;
  }
  if (parents == NULL)
  {
    fprintf(stderr, "%s:%d No parents found\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(parents->data, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  LU_free_parents(parents);
  rpsl_object_delete(obj);

  /* lookup parent of aut-num */
  LG_log(ctx, LG_INFO, "lookup lookup parent of aut-num");
  if (LU_lookup(whois, &obj, "aut-num", "AS3333", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up aut-num\n", __FILE__, __LINE__);
    return 1;
  }
  if (obj == NULL) 
  {
    fprintf(stderr, "%s:%d no aut-num AS3333\n", __FILE__, __LINE__);
    return 1;
  }
  if (LU_get_parents(whois, &parents, obj, NULL) != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error getting aut-num parents\n", __FILE__, __LINE__);
    return 1;
  }
  if (parents == NULL)
  {
    fprintf(stderr, "%s:%d No parents found\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(parents->data, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  LU_free_parents(parents);
  rpsl_object_delete(obj);

  /* lookup parent of as-block */
  /* XXX: none */
  LG_log(ctx, LG_INFO, "lookup lookup parent of as-block");
  if (LU_lookup(whois, &obj, "as-block", "AS3154 - AS3353", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up as-block\n", __FILE__, __LINE__);
    return 1;
  }
  if (obj == NULL) 
  {
    fprintf(stderr, "%s:%d no as-block for AS3154 - AS3353\n", __FILE__, __LINE__);
    return 1;
  }
  if (LU_get_parents(whois, &parents, obj, NULL) != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error getting aut-num parents\n", __FILE__, __LINE__);
    return 1;
  }
  /* XXX: need to test hierarchy */
  /********
  if (parents == NULL)
  {
    fprintf(stderr, "%s:%d No parents found\n", __FILE__, __LINE__);
    return 1;
  }
  puts(rpsl_object_get_text(parents->data, RPSL_STD_COLUMN));
  *********/
  LU_free_parents(parents);
  rpsl_object_delete(obj);

  /* lookup parent of inetnum */
  LG_log(ctx, LG_INFO, "lookup lookup parent of inetnum");
  if (LU_lookup(whois, &obj, "inetnum", "193.0.0.0 - 193.0.1.255", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up inetnum\n", __FILE__, __LINE__);
    return 1;
  }
  if (obj == NULL) 
  {
    fprintf(stderr, "%s:%d no inetnum 193.0.0.0 - 193.0.1.255\n", __FILE__, __LINE__);
    return 1;
  }
  if (LU_get_parents(whois, &parents, obj, NULL) != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error getting inetnum parents\n", __FILE__, __LINE__);
    return 1;
  }
  if (parents == NULL)
  {
    fprintf(stderr, "%s:%d No parents found\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(parents->data, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  LU_free_parents(parents);
  rpsl_object_delete(obj);

  /* lookup parent of inet6num */
  LG_log(ctx, LG_INFO, "lookup lookup parent of inet6num");
  if (LU_lookup(whois, &obj, "inet6num", "2001:0610:0240::/42", "RIPE") != LU_OKAY)
  {
    fprintf(stderr, "%s:%d Error looking up inet6num\n", __FILE__, __LINE__);
    return 1;
  }
  if (obj == NULL) 
  {
    fprintf(stderr, "%s:%d no inet6num 2001:0610:0240::/42\n", __FILE__, __LINE__);
    return 1;
  }
  if (LU_get_parents(whois, &parents, obj, NULL) != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error getting inet6num parents\n", __FILE__, __LINE__);
    return 1;
  }
  if (parents == NULL)
  {
    fprintf(stderr, "%s:%d No parents found\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(parents->data, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  LU_free_parents(parents);
  rpsl_object_delete(obj);

  /* lookup parent of domain */
  LG_log(ctx, LG_INFO, "lookup lookup parent of domain");
  obj = rpsl_object_init(arpa_domain);
  if (LU_get_parents(whois, &parents, obj, NULL) != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error getting domain parents\n", __FILE__, __LINE__);
    return 1;
  }
  if (parents == NULL)
  {
    fprintf(stderr, "%s:%d No parents found\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(parents->data, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  LU_free_parents(parents);
  rpsl_object_delete(obj);

  /* lookup parent of ccTLD domain */
  LG_log(ctx, LG_INFO, "lookup lookup parent of ccTLD");
  obj = rpsl_object_init(referral_domain);
  if (LU_get_parents(whois, &parents, obj, NULL) != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error getting domain parents\n", __FILE__, __LINE__);
    return 1;
  }
  if (parents == NULL)
  {
    fprintf(stderr, "%s:%d No parents found\n", __FILE__, __LINE__);
    return 1;
  }
  text = rpsl_object_get_text(parents->data, RPSL_STD_COLUMN);
  puts(text);
  UT_free(text);
  LU_free_parents(parents);
  rpsl_object_delete(obj);

  /* lookup parent of route */
  LG_log(ctx, LG_INFO, "lookup lookup parents of route");
  obj = rpsl_object_init(route);
  if (LU_get_parents(whois, &parents, obj, NULL) != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error getting route parents\n", __FILE__, __LINE__);
    return 1;
  }
  if (parents == NULL)
  {
    fprintf(stderr, "%s:%d No parents found\n", __FILE__, __LINE__);
    return 1;
  }
  for (p = parents; p != NULL; p = g_list_next(p))
  {
    text = rpsl_object_get_text(p->data, RPSL_STD_COLUMN);
    puts(text);
    UT_free(text);
  }
  LU_free_parents(parents);
  rpsl_object_delete(obj);

  /* lookup parent of route */
  obj = rpsl_object_init(parentless_route);
  LG_log(ctx, LG_INFO, "lookup lookup parents of parentless route");
  if (LU_get_parents(whois, &parents, obj, NULL) != LU_OKAY) 
  {
    fprintf(stderr, "%s:%d Error getting route parents\n", __FILE__, __LINE__);
    return 1;
  }
  if (parents == NULL)
  {
    fprintf(stderr, "%s:%d No parents found\n", __FILE__, __LINE__);
    return 1;
  }
  for (p = parents; p != NULL; p = g_list_next(p))
  {
    text = rpsl_object_get_text(p->data, RPSL_STD_COLUMN);
    puts(text);
    UT_free(text);
  }
  LU_free_parents(parents);
  rpsl_object_delete(obj);

  LU_cleanup(whois);
  LG_ctx_free(null_ctx);
  LG_ctx_free(ctx);
  return 0;
}

