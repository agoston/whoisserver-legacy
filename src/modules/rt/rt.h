/***************************************
  $Revision: 1.6 $

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
#ifndef RT_H
#define RT_H

#include <libxml/tree.h>
#include "lg.h"
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <stdlib.h>
#include <glib.h>

typedef struct RT_context {
  xmlDocPtr doc;
  xmlNodePtr root_node;
  GList* composites;
  xmlNodePtr current_aggregate_node;
  xmlNodePtr processed_node;
  gpointer specific_data;
} RT_context_t;


void RT_init(LG_context_t *_ctx);

RT_context_t* RT_start();
void RT_end(RT_context_t* ctx);
void RT_destroy(RT_context_t* ctx);

void RT_start_detailed_report(RT_context_t* ctx);
void RT_end_detailed_report(RT_context_t* ctx);

void RT_request_level(RT_context_t* ctx, xmlNodePtr node);
void RT_release_level(RT_context_t* ctx);

gchar* RT_get(RT_context_t* ctx, gint level, gchar* template_file);

void RT_message(RT_context_t* ctx, gchar* name, gchar* message);
void RT_process_time(RT_context_t* ctx, gchar* date_stamp);

void rt_add_text_node(xmlNodePtr parent, gchar* name, gchar* content);
void rt_xml_node_add_content(xmlNodePtr node, xmlChar* content);
void rt_xml_set_prop(xmlNodePtr node, xmlChar* id, xmlChar* value);

typedef xmlChar* (*rt_map_function)(void* data);

void rt_generate_list_map(xmlNodePtr parent_node,
                             gchar* tag, GList* list, rt_map_function f);
/* xmlNodePtr rt_generate_list_map(xmlNodePtr parent_node,
  gchar* tag, GList* list, rt_map_function f); */

#endif
