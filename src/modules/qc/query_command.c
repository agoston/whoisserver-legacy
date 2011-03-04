/***************************************
 $Revision: 1.10 $

 Query command module (qc).  This is what the whois query gets stored as in
 memory.

 Status: NOT REVUED, TESTED

 ******************//******************
 Filename            : query_command.c
 Author              : ottrey@ripe.net
 Modifications by    : marek@ripe.net
 ******************//******************
 Copyright (c) 1999                              RIPE NCC

 All Rights Reserved

 Permission to use, copy, modify, and distribute this software and its
 documentation for any purpose and without fee is hereby granted,
 provided that the above copyright notice appear in all copies and that
 both that copyright notice and this permission notice appear in
 supporting documentation, and that the name of the author not be
 used in advertising or publicity pertaining to distribution of the
 software without specific, written prior permission.

 THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
 AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ***************************************/

#define QC_IMPL
#include "rip.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* logging */
LG_context_t *qc_context;

/* cache of default sources */
GList *deflook_sources_list;

/* cache of GRS sources */
GList *grs_sources_list;

/* cache of all sources (REVERSED!) */
GList *all_sources_list_r;

/*
 note on errors:
 QC_SYNERR returns a full help text
 QC_PARERR returns only any messags
 */

/* only those are allowed in query strings*/
#define ALLOWED_QUERY_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
                            "abcdefghijklmnopqrstuvwxyz"\
                            "0123456789-_:+=.,@/?' \n"

extern char *suboptarg;
extern int getsubopt(char **optionp, char * const *tokens, char **valuep);

/*++++++++++++++++++++++++++++++++++++++
 make a list of sources. expects list to hold source handles

 char *
 qc_sources_list_to_string     returns an allocated string, must be freed

 GList *list                   list of source handles (as defined by CA)
 ++++++++++++++++++++++++++++++++++++++*/
char *
qc_sources_list_to_string(GList *list) {
    GString *tmp;
    GList *qitem;
    char *result;

    /* use GString for the result, because it resizes magically */
    tmp = g_string_sized_new(STR_L);

    /* loop through the list, creating a string with the names of the
     sources separated by commas */
    qitem = g_list_first(list);
    if (qitem != NULL) {
        for (;;) {
            ca_dbSource_t *source_hdl = (ca_dbSource_t *) (qitem->data);
            char *srcname = ca_get_srcname(source_hdl);
            g_string_append(tmp, srcname);

            qitem = g_list_next(qitem);
            if (qitem == NULL) {
                break;
            }

            g_string_append_c(tmp, ',');
        }
    }

    /* move to a new buffer for the return */
    result = UT_strdup(tmp->str);
    g_string_free(tmp, TRUE);
    return result;
}

/* Convert the query_environ to a string.

   qe    The query_environ to be converted

   Returns gchar*, to be freed by the caller.
 ++++++++++++++++++++++++++++++++++++++*/
char *QC_environ_to_string(Query_environ qe) {
    char *sources;
    GString *tmp;

    /* convert the sources and the passed address (if any) to printable strings */
    sources = qc_sources_list_to_string(qe.sources_list);
    /* format the environment info */
    tmp = g_string_sized_new(STR_L);
    g_string_sprintf(tmp, "host=%s, keep_connection=%s, sources=%s, version=%s%s%s", qe.condat.rIPs, qe.k ? "on" : "off",
                     sources, (qe.version == NULL) ? "?" : qe.version, qe.eIPs[0] == '\0' ? "" : ", passedIP=",
                     qe.eIPs);
    UT_free(sources);

    return g_string_free(tmp, FALSE);
}

/*++++++++++++++++++++++++++++++++++++++
 Convert the query_command to a string.

 Query_command *query_command The query_command to be converted.
 ++++++++++++++++++++++++++++++++++++++*/
