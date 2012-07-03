/*
 * $Id: ns_xml.h,v 1.1.4.1 2005/07/22 12:24:31 katie Exp $
 */

#ifndef NSXML_H
#define NSXML_H

#include "rip.h"
#include "au_plugins.h"

AU_ret_t ns_domain_dnscheck(au_plugin_callback_info_t * info, gchar * domain, GList *nservers, GList *ds_rdata);

#endif
