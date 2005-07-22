/*
 * $Id: ns_xml.c,v 1.1 2004/12/27 17:52:36 can Exp $
 */

#include "ns_xml.h"
#include "ns_perl.h"
#include "ns_util.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>

xmlDocPtr xml_doc;              /* constructed xml document */

/*
 * utility function for xml processing
 */
static xmlDocPtr get_doc(char *docname)
{
  xmlDocPtr doc;
  doc = xmlParseFile(docname);

  if (doc == NULL) {
    fprintf(stderr, "Document not parsed successfully. \n");
    return NULL;
  }

  return doc;
}

/*
 * utility function for xml processing
 */
static xmlXPathObjectPtr getnodeset(xmlDocPtr doc, xmlChar * xpath)
{

  xmlXPathContextPtr context;
  xmlXPathObjectPtr result;

  context = xmlXPathNewContext(doc);
  result = xmlXPathEvalExpression(xpath, context);
  if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
    fprintf(stderr, "No result\n");
    return NULL;
  }
  xmlXPathFreeContext(context);
  return result;
}

/* subfunction that iterates over problem entities
 * such structured data deserves such hacks 8-( */
void ns_report_warnings(gpointer data, gpointer user_data)
{
  au_plugin_callback_info_t *info = user_data;
  xmlChar *problem_node_template =
      "//DELCHECK_REPORT/PROBLEM_ENTITY[attribute::name=\"%s\"]/PROBLEM/SHORT";
  gchar *entity = g_strdup(data);       /* must be cast to char */
  gchar *problem_node;          /* created from template */
  gchar *warning_str;           /* formatted warning string */
  gchar *warning_str1;          /* temp formatted warning string */
  xmlXPathObjectPtr result;     /* result from an xpath query */
  xmlNodeSetPtr nodeset;        /* result nodeset from an xpath query */
  gint i;                       /* generic temporary int */
  xmlChar *keyword;             /* string format of each xpath query match */

  problem_node = g_strdup_printf(problem_node_template, entity);
  result = getnodeset(xml_doc, (xmlChar *) problem_node);
  if (result) {
    nodeset = result->nodesetval;
    for (i = 0; i < nodeset->nodeNr; i++) {
      keyword =
          xmlNodeListGetString(xml_doc,
                               nodeset->nodeTab[i]->xmlChildrenNode, (1));
      warning_str1 =
          g_strdup_printf("(related to %s) %s", entity, keyword);
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


/*
 * Check delegation related problems for the domain
 */
AU_ret_t ns_domain_delcheck(au_plugin_callback_info_t * info,
                            gchar * domain, gchar ** nservers, gchar ** ds_rdata,
                            gchar * delcheck_conf_file)
{
  xmlChar *severity_node =
      "//DELCHECK_REPORT/PROBLEM_ENTITY/PROBLEM/attribute::severity";
  xmlChar *entity_node =
      "//DELCHECK_REPORT/PROBLEM_ENTITY/attribute::name";

  gint i;                       /* generic temporary int */
  xmlChar *keyword;             /* string format of each xpath query match */
  gchar *delcheck_result = NULL;        /* output from delchecker */
  gchar *delcheck_errors = NULL;        /* error string from delchecker */
  AU_ret_t ret_val = AU_AUTHORISED;     /* return value of this function */
  gchar *tmp_file_name;         /* used while creating tmp files */
  gint f;                       /* for general file handling */
  gulong severity = 0;          /* total severity from delcheck */
  gulong current_severity = 0;  /* severity for each problem */
  GSList *entities = NULL;      /* list of entities */
  xmlXPathObjectPtr result;     /* result from an xpath query */
  xmlNodeSetPtr nodeset;        /* result nodeset from an xpath query */

  LG_log(au_context, LG_DEBUG, "invoking delcheck");
  
  LG_log(au_context, LG_DEBUG, "with parameters: ");
  LG_log(au_context, LG_DEBUG, "delcheck_conf_file=[%s]",delcheck_conf_file);
  LG_log(au_context, LG_DEBUG, "domain=[%s]",domain);

  if (nservers == NULL) {
    LG_log(au_context, LG_DEBUG, "NO nameservers");
  } else {
    LG_log(au_context, LG_DEBUG, "nservers=[%s]",g_strjoinv(",", (gchar **) nservers));
  }
  if (ds_rdata == NULL) {
    LG_log(au_context, LG_DEBUG, "NO ds-rdata");
  } else {
    LG_log(au_context, LG_DEBUG, "ds-rdata=[%s]",g_strjoinv(",", (gchar **) ds_rdata));
  }
  rdns_perl_delcheck(delcheck_conf_file, domain, nservers, ds_rdata,
                     &delcheck_result, &delcheck_errors);
  if (delcheck_errors != NULL) {
    LG_log(au_context, LG_DEBUG, "error in running delcheck:");
    LG_log(au_context, LG_DEBUG, delcheck_errors);
    g_free(delcheck_errors);
    ret_val = AU_UNAUTHORISED_END;
  } else {
    tmp_file_name = g_strdup_printf("%s/delcheckxmlXXXXXX", ca_get_tmpdir);
    f = mkstemp(tmp_file_name);
    if (f == -1) {
      LG_log(au_context, LG_DEBUG, "can't open %s for writing",
             tmp_file_name);
      ret_val = AU_UNAUTHORISED_END;
    } else {
      LG_log(au_context, LG_DEBUG, "opened %s for writing", tmp_file_name);
      write(f, delcheck_result, strlen(delcheck_result));
      close(f);

      /* parsing the result from delchecker via libxml2 */
      xml_doc = get_doc(tmp_file_name);
      /* compute the total severity */
      result = getnodeset(xml_doc, (xmlChar *) severity_node);
      if (result) {
        nodeset = result->nodesetval;
        for (i = 0; i < nodeset->nodeNr; i++) {
          keyword =
              xmlNodeListGetString(xml_doc,
                                   nodeset->nodeTab[i]->xmlChildrenNode,
                                   1);
          current_severity = strtol(keyword, (char **) NULL, 10);
          if (errno == ERANGE) {
            LG_log(au_context, LG_DEBUG,
                   "severity \"%s\" is not a valid integer", keyword);
            current_severity = 500;     /* better to reject than to ignore */
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
          keyword =
              xmlNodeListGetString(xml_doc,
                                   nodeset->nodeTab[i]->xmlChildrenNode,
                                   1);
          entities = g_slist_append(entities, strdup(keyword));
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
