/***************************************
  $Revision: 1.1 $

  Plug-in module.

  Status: REVIEWED, TESTED

 Author(s):       Tiago Antao

  ******************/ /******************
  Modification History:
        tiago (10/04/2003) Created.
  ******************/ /******************
  Copyright (c) 2003               RIPE NCC
 
  All Rights Reserved
  
  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation, and that the name of the author not be
  used in advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.
  
  THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
  AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
  DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ***************************************/

#include "pg.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

static GHashTable* plugins;
pthread_mutex_t dijkstra; /*In honour to the E. W. Dijkstra
                            (creator of the semaphore concept) */
typedef struct {
  gchar* name; //allow pass by name on configuration files
  PG_call_function_t call_function;
  PG_termination_function_t termination_function;
} pg_plugin_t;
//term_function can't alter anything

typedef struct {
  gint executed_plugins;
  GList* status;
  GList* added_state;
  PG_status_t current_status;
  gpointer local_info;
  gpointer global;
  PG_manage_function_t manage_function;
} pg_transaction_t;
// added_state - plugin/term_function communiction
// local_info - intra plugin communication
// external_info - external communication

static int state = 0;
/*+
  PG_init - Initializes the PG module.
 +*/
void PG_init() {
  if (state == 0 ) {
    plugins = g_hash_table_new(g_str_hash, g_str_equal);
    pthread_mutex_init(&dijkstra, NULL);
    state = 1;
  }
}

void pg_list_plugin_clear(gpointer key, gpointer data, gpointer user_data) {
  pg_plugin_t* plug;

  plug = (pg_plugin_t*) data;
  free(plug->name);
  free(plug);
}

/*+
  PG_end - Ends the PG module.
 +*/
void PG_end() {
  if (state == 1 ) {
    g_hash_table_foreach(plugins, pg_list_plugin_clear, NULL);
    g_hash_table_destroy(plugins);
    pthread_mutex_destroy(&dijkstra);
    state = 0;
  }
}

/*+
  +*/
PG_transaction_t* PG_transaction_new(PG_manage_function_t func) {
  pg_transaction_t* trans;

  trans = malloc(sizeof(pg_transaction_t));
  trans->executed_plugins = 0;
  trans->status = NULL;
  trans->added_state = NULL;
  trans->current_status = PG_INIT;
  trans->local_info = NULL;
  trans->global = NULL;
  trans->manage_function = func;
  if (func) {
    func(trans);
  }

  return (PG_transaction_t*)trans;
}

void PG_transaction_end(PG_transaction_t* trans) {
  pg_transaction_t* trs;

  trs = (pg_transaction_t*) trans;
  if (trs->manage_function) {
    trs->manage_function(trans);
  }

  //added_state content is each plugin responsability
  g_list_free(trs->added_state);
  g_list_free(trs->status);
  free(trs);
}

PG_plugin_t* PG_plugin_new(
  gchar* name,
  PG_call_function_t call_function,
  PG_termination_function_t termination_function ) {
  pg_plugin_t* plugin;

  plugin = malloc(sizeof(pg_plugin_t));
  plugin->name = strdup(name);
  plugin->call_function = call_function;
  plugin->termination_function = termination_function;

  return plugin;
}

void PG_register(PG_plugin_t* plugin) {
  pg_plugin_t* pg;

  pthread_mutex_lock (&dijkstra);
  pg = (PG_plugin_t*)plugin;
  if (g_hash_table_lookup(plugins, pg->name)) {
    return;
  }
  g_hash_table_insert(plugins, pg->name, pg);
  /* this is somewhat strange but note that the key never has
  to be freed (just free the value in our case) */
  pthread_mutex_unlock (&dijkstra);
}

void PG_global_set(PG_transaction_t* trans, gpointer data) {
  ((pg_transaction_t*)trans)->global = data;
}

gpointer PG_global_get(PG_transaction_t* trans) {
  return ((pg_transaction_t*)trans)->global;
}


gpointer PG_local_info_get(PG_transaction_t* trans) {
  return ((pg_transaction_t*)trans)->local_info;
}

void PG_local_info_set(PG_transaction_t* trans, gpointer data) {
  ((pg_transaction_t*)trans)->local_info = data;
}

PG_status_t PG_status_get(PG_transaction_t* trans) {
  return ((pg_transaction_t*)trans)->current_status;
}

/*+
  PG_execute - Extecute a list of plug-ins in a transaction.

  This is the juicy function.

  PG_transaction_t* trans - Transaction

  GList* plugs - List of plug-ins.
  +*/
void PG_execute(PG_transaction_t* trans, GList* plugs) {
  pg_transaction_t* trs;
  pg_plugin_t* plugin;
  GList* executed_plugs;
  GList* executed_plugs_holder;
  gpointer plugin_data;
  PG_status_t status;
  GList* added_state;

  executed_plugs = NULL; //Note, only list structure should be freed
  trs = (pg_transaction_t*) trans;
  while (plugs) {
    plugin = g_hash_table_lookup(plugins, plugs->data);
    if (plugin) {
      status = plugin->call_function(trans, &plugin_data);
      trs->executed_plugins += 1;
      executed_plugs = g_list_append(executed_plugs, plugin);
      trs->added_state = g_list_append(trs->added_state,
        plugin_data);
      //status (the list) is not in use for now...
      if (status == PG_OK || status == PG_ERROR_PROCEED) {
        plugs = plugs->next;
      }
      else {
        plugs = NULL;
      }
      if (trs->current_status != PG_ERROR_PROCEED) {
        trs->current_status = status;
      }
    }
    else {
      trs->current_status = PG_ERROR_PLUGIN_NAME;
      plugs = NULL;
    }
  }

  added_state = trs->added_state;
  executed_plugs_holder = executed_plugs;
  while (executed_plugs) {
    plugin = executed_plugs->data;
    plugin->termination_function(trans, added_state->data);
    executed_plugs = executed_plugs->next;
    added_state = added_state->next;
  }
  g_list_free(executed_plugs_holder);
}

static void pg_get_names(gpointer name, gpointer value, gpointer plist) {
  GList** list;

  list = (GList**)plist;
  *list = g_list_append(*list, g_strdup((gchar*)name));
}

/*+
  PG_get_plugins - Returns the list of registered plug-ins.

  The list has to be freed.
  +*/
GList* PG_get_plugins() {
  GList* list;

  list = NULL;

  g_hash_table_foreach(plugins, pg_get_names, &list);

  return list;
}

/*+
  PG_get_plugin - Gets information about a plug-in.

  gchar* name - plug-in name.
  +*/
PG_plugin_t* PG_get_plugin(gchar* name) {
  return g_hash_table_lookup(plugins, name);
}