char *QC_query_command_to_string(Query_command *query_command) {
    char result_buf[STR_XL];
    char *str1;
    char *str2;
    char *str3;

    str1 = MA_to_string(query_command->inv_attrs_bitmap, DF_get_attribute_names());
    str2 = MA_to_string(query_command->object_type_bitmap, DF_get_class_names());
    str3 = WK_to_string(query_command->keytypes_bitmap);

    sprintf(result_buf,
            "Query_command : inv_attrs=%s, recursive=%s, object_type=%s, (c=%s,C=%s,G=%s,B=%s,b=%s,g=%d,l=%d,m=%d,q=%d,t=%d,v=%d,x=%d,F=%d,K=%d,L=%d,M=%d,R=%d), possible keytypes=%s, keys=[%s]",
            str1, query_command->recursive ? "y" : "n", str2, query_command->c ? "TRUE" : "FALSE",
            query_command->C ? "TRUE" : "FALSE", query_command->G ? "TRUE" : "FALSE",
            query_command->B ? "TRUE" : "FALSE", query_command->b ? "TRUE" : "FALSE", query_command->g,
            query_command->l, query_command->m, query_command->q, query_command->t, query_command->v, query_command->x,
            query_command->fast, query_command->filtered, query_command->L, query_command->M, query_command->R, str3,
            query_command->keys);
    UT_free(str1);
    UT_free(str2);
    UT_free(str3);

    return UT_strdup(result_buf);

}


/*++++++++++++++++++++++++++++++++++++++
 Free the query_environ.

 Query_command *qc query_environ to be freed.
 ++++++++++++++++++++++++++++++++++++++*/
void QC_environ_free(Query_environ *qe) {
    if (qe != NULL) {
        UT_free(qe->version);
        SK_cd_free(&(qe->condat));

        if (qe->sources_list != NULL) {
            g_list_free(qe->sources_list);
        }
        UT_free(qe);
    }
}


/*++++++++++++++++++++++++++++++++++++++
 Free the query_command.

 Query_command *qc query_command to be freed.

 XXX I'm not sure the bitmaps will get freed.
 qc->inv_attrs_bitmap
 qc->object_type_bitmap
 qc->keytypes_bitmap
 ++++++++++++++++++++++++++++++++++++++*/
void QC_free(Query_command *qc) {
    GList *message_node;

    if (qc != NULL) {
        if (qc->keys != NULL) {
            g_free(qc->keys);
        }
        message_node = qc->parse_messages;
        while (message_node != NULL) {
            UT_free(message_node->data);
            message_node = g_list_next(message_node);
        }
        g_list_free(qc->parse_messages);
        UT_free(qc);
    }
} /* QC_free() */

void QC_init_struct(Query_command *query_command) {
    query_command->query_type = QC_SYNERR;
    query_command->c = TRUE; /* IRT search is on by default */
    query_command->C = FALSE;
    query_command->G = TRUE; /* grouping is on by default */
    query_command->B = FALSE; /* "original" output is off by default */
    query_command->b = FALSE;
    query_command->d = 0;
    query_command->fast = 0;
    query_command->g = 0;
    query_command->inv_attrs_bitmap = MA_new(MA_END);
    query_command->recursive = 1; /* Recursion is on by default. */
    query_command->l = 0;
    query_command->m = 0;
    query_command->q = -1;
    query_command->t = -1;
    query_command->v = -1;
    query_command->x = 0;
    query_command->filtered = 0;
    query_command->L = 0;
    query_command->M = 0;
    query_command->R = 0;
    /* XXX UGLY - "all zeros" in object_type_bitmap means the same as
     "all ones". To limit the inconsistency, this is changed at the end
     of this function, so outside "all zeros" is an illegal value. */
    query_command->object_type_bitmap = MA_new(MA_END);
    query_command->keytypes_bitmap = MA_new(MA_END);
    query_command->keys = NULL;
    query_command->parse_messages = NULL;
}

/* QC_fill() */
/*++++++++++++++++++++++++++++++++++++++
 Create a new query_command.
 Returns 0 when OK, -1 when query incorrect.

 char *query_str The garden variety whois query string.

 Query_environ *qe the environment

 More:
 +html+ <PRE>
 Authors:
 ottrey - original code
 marek - modified for my getopts, multiple sources;
 and generally cleaned.
 +html+ </PRE><DL COMPACT>
 +html+ <DT>Online References:
 +html+ <DD><UL>
 +html+ </UL></DL>

 ++++++++++++++++++++++++++++++++++++++*/
