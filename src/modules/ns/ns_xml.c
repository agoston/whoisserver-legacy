/*
 * $Id: ns_xml.c,v 1.1.4.1 2005/07/22 12:24:31 katie Exp $
 */

#include "ns_xml.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>

xmlDocPtr xml_doc; /* constructed xml document */

/*
 * utility function for xml processing
 */
static xmlDocPtr get_doc(char *docname) {
    xmlDocPtr doc;
    doc = xmlParseFile(docname);

    if (doc == NULL ) {
        fprintf(stderr, "Document not parsed successfully. \n");
        return NULL ;
    }

    return doc;
}

/*
 * utility function for xml processing
 */
static xmlXPathObjectPtr getnodeset(xmlDocPtr doc, xmlChar * xpath) {

    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;

    context = xmlXPathNewContext(doc);
    result = xmlXPathEvalExpression(xpath, context);
    if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        fprintf(stderr, "No result\n");
        return NULL ;
    }
    xmlXPathFreeContext(context);
    return result;
}

/* subfunction that iterates over problem entities
 * such structured data deserves such hacks 8-( */
void ns_report_warnings(gpointer data, gpointer user_data) {
    au_plugin_callback_info_t *info = user_data;
    xmlChar *problem_node_template = (xmlChar *) "//DELCHECK_REPORT/PROBLEM_ENTITY[attribute::name=\"%s\"]/PROBLEM/SHORT";
    gchar *entity = g_strdup(data); /* must be cast to char */
    gchar *problem_node; /* created from template */
    gchar *warning_str; /* formatted warning string */
    gchar *warning_str1; /* temp formatted warning string */
    xmlXPathObjectPtr result; /* result from an xpath query */
    xmlNodeSetPtr nodeset; /* result nodeset from an xpath query */
    gint i; /* generic temporary int */
    xmlChar *keyword; /* string format of each xpath query match */

    problem_node = g_strdup_printf((char*) problem_node_template, entity);
    result = getnodeset(xml_doc, (xmlChar *) problem_node);
    if (result) {
        nodeset = result->nodesetval;
        for (i = 0; i < nodeset->nodeNr; i++) {
            keyword = xmlNodeListGetString(xml_doc, nodeset->nodeTab[i]->xmlChildrenNode, (1));
            warning_str1 = g_strdup_printf("(related to %s) %s", entity, keyword);
            warning_str = ns_par(warning_str1);
            RT_rdns_delcheckwarning(info->ctx, warning_str);
            xmlFree(keyword);
            g_free(warning_str);
            g_free(warning_str1);
        }
        xmlXPathFreeObject(result);
    }
    xmlCleanupParser();
    g_free(problem_node);
    g_free(entity);
}

void rdns_dnscheck(gchar *domain, GList *nservers, GList *ds_rdata, gchar **delcheck_result, gchar **delcheck_errors) {
    SQ_connection_t conn;
    GList *p;
    GString *sql, *glue;

    LG_log(lu_context, LG_FUNC, ">rdns_dnscheck(%s): entered", domain);

    // get SQL connection
    if (SQ_try_connection(&conn, "dnscheck.ripe.net", 3306, "dnscheck", "njt53ntu53f8") != SQ_OK) {
        *delcheck_errors = g_sprintf("Failed to connect to dnscheck backend: %d: %s\n", SQ_errno(conn), SQ_error(conn));
        LG_log(lu_context, LG_FUNC, "<rdns_dnscheck: %s", *delcheck_errors);
        SQ_close_connection(conn);
        return;
    }

    glue = g_string_new("");
    sql = g_string_new("");

    // convert input to dnscheck format
    for (p = nservers; p; p = p->next) {
        rpsl_attr_t *attr = (rpsl_attr_t *)(p->data);
        gchar *clean_val = rpsl_attr_get_clean_value(attr);

        // replace space with / if glue present
        char *space = strchr(clean_val, ' ');
        if (space) *space = '/';

        // skip space on first entry
        if (p != nservers) g_string_append(glue, ' ');
        g_string_append(glue, clean_val);
        free(clean_val);
    }

    for (p = ds_rdata; p; p = p->next) {
        rpsl_attr_t *attr = (rpsl_attr_t *)(p->data);
        gchar *clean_val = rpsl_attr_get_clean_value(attr);

    }

    // insert
    snprintf(buf, sizeof(buf), "INSERT INTO queue (domain, priority, source_id, source_data, fake_parent_glue) VALUES ('%s', 1, 2, 'legacy-%d', '');");
    LG_log(lu_context, LG_FUNC, "rdns_dnscheck: executing query %s", *delcheck_errors);

    if (SQ_execute_query(conn, buf, &result) == -1) {
        LG_log(lu_context, LG_FATAL, "lu_whois_lookup: SQL ERROR '%d': '%s' for query '%s'", SQ_errno(conn), SQ_error(conn), buf);
        die;
    }

    if ((row = SQ_row_next(result)) != NULL) {
        char *objtext = SQ_get_column_string_nocopy(result, row, 0);
        LG_log(lu_context, LG_DEBUG, "lu_whois_lookup: got object from SQL DB: [%s]", objtext);
        *obj = rpsl_object_init(objtext);
        if (!*obj) {
            LG_log(lu_context, LG_ERROR, "lu_whois_query: error parsing object returned by WHOIS");
            retval = LU_ERROR;
        }
    } else {
        *obj = NULL;
    }


    g_string_free(sql, TRUE);
    g_string_free(glue, TRUE);
    SQ_close_connection(conn);
}

