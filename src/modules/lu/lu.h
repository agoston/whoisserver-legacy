#ifndef LU_H
#define LU_H

#include <pthread.h>
#include <glib.h>
#include <stdio.h>
#include "lg.h"
#include "syntax_api.h"

#define MAX_QUERY_LENGTH 2048

/* lookup returns */
typedef enum {
  LU_ERROR = 0,
  LU_OKAY
} LU_ret_t;

/* get a string describing the return code */
#define LU_ret2str(ret) (((ret) == LU_OKAY) ? "LU_OKAY" : \
                         (((ret) == LU_ERROR) ? "LU_ERROR" : \
                          NULL))

/* generic information for any type of server */
typedef struct {
  LU_ret_t (*lookup)();
  LU_ret_t (*inverse_lookup)();
  LU_ret_t (*get_object)();
  LU_ret_t (*get_parents)();
  LU_ret_t (*check_overlap)();
  void (*cleanup)();
  void *info;
} LU_server_t;


void LU_init(LG_context_t *ctx);
LU_ret_t LU_lookup(LU_server_t *server, rpsl_object_t **obj, const gchar *type,
                   const gchar *key, const gchar *source);
LU_ret_t LU_inverse_lookup(LU_server_t *server, GList **objects, GList *types,
                   const gchar *key, GList *inverse_keys, const gchar *source);
LU_ret_t LU_get_object(LU_server_t *server, rpsl_object_t **dbase_obj, 
                       const rpsl_object_t *obj, const gchar *source);
LU_ret_t LU_get_parents(LU_server_t *server, GList **parents, 
                        const rpsl_object_t *obj, const gchar *source);
LU_ret_t LU_check_overlap(LU_server_t *server, GList **overlap, 
                        const rpsl_object_t *obj, const gchar *source);
LU_ret_t LU_get_inetnum_from_domain(LU_server_t *,gchar *,gchar *,
                                    GList **);
void LU_free_parents(GList *parents);
void LU_cleanup(LU_server_t *server);


#endif /* LU_H */

