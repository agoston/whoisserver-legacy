#ifndef LU_WHOIS_H
#define LU_WHOIS_H

#include "lu.h"

typedef struct {
  gchar *hostname;
  int port;
  int timeout;
  int socket;
} LU_whois_info_t;

LU_server_t *LU_whois_init(const gchar *hostname, int port, int timeout);

#endif /* LU_WHOIS_H */

