#ifndef AU_H
#define AU_H

#include <glib.h>
#include "syntax_api.h"
#include "rt_dbupdate.h"
#include "lg.h"
#include "lu.h"
#include "ut_string.h"

typedef enum {
  AU_AUTHORISED,         /* Operation is authorised */
  AU_UNAUTHORISED_CONT,  /* Operation is unauthorised, continue if desired */
  AU_UNAUTHORISED_END,   /* Operation is unauthorised, do not process further */
  AU_FWD,                /* Operation should be forwarded to RIPE-DBM */
  AU_ERROR               /* Error occured when checking */
} AU_ret_t;

/* convert the enum to a printable string */
#define AU_ret2str(r) \
  (((r) == AU_AUTHORISED)        ? "AU_AUTHORISED" : \
   ((r) == AU_UNAUTHORISED_CONT) ? "AU_UNAUTHORISED_CONT" : \
   ((r) == AU_UNAUTHORISED_END)  ? "AU_UNAUTHORISED_END" : \
   ((r) == AU_FWD)               ? "AU_FWD" : \
   ((r) == AU_ERROR)             ? "AU_ERROR" : \
    NULL)

typedef enum {
  AU_CREATE, 
  AU_MODIFY, 
  AU_DELETE
} AU_check_t;


void AU_set_lookup(LU_server_t *lookup_server);
gboolean AU_init(LG_context_t *ctx, int test_mode);
void AU_end();
AU_ret_t AU_check_auth(const rpsl_object_t *obj, const GList *cred, 
                       AU_check_t type, RT_context_t *ctx, 
                       GList **mntner_used);


#endif /* AU_H */

