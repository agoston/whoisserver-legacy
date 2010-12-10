#include "lu.h"
#include "sk.h"

/* logging */
LG_context_t *lu_context;

/*
  Initialise the lookup module

  ctx      - logging context
 */
void
LU_init (LG_context_t *ctx)
{
  lu_context = ctx;
  SK_init(ctx);
}

/* 
  Lookup a list of objects

  server   - connection information
  objects  - used to return a list of objects, if any
  type     - list of classes of objects to be returned, e.g. "person", "inetnum"
             if list is empty, no type restriction is made
  key      - lookup key, e.g. "NC123-RIPE" or "193.0.1.0/24AS3333"
  inverse_keys - list of inverse keys to be looked up, e.g "fingerpr", "tech-c"
               if list is empty, error is returned
  source   - source, e.g. "RIPE"

  return       - LU_OKAY on success, LU_ERROR on error

 */
LU_ret_t
LU_inverse_lookup (LU_server_t *server, GList **objects, GList *types,
           const gchar *key, GList *inverse_keys, const gchar *source)
{
  LU_ret_t ret_val;

  LG_log(lu_context, LG_FUNC, ">LU_inverse_lookup: entered");
  ret_val = server->inverse_lookup(server, objects, types, key, inverse_keys, source);
  LG_log(lu_context, LG_FUNC, "<LU_inverse_lookup: exiting with value [%s]",
         LU_ret2str(ret_val));
  return ret_val;
}


/* 
  Lookup a single object

  server   - connection information
  obj      - used to return object, if any
  type     - class of object, e.g. "person", "inetnum"
  key      - lookup key, e.g. "NC123-RIPE" or "193.0.1.0/24AS3333"
  source   - source, e.g. "RIPE"

  return       - LU_OKAY on success, LU_ERROR on error

  This is merely a dispatch to the server's implementation.
 */
LU_ret_t 
LU_lookup (LU_server_t *server, rpsl_object_t **obj, const gchar *type,
           const gchar *key, const gchar *source)
{
  LU_ret_t ret_val;

  LG_log(lu_context, LG_FUNC, ">LU_lookup: entered");
  ret_val = server->lookup(server, obj, type, key, source);
  LG_log(lu_context, LG_FUNC, "<LU_lookup: exiting with value [%s]", 
         LU_ret2str(ret_val));
  return ret_val;
}

/* 
  Lookup a current version of an object 

  server     - connection information
  dbase_obj  - used to return object, if any
  obj        - object to look up
  source     - source, e.g. "RIPE", or NULL to get from obj

  return       - LU_OKAY on success, LU_ERROR on error

  This is merely a dispatch to the server's implementation.
 */
LU_ret_t 
LU_get_object (LU_server_t *server, rpsl_object_t **dbase_obj, 
               const rpsl_object_t *obj, const gchar *source)
{
  LU_ret_t ret_val;

  LG_log(lu_context, LG_FUNC, ">LU_get_object: entered");
  ret_val = server->get_object(server, dbase_obj, obj, source);
  LG_log(lu_context, LG_FUNC, "<LU_get_object: exiting with value [%s]", 
         LU_ret2str(ret_val));
  return ret_val;
}

/* 
  Lookup anything

 * server    - connection information
 * qyery     - query string
 * source    - update source
 * result    - resulting rpsl object list
 * key       - query search key

  return       - LU_OKAY on success, LU_ERROR on error

  This is merely a dispatch to the server's implementation.
 */
LU_ret_t
LU_get(LU_server_t *server, gchar *query_str,
                 gchar *source, GList **result, const gchar *key)
{
  LU_ret_t ret_val;

  LG_log(lu_context, LG_FUNC, ">LU_get: entered");
  ret_val = server->get(server, query_str, source, result, key);
  LG_log(lu_context, LG_FUNC, "<LU_get: exiting with value [%s]",
         LU_ret2str(ret_val));
  return ret_val;
}

/*
  Lookup a parents of an object

  server     - connection information
  parents    - used to return parents, if any
  obj        - object to look up parents of
  source     - source, e.g. "RIPE", or NULL to get from obj

  return       - LU_OKAY on success, LU_ERROR on error

  This is merely a dispatch to the server's implementation.
 */
LU_ret_t 
LU_get_parents (LU_server_t *server, GList **parents, 
                const rpsl_object_t *obj, const gchar *source)
{
  LU_ret_t ret_val;

  LG_log(lu_context, LG_FUNC, ">LU_get_parents: entered");
  ret_val = server->get_parents(server, parents, obj, source);
  LG_log(lu_context, LG_FUNC, "<LU_get_parents: exiting with value [%s]", 
         LU_ret2str(ret_val));
  return ret_val;
}
/* 
  Check if object has overlaps; supposed for inetnums only

  server     - connection information
  obj        - object to look up overlaps of
  overlap   - list of overlapping objects (if any) or NULL
  source     - source, e.g. "RIPE", or NULL to get from obj

  return       - LU_OKAY on success, LU_ERROR on error

  This is merely a dispatch to the server's implementation.
 */

LU_ret_t
LU_check_overlap (LU_server_t *server, GList **overlap,
                const rpsl_object_t *obj, const gchar *source)
{
  LU_ret_t ret_val;

  LG_log(lu_context, LG_FUNC, ">LU_check_overlap: entered");
  ret_val = server->check_overlap(server, overlap, obj, source);
  LG_log(lu_context, LG_FUNC, "<LU_check_overlap: exiting with value [%s]",
         LU_ret2str(ret_val));
  return ret_val;
}

/* 
  Cleans up a lookup connection

  server   - server connection

  This is merely a dispatch to the server's implementation.
 */
void 
LU_cleanup (LU_server_t *server)
{
  server->cleanup(server);
}


/* 
  Frees memory allocated by LU_get_parents().

  parents   - GList of rpsl_object_t returned by LU_get_parents
 */
void 
LU_free_parents (GList *parents)
{
  GList *ptr;
  rpsl_object_t *parent;

  for (ptr=parents; ptr != NULL; ptr = g_list_next(ptr))
  {
    parent = ptr->data;
    rpsl_object_delete(parent);
  }
  g_list_free(parents);
}