/*
 * Check delegation related problems for the domain
 */
AU_ret_t ns_domain_dnscheck(au_plugin_callback_info_t * info, gchar * domain, GList *nservers, GList *ds_rdata) {
    xmlChar *severity_node = (xmlChar *) "//DELCHECK_REPORT/PROBLEM_ENTITY/PROBLEM/attribute::severity";
    xmlChar *entity_node = (xmlChar *) "//DELCHECK_REPORT/PROBLEM_ENTITY/attribute::name";

    gint i; /* generic temporary int */
    xmlChar *keyword; /* string format of each xpath query match */
    gchar *delcheck_result = NULL; /* output from delchecker */
    gchar *delcheck_errors = NULL; /* error string from delchecker */
    AU_ret_t ret_val = AU_AUTHORISED; /* return value of this function */
    gchar *tmp_file_name; /* used while creating tmp files */
    gint f; /* for general file handling */
    gulong severity = 0; /* total severity from delcheck */
    gulong current_severity = 0; /* severity for each problem */
    GSList *entities = NULL; /* list of entities */
    xmlXPathObjectPtr result; /* result from an xpath query */
    xmlNodeSetPtr nodeset; /* result nodeset from an xpath query */

    LG_log(au_context, LG_DEBUG, "invoking dnscheck");

    LG_log(au_context, LG_DEBUG, "with parameters: ");
    LG_log(au_context, LG_DEBUG, "domain=[%s]", domain);

    if (nservers == NULL ) {
        LG_log(au_context, LG_DEBUG, "NO nameservers");
    } else {
        LG_log(au_context, LG_DEBUG, "nservers=[%s]", g_strjoinv(",", (gchar **) nservers));
    }
    if (ds_rdata == NULL ) {
        LG_log(au_context, LG_DEBUG, "NO ds-rdata");
    } else {
        LG_log(au_context, LG_DEBUG, "ds-rdata=[%s]", g_strjoinv(",", (gchar **) ds_rdata));
    }

    rdns_dnscheck(domain, nservers, ds_rdata, &delcheck_result, &delcheck_errors);

    if (delcheck_errors != NULL ) {
        LG_log(au_context, LG_DEBUG, "error in running dnscheck:");
        LG_log(au_context, LG_DEBUG, delcheck_errors);
        g_free(delcheck_errors);
        ret_val = AU_UNAUTHORISED_END;
    } else {
        tmp_file_name = g_strdup_printf("%s/dnscheckxmlXXXXXX", ca_get_tmpdir);
        f = mkstemp(tmp_file_name);
        if (f == -1) {
            LG_log(au_context, LG_DEBUG, "can't open %s for writing", tmp_file_name);
            ret_val = AU_UNAUTHORISED_END;
        } else {
            LG_log(au_context, LG_DEBUG, "opened %s for writing", tmp_file_name);
            write(f, delcheck_result, strlen(delcheck_result));
            close(f);

            /* parsing the result from dnschecker via libxml2 */
            xml_doc = get_doc(tmp_file_name);
            /* compute the total severity */
            result = getnodeset(xml_doc, (xmlChar *) severity_node);
            if (result) {
                nodeset = result->nodesetval;
                for (i = 0; i < nodeset->nodeNr; i++) {
                    keyword = xmlNodeListGetString(xml_doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
                    current_severity = strtol((char *) keyword, (char **) NULL, 10);
                    if (errno == ERANGE) {
                        LG_log(au_context, LG_DEBUG, "severity \"%s\" is not a valid integer", keyword);
                        current_severity = 500; /* better to reject than to ignore */
                    }
                    severity = severity + current_severity;
                    xmlFree(keyword);
                }
                xmlXPathFreeObject(result);
            }
            xmlCleanupParser();

            /* find entities */
            result = getnodeset(xml_doc, (xmlChar *) entity_node);
            if (result) {
                nodeset = result->nodesetval;
                for (i = 0; i < nodeset->nodeNr; i++) {
                    keyword = xmlNodeListGetString(xml_doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
                    entities = g_slist_append(entities, strdup((char *) keyword));
                }
                xmlXPathFreeObject(result);
                xmlCleanupParser();

                /* report warnings for each entity */
                g_slist_foreach(entities, ns_report_warnings, info);

                g_slist_free(entities);
            }

            if (severity >= ca_get_ns_severity) {
                RT_rdns_threshold(info->ctx);
                ret_val = AU_UNAUTHORISED_CONT;
            }

            unlink(tmp_file_name);
        }
        g_free(tmp_file_name);
    }
    return ret_val;
}
