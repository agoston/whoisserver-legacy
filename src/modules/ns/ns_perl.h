/*
 * $Id: ns_perl.h,v 1.1.4.1 2005/07/22 12:24:31 katie Exp $
 */

#ifndef NS_PERL_H_
#define NS_PERL_H_

#include <glib.h>
#include "ca_configFns.h"
#include "ca_defs.h"
#include "ca_dictionary.h"
#include "ca_macros.h"
#include "lg.h"

void rdns_perl_delcheck(gchar *, gchar *, gchar **, gchar **, gchar **, gchar **);
gchar *ns_find_delcheck_conf(LG_context_t *, gchar *);

#endif
