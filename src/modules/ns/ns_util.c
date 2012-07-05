/*
 * $Id: ns_util.c,v 1.7 2006/08/07 11:20:28 katie Exp $
 */

#include "ns_util.h"
#include "up_util.h"
#include "regex.h"

/*
 * Case insensitive strstr()
 */
static gchar *stristr(const gchar * haystack, const gchar * needle) {
    /* use copies, to prevent string modification */
    gchar *haystack_copy = NULL, *needle_copy = NULL, *result_copy;
    gchar *result;

    if (haystack == NULL ) {
        result = NULL;
    } else {
        haystack_copy = g_strdup(haystack);
        if (needle != NULL ) {
            needle_copy = g_strdup(needle);
        }
        g_strdown(needle_copy);
        g_strdown(haystack_copy);
        result_copy = strstr(haystack_copy, needle_copy);
        if (result_copy == NULL ) {
            result = NULL;
        } else {
            result = (gchar *) (haystack + (result_copy - haystack_copy));
        }
    }
    if (needle_copy != NULL ) {
        g_free(needle_copy);
    }
    if (haystack_copy != NULL ) {
        g_free(haystack_copy);
    }
    return (result);
}


/*
 * checks whether the suffix is rdns related
 */
static gboolean ns_check_suffix(LG_context_t * lg_ctx, rpsl_object_t * obj, gboolean with_dot) {
    gchar *domain; /* domain name */
    gchar *p; /* temporary pointer */
    gboolean ret_val = FALSE; /* result */
    gint i = 0; /* generic iterator */
    gchar **ns_suffix; /* ns suffixes we recognize */
    char *ns_suffix_from_config;

    /* Extract the domain name */
    domain = rpsl_object_get_key_value(obj);
    if (!domain)
        return ret_val;

    LG_log(lg_ctx, LG_FUNC, ">Entering ns_check_suffix(%s)", domain);

    ns_suffix_from_config = ca_get_ns_suffix;
    ns_suffix = ut_g_strsplit_v1(ns_suffix_from_config, "\n", 0);
    free(ns_suffix_from_config);

    /* remove trailing dot */
    if (with_dot) {
        domain[strlen(domain) - 1] = 0;
    }

    /* convert to lowercase */
    p = g_ascii_strdown(domain, -1);
    g_free(domain);
    domain = p;

    /* check against list of ns suffixes */
    for (i = 0; ns_suffix[i]; i++) {
        LG_log(lg_ctx, LG_DEBUG, "comparing [%s] with [%s]", domain, ns_suffix[i]);
        if (g_str_has_suffix(domain, ns_suffix[i])) {
            ret_val = TRUE;
            break;
        }
    }

    g_free(domain);
    g_strfreev(ns_suffix);
    LG_log(lg_ctx, LG_FUNC, "<Exiting ns_check_suffix with result %d", ret_val);
    return ret_val;
}

/*
 * checks if the domain object has a dot at the end
 */
static gboolean ns_is_domain_dotted(rpsl_object_t * obj) {
    gchar *domain;
    gchar last_char;
    gchar *rpsl_text;
    gboolean ret_val = FALSE;

    /* Find what kind of object it is */
    rpsl_text = rpsl_object_get_text(obj, 0);
    if (strncmp(rpsl_text, "domain:", 7) == 0) { /* it is a domain object */

        /* Extract the domain name */
        domain = rpsl_object_get_key_value(obj);
        if (strlen(domain) > 0) {
            last_char = domain[(strlen(domain) - 1)];
            if (last_char == '.') {
                ret_val = TRUE;
            }
        }
        free(domain);
    }

    free(rpsl_text);
    return ret_val;
}

/*
 * removes the trailing dot from string
 */
