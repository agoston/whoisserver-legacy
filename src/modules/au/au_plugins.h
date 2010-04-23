#ifndef AU_PLUGINS_H
#define AU_PLUGINS_H

#include <glib.h>
#include <string.h>

#include "pg.h"
#include "au.h"

/* this is the data structure that gets passed to plugins */
typedef struct {
  rpsl_object_t *obj;
  GList *cred;
  AU_check_t type;
  RT_context_t *ctx;
  AU_ret_t ret_val;
  GList *mntner_used;
} au_plugin_callback_info_t;

/* this is the actual plugin definition */
typedef struct {
  gchar *name;
  gboolean (*init_func)();
  PG_call_function_t call_func;
} au_plugin_t;

/* array of plugins (in au_plugins.c) */
extern const au_plugin_t au_plugins[];


/* used in the by-type support function */
typedef struct {
  char *object_type;
  AU_ret_t (*check_create)();
  AU_ret_t (*check_delete)();
  AU_ret_t (*check_modify)();
} au_check_by_type_t;

/* support function */
AU_ret_t AU_check_by_type(const au_check_by_type_t *type_table,
                          const au_plugin_callback_info_t *info);

/* authorisation logging context, defined in au_main.c */
extern LG_context_t *au_context;

/* lookup server information, defined in au_main.c */
extern LU_server_t *au_lookup;

/* flag to determine if dbupdate is running in test mode */
extern int db_test_mode;


#endif /* AU_PLUGINS_H */

