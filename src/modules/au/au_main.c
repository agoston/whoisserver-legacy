#include "au.h"
#include "au_plugins.h"

#undef MIN
#undef MAX
#include "memwrap.h"

/* XXX: add */
/*#include "ca.h"*/
#define ca_get_auth_plugins my_ca_get_auth_plugins()
char *my_ca_get_auth_plugins() 
{
#ifdef RDNSYES
  return UT_strdup("rpsl\nripe\nirt\norganisation\nrdns");
#else
  return UT_strdup("rpsl\nripe\nirt\norganisation");
#endif
}


/* this list contains the names of the plugins to call when checking auth */
static GList *au_plugins_to_call = NULL;

/* the logging context */
LG_context_t *au_context;

/* lookup server */
LU_server_t *au_lookup;

/* flag for test mode */
int db_test_mode;

/* prototypes */
void au_manage (PG_transaction_t *trans);
void au_null_termination (PG_transaction_t *trans, gpointer my_data);


/*
The parameters are:

 obj    A parsed object from the rpsl module.
 cred   A list of creditentials from the cr module.
 type   One of AU_CREATE, AU_MODIFY, or AU_DELETE.
 ctx    A reporting context from the rt module.

The return value is one of:

 AU_AUTHORISED         Operation is authorised
 AU_UNAUTHORISED_CONT  Operation is unauthorised, continue if desired
 AU_UNAUTHORISED_END   Operation is unauthorised, do not process further
 AU_ERROR              Error occured when checking, process no more objects
*/
AU_ret_t AU_check_auth (const rpsl_object_t *obj, 
                        const GList *cred, 
                        AU_check_t type,
                        RT_context_t *ctx,
                        GList **mntner_used)
{
  au_plugin_callback_info_t info;
  PG_transaction_t *trans;
  AU_ret_t ret_val;

  LG_log(au_context, LG_FUNC, "AU_check_auth: entering");

  /* put our information into something we can pass to our plug-ins */
  info.obj = (rpsl_object_t *)obj;  /* discard const - we re-const later */
  info.cred = (GList *)cred;        /* discard const - we re-const later */
  info.type = type;
  info.ctx = ctx;
  info.ret_val = AU_AUTHORISED;
  info.mntner_used = NULL;

  /* create and execute the check */
  trans = PG_transaction_new(au_manage);
  PG_global_set(trans, &info);
  PG_execute(trans, au_plugins_to_call);
  ret_val = info.ret_val;
  *mntner_used = info.mntner_used;

  /* NULL our maintainers if an error occurred */
  /* XXX: is this okay? */
//  if (ret_val == AU_ERROR) {
//      *mntner_used = NULL;
//  }
  PG_transaction_end(trans);

  /* done */
  LG_log(au_context, LG_FUNC, "AU_check_auth: exiting with value [%s]", 
         AU_ret2str(ret_val));
  return ret_val;
}

/* we don't actually do anything here*/
void au_manage (PG_transaction_t *trans)
{
  /* do nothing */
}

/* no rollback in authorisation checks, so we don't need this */
void au_null_termination (PG_transaction_t *trans, gpointer my_data)
{
  /* do nothing */
}

/* record our server */
void AU_set_lookup(LU_server_t *lookup_server)
{
  au_lookup = lookup_server;
}

/* 
we use the string "auth_plugin:" followed by the specific plugin name 
to create a sort of name space for the authorisation plugins 
*/
gboolean AU_init (LG_context_t *ctx, int test_mode)
{
  const au_plugin_t *plugin;
  PG_plugin_t *pg;
  GHashTable *name_to_plugin;
  gchar *registered_name;
  gchar **plugins_to_call;
  int i;
  gboolean success_flag;
  int num_plugin;

  /* record our logging context */
  au_context = ctx;
  db_test_mode = test_mode;
  
  /* initialise the pg module */
  PG_init();

  LG_log(au_context, LG_FUNC, "AU_init: entering");

  /* return flag */
  success_flag = TRUE;

  /* initialise our map of names to information */
  name_to_plugin = g_hash_table_new(g_str_hash, g_str_equal);
    
  /* register our callbacks */
  plugin = au_plugins;
  num_plugin = 0;
  LG_log(au_context, LG_INFO, "AU_init: loading authorisation plugins");
  while (plugin->name != NULL) 
  {
    g_hash_table_insert(name_to_plugin, plugin->name, (gpointer)plugin);

    LG_log(au_context, LG_INFO, "AU_init:  loading %s", plugin->name);

    registered_name = g_strjoin("", "auth_plugin:", plugin->name, NULL);
    pg = PG_plugin_new(registered_name, plugin->call_func, au_null_termination);
    PG_register(pg);
    num_plugin++;

    plugin++;
  }
  LG_log(au_context, LG_INFO, "AU_init: loaded %d plugin%s", num_plugin,
         ((num_plugin == 1) ? "" : "s"));

  /* record which plugins to call */
  LG_log(au_context, LG_INFO, "AU_init: registering authorisation plugins to call");
  plugins_to_call = ut_g_strsplit_v1(ca_get_auth_plugins, "\n", 0);
  num_plugin = 0;
  for (i=0; plugins_to_call[i] != NULL; i++) 
  {
    /* XXX: skip empty entries? */
    plugin = g_hash_table_lookup(name_to_plugin, plugins_to_call[i]);
    if (plugin == NULL) {
      LG_log(au_context, LG_ERROR, "AU_init: missing authorisation plugin \"%s\"", 
             plugins_to_call[i]);
      success_flag = FALSE;
    } else if ((plugin->init_func)() != TRUE) { 
      LG_log(au_context, LG_ERROR, "AU_init: error initialising authorisation plugin \"%s\"", 
             plugins_to_call[i]);
      success_flag = FALSE;
    } else {
      registered_name = g_strjoin("", "auth_plugin:", plugins_to_call[i], NULL);
      LG_log(au_context, LG_DEBUG, "AU_init: registering %s",registered_name);
      au_plugins_to_call = 
          g_list_append(au_plugins_to_call, registered_name);
      num_plugin++;
      LG_log(au_context, LG_INFO, "AU_init:  registered %s", plugins_to_call[i]);
    }
  }
  g_strfreev(plugins_to_call);
  LG_log(au_context, LG_INFO, "AU_init: registered %d plugin%s", num_plugin,
         ((num_plugin == 1) ? "" : "s"));

  /* free our map */
  g_hash_table_destroy(name_to_plugin);

  /* return our result */
  LG_log(au_context, LG_FUNC, "AU_init: exiting with value [%s]", 
      success_flag ? "TRUE" : "FALSE");
  return success_flag;
}


/* clean up at end of auth checking 
*/

void AU_end()
{
  GList *item;
  
  for ( item = au_plugins_to_call; item != NULL; item = g_list_next(item))
  {
    if (item->data)
      free(item->data);
  }
  if (au_plugins_to_call)
    g_list_free(au_plugins_to_call);
}