gboolean ns_remove_trailing_dot(LG_context_t * lg_ctx, gchar ** object_str) {
    rpsl_object_t *object; /* object itself */
    const rpsl_attr_t *domain_attr; /* key attribute */
    gchar *domain; /* extracted domain string */
    gchar *p; /* temporary pointer for iteration */
    const gchar *gp; /* temporary pointer */
    gchar *new_object_str; /* replacement object string */
    gboolean ret_val = FALSE; /* our return value */
    gint space_count = 0; /* leading spaces in the original attr */
    gchar *space_str; /* copy of leading spaces */
    gchar *domain_with_space; /* removed version without space */

    LG_log(lg_ctx, LG_DEBUG, "checking whether to remove trailing dot from domain attribute");
    if (*object_str == NULL ) {
        LG_log(lg_ctx, LG_DEBUG, "NULL string in object_str");
        return FALSE;
    }
    object = rpsl_object_init(*object_str);

    /* Extract the domain name */
    domain = rpsl_object_get_key_value(object);

    if ((!ns_check_suffix(lg_ctx, object, TRUE)) && (!ns_is_domain_dotted(object))){
    LG_log(lg_ctx, LG_DEBUG, "object is not a domain with trailing dot");
    ret_val = FALSE;
} else {
    LG_log(lg_ctx, LG_DEBUG, "object is a domain with trailing dot");
    /* Remove . at the end if any */
    p = strrchr(domain, '.');
    if ((p != NULL) && (strcmp(p, ".") == 0)) {
        /* removing . */
        LG_log(lg_ctx, LG_DEBUG, "removing trailing . from %s", domain);
        *p = 0;

        /* copy leading whitespace - we assume domain starts with a number
         * disclaimer: any failure will make the object only uglier, there
         * are no other side effects */
        domain_attr = rpsl_object_get_attr_by_ofs(object, 0);
        gp = rpsl_attr_get_value(domain_attr);
        if (gp == NULL) {
            LG_log(lg_ctx, LG_DEBUG, "can't get value of domain attribute");
            ret_val = FALSE;
        } else {
            p = (gchar *) gp;
            while (!isalnum((int) *p)) {
                p++;
                space_count++;
            }
            space_str = g_strdup(gp);
            g_snprintf(space_str, space_count, "%s", gp);
            domain_with_space = g_strdup_printf(" %s%s", space_str, domain);
            g_free(space_str);
            rpsl_attr_replace_value((rpsl_attr_t *) domain_attr,
                    domain_with_space);
            g_free(domain_with_space);
            new_object_str = rpsl_object_get_text(object, 0);
            strcpy(*object_str, new_object_str);
            free(new_object_str);
            ret_val = TRUE;
        }
    } else {
        LG_log(lg_ctx, LG_DEBUG, "no trailing dot at the end of %s", domain);
    }
}

    rpsl_object_delete(object);
    free(domain);
    return ret_val;
}

/* 
 * check whether this is e164.arpa domain
 */
gboolean ns_is_e164_arpa(au_plugin_callback_info_t * info) {
    gchar *domain;

    domain = rpsl_object_get_key_value(info->obj);

    if (ns_has_suffix(domain, "e164.arpa")) {
        return TRUE;
    } else {
        return FALSE;
    }

}

/* domain has suffix */
gboolean ns_has_suffix(gchar * domain, const char * suffix) {
    gchar *p;

    p = stristr(domain, suffix);
    if (p != NULL && (strcasecmp(p, suffix) == 0)) {
        LG_log(au_context, LG_DEBUG, "object is %s related", suffix);
        return TRUE;
    } else {
        LG_log(au_context, LG_DEBUG, "object is NOT %s related", suffix);
        return FALSE;
    }
}

/*
 * checks whether the suffix is rdns related
 */
gboolean ns_is_rdns_suffix(au_plugin_callback_info_t * info) {
    if (ns_check_suffix(au_context, info->obj, FALSE)) {
        LG_log(au_context, LG_DEBUG, "object is rdns related");
        return TRUE;
    } else {
        LG_log(au_context, LG_DEBUG, "object is NOT rdns related");
        return FALSE;
    }
}
