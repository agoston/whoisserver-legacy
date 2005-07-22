/*
 * $Id: ns_util.h,v 1.1 2004/12/27 17:52:36 can Exp $
 */

#ifndef NSUTIL_H
#define NSUTIL_H


#include "rip.h"
#include "dbupdate.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <au_plugins.h>
#include <km.h>
#include <rt_dbupdate.h>

#include "up_util.h"
#include "ep.h"
#include <glib.h>

gchar *ns_par(gchar *);

gchar **ns_nservers(rpsl_object_t *, RT_context_t *, gchar *, AU_ret_t *);

gchar **ns_ds_rdata(rpsl_object_t *, RT_context_t *, gchar *, AU_ret_t *);

gboolean ns_is_rdns_suffix(au_plugin_callback_info_t *);

AU_ret_t ns_is_parent_ours(au_plugin_callback_info_t *, gchar *);

gboolean ns_remove_trailing_dot(LG_context_t *, gchar **);

gboolean ns_is_decompose(LG_context_t *, const gchar *, gint *, gint *);

gchar **ns_decompose_object(LG_context_t *, RT_context_t *, gchar *, gint,
                            gint);

#endif
