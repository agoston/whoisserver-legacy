/*
 * $Id: ns_xml.h,v 1.1 2004/12/27 16:38:43 can Exp $
 */

#ifndef NSXML_H
#define NSXML_H

#include "rip.h"
#include <glib.h>
#include "au_plugins.h"

AU_ret_t ns_domain_delcheck(au_plugin_callback_info_t *, gchar *, gchar **,
                            gchar *);

#endif
