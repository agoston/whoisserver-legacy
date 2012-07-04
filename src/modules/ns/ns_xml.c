/*
 * $Id: ns_xml.c,v 1.1.4.1 2005/07/22 12:24:31 katie Exp $
 */

#include "ns_xml.h"
#include "ns_util.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>

xmlDocPtr xml_doc; /* constructed xml document */

AU_ret_t rdns_dnscheck(au_plugin_callback_info_t *info, gchar *domain, GList *nservers, GList *ds_rdata) {
    SQ_connection_t *conn = NULL;
    SQ_result_set_t *sql_result = NULL;
    SQ_row_t *row = NULL;
    GString *sql = NULL, *glue = NULL, *report = NULL;
    GList *p;
    char process_id[256];
    int timeleft = 300;
    long long int id = -1;
    long count_critical = -1, count_error = -1;
    AU_ret_t retval = AU_AUTHORISED;

    LG_log(au_context, LG_FUNC, ">rdns_dnscheck(%s): entered", domain);

    // get SQL connection
    if (SQ_try_connection(&conn, "dnscheck.ripe.net", 3306, "dnscheck", "whois", "njt53ntu53f8") != SQ_OK) {
        LG_log(au_context, LG_FATAL, "Failed to connect to dnscheck backend: %d: %s", SQ_errno(conn), SQ_error(conn));
        retval = AU_UNAUTHORISED_CONT;
        goto rdns_dnscheck_bail;
    }

    glue = g_string_new("");

    // convert input to dnscheck format
    for (p = nservers; p; p = p->next) {
        rpsl_attr_t *attr = (rpsl_attr_t *)(p->data);
        gchar *clean_val = rpsl_attr_get_clean_value(attr);

        LG_log(au_context, LG_FUNC, "rdns_dnscheck: nserver: %s", clean_val);

        // replace space with / if glue present
        char *space = strchr(clean_val, ' ');
        if (space) *space = '/';

        // skip space on first entry
        if (p != nservers) g_string_append_c(glue, ' ');
        g_string_append(glue, clean_val);
        free(clean_val);
    }

    for (p = ds_rdata; p; p = p->next) {
        rpsl_attr_t *attr = (rpsl_attr_t *)(p->data);
        gchar *clean_val = rpsl_attr_get_clean_value(attr);
        gchar *pp;

        LG_log(au_context, LG_FUNC, "rdns_dnscheck: nserver: %s", clean_val);

        g_string_append(glue, " ds:/");
        g_string_append(glue, domain);
        g_string_append(glue, "_DS_");
        for (pp = clean_val; *pp; pp++) {
            if (*pp == ' ') *pp = '_';
        }
        g_string_append(glue, clean_val);

        free(clean_val);
    }

    LG_log(au_context, LG_FUNC, "rdns_dnscheck: glue: %s", glue->str);

    // generate process id
    snprintf(process_id, 256, "lgc-%ld-%d", time(NULL), getpid());

    sql = g_string_new("");

    // insert
    g_string_printf(sql, "INSERT INTO queue (domain, priority, source_id, source_data, fake_parent_glue) VALUES ('%s', 1, 2, '%s', '%s')", domain, process_id, glue->str);
    LG_log(au_context, LG_FUNC, "rdns_dnscheck: executing query %s", sql->str);

    if (SQ_execute_query(conn, sql->str, (SQ_result_set_t **)NULL) < 0) {
        LG_log(au_context, LG_FATAL, "SQL ERROR '%d': '%s' for query '%s'", SQ_errno(conn), SQ_error(conn), sql->str);
        retval = AU_UNAUTHORISED_CONT;
        goto rdns_dnscheck_bail;
    }

    // poll table results for 5 minutes
    g_string_printf(sql, "SELECT id, end, count_critical, count_error FROM tests WHERE source_id=2 and source_data='%s'", process_id);
    LG_log(au_context, LG_FUNC, "rdns_dnscheck: executing query %s", sql->str);

    for (; timeleft > 0; timeleft--) {
        sleep(1);

        if (SQ_execute_query(conn, sql->str, &sql_result) < 0) {
            LG_log(au_context, LG_FATAL, "SQL ERROR '%d': '%s' for query '%s', timeleft: %d", SQ_errno(conn), SQ_error(conn), sql->str, timeleft);
            retval = AU_UNAUTHORISED_CONT;
            goto rdns_dnscheck_bail;
        }

        if ((row = SQ_row_next(sql_result)) != NULL) {
            char *raw_id = SQ_get_column_string_nocopy(sql_result, row, 0);
            char *raw_end = SQ_get_column_string_nocopy(sql_result, row, 1);
            char *raw_crit = SQ_get_column_string_nocopy(sql_result, row, 2);
            char *raw_err = SQ_get_column_string_nocopy(sql_result, row, 3);

            LG_log(au_context, LG_FUNC, "id:[%s] end:[%s] crit:[%s] err:[%s]", raw_id, raw_end, raw_crit, raw_err);

            if (raw_end) {
                if (!SQ_get_column_llint(sql_result, row, 0, &id) && !SQ_get_column_int(sql_result, row, 2, &count_critical) && !SQ_get_column_int(sql_result, row, 3, &count_error)) {
                    LG_log(au_context, LG_FUNC, "rdns_dnscheck: got id: %lld, end: %s, count_critical: %ld, count_error: %ld, timeleft: %d", id, raw_end, count_critical, count_error, timeleft);
                    break;
                }
            }
        }

        if (sql_result) {
            SQ_free_result(sql_result);
            sql_result = NULL;
        }
    }

    if (timeleft <= 0) {
        LG_log(au_context, LG_FATAL, "rdns_dnscheck: timeout; id: %lld, count_critical: %ld, count_error: %ld", id, count_critical, count_error);
        retval = AU_UNAUTHORISED_CONT;
        goto rdns_dnscheck_bail;
    }

    // gather messages
    if (count_critical > 0 || count_error > 0) {
        report = g_string_new("");

        g_string_printf(sql, "SELECT level,formatstring,description,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9 FROM (SELECT * FROM results WHERE test_id=%lld AND level IN ('ERROR','CRITICAL', 'WARNING')) AS tmp LEFT JOIN messages ON tmp.message = messages.tag ORDER BY tmp.id ASC", id);
        LG_log(au_context, LG_FUNC, "rdns_dnscheck: executing query %s", sql->str);

        if (SQ_execute_query(conn, sql->str, &sql_result) < 0) {
            LG_log(au_context, LG_FATAL, "SQL ERROR '%d': '%s' for query '%s'", SQ_errno(conn), SQ_error(conn), sql->str);
            retval = AU_UNAUTHORISED_CONT;
            goto rdns_dnscheck_bail;
        }

        while ((row = SQ_row_next(sql_result)) != NULL) {
            char *level = SQ_get_column_string_nocopy(sql_result, row, 0);
            char *formatstring = SQ_get_column_string_nocopy(sql_result, row, 1);
            char *description = SQ_get_column_string_nocopy(sql_result, row, 2);

            if (level && formatstring) {
                char *warning_str;
                g_string_printf(report, "(related to %s) %s: ", domain, level);
                g_string_append_printf(report, formatstring, SQ_get_column_string_nocopy(sql_result, row, 3),
                        SQ_get_column_string_nocopy(sql_result, row, 4),
                        SQ_get_column_string_nocopy(sql_result, row, 5),
                        SQ_get_column_string_nocopy(sql_result, row, 6),
                        SQ_get_column_string_nocopy(sql_result, row, 7),
                        SQ_get_column_string_nocopy(sql_result, row, 8),
                        SQ_get_column_string_nocopy(sql_result, row, 9),
                        SQ_get_column_string_nocopy(sql_result, row, 10));
                if (description) {
                    g_string_append_printf(report, "\n\n%s", description);
                }

                warning_str = ns_par(report->str);
                RT_rdns_delcheckwarning(info->ctx, warning_str);
                LG_log(au_context, LG_FUNC, "rdns_dnscheck: got: %s", report->str);
                if (strncmp(level, "WARNING", 7)) {
                    retval = AU_UNAUTHORISED_CONT;
                }
            } else {
                LG_log(au_context, LG_FUNC, "rdns_dnscheck: invalid entry in results table: column 'level' or 'formatstring' is NULL");
            }
        }

        if (sql_result) {
            SQ_free_result(sql_result);
            sql_result = NULL;
        }
    }

rdns_dnscheck_bail:
    LG_log(au_context, LG_FUNC, "<rdns_dnscheck: exiting (with %s)", AU_ret2str(retval));

    if (sql_result) SQ_free_result(sql_result);
    if (sql) g_string_free(sql, TRUE);
    if (glue) g_string_free(glue, TRUE);
    if (report) g_string_free(report, TRUE);
    if (conn) SQ_close_connection(conn);

    return retval;
}
