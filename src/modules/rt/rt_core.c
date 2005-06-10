/***************************************
  $Revision: 1.1 $

  Reporting module.

  Status: REVIEWED, TESTED

 Author(s):       Tiago Antao

  ******************/ /******************
  Modification History:
        tiago (10/04/2003) Created.
  ******************/ /******************
  Copyright (c) 2003               RIPE NCC
 
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
#include "rip.h"
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <stdlib.h>
#include "rt.h"
#include "rt_internal.h"

void RT_init(LG_context_t *_ctx)
{
  lg_ctx = _ctx;
}

/*+
  RT_start - generates a new reporting context.

  This function initializes a report and returns a handle to it.
  +*/
RT_context_t* RT_start() {
  RT_context_t* ctx;

  ctx = UT_malloc(sizeof(RT_context_t));
  ctx->doc = xmlNewDoc((xmlChar*)"1.0");

  ctx->root_node = xmlNewNode(NULL, (xmlChar *)"report");
  xmlDocSetRootElement(ctx->doc, ctx->root_node);
  ctx->current_aggregate_node = ctx->root_node;
  ctx->processed_node = NULL;
  ctx->composites = g_list_append(NULL, ctx->root_node);
  return ctx;
}

/*+
  RT_end - Notifies the end of event reporting.

  Don't confuse this with RT_destroy used to end the reporting context.

  RT_context_t* ctx - The context that is to be ended.
  +*/
void RT_end(RT_context_t* ctx){
  if (ctx->processed_node) {
    xmlAddChild(ctx->root_node,ctx->processed_node);
  }
  ctx->processed_node = NULL;
}

/*+
  RT_destroy - Destroys a report handle.

  This completely destroys a report structure.

  RT_context_t* ctx - The context that is to be destroyed.
  +*/
void RT_destroy(RT_context_t* ctx){
  g_list_free(ctx->composites);
  xmlFreeDoc(ctx->doc);
  if (ctx->processed_node) {
    xmlFreeNode(ctx->processed_node);
  }
  UT_free(ctx);
}


GString* rt_generate_dump(gint level, gint parent_level, xmlNodePtr text) {
  xmlNodePtr current_node;
  xmlAttrPtr level_attr;
  gint current_level;
  GString* string;
  GString* tmp_string;

  if (text == NULL) {
    return g_string_new("");
  }
  //root_node = xmlDocGetRootElement(text);
  string = g_string_new("");
  current_level = parent_level;

  current_node = text;
  while (current_node != NULL) {
    if (strcmp((char*)current_node->name, "level") == 0) {
      level_attr = current_node->properties;
      if (level_attr != NULL &&
	  strcmp((char*)level_attr->name,"level") == 0 ) {
	current_level = atoi((char*)level_attr->children->content);
      }
      if (current_level>=level && current_node->children != NULL){
	tmp_string = rt_generate_dump(level, current_level,
					 current_node->children);
	g_string_append(string, tmp_string->str);
	g_string_free(tmp_string, TRUE);

      }
    }
    else {
      g_string_append(string, (char*)current_node->content);
      
    }

    current_node = current_node ->next;
  }

  return string;
}

gchar* rt_clean_entities(gchar* dirrty) {
  // dirrty and cllean are inspired in the awful Dirrty from Cristina
  // Aguilera
  gchar* cllean;
  gchar* cret;
  int c;
#define ENT_QT 4 /* If you change the array below, reflect it here!*/
  gchar* ents[][2] = {  {"&amp;", "&"}, {"&quot;", "\""}, {"&gt;", ">"}, {"&lt;", "<"}, 
}; 

  cllean = g_malloc(strlen(dirrty) + 1);
  cret = cllean;
  while (*dirrty) {
    *cllean = *dirrty;

    for (c=0; c<ENT_QT; c++) {
      if (strncmp(dirrty, ents[c][0], strlen(ents[c][0])) == 0) {
        *cllean = ents[c][1][0];
        dirrty = dirrty + strlen(ents[c][0]) - 1;
        break;
      }
    }
    cllean++;
    dirrty++;
  }
  *cllean = 0;

  return cret;
}

/*+
  RT_get - Gets a report acconding to a specified template file.

  RT_context_t* ctx - The context from what a report will be generated.

  gint level - The minimum level of reporting desired.

  gchar* template_name - The file containing the template.
  +*/