static int QC_fill(const char *query_str, Query_command *query_command, Query_environ *qe) {

    int c;
    int synerrflg = 0;
    int badparerr = 0;
    int minusk = 0;
    mask_t inv_attr_mask;
    gchar **inv_attrs;
    int opt_argc;
    gchar **opt_argv;
    gchar **opt_argv_copy;
    int i;
    C_Type_t type;
    A_Type_t attr;
    getopt_state_t *gst = NULL;
    gchar **types;
    gchar **version_info;

    int ip_flag_used = 0;
    int ip_flag_duplicated = 0;
    GList *sources_specified_r = NULL;      /* in reverse order */
    int num_flags = 0;
    int num_client_ip = 0;

    gboolean is_ip_key, is_rdns_key, is_inverse;
    gboolean fixed_lookup;
    char lookup[64];

    /* split into words */
    opt_argv = ut_g_strsplit_v1((char*) query_str, " ", -1);

    /* count number of non-empty words */
    opt_argc = 0;
    for (i = 0; opt_argv[i] != NULL; i++) {
        if (opt_argv[i][0] != '\0') {
            opt_argc++;
        }
    }

    /* make a copy of the arguments with empty words removed */
    opt_argv_copy = g_new(gchar*, opt_argc+1);
    opt_argc = 0;
    for (i = 0; opt_argv[i] != NULL; i++) {
        if (opt_argv[i][0] != '\0') {
            opt_argv_copy[opt_argc] = opt_argv[i];
            opt_argc++;
        } else {
            g_free(opt_argv[i]);
        }
    }
    opt_argv_copy[opt_argc] = NULL;
    g_free(opt_argv);
    opt_argv = opt_argv_copy;

    dieif((gst = mg_new(0)) == NULL);

    while ((c = mg_getopt(opt_argc, opt_argv, "acdgi:klbmq:rs:t:v:xBCGFKLMRST:V:", gst)) != EOF) {
        num_flags++;

        switch (c) {
            case 'a':
                if (sources_specified_r) {
                    g_list_free(sources_specified_r);
                }
                sources_specified_r = g_list_copy(all_sources_list_r);
                break;

            case 'C':
                query_command->c = FALSE;
                query_command->C = TRUE;
                break;

            case 'c':
                query_command->c = TRUE;
                query_command->C = FALSE;
                break;

            case 'G':
                query_command->G = FALSE;
                break;

            case 'B':
                query_command->B = TRUE;
                break;

            case 'b':
                query_command->b = TRUE;
                query_command->c = TRUE;
                query_command->l = 0;
                query_command->m = 0;
                query_command->x = 0;
                query_command->L = 0;
                query_command->M = 0;
                if (ip_flag_used)
                    ip_flag_duplicated++;
                ip_flag_used = 'b';
                break;

            case 'd':
                query_command->d = 1;
                break;

            case 'g':
                query_command->g = 1;
                break;

            case 'i':
                /* always get an argument here (according to the code) */
                dieif(gst->optarg == NULL);

                /* check each argument present */
                inv_attr_mask = MA_new(INV_ATTR_MASK);
                inv_attrs = ut_g_strsplit_v1(gst->optarg, ",", -1);
                for (i = 0; inv_attrs[i] != NULL; i++) {
                    g_strdown(inv_attrs[i]);
                    if (!strcmp(inv_attrs[i], "pn") || !strcmp(inv_attrs[i], "person")) {
                        /* person means "admin-c,tech-c,zone-c,author,ping-hdl" */
                        MA_set(&(query_command->inv_attrs_bitmap), A_AC, 1);
                        MA_set(&(query_command->inv_attrs_bitmap), A_TC, 1);
                        MA_set(&(query_command->inv_attrs_bitmap), A_ZC, 1);
                        MA_set(&(query_command->inv_attrs_bitmap), A_AH, 1);
                        MA_set(&(query_command->inv_attrs_bitmap), A_PC, 1);
                    } else {
                        /* otherwise it should be an inverse attribute name */
                        attr = DF_attribute_name2type(inv_attrs[i]);
                        if ((int) attr < 0) {
                            /* ERROR:104 */
                            /* "%s" is not a known RPSL attribute. */
                            char *fmt = ca_get_qc_fmt_badattr;
                            query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, inv_attrs[i]));
                            UT_free(fmt);
                            badparerr++;
                        } else if (!MA_isset(inv_attr_mask, attr)) {
                            /* ERROR:105 */
                            /* "%s" is not an inverse searchable attribute. */
                            char *fmt = ca_get_qc_fmt_attrnotinv;
                            query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, inv_attrs[i]));
                            UT_free(fmt);
                            badparerr++;
                        } else {
                            /* Add the attr to the bitmap. */
                            MA_set(&(query_command->inv_attrs_bitmap), attr, 1);
                        }
                    }
                }
                g_strfreev(inv_attrs);

                break;

            case 'k':
                minusk = 1;
                break;

            case 'r':
                query_command->recursive = 0; /* no recursion */
                break;

            case 'l':
                query_command->c = FALSE;
                query_command->l = 1;
                query_command->m = 0;
                query_command->x = 0;
                query_command->L = 0;
                query_command->M = 0;
                query_command->b = FALSE;
                if (ip_flag_used)
                    ip_flag_duplicated++;
                ip_flag_used = 'l';
                break;

            case 'm':
                query_command->c = FALSE;
                query_command->l = 0;
                query_command->m = 1;
                query_command->x = 0;
                query_command->L = 0;
                query_command->M = 0;
                query_command->b = FALSE;
                if (ip_flag_used)
                    ip_flag_duplicated++;
                ip_flag_used = 'm';
                break;

            case 'q':
                /* always get an argument here (according to the code) */
                dieif(gst->optarg == NULL);

                if (strcasecmp(gst->optarg, "sources") == 0) {
                    query_command->q = QC_Q_SOURCES;
                } else if (strcasecmp(gst->optarg, "version") == 0) {
                    query_command->q = QC_Q_VERSION;
                } else if (strcasecmp(gst->optarg, "types") == 0) {
                    query_command->q = QC_Q_TYPES;
                } else {
                    synerrflg++;
                }
                break;

            case 's': {

                dieif(gst->optarg == NULL); /* always get an argument here (according to the code) */

                gchar **sources;
                ca_dbSource_t *hdl;

                g_strup(gst->optarg);

                /* go through specified sources */
                sources = ut_g_strsplit_v1(gst->optarg, ",", -1);
                for (i = 0; sources[i]; i++) {
                    if (!strcmp(sources[i], "GRS")) {
                        /* append all GRS sources to sources_specified */
                        GList *p;
                        for (p = g_list_first(grs_sources_list); p; p = g_list_next(p)) {
                            sources_specified_r = g_list_prepend(sources_specified_r, p->data);
                        }
                    } else {
                        hdl = ca_get_SourceHandleByName(sources[i]);

                        /* FIXME: temporary hack to allow using old names for GRS sources; e.g. ARIN instead of ARIN-GRS
                         * Should be removed after a grace period - agoston, 2011-03-03 */
                        if (!hdl) {
                            char t[STR_M];
                            snprintf(t, STR_M, "%s-GRS", sources[i]);
                            hdl = ca_get_SourceHandleByName(t);
                        }

                        if (!hdl) {
                            /* ERROR:102 */
                            /* Unknown source %s requested. */
                            char *fmt = ca_get_qc_fmt_badsource;
                            query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, sources[i]));
                            UT_free(fmt);
                            badparerr++;
                        } else {
                            /* append */
                            sources_specified_r = g_list_prepend(sources_specified_r, hdl);
                        }
                    }
                    g_strfreev(sources);
                }
                break;
            }

            case 't':
                /* always get an argument here (according to the code) */
                dieif(gst->optarg == NULL);

                /* parse the specification */
                g_strdown(gst->optarg);
                type = DF_class_name2type(gst->optarg);
                if (type < 0) {
                    /* ERROR:103 */
                    /* "%s" is not a known RPSL object type. */
                    char *fmt = ca_get_qc_fmt_badobjtype;
                    query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, gst->optarg));
                    UT_free(fmt);
                    badparerr++;
                } else {
                    query_command->t = type;
                }
                break;

            case 'v':
                /* always get an argument here (according to the code) */
                dieif(gst->optarg == NULL);

                /* parse the specification */
                g_strdown(gst->optarg);
                type = DF_class_name2type(gst->optarg);
                if (type < 0) {
                    /* ERROR:103 */
                    /* "%s" is not a known RPSL object type. */
                    char *fmt = ca_get_qc_fmt_badobjtype;
                    query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, gst->optarg));
                    UT_free(fmt);
                    badparerr++;
                } else {
                    query_command->v = type;
                }
                break;

            case 'x':
                query_command->c = FALSE;
                query_command->l = 0;
                query_command->m = 0;
                query_command->x = 1;
                query_command->L = 0;
                query_command->M = 0;
                if (ip_flag_used)
                    ip_flag_duplicated++;
                ip_flag_used = 'x';
                break;

            case 'F':
                query_command->fast = 1;
                query_command->recursive = 0; /* no recursion */
                break;

            case 'K':
                query_command->filtered = 1;
                query_command->recursive = 0; /* no recursion */
                break;

            case 'L':
                query_command->c = FALSE;
                query_command->l = 0;
                query_command->m = 0;
                query_command->x = 0;
                query_command->L = 1;
                query_command->M = 0;
                query_command->b = FALSE;
                if (ip_flag_used)
                    ip_flag_duplicated++;
                ip_flag_used = 'L';
                break;

            case 'M':
                query_command->c = FALSE;
                query_command->l = 0;
                query_command->m = 0;
                query_command->x = 0;
                query_command->L = 0;
                query_command->M = 1;
                query_command->b = FALSE;
                if (ip_flag_used)
                    ip_flag_duplicated++;
                ip_flag_used = 'M';
                break;

            case 'R':
                query_command->R = 1;
                break;

            case 'T':
                /* always get an argument here (according to the code) */
                dieif(gst->optarg == NULL);

                /* parse the specification */
                g_strdown(gst->optarg);
                types = ut_g_strsplit_v1(gst->optarg, ",", -1);
                for (i = 0; types[i] != NULL; i++) {
                    type = DF_class_name2type(types[i]);
                    if (type < 0) {
                        /* ERROR:103 */
                        /* "%s" is not a known RPSL object type. */
                        char *fmt = ca_get_qc_fmt_badobjtype;
                        query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, types[i]));
                        UT_free(fmt);
                        badparerr++;
                    } else {
                        MA_set(&(query_command->object_type_bitmap), (unsigned) type, 1);
                    }
                }
                g_strfreev(types);
                break;

            case 'V': {
                int num = 0;
                version_info = g_strsplit(gst->optarg, ",", 3);
                for (; version_info[num]; num++) {
                    g_strstrip(version_info[num]);
                }

                if (num > 2) {
                    /* more than two parameters */
                    synerrflg++;
                } else {
                    if (num > 0) {
                        /* first parameter */
                        if (qe->version) UT_free(qe->version);
                        qe->version = UT_strdup(version_info[0]);
                    }

                    if (num > 1) {
                        /* second parameter */
                        if (IP_addr_e2b(&(qe->eIP), version_info[1]) != IP_OK) {
                            synerrflg++;
                        } else {
                            num_client_ip++;
                            strncpy(qe->eIPs, version_info[1], sizeof(qe->eIPs));
                            qe->eIPs[sizeof(qe->eIPs)-1] = 0;
                        }
                    }
                }
                g_strfreev(version_info);
                num_flags--; /* don't count -V flags in our total */
                break;
            }

                /* any other flag, including '?' */
            default:
                synerrflg++;
        }
    }

    /* update sources list if specified */
    if (sources_specified_r) {
        GList *p;

        /* free space from previous version of the list */
        g_list_free(qe->sources_list);
        qe->sources_list = NULL;

        /* add each new source */
        for (p = g_list_first(sources_specified_r); p; p = g_list_next(p)) {
            /* only add this if it does not appear later in the list;
             this removes duplicate source specifications */
            if (!g_list_find(g_list_next(p), p->data)) {
                qe->sources_list = g_list_prepend(qe->sources_list, p->data);
            }
        }
        g_list_free(sources_specified_r);
    }

    /* check for duplicate IP flags */
    if (ip_flag_duplicated) {
        /* WARNING:901 */
        char *fmt = ca_get_qc_fmt_dupipflag;
        char ip_flag_str[2];
        ip_flag_str[0] = ip_flag_used;
        ip_flag_str[1] = '\0';
        query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, ip_flag_str));
        UT_free(fmt);
    }

    /* check for duplicate proxy IP settings */
    if (num_client_ip > 1) {
        /* ERROR:205 */
        query_command->parse_messages = g_list_append(query_command->parse_messages, ca_get_qc_dupproxyipflag);
        badparerr++;
    }

    /* check for flags that should appear alone */
    if (num_flags != 1) {
        if (query_command->q != -1) {
            synerrflg++;
        }
        if (query_command->t != -1) {
            synerrflg++;
        }
        if (query_command->v != -1) {
            synerrflg++;
        }
    }

    /* check incompatible flags */
    if ((query_command->b == 1) && (query_command->filtered == 1)) {
        /* -b -K is error, we need person/role/organisation objects */
        /* ERROR:109 */
        char *fmt = ca_get_qc_fmt_uncompflag;
        query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, "-b", "-K"));

        UT_free(fmt);
        badparerr++;
    } else if ((query_command->b == 1) && (query_command->fast == 1)) {
        /* -b -F is error, we need person/role/organisation objects */
        /* ERROR:109 */
        char *fmt = ca_get_qc_fmt_uncompflag;
        query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, "-b", "-F"));

        UT_free(fmt);
        badparerr++;
    } else if ((query_command->b == 1) && (query_command->recursive == 0)) {
        /* -b -r is error, we need person/role/organisation objects */
        /* ERROR:109 */
        char *fmt = ca_get_qc_fmt_uncompflag;
        query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, "-b", "-r"));

        UT_free(fmt);
        badparerr++;
    }

    if ((query_command->b == 1) && (query_command->G == 0)) {
        /* -G -b is error, we need grouping */
        /* ERROR:109 */
        char *fmt = ca_get_qc_fmt_uncompflag;
        query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, "-b", "-G"));
        UT_free(fmt);
        badparerr++;
    }

    if ((query_command->b == 1) && (query_command->B == 1)) {
        /* -b -B is error: can't be brief and original */
        /* ERROR:109 */
        char *fmt = ca_get_qc_fmt_uncompflag;
        query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, "-b", "-B"));
        UT_free(fmt);
        badparerr++;
    }

    /* copy the key */
    query_command->keys = g_strjoinv(" ", opt_argv + gst->optind);

    /* check for using keys on queries that should not have them */
    if (query_command->keys[0] != '\0') {
        if (query_command->q != -1) {
            synerrflg++;
        }
        if (query_command->t != -1) {
            synerrflg++;
        }
        if (query_command->v != -1) {
            synerrflg++;
        }
    }

    /* if no error, process the key, otherwise don't bother */
    if (!synerrflg && !badparerr) {
        /* convert the key to uppercase. */
        g_strup(query_command->keys);

        /* make the keytypes_bitmap. */
        query_command->keytypes_bitmap = WK_new(query_command->keys);

        /* fix the object type bitmap - turn "all zeros" into "all ones" */
        if (MA_bitcount(query_command->object_type_bitmap) == 0) {
            query_command->object_type_bitmap = MA_not(query_command->object_type_bitmap);
        }

        /* XXX: missing checks for "-i" and "-T" versus key types */

        is_ip_key = MA_isset(query_command->keytypes_bitmap, WK_IPADDRESS)
                || MA_isset(query_command->keytypes_bitmap, WK_IPRANGE)
                || MA_isset(query_command->keytypes_bitmap, WK_IPPREFIX)
                || MA_isset(query_command->keytypes_bitmap, WK_IP6PREFIX);

        {   /* determine if is_rdns_key */
            ip_prefix_t ign;

            is_rdns_key = MA_isset(query_command->keytypes_bitmap, WK_REVDOMAIN);

            if (is_rdns_key && IP_revd_t2b(&ign, query_command->keys, IP_EXPN) != IP_OK) {
                /* not a reverse domain - adjust flags accordingly */
                MA_set(&query_command->keytypes_bitmap, WK_REVDOMAIN, 0);
                is_rdns_key = FALSE;
            }
        }

        /* determine if inverse query */
        is_inverse = MA_bitcount(query_command->inv_attrs_bitmap) > 0;

        /* remove domain keytype if revdomain key was used in forward lookup
         * this is needed to avoid searching the domain table AND the domain radix tree
         * Note: in reverse lookup, we don't do this, as there are many inverse searchable attributes with domain syntax
         * FIXME: there should be a clear separation between forward and reverse domain in the WK module and all across whois code */
        if (!is_inverse && is_rdns_key) {
            // it must have WK_DOMAIN set also, as it is a lot more relaxed than WK_REVDOMAIN
            MA_set(&query_command->keytypes_bitmap, WK_DOMAIN, 0);
            /* no use of irt lookup (the -c query flag) on domain objects, as there is not even an mnt-irt attribute
             * Maybe later we could add some smart lookup feature */
            query_command->c = FALSE;
        }

        /* check for use of IP flags on non-IP lookups */
        if ((ip_flag_used || query_command->d) && !(is_ip_key || is_rdns_key)) {
            /* WARNING:902, meaningless IP flag */
            query_command->parse_messages = g_list_append(query_command->parse_messages, ca_get_qc_uselessipflag);
        }

#if 0
        /* check for use of -d flag on revd lookups */
        if (query_command->d && is_rdns_key) {
            /* WARNING:903, meaningless -d flag */
            query_command->parse_messages = g_list_append(query_command->parse_messages, ca_get_qc_uselessdflag);
        }
#endif

        /* check for "fixed" lookups on IP addresses */
        fixed_lookup = 0;
        if (MA_isset(query_command->keytypes_bitmap, WK_IPADDRESS)) {
            ip_addr_t addr;
            if (NOERR(IP_addr_a2b(&addr, query_command->keys))) {
                IP_addr_b2a(&addr, lookup, sizeof(lookup));
                if (strcmp(query_command->keys, lookup) != 0) {
                    fixed_lookup = 1;
                }
            }
        } else if (MA_isset(query_command->keytypes_bitmap, WK_IPRANGE)) {
            ip_range_t rang;
            if (NOERR(IP_rang_a2b(&rang, lookup))) {
                IP_rang_b2a(&rang, lookup, sizeof(lookup));
                if (strcmp(query_command->keys, lookup) != 0) {
                    fixed_lookup = 1;
                }
            }
        } else if (MA_isset(query_command->keytypes_bitmap, WK_IPPREFIX) || MA_isset(query_command->keytypes_bitmap, WK_IP6PREFIX)) {
            ip_prefix_t pref;
            int ip_pref_a2b_retval = IP_pref_a2b(&pref, query_command->keys);
            /* IP_INVBIT error seems to do no harm, but good */
            if (NOERR(ip_pref_a2b_retval) || (ip_pref_a2b_retval == IP_INVBIT)) {
                IP_pref_b2a(&pref, lookup, sizeof(lookup));
                if (strcasecmp(query_command->keys, lookup) != 0) {
                    fixed_lookup = 1;
                }
            }
        }

        /* now check ASx - ASy for x>y */
        if (MA_isset(query_command->keytypes_bitmap, WK_ASRANGE) && !MA_isset(query_command->keytypes_bitmap, WK_AUTNUM)) {
            unsigned long asnum1, asnum2;
            if (convert_as_range(query_command->keys, &asnum1, &asnum2)) {
                LG_log(qc_context, LG_FATAL, "ASx>ASy INTERNAL ERROR: convert_as_range('%s') failed", query_command->keys);
            } else if (asnum1 > asnum2) {
                query_command->parse_messages = g_list_append(query_command->parse_messages, ca_get_qi_badrange);
                badparerr++;
            }
        }

        if (fixed_lookup) {
            /* WARNING:905 */
            char *fmt = ca_get_qc_fmt_fixedlookup;
            query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, query_command->keys, lookup));
            /* here we also need to set query_command->keys to 'lookup'
             variable, otherwise, in some cases the object might not
             get returned from radix tree. EG 2003-08-07 */
            UT_free(query_command->keys);
            query_command->keys = g_strdup(lookup);
            UT_free(fmt);
        }

        /* exclude revdomain on IP key if no -d is used */
        if (is_ip_key && !query_command->d) {
            MA_set(&(query_command->object_type_bitmap), C_DN, 0);
        }

        /* exclude inet(6)num on revdomain key if no -d is used */
        if (is_rdns_key && !query_command->d) {
            MA_set(&(query_command->object_type_bitmap), C_IN, 0);
            MA_set(&(query_command->object_type_bitmap), C_I6, 0);
        }

        /* if -R on anything else than a forward domain (revdomain is a subset of domain, hence we need to explicitly deny it) */
        if (query_command->R && (!MA_isset(query_command->keytypes_bitmap, WK_DOMAIN) || MA_isset(query_command->keytypes_bitmap, WK_REVDOMAIN))) {
            /* WARNING:904, meaningless no-referral flag */
            query_command->parse_messages = g_list_append(query_command->parse_messages, ca_get_qc_uselessnorefflag);
        }

        /* check if the domain query contains a dot at the end, remove if any WARNING:906 */
        if (MA_isset(query_command->keytypes_bitmap, WK_DOMAIN)) {
            char *domain_dot = strrchr(query_command->keys, '.');
            if ((domain_dot != NULL) && (strcmp(domain_dot, ".") == 0)) {
                char *fmt = ca_get_qc_trailingdotindomain;
                domain_dot[0] = '\0';
                query_command->parse_messages = g_list_append(query_command->parse_messages, g_strdup_printf(fmt, query_command->keys));
            }
        }

        /* "keep connection" processing:
         when opening connection, -k may be alone or with a query
         later -k must appear alone (or there must be an empty line,
         or an error) for the connection to close.
         */
        if (minusk) {
            if (qe->k == 0) { /* opening */
                qe->k = 1;
            } else { /* closing, if no key; otherwise keep open */
                if (query_command->keys[0] == '\0') {
                    qe->k = 0;
                }
            }
        }

    } /* if no error */

    /* we don't need this anymore */
    UT_free(gst);
    g_strfreev(opt_argv);

    if (synerrflg > 0) { /* severe syntax error. Usage must be printed */
        return QC_SYNERR;
    } else if (badparerr > 0) { /* the requester has a clue. No Usage info */
        return QC_PARERR;
    } else {
        return 0;
    }
} /* QC_fill() */



