#ifndef AU_UTIL_H
#define AU_UTIL_H

#include "au_plugins.h"

void au_override(AU_ret_t *ret_val, 
                 gboolean *override,
                 au_plugin_callback_info_t *info);
AU_ret_t au_check_authentications (GList *auth_attrs, GList *cred);
AU_ret_t au_mntner_authenticate(RT_context_t *ctx, 
                                const gchar *mntner_name, 
                                LU_server_t *lu,
                                const gchar *source_name, 
                                GList *cred, 
                                rpsl_object_t **mntner);
AU_ret_t au_check_mnt_routes_prefix (GList **mntners, 
                                     rpsl_object_t *parent, 
                                     char *parent_text, 
                                     au_plugin_callback_info_t *info);
AU_ret_t
au_check_multiple_authentications (char *attr_to_check[],
                                   rpsl_object_t *obj,
                                   char *parent_text,
                                   au_plugin_callback_info_t *info);
AU_ret_t
au_rir_auth_check(au_plugin_callback_info_t *info, const rpsl_object_t *obj, GList *rir_mntner_list);


#endif /* AU_UTIL_H */
