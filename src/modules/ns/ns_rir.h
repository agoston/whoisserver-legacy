/*
 * $Id: ns_rir.h,v 1.1 2004/12/27 16:38:43 can Exp $
 */

#ifndef NSRIR_H_
#define NSRIR_H_

#include <glib.h>
#include <au_util.h>

void rdns_done_delegations(GTree *);

gchar *rdns_who_delegates(LG_context_t *, GTree *, gchar *);

AU_ret_t ns_find_rir(au_plugin_callback_info_t *, gchar *);

gboolean ns_is_erx(gchar *);

#endif