/* Create a new query environment */
Query_environ *QC_environ_new(int sock) {
    Query_environ *qe;

    qe = (Query_environ *) UT_calloc(1, sizeof(Query_environ));
    qe->sources_list = g_list_copy(deflook_sources_list);

    /* init the connection structure, set timeout for reading the query */
    SK_cd_make(&(qe->condat), sock, (unsigned) ca_get_keepopen);

    TA_setcondat(&(qe->condat));

    return qe;
}


/* QC_create() */
/*++++++++++++++++++++++++++++++++++++++
 try to parse the query and fill in the QC struct, setting
 qc->query_type accordingly.

 Query_command *QC_create    returns allocated structure

 char *input                 user query

 Query_environ *qe           query environment structure

 Author:
 marek.

 ++++++++++++++++++++++++++++++++++++++*/

Query_command *QC_create(const char *input, Query_environ *qe) {
    Query_command *qc;
    int qt;

    qc = (Query_command *) UT_calloc(1, sizeof(Query_command));
    QC_init_struct(qc);

    /* search the string for illegal characters */
    if (input[strspn(input, ALLOWED_QUERY_CHARS)]) {
        qc->query_type = QC_PARERR;
        qc->parse_messages = g_list_append(qc->parse_messages, ca_get_qc_badinput);
        return qc;
    }

    if (!*input) {
        /* An empty query (Ie return) was sent */
        qc->query_type = QC_EMPTY;
    } else { /* else <==> input_length > 0 ) */
        /* parse query */
        qt = QC_fill(input, qc, qe);

        if (qt == QC_SYNERR || qt == QC_PARERR) {
            qc->query_type = qt;
        } else {

            /* Only do a query if there are keys. */
            if (qc->keys == NULL || strlen(qc->keys) == 0) {
                if ((qc->q != -1 || qc->t != -1 || qc->v != -1)) {
                    qc->query_type = QC_TEMPLATE;
                } else {
                    qc->query_type = QC_NOKEY;
                }
            } else {
                if (strcmp(qc->keys, "HELP") == 0) {
                    qc->query_type = QC_HELP;
                } else if (qc->filtered) {
                    qc->query_type = QC_FILTERED;
                } else {
                    qc->query_type = QC_REAL;
                }
            }
        }
    }

    return qc;
}

