#ifndef _CR_H_
#define _CR_H_

#include <glib.h>
#include <unistd.h>
#include <md5.h>
#include "lg.h"

typedef void CR_credential_t;

typedef enum {
  CR_PASSWORD,
  CR_OVERRIDE,
  CR_PGP,
  CR_X509,
  CR_FROM
} CR_type;

/* convert the enum to a printable string */
#define CR_type2str(r) \
  (((r) == CR_PASSWORD)     ? "CR_PASSWORD" : \
   ((r) == CR_OVERRIDE)     ? "CR_OVERRIDE" : \
   ((r) == CR_PGP)          ? "CR_PGP" : \
   ((r) == CR_X509)         ? "CR_X509" : \
   ((r) == CR_FROM)         ? "CR_FROM" : \
                              "UNKNOWN")

void CR_init(LG_context_t* ctx);
CR_credential_t* CR_credential_new(CR_type type, const char* value,
  gboolean valid);
CR_credential_t* CR_credential_duplicate(CR_credential_t* credential);
void CR_credential_free(CR_credential_t* credential);

CR_type CR_credential_get_type(CR_credential_t* credential);
const char* CR_credential_get_value(CR_credential_t* credential);
gboolean CR_credential_get_validity(CR_credential_t* credential);

gboolean CR_credential_list_has_credential(GList* list,
  CR_type type, const char *value, gboolean include_invalid);
gboolean CR_credential_list_check(GList* list,
  CR_type type, const char *value, gboolean include_invalid);
GList* CR_credential_list_duplicate(GList* credentials);
void CR_credential_list_free(GList* creds);

#endif