gchar* RT_get(RT_context_t* ctx, gint level, gchar* template_name) {
  xsltStylesheetPtr xslt;
  xmlDocPtr         text;
  const char*       params[1];
  GString*          dump;
  int               name_len;
  xmlChar*          doc_dump;
  int               size;
  xmlDocPtr         doc;
  gchar             level_string[10];
  gchar*            return_str;
  gchar*            real_return_str;

  //xmlDocDump(stdout, ctx->doc);

  xslt = xsltParseStylesheetFile((const xmlChar*)template_name);
  params[0] = NULL;
  doc = xmlCopyDoc(ctx->doc, 1);
  sprintf(level_string, "%d", level); 
  xmlNewProp(xmlDocGetRootElement(doc), (xmlChar*)"level", (xmlChar*)level_string);

  xmlDocDump(stdout, doc);

  text = xsltApplyStylesheet(xslt, doc, params);
  xmlFreeDoc(doc);
  xsltFreeStylesheet(xslt);

  //xmlDocDump(stdout, text);
  name_len = strlen(template_name);

  xmlDocDumpMemory(text, &doc_dump, &size);
  dump = g_string_new((gchar*) doc_dump);
  free(doc_dump);
  return_str = dump->str;
  g_string_free(dump, FALSE);
  if (template_name[name_len-2] == '.' && template_name[name_len-1] == 'x' ) {
    real_return_str = return_str;
  }
  else {
    real_return_str = return_str;
    while (*real_return_str != '\n') real_return_str++ ;
    real_return_str++;
    if (real_return_str) {
      real_return_str = rt_clean_entities(real_return_str);
    }
    else {
      real_return_str = strdup("");
    }
    free(return_str);
  }

  return real_return_str;
}

/*+
  RT_start_detailed_report - Notifies the beginning of a detailed report.

  A detailed report is a part of the report that is considered to be,
  by tue user, about the same event.

  Example: a report about route creation authentication will consist of
  4 related sub-events: authentication of AS, mnt-routes, mnt-by and
  the end result of it all.

  The last call before RT_end_detailed_report is considered to be a
  summary call (eg the end result of a route creation attempt).

  RT_context_t* ctx - The context.

  +*/
void RT_start_detailed_report(RT_context_t* ctx) {
  xmlNodePtr details;

  LG_log(lg_ctx, LG_FUNC,">RT_start_detailed_report: entered");

  if (ctx->processed_node != NULL) {
    xmlAddChild(ctx->current_aggregate_node, ctx->processed_node);
    ctx->processed_node = NULL;
  }
  details = xmlNewNode(NULL, (xmlChar *)"details");
  ctx->current_aggregate_node = details;
  ctx->composites = g_list_append(ctx->composites, details);

  LG_log(lg_ctx, LG_FUNC,"<RT_start_detailed_report: exiting");
}

/*+
  RT_end_detailed_report - Notifies the ending of a detailed report.

  See RT_start_detailed_report for details.

  RT_context_t* ctx - The context.
  +*/
void RT_end_detailed_report(RT_context_t* ctx) {
  xmlNodePtr summary;
  xmlNodePtr composite;

  LG_log(lg_ctx, LG_FUNC,">RT_end_detailed_report: entered");

  //xmlDocDump(stdout, ctx->doc);

  if (ctx->processed_node == NULL) {
    LG_log(lg_ctx, LG_DEBUG, "end of detail without report");
    exit(1);
  }
  if (ctx->root_node == ctx->current_aggregate_node) {
    LG_log(lg_ctx, LG_DEBUG, "requesting end of detail on root");
    exit(1);
  }

  summary = xmlNewNode(NULL, (xmlChar *)"summary");
  xmlAddChild(summary, ctx->processed_node);
  ctx->processed_node = NULL;

  composite = xmlNewNode(NULL, (xmlChar *)"composite");
  xmlAddChild(composite, summary);
  xmlAddChild(composite, ctx->current_aggregate_node);
  ctx->composites = g_list_remove(ctx->composites, ctx->current_aggregate_node);
  ctx->current_aggregate_node = (xmlNodePtr)g_list_last(ctx->composites)->data;
  xmlAddChild(ctx->current_aggregate_node, composite);

  //xmlDocDump(stdout, ctx->doc);

  LG_log(lg_ctx, LG_FUNC,"<RT_end_detailed_report: exiting");
}