/*++++++++++++++++++++++++++++++++++++++

 Get the name of the given query type code.

 char *QC_get_qrytype   returns a pointer to an element of array of static strings

 qc_qtype_t qrytype     query type code

 ++++++++++++++++++++++++++++++++++++++*/

char *QC_get_qrytype(qc_qtype_t qrytype) {
    dieif(qrytype >= QC_TYPE_MAX);

    return qrytype_str[qrytype];
}

void QC_init(LG_context_t *ctx) {
    int i;
    ca_dbSource_t *hdl;

    /* init logging context */
    qc_context = ctx;

    /* init GList caches */
    all_sources_list_r = NULL;
    deflook_sources_list = NULL;
    grs_sources_list = NULL;

    for (i = 0; (hdl = ca_get_SourceHandleByPosition(i)) != NULL; i++) {
        all_sources_list_r = g_list_prepend(all_sources_list_r, (void *) hdl);

        char *isdeflook = ca_get_srcdeflook(hdl);
        if (strcmp(isdeflook, "y") == 0) {
            deflook_sources_list = g_list_append(deflook_sources_list, (void *) hdl);
        }
        UT_free(isdeflook);

        if (hdl->isGRS) {
            grs_sources_list = g_list_append(grs_sources_list, (void *) hdl);
        }
    }
}