void RT_request_level(RT_context_t* ctx, xmlNodePtr node) {

  if (ctx->processed_node != NULL) {
    xmlAddChild(ctx->current_aggregate_node, ctx->processed_node);
    ctx->processed_node = NULL;
  }
  ctx->current_aggregate_node = node;
  ctx->composites = g_list_append(ctx->composites, node);
}

void RT_release_level(RT_context_t* ctx) {
  xmlNodePtr old_aggregate;

  if (ctx->root_node == ctx->current_aggregate_node) {
    printf("requesting end of level on root\n");
    exit(1);
  }
  old_aggregate = ctx->current_aggregate_node;

  if (ctx->processed_node != NULL) {
    xmlAddChild(old_aggregate, ctx->processed_node);
  }

  ctx->composites = g_list_remove(ctx->composites, old_aggregate);
  ctx->current_aggregate_node = (xmlNodePtr)g_list_last(ctx->composites)->data;
  xmlAddChild(ctx->current_aggregate_node, old_aggregate);
  ctx->processed_node = NULL;
}

void rt_prepare_node(RT_context_t* ctx, xmlNodePtr node);

/*+
  RT_message - Reports a message directly without using intermedieate layers.

  RT_context_t* ctx - The context.

  gchar* name - The name of the attribute.

  gchar* message - The message to be reported.
  +*/
void RT_message(RT_context_t* ctx, gchar* name, gchar* message) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)name);
  xmlNodeAddContent(node, (xmlChar*)message);

  rt_prepare_node(ctx, node);
}

//xmlNodePtr 
void rt_generate_list(xmlNodePtr parent_node, gchar* tag, GList* list) {
  xmlNodePtr node;

  while (list) {
    node = xmlNewNode(NULL, (xmlChar*)tag);
    xmlNodeSetContent(node, (xmlChar*)list->data);
    xmlAddChild(parent_node, node);
    list = g_list_next(list);
  }
}

//xmlNodePtr 
void rt_generate_list_map(xmlNodePtr parent_node,
  gchar* tag, GList* list, rt_map_function f) {
  xmlNodePtr node;

  while (list) {
    node = xmlNewNode(NULL, (xmlChar*)tag);
    xmlNodeSetContent(node, f(list->data));
    xmlAddChild(parent_node, node);
    list = g_list_next(list);
  }
}


void rt_prepare_node(RT_context_t* ctx, xmlNodePtr node) {
  if (ctx->processed_node) {
    xmlAddChild(ctx->current_aggregate_node, ctx->processed_node);
  }
  ctx->processed_node = node;
}

void rt_clean_data(unsigned char* data) {
  unsigned char* the_char;

  the_char = data;
  while (the_char && *the_char) {
    if (*the_char>127) {
      *the_char = '.';
    }
    the_char++;
  }
}

void rt_xml_node_add_content(xmlNodePtr node, xmlChar* content) {

  rt_clean_data(content);
  xmlNodeAddContent(node, content);
}

void rt_xml_set_prop(xmlNodePtr node, xmlChar* id, xmlChar* value) {

  rt_clean_data(value);
  xmlSetProp(node, id, value);
}

void rt_add_text_node(xmlNodePtr parent, gchar* name, gchar* content) {
//  xmlNodePtr node;

  char *copy_content = NULL;
  
  /* Introduced a copy of the content because many functions pass
     const pointers as the content. 
     This function calls rt_clean_data that changes the contents of the content
     pointer. */
  if (content)
  {
    copy_content = strdup(content);

    /*
    node = xmlNewNode(NULL, (xmlChar*)name);
    xmlNodeAddContent(node, (xmlChar*)content);
    xmlAddChild(parent, node);
    */
    // printf("XXXXX%dXXXX%s\n", xmlCheckUTF8(content), content);
    //if it crashes here remove the next line
    rt_clean_data(copy_content);
    xmlNewTextChild(parent, NULL, (xmlChar*)name, (xmlChar*)copy_content);
    free(copy_content);
  }
  
}



